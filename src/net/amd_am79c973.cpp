#include "net/amd_am79c973.h"
#include "print.h"
namespace myos {
	namespace net {
		RawDataHandler::RawDataHandler(AmdAm78c973* backend) {
			this->backend = backend;
			backend->SetHandler(this);
		}

		RawDataHandler::~RawDataHandler() {
			backend->SetHandler(nullptr);
		}

		bool RawDataHandler::OnRawDataReceived(uint8_t* buffer, uint32_t size) {
			return false;
		}

		void RawDataHandler::Send(uint8_t* buffer, uint32_t size) {
			backend->Send(buffer, size);
		}

		AmdAm78c973::AmdAm78c973(kernel::PeripheralComponentInterconnectDriverDescriptor* dev, kernel::InterruptManager* interrupts) :
			Driver(),
			InterruptHandler(dev->interrupt + interrupts->HardwareInterruptOffset(), interrupts),
			MACAddress0Port(dev->portBase),
			MACAddress2Port(dev->portBase + 0x02),
			MACAddress4Port(dev->portBase + 0x04),
			registerDataPort(dev->portBase + 0x10),
			registerAddressPort(dev->portBase + 0x12),
			resetPort(dev->portBase + 0x14),
			busConstrolRegisterDataPort(dev->portBase + 0x16)
		{
			tools::printf("AmdAm78c973 init success\n");

			handler = nullptr;
			currentSendBuffer = 0;
			currentRecvBuffer = 0;

			uint64_t MAC0 = MACAddress0Port.read() % 256;
			uint64_t MAC1 = MACAddress0Port.read() / 256;
			uint64_t MAC2 = MACAddress2Port.read() % 256;
			uint64_t MAC3 = MACAddress2Port.read() / 256;
			uint64_t MAC4 = MACAddress4Port.read() % 256;
			uint64_t MAC5 = MACAddress4Port.read() / 256;

			uint64_t MAC = MAC5 << 40 | MAC4 << 32 | MAC3 << 24 | MAC2 << 16 | MAC1 << 8 | MAC0;

			registerAddressPort.write(20);
			busConstrolRegisterDataPort.write(0x102);

			registerAddressPort.write(0);
			registerDataPort.write(0x04);

			initBlock.mode = 0;
			initBlock.reserved1 = 0;
			initBlock.numSendBuffers = 3;
			initBlock.reserved2 = 0;
			initBlock.numRecvBuffers = 3;
			initBlock.physicalAddress = MAC;
			initBlock.reserved3 = 0;
			initBlock.logicalAddress = 0;

			sendBufferDesc = (BufferDescriptor*)(((uint32_t)&sendBufferDescMemory[0] + 15) & 0xfff0);
			initBlock.sendBufferDescAddress = (uint32_t)sendBufferDesc;
			recvBufferDesc = (BufferDescriptor*)(((uint32_t)&recvBufferDescMemory[0] + 15) & 0xfff0);
			initBlock.recvBufferDescAddress = (uint32_t)recvBufferDesc;

			for (uint8_t i = 0; i < 8; i++) {
				sendBufferDesc[i].address = (((uint32_t)&sendBuffers[i] + 15) & 0xfff0);
				sendBufferDesc[i].flags = 0xf7ff;
				sendBufferDesc[i].flags2 = 0;
				sendBufferDesc[i].avail = 0;

				recvBufferDesc[i].address = (((uint32_t)&recvBuffers[i] + 15) & 0xfff0);
				recvBufferDesc[i].flags = 0xf7ff | 0x80000000;
				recvBufferDesc[i].flags2 = 0;
				recvBufferDesc[i].avail = 0;
			}

			registerAddressPort.write(1);
			registerDataPort.write((uint32_t)&initBlock);
			registerAddressPort.write(2);
			registerDataPort.write((uint32_t)&initBlock >> 16);
		}

		AmdAm78c973::~AmdAm78c973() {

		}

		void AmdAm78c973::Activate() {
			registerAddressPort.write(0);
			registerDataPort.write(0x41);

			registerAddressPort.write(4);
			uint32_t tmp = registerDataPort.read();
			registerAddressPort.write(4);
			registerDataPort.write(tmp | 0xC00);

			registerAddressPort.write(0);
			registerDataPort.write(0x42);
		}

		int AmdAm78c973::Reset() {
			resetPort.read();
			resetPort.write(0);
			return 10;
		}

		uint32_t AmdAm78c973::HandlerInterrupt(uint32_t esp) {
			// 此处添加了输出语句后会导致registerDataPort.read()读取数据不同，0x06fb时间延迟！
			registerAddressPort.write(0);
			uint16_t tmp = registerDataPort.read();
			if ((tmp & 0x8000) == 0x8000) tools::printf("AMD AM79C973 ERROR\n");
			else if ((tmp & 0x2000) == 0x2000) tools::printf("AMD AM79C973 Collision Error\n");
			else if ((tmp & 0x1000) == 0x1000) tools::printf("AMD AM79C973 Missed Frame\n");
			else if ((tmp & 0x0800) == 0x0800) tools::printf("AMD AM79C973 Memory Error\n");
			else if ((tmp & 0x0400) == 0x0400) Receive();
			else if ((tmp & 0x0200) == 0x0200) tools::printf("SEND\n");

			registerAddressPort.write(0);
			registerDataPort.write(tmp);

			if ((tmp & 0x0100) == 0x0100) tools::printf("AMD AM79C973 INIT DONE\n");
			return esp;
		}

		void AmdAm78c973::Send(uint8_t* buffer, int size) {
			int sendDesc = currentSendBuffer;
			currentSendBuffer = (currentSendBuffer + 1) % 8;
			// 1518 以太网的帧
			if (size > 1518) {
				size = 1518;
			}
			for (uint8_t* src = buffer + size - 1,
				*dst = (uint8_t*)(sendBufferDesc[sendDesc].address + size - 1);
				src >= buffer; src--, dst--) {
				*dst = *src;
			}
			tools::printf("Sending: ");
			for (int i = 0; i < size; i++) {
				tools::printf("%x ", buffer[i]);
			}
			tools::printf("\n");

			sendBufferDesc[sendDesc].avail = 0;
			sendBufferDesc[sendDesc].flags = 0x8300f000 | ((uint16_t)((-size) & 0xfff));
			sendBufferDesc[sendDesc].flags2 = 0;

			registerAddressPort.write(0);
			registerDataPort.write(0x48);
		}

		void AmdAm78c973::Receive() {
			tools::printf("AmdAm78c973 RECEIVED\n");
			for (; (recvBufferDesc[currentRecvBuffer].flags & 0x80000000) == 0; currentRecvBuffer == (currentRecvBuffer + 1) % 8) {
				if (!(recvBufferDesc[currentRecvBuffer].flags & 0x40000000) && (recvBufferDesc[currentRecvBuffer].flags & 0x03000000) == 0x03000000) {
					uint32_t size = recvBufferDesc[currentRecvBuffer].flags & 0xfff;
					if (size > 64) {
						size -= 4;
					}

					uint8_t* buffer = (uint8_t*)(recvBufferDesc[currentRecvBuffer].address);
					for (int i = 0; i < (size > 64 ? 64 : size); i++) {
						tools::printf("%x ", buffer[i]);
					}
					tools::printf("\n");
					if (handler != nullptr) {
						if (handler->OnRawDataReceived(buffer, size)) {
							Send(buffer, size);
						}
					}
				}

				recvBufferDesc[currentRecvBuffer].flags2 = 0;
				recvBufferDesc[currentRecvBuffer].flags = 0x8000f7ff;
			}
		}

		void AmdAm78c973::SetHandler(RawDataHandler* handler) {
			this->handler = handler;
		}

		uint64_t AmdAm78c973::GetMacAddress() {
			return initBlock.physicalAddress;
		}

		uint32_t AmdAm78c973::GetIPAddress() {
			return initBlock.logicalAddress;
		}

		void AmdAm78c973::SetIPAddress(uint32_t ip_be) {
			// 直接ip_be赋值会出问题，不知道什么原因
			uint64_t tmp_ip_be = ip_be;
			initBlock.logicalAddress = tmp_ip_be;
		}
	}
}