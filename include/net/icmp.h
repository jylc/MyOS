#ifndef MYOS_ICMP_H
#define MYOS_ICMP_H
#include "amd_am79c973.h"
#include "types.h"
#include "ipv4.h"
namespace myos {
	namespace net {
		struct InternetControlMessageProtocolMessage {
			uint8_t type;
			uint8_t code;
			uint16_t checkSum;
			uint32_t data;
		}__attribute__((packed));

		class InternetControlMessageProtocol :public InternetProtocolHandler {
		public:
			InternetControlMessageProtocol(InternetProtocolProvider* backend);
			~InternetControlMessageProtocol();

			bool OnInternetProtocolRecevied(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size)override;

			void RequestEchoReply(uint32_t ip_be);
		};
	}
}
#endif // !MYOS_ICMP_H
