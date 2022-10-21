#ifndef MYOS_ARP_H
#define MYOS_ARP_H
#include "types.h"
#include "net/etherframe.h"
namespace myos {
	namespace net {
		struct AddressResolutionProtocolMessage {
			uint16_t hardwareType;
			uint16_t protocal;
			uint8_t hardwareAddresSize;
			uint8_t protocolAddressSize;
			uint16_t command;

			uint64_t srcMAC : 48;
			uint64_t srcIP;
			uint64_t dstMAC : 48;
			uint64_t dstIP;
		}__attribute__((packed));

		class AddressResolutionProtocol :public EtherFrameHandler {
		public:
			AddressResolutionProtocol(EtherFrameProvider* backend);
			~AddressResolutionProtocol();

			bool OnEtherFrameReceived(uint8_t* etherFramePayload, uint32_t size) override;
			
			void RequestMACAddress(uint32_t IP_BE);
			uint64_t Resolve(uint32_t IP_BE);
			uint64_t GetMACFromCache(uint64_t IP_BE);


		private:
			uint32_t IPcache[128];
			uint64_t MACcache[128];
			int numCacheEntries;
		};
	}
}
#endif // !MYOS_ARP_H
