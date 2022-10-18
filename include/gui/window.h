#ifndef MYOS_WINDOW_H
#define MYOS_WINDOW_H
#include "types.h"
#include "graphicscontext.h"
#include "widget.h"
namespace myos {
	namespace gui {
		class Window :public CompositeWidget {
		public:
			Window(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b);
			~Window();

			virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
			virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
			virtual void OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny);

		private:
			bool Dragging;
		};
	}
}


#endif // !MYOS_WINDOW_H
