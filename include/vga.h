#ifndef MYOS_VGA_H
#define MYOS_VGA_H
#include "types.h"
#include "interrupts.h"
#include "driver.h"

namespace myos {
	namespace kernel {
		// TODO 与视频中的实现效果不一致
		class VideoGraphicsArray {
		public:
			VideoGraphicsArray();
			~VideoGraphicsArray();
			bool SupportsMode(uint32_t width, uint32_t height, uint32_t colordepth);
			bool SetMode(uint32_t width, uint32_t height, uint32_t colordepth);
			void PutPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b);
			void PutPixel(int32_t x, int32_t y, uint8_t colorindex);


			void FillRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b);

		private:
			Port8Bit miscPort;
			Port8Bit crtcIndexPort;
			Port8Bit crtcDataPort;
			Port8Bit sequencerIndexPort;
			Port8Bit sequencerDataPort;
			Port8Bit graphicsControllerIndexPort;
			Port8Bit graphicsControllerDataPort;
			Port8Bit attributeControllerIndexPort;
			Port8Bit attributeControllerReadPort;
			Port8Bit attributeControllerWritePort;
			Port8Bit attributeControllerResetPort;

			void WriteRegisters(uint8_t* registers);

			uint8_t GetColorIndex(uint8_t r, uint8_t g, uint8_t b);
			uint8_t* GetFrameBufferSegment();
		};
	}
}

#endif // !MYOS_VGA_H
