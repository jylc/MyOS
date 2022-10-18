#ifndef MYOS_DESKTOP_H
#define MYOS_DESKTOP_H
#include "types.h"
#include "graphicscontext.h"
#include "widget.h"
#include "mouse.h"
#include "keyboard.h"
namespace myos {
	namespace gui {
		class Desktop :public CompositeWidget, public kernel::MouseEventHandler {
		public:
			Desktop(int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);
			~Desktop();

			void Draw(GraphicsContext* gc);
			void OnMouseDown(uint8_t button);
			void OnMouseUp(uint8_t button);
			void OnMouseMove(int32_t x, int32_t y);

		private:
			uint32_t MouseX, MouseY;
		};
	}
}


#endif // ! MYOS_DESKTOP_H