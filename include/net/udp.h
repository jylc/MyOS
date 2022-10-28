#ifndef MYOS_UDP_H
#define MYOS_DP_H
#include "types.h"
#include "net/icmp.h"
namespace myos {
	namespace net {
		struct UserDatagramProtocolHeader {
			uint16_t srcPort;
			uint16_t dstPort;
			uint16_t length;
			uint16_t checkSum;
		}__attribute__((packed));

		class UserDatagramProtocolProvider;
		class UserDatagramProtocolSocket;


		class UserDatagramProtocolHandler {
		public:
			UserDatagramProtocolHandler();
			~UserDatagramProtocolHandler();

			virtual void HandleUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket, uint8_t* data, uint32_t size);
		};


		class UserDatagramProtocolSocket {
			friend class UserDatagramProtocolProvider;
		public:
			UserDatagramProtocolSocket(UserDatagramProtocolProvider* backend);
			~UserDatagramProtocolSocket();

			virtual void HandleUserDatagramProtocolMessage(uint8_t* data, uint32_t size);
			virtual void Send(uint8_t* data, uint32_t size);
			virtual void Disconnect();

		protected:
			uint16_t remotePort;
			uint16_t remoteIP;
			uint16_t localPort;
			uint16_t localIP;

			bool listening;
			// 多态还是包含？
			UserDatagramProtocolHandler* handler;
			UserDatagramProtocolProvider* backend;
		};

		class UserDatagramProtocolProvider :public InternetProtocolHandler {
		public:
			UserDatagramProtocolProvider(InternetProtocolProvider*backend);
			~UserDatagramProtocolProvider();

			virtual bool OnInternetProtocolRecevied(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size);
			virtual UserDatagramProtocolSocket* Connect(uint32_t ip, uint16_t port);
			virtual UserDatagramProtocolSocket* Listen(uint16_t port);
			virtual void Disconnect(UserDatagramProtocolSocket* socket);
			virtual void Send(UserDatagramProtocolSocket* socket, uint8_t* data, uint32_t size);
			virtual void Bind(UserDatagramProtocolSocket* socket, UserDatagramProtocolHandler* handler);

		protected:
			UserDatagramProtocolSocket* sockets[65535];
			uint16_t numSockets;
			uint16_t freePort;
		};
	}
}

#endif // !MYOS_UDP_H
