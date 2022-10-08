#ifndef MYOS_KEYBOARD_H
#define MYOS_KEYBOARD_H
#include "types.h"
#include "port.h"
#include "interrupts.h"
#include "driver.h"

namespace myos {
	namespace kernel {
		class KeybordEventHandler {
		public:
			KeybordEventHandler();

			virtual void OnKeyDown(const char*);
			virtual void OnKeyUp(const char*);
		};


		class KeyboardDriver :public InterruptHandler,public Driver {
		public:
			KeyboardDriver(InterruptManager* manager,KeybordEventHandler*handler);
			~KeyboardDriver();
			virtual uint32_t HandlerInterrupt(uint32_t esp);
			virtual void Activate();
			virtual int Reset();
			virtual void Deactivate();
		private:
			Port8Bit dataport;
			Port8Bit commandport;
			KeybordEventHandler* handler;
		};
	}
}


#endif // !MYOS_KEYBOARD_H
