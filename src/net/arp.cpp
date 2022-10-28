#include "net/arp.h"
#include "tools/print.h"
namespace myos {
	namespace net {
		AddressResolutionProtocol::AddressResolutionProtocol(EtherFrameProvider* backend) :
			EtherFrameHandler(backend, 0x806) {
			numCacheEntries = 0;
		}

		AddressResolutionProtocol::~AddressResolutionProtocol() {

		}


		bool AddressResolutionProtocol::OnEtherFrameReceived(uint8_t* etherFramePayload, uint32_t size) {
			if (size < sizeof(AddressResolutionProtocolMessage)) {
				return false;
			}
			AddressResolutionProtocolMessage* arp = (AddressResolutionProtocolMessage*)etherFramePayload;
			if (arp->hardwareType == 0x0100) {
				if (arp->protocol == 0x0008 &&
					arp->hardwareAddresSize == 6 &&
					arp->protocolAddressSize == 4 &&
					arp->dstIP == backend->GetIPAddress()) {
					switch (arp->command) {
					case 0x0100: // request
						arp->command = 0x0200;
						arp->dstIP = arp->srcIP;
						arp->dstMAC = arp->srcMAC;
						arp->srcIP = backend->GetIPAddress();
						arp->srcMAC = backend->GetMACAddress();
						return true;
					case 0x0200: // response
						if (numCacheEntries < 128) {
							IPcache[numCacheEntries] = arp->srcIP;
							MACcache[numCacheEntries] = arp->srcMAC;
							numCacheEntries++;
						}
						break;
					}
				}
			}
			return false;
		}

		void AddressResolutionProtocol::RequestMACAddress(uint32_t IP_BE) {
			AddressResolutionProtocolMessage arp;
			arp.hardwareType = 0x0100;
			arp.protocol = 0x0008;
			arp.hardwareAddresSize = 6;
			arp.protocolAddressSize = 4;
			arp.command = 0x0100;
			arp.srcMAC = backend->GetMACAddress();
			arp.srcIP = backend->GetIPAddress();
			arp.dstMAC = 0xffffffffffff;
			// 非常奇怪的问题，uint32_t的IP_BE不能直接赋值给uint64_t的dstIP
			arp.dstIP = IP_BE;
			this->Send(arp.dstMAC, (uint8_t*)&arp, sizeof(AddressResolutionProtocolMessage));
		}

		uint64_t AddressResolutionProtocol::Resolve(uint32_t IP_BE) {
			uint64_t result = GetMACFromCache(IP_BE);
			if (result == 0xffffffffffff) {
				RequestMACAddress(IP_BE);
			}
			while (result == 0xffffffffffff) {
				result = GetMACFromCache(IP_BE);
				break;
			}
			return result;
		}

		uint64_t AddressResolutionProtocol::GetMACFromCache(uint64_t IP_BE) {
			for (int i = 0; i < numCacheEntries; i++) {
				if (IPcache[i] == IP_BE) {
					return MACcache[i];
				}
			}
			return 0xffffffffffff;
		}

		void AddressResolutionProtocol::BroadcastMACAddress(uint32_t IP_BE) {
			AddressResolutionProtocolMessage arp;
			arp.hardwareType = 0x0100;
			arp.protocol = 0x0008;
			arp.hardwareAddresSize = 6;
			arp.protocolAddressSize = 4;
			arp.command = 0x0200;
			arp.srcMAC = backend->GetMACAddress();
			arp.srcIP = backend->GetIPAddress();
			arp.dstMAC = Resolve(IP_BE);
			// 真的很奇怪，不能直接强转型
			arp.dstIP = IP_BE;
			this->Send(arp.dstMAC, (uint8_t*)&arp, sizeof(AddressResolutionProtocolMessage));
		}

	}
}