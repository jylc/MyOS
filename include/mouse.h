#ifndef MYOS_MOUSE_H
#define MYOS_MOUSE_H
#include "types.h"
#include "interrupts.h"
#include "port.h"
#include "driver.h"

namespace myos {
	namespace kernel {
		class MouseEventHandler {
		public:
			MouseEventHandler();
			virtual void OnActivate();
			virtual void OnMouseDown(uint8_t);
			virtual void OnMouseUp(uint8_t);
			virtual void OnMouseMove(uint8_t nx, uint8_t ny);
		
		};

		//TODO 鼠标输出存在问题
		class MouseDriver :public InterruptHandler,public Driver {
		public:
			MouseDriver(InterruptManager* manager,MouseEventHandler *handler);
			~MouseDriver();
			virtual uint32_t HandlerInterrupt(uint32_t esp);
			virtual void Activate();
			virtual int Reset();
			virtual void Deactivate();
		private:
			Port8Bit dataport;
			Port8Bit commandport;

			uint8_t buffer[3];
			uint8_t offset;
			uint8_t buttons;
			int8_t x,y;

			MouseEventHandler* handler;
		};
	}
}


#endif // !MYOS_MOUSE_H
