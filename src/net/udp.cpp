#include "net/udp.h"
#include "memorymanagement.h"
namespace myos {
	namespace net {
		UserDatagramProtocolHandler::UserDatagramProtocolHandler() {

		}

		UserDatagramProtocolHandler::~UserDatagramProtocolHandler() {

		}

		void UserDatagramProtocolHandler::HandleUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket, uint8_t* data, uint32_t size) {

		}

		UserDatagramProtocolSocket::UserDatagramProtocolSocket(UserDatagramProtocolProvider* backend) {
			this->backend = backend;
			this->handler = nullptr;

		}
		UserDatagramProtocolSocket::~UserDatagramProtocolSocket() {

		}

		void UserDatagramProtocolSocket::HandleUserDatagramProtocolMessage(uint8_t* data, uint32_t size) {
			if (handler != nullptr) {
				handler->HandleUserDatagramProtocolMessage(this, data, size);
			}
		}

		void UserDatagramProtocolSocket::Send(uint8_t* data, uint32_t size) {
			backend->Send(this, data, size);
		}

		void UserDatagramProtocolSocket::Disconnect() {
			backend->Disconnect(this);
		}

		UserDatagramProtocolProvider::UserDatagramProtocolProvider(InternetProtocolProvider* backend) :
			InternetProtocolHandler(backend, 0x11) {
			for (int i = 0; i < 65535; i++) {
				sockets[i] = nullptr;
			}
			numSockets = 0;
			freePort = 1024;
		}

		UserDatagramProtocolProvider::~UserDatagramProtocolProvider() {

		}

		bool UserDatagramProtocolProvider::OnInternetProtocolRecevied(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size) {
			if (size < sizeof(UserDatagramProtocolHeader)) {
				return false;
			}
			UserDatagramProtocolHeader* msg = (UserDatagramProtocolHeader*)internetProtocolPayload;
			uint16_t localPort = msg->dstPort;
			uint16_t remotePort = msg->srcPort;

			UserDatagramProtocolSocket* socket = nullptr;
			for (int i = 0; i < numSockets && socket == nullptr; i++) {
				if (sockets[i]->localPort == msg->dstPort &&
					sockets[i]->localIP == dstIP_BE) {
					if (sockets[i]->listening) {
						socket = sockets[i];
						socket->listening = false;
						socket->remotePort = msg->srcPort;
						socket->remoteIP = srcIP_BE;
					}
					else if (
						sockets[i]->remotePort == msg->srcPort &&
						sockets[i]->remoteIP == srcIP_BE) {
						socket = sockets[i];
					}
				}
			}

			if (socket != nullptr) {
				socket->HandleUserDatagramProtocolMessage(internetProtocolPayload + sizeof(UserDatagramProtocolHeader), size - sizeof(UserDatagramProtocolHeader));
			}
			return false;
		}

		UserDatagramProtocolSocket* UserDatagramProtocolProvider::Connect(uint32_t ip, uint16_t port) {
			UserDatagramProtocolSocket* socket = (UserDatagramProtocolSocket*)kernel::MemoryManager::activeMemoryManager->malloc(sizeof(UserDatagramProtocolSocket));
			if (socket != nullptr) {
				socket = new(socket)UserDatagramProtocolSocket(this);
				socket->remoteIP = ip;
				socket->remotePort = port;
				socket->localIP = backend->GetIPAddress();
				socket->localPort = freePort;

				socket->remotePort = ((socket->remotePort & 0xff00) >> 8 | ((socket->remotePort & 0x00ff) << 8));
				socket->localPort = ((socket->localPort & 0xff00) >> 8 | ((socket->localPort & 0x00ff) << 8));
				sockets[numSockets++] = socket;
			}
			return socket;
		}

		UserDatagramProtocolSocket* UserDatagramProtocolProvider::Listen(uint16_t port) {
			UserDatagramProtocolSocket* socket = (UserDatagramProtocolSocket*)kernel::MemoryManager::activeMemoryManager->malloc(sizeof(UserDatagramProtocolSocket));
			if (socket != nullptr) {
				socket = new(socket)UserDatagramProtocolSocket(this);

				socket->listening = true;
				socket->localPort = port;
				socket->localIP = backend->GetIPAddress();
			}
			return socket;
		}

		void UserDatagramProtocolProvider::Disconnect(UserDatagramProtocolSocket* socket) {
			for (int i = 0; i < numSockets && socket == nullptr; i++) {
				if (sockets[i] == socket) {
					sockets[i] = sockets[--numSockets];
					kernel::MemoryManager::activeMemoryManager->free(socket);
					break;
				}
			}
		}

		void UserDatagramProtocolProvider::Send(UserDatagramProtocolSocket* socket, uint8_t* data, uint32_t size) {
			uint16_t totalLength = size + sizeof(UserDatagramProtocolHeader);
			uint8_t* buf = (uint8_t*)kernel::MemoryManager::activeMemoryManager->malloc(totalLength);
			uint8_t* buf2 = buf + sizeof(UserDatagramProtocolHeader);

			UserDatagramProtocolHeader* msg = (UserDatagramProtocolHeader*)buf;
			msg->srcPort = socket->localPort;
			msg->dstPort = socket->remotePort;
			msg->length = ((totalLength & 0xff00) >> 8 | (totalLength & 0x00ff) << 8);

			for (int i = 0; i < size; i++) {
				buf2[i] = data[i];
			}

			msg->checkSum = 0;
			InternetProtocolHandler::Send(socket->remoteIP, buf, totalLength);

			kernel::MemoryManager::activeMemoryManager->free(buf);
		}

		void UserDatagramProtocolProvider::Bind(UserDatagramProtocolSocket* socket, UserDatagramProtocolHandler* handler) {
			socket->handler = handler;
		}
	}
}