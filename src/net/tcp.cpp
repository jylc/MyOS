#include "net/tcp.h"
#include "memorymanagement.h"
#include "tools/print.h"
namespace myos {
	namespace net {

		TransmissionControlProtocolHandler::TransmissionControlProtocolHandler() {}

		TransmissionControlProtocolHandler::~TransmissionControlProtocolHandler() {}

		bool TransmissionControlProtocolHandler::HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket, uint8_t* data, uint32_t size) {
			return true;
		}

		TransmissionControlProtocolSocket::TransmissionControlProtocolSocket(TransmissionControlProtocolProvider* backend) {
			this->backend = backend;
			handler = nullptr;
			state = CLOSED;
		}

		TransmissionControlProtocolSocket::~TransmissionControlProtocolSocket() {}

		bool TransmissionControlProtocolSocket::HandleTransmissionControlProtocolMessage(uint8_t* data, uint32_t size) {
			if (handler != nullptr) {
				handler->HandleTransmissionControlProtocolMessage(this, data, size);
			}
			return false;
		}

		void TransmissionControlProtocolSocket::Send(uint8_t* data, uint32_t size) {
			while (state != ESTABLISHED)
			{
			}
			backend->Send(this, data, size, PSH | ACK);
		}

		void TransmissionControlProtocolSocket::Disconnect() {
			backend->Disconnect(this);
		}


		TransmissionControlProtocolProvider::TransmissionControlProtocolProvider(InternetProtocolProvider* backend) :
			InternetProtocolHandler(backend, 0x06) {
			for (int i = 0; i < 65535; i++) {
				sockets[i] = nullptr;
			}
			numSockets = 0;
			freePort = 1024;
		}

		TransmissionControlProtocolProvider::~TransmissionControlProtocolProvider() {}

		uint32_t bigEndian32(uint32_t x) {
			return ((x & 0xFF000000) >> 24)
				| ((x & 0x00FF0000) >> 8)
				| ((x & 0x0000FF00) << 8)
				| ((x & 0x000000FF) << 24);
		}

		void ShowState(TransmissionControlProtocolSocketState state) {
			switch (state) {
			case CLOSED:
				tools::printf("CLOSED\n");
				break;
			case LISTEN:
				tools::printf("LISTEN\n");
				break;
			case SYN_SENT:
				tools::printf("SYN_SENT\n");
				break;
			case SYN_RECEIVED:
				tools::printf("SYN_RECEIVED\n");
				break;
			case ESTABLISHED:
				tools::printf("ESTABLISHED\n");
				break;
			case FIN_WAIT1:
				tools::printf("FIN_WAIT1\n");
				break;
			case FIN_WAIT2:
				tools::printf("FIN_WAIT2\n");
				break;
			case CLOSING:
				tools::printf("CLOSING\n");
				break;
			case TIME_WAIT:
				tools::printf("TIME_WAIT\n");
				break;
			case CLOSE_WAIT:
				tools::printf("CLOSE_WAIT\n");
				break;
			default:
				tools::printf("DEFAULT: %x\n", state);
			}
		}

		// 处理TCP数据
		bool TransmissionControlProtocolProvider::OnInternetProtocolRecevied(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size) {
			if (size < 20) {
				return false;
			}
			TransmissionControlProtocolHeader* msg = (TransmissionControlProtocolHeader*)internetProtocolPayload;
			uint16_t localPort = msg->dstPort;
			uint16_t remotePort = msg->srcPort;

			TransmissionControlProtocolSocket* socket = nullptr;
			for (int i = 0; i < numSockets && socket == nullptr; i++) {
				if (sockets[i]->localPort == msg->dstPort &&
					sockets[i]->localIP == dstIP_BE &&
					sockets[i]->state == LISTEN &&
					(((msg->flags) & (SYN | ACK)) == SYN)) {
					socket = sockets[i];
				}
				else if (sockets[i]->localPort == msg->dstPort &&
					sockets[i]->localIP == dstIP_BE &&
					sockets[i]->remotePort == msg->srcPort &&
					sockets[i]->remoteIP == srcIP_BE) {
					socket = sockets[i];
				}
			}
			bool reset = false;

			tools::printf("[TransmissionControlProtocolProvider::OnInternetProtocolRecevied] Line84\n");
			tools::printf("flags: %x", msg->flags);
			tools::printf("socket:");
			if (socket == nullptr) {
				tools::printf("is null");
			}
			else {
				ShowState(socket->state);
			}

			tools::printf("B-localhost:%x B-localport:%x B-remotehost:%x B-remoteport:%x\n", dstIP_BE, msg->dstPort, srcIP_BE, msg->srcPort);

			if (socket != nullptr && msg->flags & RST) {
				socket->state = CLOSED;
			}

			if (socket != nullptr && socket->state != CLOSED) {
				switch ((msg->flags) & (SYN | ACK | FIN))
				{
				case SYN:
					if (socket->state == LISTEN) {
						socket->state = SYN_RECEIVED;
						socket->remoteIP = srcIP_BE;
						socket->remotePort = msg->srcPort;
						socket->acknowledgementNumber = bigEndian32(msg->sequenceNumber) + 1;
						socket->sequenceNumber = 0xbeefcafe;
						Send(socket, 0, 0, SYN | ACK);
						socket->sequenceNumber++;
					}
					else {
						reset = true;
					}
					break;
				case SYN | ACK:
					if (socket->state == SYN_SENT) {
						socket->state = ESTABLISHED;
						socket->acknowledgementNumber = bigEndian32(msg->sequenceNumber) + 1;
						socket->sequenceNumber++;
						Send(socket, 0, 0, ACK);
					}
					else {
						reset = true;
					}
					break;
				case SYN | FIN:
				case SYN | FIN | ACK:
					reset = true;
					break;
				case FIN:
				case FIN | ACK:
					if (socket->state == ESTABLISHED) {
						socket->state = CLOSE_WAIT;
						socket->acknowledgementNumber++;
						Send(socket, 0, 0, ACK);
						Send(socket, 0, 0, FIN | ACK);
					}
					else if (socket->state == CLOSE_WAIT) {
						socket->state = CLOSED;
					}
					else if (socket->state == FIN_WAIT1 ||
						socket->state == FIN_WAIT2) {
						socket->state = CLOSED;
						socket->acknowledgementNumber++;
						Send(socket, 0, 0, ACK);
					}
					else {
						reset = true;
					}
					break;
				case ACK:
					if (socket->state == SYN_RECEIVED) {
						socket->state = ESTABLISHED;
						return false;
					}
					else if (socket->state == FIN_WAIT1) {
						socket->state = FIN_WAIT2;
						return false;
					}
					else if (socket->state == CLOSE_WAIT) {
						socket->state = CLOSED;
						return false;
					}
					if (msg->flags == ACK)break;
				default:
					if (socket->acknowledgementNumber == bigEndian32(msg->sequenceNumber)) {
						reset = !(socket->HandleTransmissionControlProtocolMessage(internetProtocolPayload + msg->headerSize32 * 4, size - msg->headerSize32 * 4));
						if (!reset) {
							int x = 0;
							for (int i = msg->headerSize32 * 4; i < size; i++)
								if (internetProtocolPayload[i] != 0)
									x = i;
							socket->acknowledgementNumber += x - msg->headerSize32 * 4 + 1;
							Send(socket, 0, 0, ACK);
						}
					}
					else {
						reset = true;
					}
				}
			}

			if (reset)
			{
				if (socket != nullptr)
				{
					Send(socket, 0, 0, RST);
				}
				else
				{
					TransmissionControlProtocolSocket socket(this);
					socket.remotePort = msg->srcPort;
					socket.remoteIP = srcIP_BE;
					socket.localPort = msg->dstPort;
					socket.localIP = dstIP_BE;
					socket.sequenceNumber = bigEndian32(msg->acknowledgementNumber);
					socket.acknowledgementNumber = bigEndian32(msg->sequenceNumber) + 1;
					Send(&socket, 0, 0, RST);
				}
			}

			if (socket != nullptr && socket->state == CLOSED)
				for (uint16_t i = 0; i < numSockets && socket == 0; i++)
					if (sockets[i] == socket)
					{
						sockets[i] = sockets[--numSockets];
						kernel::MemoryManager::activeMemoryManager->free(socket);
						break;
					}

			return false;
		}

		TransmissionControlProtocolSocket* TransmissionControlProtocolProvider::Connect(uint32_t ip, uint16_t port) {
			TransmissionControlProtocolSocket* socket = (TransmissionControlProtocolSocket*)kernel::MemoryManager::activeMemoryManager->malloc(sizeof(TransmissionControlProtocolSocket));
			if (socket != nullptr) {
				new(socket)TransmissionControlProtocolSocket(this);

				socket->remotePort = port;
				socket->remoteIP = ip;
				socket->localPort = freePort++;
				socket->localIP = backend->GetIPAddress();

				socket->remotePort = ((socket->remotePort & 0xff00) >> 8) | ((socket->remotePort & 0x00ff) << 8);
				socket->localPort = ((socket->localPort & 0xff00) >> 8) | ((socket->localPort & 0x00ff) << 8);
				sockets[numSockets++] = socket;
				socket->state = SYN_SENT;
				socket->sequenceNumber = 0xbeefcafe;
				socket->acknowledgementNumber = 0;
				Send(socket, 0, 0, SYN);
			}
			return socket;
		}

		TransmissionControlProtocolSocket* TransmissionControlProtocolProvider::Listen(uint16_t port) {
			TransmissionControlProtocolSocket* socket = (TransmissionControlProtocolSocket*)kernel::MemoryManager::activeMemoryManager->malloc(sizeof(
				TransmissionControlProtocolSocket));
			if (socket != nullptr) {
				new(socket)TransmissionControlProtocolSocket(this);

				socket->state = LISTEN;
				socket->localPort = ((port & 0xFF00) >> 8) | ((port & 0x00FF) << 8);
				socket->localIP = backend->GetIPAddress();
				sockets[numSockets++] = socket;
			}
		}
		void TransmissionControlProtocolProvider::Disconnect(TransmissionControlProtocolSocket* socket) {
			socket->state = FIN_WAIT1;
			Send(socket, 0, 0, FIN + ACK);
			socket->sequenceNumber++;
		}

		void TransmissionControlProtocolProvider::Send(TransmissionControlProtocolSocket* socket, uint8_t* data, uint32_t size, uint16_t flags) {
			uint16_t totalLength = size + sizeof(TransmissionControlProtocolHeader);
			uint16_t lengthIncIPHdr = totalLength + sizeof(TransmissionControllProtocolPseudoHeader);
			uint8_t* buf = (uint8_t*)kernel::MemoryManager::activeMemoryManager->malloc(lengthIncIPHdr);

			TransmissionControllProtocolPseudoHeader* phdr = (TransmissionControllProtocolPseudoHeader*)buf;
			TransmissionControlProtocolHeader* msg = (TransmissionControlProtocolHeader*)(buf + sizeof(TransmissionControllProtocolPseudoHeader));
			uint8_t* buf2 = buf + sizeof(TransmissionControlProtocolHeader) + sizeof(TransmissionControllProtocolPseudoHeader);

			msg->headerSize32 = sizeof(TransmissionControlProtocolHeader) / 4;
			msg->srcPort = socket->localPort;
			msg->dstPort = socket->remotePort;
			msg->acknowledgementNumber = bigEndian32(socket->acknowledgementNumber);
			msg->sequenceNumber = bigEndian32(socket->sequenceNumber);
			msg->reserved = 0;
			msg->flags = flags;
			msg->windowSize = 0xffff;
			msg->urgentPrt = 0;

			msg->options = ((flags & SYN) != 0) ? 0xb4050402 : 0;

			socket->sequenceNumber += size;

			for (int i = 0; i < size; i++) {
				buf2[i] = data[i];
			}


			phdr->srcIP = socket->localIP;
			phdr->dstIP = socket->remoteIP;
			phdr->protocol = 0x0600;
			phdr->totalLength = (((totalLength & 0x00ff) << 8) | ((totalLength & 0xff00) >> 8));
			msg->checkSum = 0;
			msg->checkSum = InternetProtocolProvider::CheckSum((uint16_t*)buf, lengthIncIPHdr);
			InternetProtocolHandler::Send(socket->remoteIP, (uint8_t*)msg, totalLength);
			kernel::MemoryManager::activeMemoryManager->free(buf);
		}

		void TransmissionControlProtocolProvider::Bind(TransmissionControlProtocolSocket* socket, TransmissionControlProtocolHandler* handler) {
			socket->handler = handler;
		}
	}
}