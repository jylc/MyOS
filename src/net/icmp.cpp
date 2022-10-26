#include "net/icmp.h"
#include "tools/print.h"
namespace myos {
	namespace net {
		InternetControlMessageProtocol::InternetControlMessageProtocol(InternetProtocolProvider* backend):
		InternetProtocolHandler(backend,0x01) {

		}

		bool InternetControlMessageProtocol::OnInternetProtocolRecevied(uint32_t srcIP_BE, uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size) {
			if (size < sizeof(InternetControlMessageProtocolMessage)) {
				return false;
			}
			InternetControlMessageProtocolMessage* msg = (InternetControlMessageProtocolMessage*)internetProtocolPayload;
			
			switch (msg->type)
			{
			case 0:
				tools::printf("ping response from %x\n",srcIP_BE);
				break;
			case 8:
				msg->type = 0;
				msg->checkSum = InternetProtocolProvider::CheckSum((uint16_t*)msg, sizeof(InternetControlMessageProtocolMessage));
				return true;
			}
			return false;
		}

		void InternetControlMessageProtocol::RequestEchoReply(uint32_t ip_be) {
			InternetControlMessageProtocolMessage icmp;
			icmp.type = 0;
			icmp.code = 0;
			icmp.data = 0x3713;
			icmp.checkSum = InternetProtocolProvider::CheckSum((uint16_t*)&icmp, sizeof(InternetControlMessageProtocolMessage));

			InternetProtocolHandler::Send(ip_be,(uint8_t*)&icmp,sizeof(InternetControlMessageProtocolMessage));

		}
	}
}