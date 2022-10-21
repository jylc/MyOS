#include "net/etherframe.h"
#include "memorymanagement.h"

namespace myos {
	namespace net {
		EtherFrameHandler::EtherFrameHandler(EtherFrameProvider* backend, uint16_t etherType) {
			this->etherType_BE = ((etherType & 0x00ff) << 8) | ((etherType & 0xff00) >> 8);
			this->backend = backend;
			backend->handlers[etherType_BE] = this;
		}

		EtherFrameHandler::~EtherFrameHandler() {
			backend->handlers[etherType_BE] = nullptr;
		}

		bool  EtherFrameHandler::OnEtherFrameReceived(uint8_t* etherFramePayload, uint32_t size) {
			return false;
		}

		void EtherFrameHandler::Send(uint64_t dstMAC_BE, uint8_t* etherFramePayload, uint32_t size) {
			backend->Send(dstMAC_BE, etherType_BE, etherFramePayload, size);
		}

		EtherFrameProvider::EtherFrameProvider(AmdAm78c973* backend) :
			RawDataHandler(backend) {
			for (uint32_t i = 0; i < 65535; i++) {
				this->handlers[i] = 0;
			}
		} 

		EtherFrameProvider::~EtherFrameProvider() {}

		bool EtherFrameProvider::OnRawDataReceived(uint8_t* buffer, uint32_t size) {
			EtherFrameHeader* frame = (EtherFrameHeader*)buffer;
			
			bool sendBack = false;
			if ((frame->dstMAC_BE == 0xffffffffffff)||
				(frame->dstMAC_BE==backend->GetMacAddress())) {
				if (handlers[frame->etherType_BE] != 0) {
					sendBack = handlers[frame->etherType_BE]->OnEtherFrameReceived(
						buffer + sizeof(EtherFrameHeader), size - sizeof(EtherFrameHeader)
					);
				}
			}

			if (sendBack) {
				frame->dstMAC_BE = frame->srcMAC_BE;
				frame->srcMAC_BE = backend->GetMacAddress();
			}
			return sendBack; 
		}

		void EtherFrameProvider::Send(uint64_t dstMAC_BE, uint16_t etherType_BE, uint8_t* buffer, uint32_t size) {
			uint8_t* buffer2 = (uint8_t*)kernel::MemoryManager::activeMemoryManager->malloc(sizeof(EtherFrameHeader)+size);
			EtherFrameHeader* frame = (EtherFrameHeader*)buffer2;

			frame->dstMAC_BE = dstMAC_BE;
			frame->srcMAC_BE = backend->GetMacAddress();
			frame->etherType_BE = etherType_BE;

			uint8_t* src = buffer;
			uint8_t* dst = buffer2 + sizeof(EtherFrameHeader);
			for (uint32_t i = 0; i < size; i++) dst[i] = src[i];

			backend->Send(buffer2, size + sizeof(EtherFrameHeader));
		}
	}
}