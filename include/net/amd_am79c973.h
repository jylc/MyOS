#ifndef MYOS_AMD_AM79C973_H
#define MYOS_AMD_AM79C973_H
#include "types.h"
#include "pci.h"
#include "port.h"

namespace myos {
	namespace net {
		class AmdAm78c973;

		class RawDataHandler {
		public:
			RawDataHandler(AmdAm78c973* backend);
			~RawDataHandler();

			bool OnRawDataReceived(uint8_t* buffer, uint32_t size);
			void Send(uint8_t* buffer, uint32_t size);
		protected:
			AmdAm78c973* backend;
		};


		class AmdAm78c973 :public kernel::Driver, kernel::InterruptHandler {
		public:
			AmdAm78c973(kernel::PeripheralComponentInterconnectDriverDescriptor* dev, kernel::InterruptManager* interrupts);
			~AmdAm78c973();

			void Activate()override;
			int Reset()override;
			uint32_t HandlerInterrupt(uint32_t esp)override;

			void Send(uint8_t* buffer, int size);
			void Receive();

			void SetHandler(RawDataHandler* rawDataHandler);

			uint64_t GetMacAddress();
		private:
			struct InitializationBlock {
				uint16_t mode;
				unsigned reserved1 : 4;
				unsigned numSendBuffers : 4;
				unsigned reserved2 : 4;
				unsigned numRecvBuffers : 4;
				uint64_t physicalAddress : 48;
				uint16_t reserved3;
				uint64_t logicalAddress;
				uint32_t recvBufferDescAddress;
				uint32_t sendBufferDescAddress;
			}__attribute__((packed));


			struct BufferDescriptor
			{
				uint32_t address;
				uint32_t flags;
				uint32_t flags2;
				uint32_t avail;
			} __attribute__((packed));

			kernel::Port16Bit MACAddress0Port;
			kernel::Port16Bit MACAddress2Port;
			kernel::Port16Bit MACAddress4Port;
			kernel::Port16Bit registerDataPort;
			kernel::Port16Bit registerAddressPort;
			kernel::Port16Bit resetPort;
			kernel::Port16Bit busConstolRegisterDataPort;

			InitializationBlock initBlock;
			BufferDescriptor* sendBufferDesc;
			// TODO ÎªÊ²Ã´ÊÇ2048 + 15
			uint8_t sendBufferDescMemory[2048 + 15];
			uint8_t sendBuffers[8][2049 + 15];
			uint8_t currentSendBuffer;

			BufferDescriptor* recvBufferDesc;
			uint8_t recvBufferDescMemory[2048 + 15];
			uint8_t recvBuffers[8][2048 + 15];
			uint8_t currentRecvBuffer;


			RawDataHandler* handler;
		};

	}
}

#endif // !MYOS_AMD_AM79C973_H
