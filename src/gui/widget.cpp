#include "gui/widget.h"

namespace myos {
	namespace gui {
		Widget::Widget(Widget* parent, int32_t x, int32_t y,
			int32_t w, int32_t h,
			uint8_t r, uint8_t g, uint8_t b) :
			KeybordEventHandler(),
			parent(parent),
			x(x), y(y),w(w),h(h),
			r(r), g(g), b(b),
			Focusable(true) {}
		Widget::~Widget(){}
		void Widget::GetFocus(Widget* widget) {
			if (parent != 0) {
				parent->GetFocus(widget);
			}
		}

		void Widget::ModeToScreen(int32_t& x, int32_t& y) {
			if (parent != 0) {
				parent->ModeToScreen(x, y);
			}

			x += this->x;
			y += this->y;
		}

		void Widget::Draw(GraphicsContext* gc) {
			int X = 0, Y = 0;
			ModeToScreen(X, Y);
			gc->FillRectangle(X, Y, w, h, r, g, b);
		}

		void Widget::OnMouseDown(int32_t x, int32_t y,uint8_t bottom) {
			if (Focusable) {
				GetFocus(this);
			}
		}

		void Widget::OnMouseUp(int32_t x, int32_t y, uint8_t bottom) {

		}

		void Widget::OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny) {

		}


		bool Widget::ContainsCordianate(int32_t x, int32_t y) {
			return this->x <= x && this->y <= y && this->x + this->w > x && this->y + this->h > y;
		}

		CompositeWidget::CompositeWidget(Widget* parent, int32_t x, int32_t y,
			int32_t w, int32_t h,
			uint8_t r, uint8_t g, uint8_t b) :
			Widget(parent,x,y,w,h,r,g,b),
			numChildren(0),
			focusedChild(0)
		 {

		}

		void CompositeWidget::GetFocus(Widget* widget) {
			focusedChild = widget;
			if (parent != 0) {
				parent->GetFocus(this);
			}
		}

		bool CompositeWidget::AddChild(Widget* child) {
			if (numChildren >= 100) {
				return false;
			}
			children[numChildren++] = child;
			return true;
		}

		void CompositeWidget::Draw(GraphicsContext* gc) {
			Widget::Draw(gc);
			for (uint32_t i = numChildren - 1; ~i; i--) {
				children[i]->Draw(gc);
			}
		}

		void CompositeWidget::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
			for (int32_t i = 0; i < numChildren; i++) {
				if (children[i]->ContainsCordianate(x - this->x, y - this->y)) {
					children[i]->OnMouseDown(x - this->x, y - this->y,button);
					break;
				}
			}
		}

		void CompositeWidget::OnMouseUp(int32_t x, int32_t y, uint8_t button){
			for (int32_t i = 0; i < numChildren; i++) {
				if (children[i]->ContainsCordianate(x - this->x, y - this->y)) {
					children[i]->OnMouseUp(x - this->x, y - this->y,button);
					break;
				}
			}
		}

		void CompositeWidget::OnMouseMove(int32_t x, int32_t y, int32_t nx, int32_t ny) {
			int32_t firstChild = -1;
			for (int32_t i = 0; i < numChildren; i++) {
				if (children[i]->ContainsCordianate(x - this->x, y - this->y)) {
					children[i]->OnMouseMove(x - this->x, y - this->y, nx - this->x, ny - this->y);
					firstChild = i;
					break;
				}
			}

			for (int32_t i = 0; i < numChildren; i++) {
				if (children[i]->ContainsCordianate(nx - this->x, ny - this->y)) {
					if (firstChild != i) {
						children[i]->OnMouseMove(x - this->x, y - this->y, nx - this->x, ny - this->y);
					}
					break;
				}
			}
		}
		void CompositeWidget::OnKeyDown(const char* x) {
			if (focusedChild != 0) {
				focusedChild->OnKeyDown(x);
			}
		}

		void CompositeWidget::OnKeyUp(const char* x) {
			if (focusedChild != 0) {
				focusedChild->OnKeyUp(x);
			}
		}

		void CompositeWidget::ModeToScreen(int32_t& x, int32_t& y) {

		}

		CompositeWidget::~CompositeWidget() {

		}
	}
}