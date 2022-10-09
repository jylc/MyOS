#include "mouse.h"
namespace myos {
	namespace kernel {
		MouseEventHandler::MouseEventHandler() {}
		void MouseEventHandler::OnActivate() {}
		void MouseEventHandler::OnMouseDown(uint8_t button) {}
		void MouseEventHandler::OnMouseUp(uint8_t button) {}
		void MouseEventHandler::OnMouseMove(int8_t nx, int8_t ny) {}

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

			uint8_t data = dataport.read();
			DecodeInfo(&info, data);

			if (info.phase == 1) {
				handler->OnMouseMove(info.info[1], -info.info[2]);

				/*for (uint8_t i = 0; i < 3; i++) {
					if ((buffer[0] & (1 << i)) != (buttons & (1 << i))) {
						if (buttons & (1 << i)) {
							handler->OnMouseUp(i + 1);
						}
						else {
							handler->OnMouseDown(i + 1);
						}
					}
				}
				buttons = buffer[0];*/
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

		int MouseDriver::DecodeInfo(MouseDataInfo* info, uint8_t data) {
			if (info->phase == 0) {
				if (data == 0xFA) {
					info->phase = 1;
				}
				return 0;
			}

			if (info->phase == 1) {
				if ((data & 0xC8) == 0x08) {
					info->info[0] = data;
					info->phase = 2;
				}
				return 0;
			}

			if (info->phase == 2) {
				info->info[1] = data;
				info->phase = 3;
				return 0;
			}

			if (info->phase == 3) {
				info->info[2] = data;
				info->phase = 1;
				info->button = info->info[0] & 0x07;  //0000 0111
				info->x = info->info[1];
				info->y = info->info[2];
				if ((info->info[0] & 0x10) != 0) {                 //0001 0000 Ïò×ó
					info->x |= 0xFFFFFF00;
				}
				if ((info->info[0] & 0x20) != 0) {                 //0010 0000 ÏòÏÂ
					info->y |= 0xFFFFFF00;
				}
				info->y = -(info->y);
				return 1;
			}
			return -1;
		}
	}
}