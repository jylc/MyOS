#ifndef MYOS_PIV4_H
#define MYOS_IPV4_H

#include "common/types.h"
#include "arp.h"
#include "etherframe.h"

namespace myos {
	namespace net {
		struct InternetProtocolV4Message {
			uint8_t version : 4;
			uint8_t headerLength : 4;
			uint8_t tos;
			uint16_t totalLength;

			uint16_t ident;
			uint16_t flagsAndOffset;

			uint8_t timeToLive;
			uint8_t protocol;
			uint16_t checkSum;
			
			uint32_t srcIP;

			uint32_t dstIP;
		}__attribute__((packed));


		class InternetProtocolProvider;
		class InternetProtocolHandler {
		public:
			InternetProtocolHandler(InternetProtocolProvider*backend,uint8_t protocol);
			~InternetProtocolHandler();

			virtual bool OnInternetProtocolRecevied(uint32_t srcIP_BE,uint32_t dstIP_BE,uint8_t* internetProtocolPayload,uint32_t size);
			void Send(uint32_t dstIP_BE, uint8_t* internetProtocolPayload, uint32_t size);

		protected:
			InternetProtocolProvider* backend;
			uint8_t protocol;
		};

		class InternetProtocolProvider :public EtherFrameHandler {
			friend class InternetProtocolHandler;
		public:
			InternetProtocolProvider(EtherFrameProvider* backend,
				AddressResolutionProtocol* arp,
				uint32_t gatewayIP,
				uint32_t subnetMask);

			~InternetProtocolProvider();

			bool OnEtherFrameRecevied(uint8_t* buffer, uint32_t size);
			void Send(uint32_t dstIP_BE, uint8_t protol, uint8_t* data, uint32_t size);

			static uint16_t CheckSum(uint16_t* data, uint32_t size);

		protected:
			InternetProtocolHandler* handlers[255];
			AddressResolutionProtocol* arp;
			uint32_t gatewayIP;
			uint32_t subnetMask;
		};

	}
}

#endif // !MYOS_PIV4_H
