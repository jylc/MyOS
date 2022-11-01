#include "keyboard.h"
#include "print.h"
using namespace myos::tools;
namespace myos {
	namespace kernel {
		KeybordEventHandler::KeybordEventHandler() {}

		void KeybordEventHandler::OnKeyDown(const char* c) {
			printf(c);
		}

		void KeybordEventHandler::OnKeyUp(const char*) {}


		KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeybordEventHandler* handler) :
			// ¼üÅÌÖÐ¶Ï 0x21
			InterruptHandler(0x01 + manager->HardwareInterruptOffset(), manager),
			handler(handler),
			dataport(0x60),
			commandport(0x64) {
		}

		KeyboardDriver::~KeyboardDriver() {

		}

		uint32_t KeyboardDriver::HandlerInterrupt(uint32_t esp) {
			uint8_t key = dataport.read();
			static uint8_t shift;
			if (handler == nullptr) {
				return esp;
			}
			switch (key)
			{
			case 0x02: if (shift & SHIFT)handler->OnKeyDown("!"); else handler->OnKeyDown("1"); break;
			case 0x03: if (shift & SHIFT)handler->OnKeyDown("@"); else handler->OnKeyDown("2"); break;
			case 0x04: if (shift & SHIFT)handler->OnKeyDown("#"); else handler->OnKeyDown("3"); break;
			case 0x05: if (shift & SHIFT)handler->OnKeyDown("$"); else handler->OnKeyDown("4"); break;
			case 0x06: if (shift & SHIFT)handler->OnKeyDown("%"); else handler->OnKeyDown("5"); break;
			case 0x07: if (shift & SHIFT)handler->OnKeyDown("^"); else handler->OnKeyDown("6"); break;
			case 0x08: if (shift & SHIFT)handler->OnKeyDown("&"); else handler->OnKeyDown("7"); break;
			case 0x09: if (shift & SHIFT)handler->OnKeyDown("*"); else handler->OnKeyDown("8"); break;
			case 0x0A: if (shift & SHIFT)handler->OnKeyDown("("); else handler->OnKeyDown("9"); break;
			case 0x0B: if (shift & SHIFT)handler->OnKeyDown(")"); else handler->OnKeyDown("0"); break;

			case 0x10: if (shift & SHIFT)handler->OnKeyDown("Q"); else handler->OnKeyDown("q"); break;
			case 0x11: if (shift & SHIFT)handler->OnKeyDown("W"); else handler->OnKeyDown("w"); break;
			case 0x12: if (shift & SHIFT)handler->OnKeyDown("E"); else handler->OnKeyDown("e"); break;
			case 0x13: if (shift & SHIFT)handler->OnKeyDown("R"); else handler->OnKeyDown("r"); break;
			case 0x14: if (shift & SHIFT)handler->OnKeyDown("T"); else handler->OnKeyDown("t"); break;
			case 0x15: if (shift & SHIFT)handler->OnKeyDown("Z"); else handler->OnKeyDown("z"); break;
			case 0x16: if (shift & SHIFT)handler->OnKeyDown("U"); else handler->OnKeyDown("u"); break;
			case 0x17: if (shift & SHIFT)handler->OnKeyDown("I"); else handler->OnKeyDown("i"); break;
			case 0x18: if (shift & SHIFT)handler->OnKeyDown("O"); else handler->OnKeyDown("o"); break;
			case 0x19: if (shift & SHIFT)handler->OnKeyDown("P"); else handler->OnKeyDown("p"); break;

			case 0x1E: if (shift & SHIFT)handler->OnKeyDown("A"); else handler->OnKeyDown("a"); break;
			case 0x1F: if (shift & SHIFT)handler->OnKeyDown("S"); else handler->OnKeyDown("s"); break;
			case 0x20: if (shift & SHIFT)handler->OnKeyDown("D"); else handler->OnKeyDown("d"); break;
			case 0x21: if (shift & SHIFT)handler->OnKeyDown("F"); else handler->OnKeyDown("f"); break;
			case 0x22: if (shift & SHIFT)handler->OnKeyDown("G"); else handler->OnKeyDown("g"); break;
			case 0x23: if (shift & SHIFT)handler->OnKeyDown("H"); else handler->OnKeyDown("h"); break;
			case 0x24: if (shift & SHIFT)handler->OnKeyDown("J"); else handler->OnKeyDown("j"); break;
			case 0x25: if (shift & SHIFT)handler->OnKeyDown("K"); else handler->OnKeyDown("k"); break;
			case 0x26: if (shift & SHIFT)handler->OnKeyDown("L"); else handler->OnKeyDown("l"); break;

			case 0x2C: if (shift & SHIFT)handler->OnKeyDown("Y"); else handler->OnKeyDown("y"); break;
			case 0x2D: if (shift & SHIFT)handler->OnKeyDown("X"); else handler->OnKeyDown("x"); break;
			case 0x2E: if (shift & SHIFT)handler->OnKeyDown("C"); else handler->OnKeyDown("c"); break;
			case 0x2F: if (shift & SHIFT)handler->OnKeyDown("V"); else handler->OnKeyDown("v"); break;
			case 0x30: if (shift & SHIFT)handler->OnKeyDown("B"); else handler->OnKeyDown("b"); break;
			case 0x31: if (shift & SHIFT)handler->OnKeyDown("N"); else handler->OnKeyDown("n"); break;
			case 0x32: if (shift & SHIFT)handler->OnKeyDown("M"); else handler->OnKeyDown("m"); break;
			case 0x33: if (shift & SHIFT)handler->OnKeyDown("<"); else handler->OnKeyDown(","); break;
			case 0x34: if (shift & SHIFT)handler->OnKeyDown(">"); else handler->OnKeyDown("."); break;
			case 0x35: if (shift & SHIFT)handler->OnKeyDown("_"); else handler->OnKeyDown("-"); break;


			case 0x48:	handler->OnKeyDown("U ARROW"); break;
			case 0x4b:	handler->OnKeyDown("L ARROW"); break;
			case 0x50:	handler->OnKeyDown("D ARROW"); break;
			case 0x4d:	handler->OnKeyDown("R ARROW"); break;

			case 0x1C: if (shift & SHIFT)handler->OnKeyDown("\n"); break;
			case 0x39: if (shift & SHIFT)handler->OnKeyDown(" ");  break;
			case 0x2A: case 0x36:shift |= SHIFT; break;
			case 0xAA: case 0x86:shift &= ~SHIFT; break;

			case 0x45:break;
			default:
				if (key < 0x80) {
					tools::printf("[KeyboardDriver::HandlerInterrupt] unknown interrupts %x\n", key);
				}
			}
			return esp;
		}

		void KeyboardDriver::Activate() {
			// ¼üÅÌÇå¿Õ
			while (commandport.read() & 0x1) {
				dataport.read();
			}
			commandport.write(0xae);
			commandport.write(0x20);
			uint8_t status = (dataport.read() | 1) & ~0x10;

			commandport.write(0x60);
			dataport.write(status);
			dataport.write(0xf4);
		}

		int KeyboardDriver::Reset() { return 0; }

		void KeyboardDriver::Deactivate() {}
	}
}