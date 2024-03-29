#include "vga.h"

namespace myos {
	namespace kernel {
		VideoGraphicsArray::VideoGraphicsArray() :
			miscPort(0x3c2),
			crtcIndexPort(0x3d4), crtcDataPort(0x3d5),
			sequencerIndexPort(0x3c4), sequencerDataPort(0x3c5),
			graphicsControllerIndexPort(0x3ce), graphicsControllerDataPort(0x3cf),
			attributeControllerIndexPort(0x3c0),
			attributeControllerReadPort(0x3c1),
			attributeControllerWritePort(0x3c0),
			attributeControllerResetPort(0x3da) {}
		VideoGraphicsArray::~VideoGraphicsArray() {};

		void VideoGraphicsArray::WriteRegisters(uint8_t* registers) {
			miscPort.write(*(registers++));

			for (uint8_t i = 0; i < 5; i++) {
				sequencerIndexPort.write(i);
				sequencerDataPort.write(*(registers++));
			}
			crtcIndexPort.write(0x03);
			crtcDataPort.write(crtcDataPort.read() | 0x80);
			crtcIndexPort.write(0x11);
			crtcDataPort.write(crtcDataPort.read() & ~0x80);

			registers[0x03] = registers[0x03] | 0x80;
			registers[0x11] = registers[0x11] & ~0x80;

			for (uint8_t i = 0; i < 25; i++) {
				crtcIndexPort.write(i);
				crtcDataPort.write(*(registers++));
			}

			for (uint8_t i = 0; i < 9; i++) {
				graphicsControllerIndexPort.write(i);
				graphicsControllerDataPort.write(*(registers++));
			}

			for (uint8_t i = 0; i < 21; i++) {
				attributeControllerReadPort.read();
				attributeControllerIndexPort.write(i);
				attributeControllerWritePort.write(*(registers++));
			}

			attributeControllerResetPort.read();
			attributeControllerIndexPort.write(0x20);
		}

		bool VideoGraphicsArray::SupportsMode(uint32_t width, uint32_t height, uint32_t colordepth) {
			return width == 320 && height == 200 && colordepth == 8;
		}

		bool VideoGraphicsArray::SetMode(uint32_t width, uint32_t height, uint32_t colordepth) {
			if (!SupportsMode(width, height, colordepth))return false;

			unsigned char g_320x200x256[] =
			{
				/* MISC */
				0x63,
				/* SEQ */
				0x03, 0x01, 0x0F, 0x00, 0x0E,
				/* CRTC */
				0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
				0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
				0xFF,
				/* GC */
				0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
				0xFF,
				/* AC */
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
				0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
				0x41, 0x00, 0x0F, 0x00,	0x00
			};

			WriteRegisters(g_320x200x256);
			return true;
		}

		void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b) {
			PutPixel(x, y, GetColorIndex(r, g, b));
		}

		void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t colorindex) {
			if (x < 0 || 320 <= x || y < 0 || 200 <= y)return;
			uint8_t* pixelAddress = GetFrameBufferSegment() + 320 * y + x;
			*pixelAddress = colorindex;
		}

		uint8_t* VideoGraphicsArray::GetFrameBufferSegment() {
			graphicsControllerIndexPort.write(0x06);
			uint8_t segmentNumber = graphicsControllerDataPort.read() & (3 << 2);

			switch (segmentNumber)
			{
			case 0 << 2: return (uint8_t*)0xa0000;
			case 1 << 2: return (uint8_t*)0xa0000;
			case 2 << 2: return (uint8_t*)0xb0000;
			case 3 << 2: return (uint8_t*)0xb8000;
			default:return (uint8_t*)0xb8000;
			}
		}

		uint8_t VideoGraphicsArray::GetColorIndex(uint8_t r, uint8_t g, uint8_t b) {
			if (r == 0x00, g == 0x00, b == 0x00)return 0x00; 
			if (r == 0x00, g == 0x00, b == 0xa8)return 0x11; 
			if (r == 0x00, g == 0xa8, b == 0x00)return 0x21; 
			if (r == 0xa8, g == 0x00, b == 0xa8)return 0x41; 
			if (r == 0xff, g == 0xff, b == 0xff)return 0x3f; 
			return 0x01;
		}

		void VideoGraphicsArray::FillRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b) {
			for (uint32_t Y = y; Y < y + h; Y++) {
				for (uint32_t X = x; X < x + w; X++) {
					PutPixel(X, Y, r, g, b);
				}
			}
		}

	}
}