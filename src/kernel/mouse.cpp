#include "mouse.h"
void printf(const char* str);
void printfHex(char* str, myos::uint8_t key);
namespace myos {
	namespace kernel {
		MouseEventHandler::MouseEventHandler() {}
		void MouseEventHandler::OnActivate() {}
		void MouseEventHandler::OnMouseDown(uint8_t button) {}
		void MouseEventHandler::OnMouseUp(uint8_t button) {}
		void MouseEventHandler::OnMouseMove(uint8_t nx, uint8_t ny) {}

		MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler) :
			InterruptHandler(0x0C + manager->HardwareInterruptOffset(), manager),
			dataport(0x60),
			commandport(0x64),
			offset(0),
			buttons(0),
			x(40), y(12), handler(handler) {}

		MouseDriver::~MouseDriver() {}

		uint32_t MouseDriver::HandlerInterrupt(uint32_t esp) {
			uint8_t status = commandport.read();
			if (!(status & 0x20) || handler == nullptr)return esp;

			buffer[offset] = dataport.read();
			offset = (offset + 1) % 3;
			if (offset == 0) {
				handler->OnMouseMove(buffer[1], -buffer[2]);

				for (uint8_t i = 0; i < 3; i++) {
					if ((buffer[0] & (1 << i)) != (buttons & (1 << i))) {
						if (buttons & (1 << i)) {
							handler->OnMouseUp(i + 1);
						}
						else {
							handler->OnMouseDown(i + 1);
						}
					}
				}
				buttons = buffer[0];
			}
			return esp;
		}

		void MouseDriver::Activate() {
			if (handler != nullptr) {
				handler->OnActivate();
			}
			commandport.write(0xa8);
			commandport.write(0x20);
			uint8_t status = (dataport.read() | 2) & ~0x20;

			commandport.write(0x60);
			dataport.write(status);

			commandport.write(0xd4);
			dataport.write(0xf4);
			dataport.read();
		}
		int MouseDriver::Reset() { return 0; }
		void MouseDriver::Deactivate() {}
	}
}