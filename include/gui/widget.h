#ifndef MYOS_WIDGET_H
#define MYOS_WIDGET_H
#include "types.h"
#include "graphicscontext.h"
#include "keyboard.h"

namespace myos {
	namespace gui {
		class Widget :public kernel::KeybordEventHandler {
		public:
			Widget(Widget* parent, int32_t x, int32_t y,
				int32_t w, int32_t h,
				uint8_t r, uint8_t g, uint8_t b);
			~Widget();

			virtual void GetFocus(Widget* widget);
			virtual void ModeToScreen(int32_t& x, int32_t& y);
			virtual bool ContainsCordianate(int32_t x, int32_t y);

			virtual void Draw(GraphicsContext* gc);
			virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
			virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
			virtual void OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny);

		protected:
			Widget* parent;
			int32_t x, y, w, h;
			uint8_t r, g, b;
			bool Focusable;
		};

		class CompositeWidget :public Widget {
		public:
			CompositeWidget(Widget* parent, int32_t x, int32_t y,
				int32_t w, int32_t h,
				uint8_t r, uint8_t g, uint8_t b);
			~CompositeWidget();


			virtual void GetFocus(Widget* widget);
			virtual bool AddChild(Widget* child);
			virtual void ModeToScreen(int32_t& x, int32_t& y);

			virtual void Draw(GraphicsContext* gc);
			virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
			virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
			virtual void OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny);

			virtual void OnKeyDown(const char* x);
			virtual void OnKeyUp(const char* x);

		private:
			Widget* children[100];
			uint32_t numChildren;
			Widget* focusedChild;
		};
	}
}

#endif // !MYOS_WIDGET_H
