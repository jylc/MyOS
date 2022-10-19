#include "interrupts.h"
#include "print.h"
using namespace myos::tools;
namespace myos {
	namespace kernel {
		InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
		InterruptManager* InterruptManager::ActiveInterruptManager = 0;

		InterruptHandler::InterruptHandler(uint8_t interrupnumber, InterruptManager* interruptManager) {
			this->interrupnumber = interrupnumber;
			this->interruptManager = interruptManager;
			interruptManager->handlers[interrupnumber] = this;
		}

		InterruptHandler::~InterruptHandler() {
			if (interruptManager->handlers[interrupnumber] == this) {
				interruptManager->handlers[interrupnumber] = 0;
			}
		}

		uint32_t InterruptHandler::HandlerInterrupt(uint32_t esp) {
			return esp;
		}


		void InterruptManager::SetInterruptDescriptorTableEntry(
			uint8_t interruptNumber,
			uint16_t codeSegementSelectorOffset,
			void (*handler)(),
			uint8_t DescriptorPrivilegeLevel,
			uint8_t DescriptorType) {
			const uint8_t IDT_DESC_PRESENT = 0x80;

			interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xffff;
			interruptDescriptorTable[interruptNumber].handlerAdressighBits = ((uint32_t)handler >> 16) & 0xffff;
			interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegementSelectorOffset;
			interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | ((DescriptorPrivilegeLevel & 3) << 5) | DescriptorType;
			interruptDescriptorTable[interruptNumber].reserved = 0;
		}

		InterruptManager::InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* gdt, TaskManager* taskManager) :
			// ���ó�ʼ���˿�
			picMasterCommand(0x20), //��оƬ�˿�
			picMasterData(0x21),
			picSlaveCommand(0xA0), //��оƬ�˿�
			picSlaveData(0xA1),
			taskManager(taskManager) {
			uint16_t codeSegment = (gdt->CodeSegmentSelector()) << 3;
			this->hardwareInterruptOffset = hardwareInterruptOffset;

			const uint8_t IDT_INTERRUPT_GATE = 0xe;
			for (uint16_t i = 0; i < 256; i++) {
				handlers[i] = 0;
				SetInterruptDescriptorTableEntry(i, codeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
			}


			SetInterruptDescriptorTableEntry(0x00, codeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x01, codeSegment, &HandleException0x01, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x02, codeSegment, &HandleException0x02, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x03, codeSegment, &HandleException0x03, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x04, codeSegment, &HandleException0x04, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x05, codeSegment, &HandleException0x05, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x06, codeSegment, &HandleException0x06, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x07, codeSegment, &HandleException0x07, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x08, codeSegment, &HandleException0x08, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x09, codeSegment, &HandleException0x09, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x0A, codeSegment, &HandleException0x0A, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x0B, codeSegment, &HandleException0x0B, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x0C, codeSegment, &HandleException0x0C, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x0D, codeSegment, &HandleException0x0D, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x0E, codeSegment, &HandleException0x0E, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x0F, codeSegment, &HandleException0x0F, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x10, codeSegment, &HandleException0x10, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x11, codeSegment, &HandleException0x11, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x12, codeSegment, &HandleException0x12, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(0x13, codeSegment, &HandleException0x13, 0, IDT_INTERRUPT_GATE);

			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, codeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, codeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x02, codeSegment, &HandleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x03, codeSegment, &HandleInterruptRequest0x03, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x04, codeSegment, &HandleInterruptRequest0x04, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x05, codeSegment, &HandleInterruptRequest0x05, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x06, codeSegment, &HandleInterruptRequest0x06, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x07, codeSegment, &HandleInterruptRequest0x07, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x08, codeSegment, &HandleInterruptRequest0x08, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x09, codeSegment, &HandleInterruptRequest0x09, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0A, codeSegment, &HandleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0B, codeSegment, &HandleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0C, codeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0D, codeSegment, &HandleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0E, codeSegment, &HandleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x0F, codeSegment, &HandleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE);
			SetInterruptDescriptorTableEntry(hardwareInterruptOffset + 0x31, codeSegment, &HandleInterruptRequest0x31, 0, IDT_INTERRUPT_GATE);

			/*
			* ��ʼ��˳��
			* 1. ���˿�20h����Ƭ����A0h����Ƭ)����ICW1
			* 2. ���˿�21h����Ƭ����A1h����Ƭ)����ICW2
			* 3. ���˿�21h����Ƭ����A1h����Ƭ)����ICW3
			* 4. ���˿�21h����Ƭ����A1h����Ƭ)����ICW4
			*/


			// 11 ��ʾ��ʼ�����ʼ����ICW1 ������
			picMasterCommand.write(0x11);
			picSlaveCommand.write(0x11);

			// IRQ0 ��Ӧ�ж����� 0x20
			// Linuxϵͳ����Ƭ��ICW2����Ϊ0x20����ʾ��Ƭ�ж�����0��-7����Ӧ���жϺŷ�Χ��0x20-0x27��
			// ����Ƭ��ICW2�����ó�0x28����ʾ��Ƭ�ж�����8��-15����Ӧ���жϺŷ�Χ��0x28-0x2f
			picMasterData.write(hardwareInterruptOffset);
			picSlaveData.write(hardwareInterruptOffset + 8);

			picMasterData.write(0x04); // ��8259, ICW3.
			picSlaveData.write(0x02); // ��8259, ICW3.

			picMasterData.write(0x01); // ��8259, ICW4.
			picSlaveData.write(0x01); // ��8259, ICW4.

			picMasterData.write(0x00);
			picSlaveData.write(0x00);

			InterruptDescriptorTablePointer idt;
			idt.size = 256 * sizeof(GateDescriptor) - 1;
			idt.base = (uint32_t)interruptDescriptorTable;
			// �� m ���ص� IDTR
			asm volatile("lidt %0": : "m" (idt));
		}

		InterruptManager::~InterruptManager() {}

		void InterruptManager::Activate() {
			if (ActiveInterruptManager != 0) {
				ActiveInterruptManager->DeActivate();
			}
			ActiveInterruptManager = this;
			asm("sti");
		}

		void InterruptManager::DeActivate() {
			if (ActiveInterruptManager == this) {
				ActiveInterruptManager = 0;
				asm("cli");
			}
		}

		// HandleInterrupt ���쳣�������Ļ��������
		uint32_t InterruptManager::HandleInterrupt(uint8_t interruptNumber, uint32_t esp) {
			if (ActiveInterruptManager != 0) {
				return ActiveInterruptManager->DoHandleInterrupt(interruptNumber, esp);
			}
			return esp;
		}

		uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp) {
			// �����ж�������Ѱ���жϴ���
			if (handlers[interruptNumber] != 0) {
				esp = handlers[interruptNumber]->HandlerInterrupt(esp);
			}
			else if (interruptNumber != hardwareInterruptOffset) {
				char* foo = (char*)"[InterruptManager::DoHandleInterrupt] unknown interrupts %lx\n";
				myos::tools::printfHex(foo, interruptNumber);
			}

			if (interruptNumber == hardwareInterruptOffset) { //����ʱ���ж��л�����
				esp = (uint32_t)taskManager->Schedule((CPUState*)esp);
			}

			if (hardwareInterruptOffset <= interruptNumber && interruptNumber < hardwareInterruptOffset + 16) {
				picMasterCommand.write(0x20);
				if (hardwareInterruptOffset + 8 <= interruptNumber) {
					picSlaveCommand.write(0x20);
				}
			}
			return esp;
		}

		uint16_t InterruptManager::HardwareInterruptOffset() {
			return hardwareInterruptOffset;
		}
	}
}
