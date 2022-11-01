#ifndef MYOS_TCP_H
#define MYOS_TCP_H
#include "types.h"
#include "net/ipv4.h"
namespace myos {
	namespace net {
		enum TransmissionControlProtocolSocketState {
			CLOSED,
			LISTEN,
			SYN_SENT,
			SYN_RECEIVED,
			ESTABLISHED,
			FIN_WAIT1,
			FIN_WAIT2,
			CLOSING,
			TIME_WAIT,
			CLOSE_WAIT,
			//LAST_ACK,
		};

		enum TransmissionControlProtocolFlag {
			FIN = 1,
			SYN = 2,
			RST = 4,
			PSH = 8,
			ACK = 16,
			URG = 32,
			ECE = 64,
			CWR = 128,
			NS = 256,
		};

		struct TransmissionControlProtocolHeader {
			uint16_t srcPort;
			uint16_t dstPort;
			uint32_t sequenceNumber;
			uint32_t acknowledgementNumber;

			uint8_t reserved : 4;
			uint8_t headerSize32 : 4;
			uint8_t flags;

			uint16_t windowSize;
			uint16_t checkSum;
			uint16_t urgentPrt;
			uint32_t options;
		}__attribute__((packed));

		struct TransmissionControllProtocolPseudoHeader {
			uint32_t srcIP;
			uint32_t dstIP;
			uint16_t protocol;
			uint16_t totalLength;
		}__attribute__((packed));

		class TransmissionControlProtocolSocket;
		class TransmissionControlProtocolProvider;
		class TransmissionControlProtocolHandler {
		public:
			TransmissionControlProtocolHandler();
			~TransmissionControlProtocolHandler();
			virtual bool HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket, uint8_t* data, uint32_t size);
		};

		class TransmissionControlProtocolSocket {
			friend class TransmissionControlProtocolProvider;
		public:
			TransmissionControlProtocolSocket(TransmissionControlProtocolProvider* backend);
			~TransmissionControlProtocolSocket();

			virtual bool HandleTransmissionControlProtocolMessage(uint8_t* data, uint32_t size);
			virtual void Send(uint8_t* data, uint32_t size);
			virtual void Disconnect();
		protected:
			uint16_t remotePort;
			uint32_t remoteIP;
			uint16_t localPort;
			uint32_t localIP;
			uint32_t sequenceNumber;
			uint32_t acknowledgementNumber;

			TransmissionControlProtocolProvider* backend;
			TransmissionControlProtocolHandler* handler;

			TransmissionControlProtocolSocketState state;
		};

		class TransmissionControlProtocolProvider :public InternetProtocolHandler {
		public:
			TransmissionControlProtocolProvider(InternetProtocolProvider* backend);
			~TransmissionControlProtocolProvider();

			virtual bool OnInternetProtocolRecevied(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size)override;
			virtual TransmissionControlProtocolSocket* Connect(uint32_t ip, uint16_t port);
			virtual TransmissionControlProtocolSocket* Listen(uint16_t port);
			virtual void Disconnect(TransmissionControlProtocolSocket* socket);
			virtual void Send(TransmissionControlProtocolSocket* socket, uint8_t* data, uint32_t size, uint16_t flags = 0);
			virtual void Bind(TransmissionControlProtocolSocket* socket, TransmissionControlProtocolHandler* handler);

		protected:
			TransmissionControlProtocolSocket* sockets[65535];
			uint16_t numSockets;
			uint16_t freePort;
		};
	}
}

#endif // !MYOS_TCP_H
