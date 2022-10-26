#ifndef MYOS_ETHERFRAME_H
#define MYOS_ETHERFRAME_H
#include "types.h"
#include "port.h"
#include "amd_am79c973.h"
namespace myos {
	namespace net {
		struct EtherFrameHeader {
			uint64_t dstMAC_BE : 48;
			uint64_t srcMAC_BE : 48;
			uint16_t etherType_BE;
		}__attribute__((packed));

		typedef uint32_t EtherFrameFooter;

		class EtherFrameProvider;
		class EtherFrameHandler {
		public:
			EtherFrameHandler(EtherFrameProvider* backend, uint16_t etherType);
			~EtherFrameHandler();

			virtual bool OnEtherFrameReceived(uint8_t* etherFramePayload, uint32_t size);
			void Send(uint64_t dstMAC_BE, uint8_t* etherFramePayload, uint32_t size);

		protected:
			EtherFrameProvider* backend;
			uint16_t etherType_BE;
		};

		class EtherFrameProvider :public RawDataHandler {
			friend class EtherFrameHandler;
		public:
			EtherFrameProvider(AmdAm78c973* backend);
			~EtherFrameProvider();

			bool OnRawDataReceived(uint8_t* buffer, uint32_t size);
			void Send(uint64_t dstMAC_BE,uint16_t etherType_BE,uint8_t*buffer,uint32_t size);

			uint64_t GetMACAddress();
			uint32_t GetIPAddress();
		private:
			EtherFrameHandler* handlers[65535];
		};

	}
}

#endif // !MYOS_ETHERFRAME_H
