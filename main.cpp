// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include "Frame.h"
#include "ImageProcessing.h"

#define THREAD
#ifndef THREAD
	#define SIMD
#endif // !THREAD

const char	*yuvfile	= "park_joy_1080p50.yuv";
const char	*NewYUV		= "NEW.yuv";
const char	*bitmapfile = "test_colorbars_1920x1080.bmp";

const uint32_t RGB_Width	= 1920;
const uint32_t RGB_Height	= 1080;
const uint32_t YUV_Width	= 1920;
const uint32_t YUV_Height	= 1080;

int main()
{
	Frame RGB_Frame(RGB_Width, RGB_Height, bitmapfile, "rb");
	Frame YUV_Frame(YUV_Width, YUV_Height, yuvfile, "rb");
	Frame NewYUV_Frame(YUV_Width, YUV_Height, NewYUV, "wb");
	ImageProcessing RGB2YUV(YUV_Width, YUV_Height);
	ImageProcessing FRAMES_ADD(YUV_Width, YUV_Height, YUV_Width, YUV_Height);

	RGB_Frame.readBMP();

	uint8_t* bgr_frame = RGB_Frame.getFrame();

	uint8_t* yuv_from_rgb_frame = NULL;

#ifdef SIMD
	RGB2YUV.Bitmap2yuv_SIMD(bgr_frame, 0);
#endif // SMID

#ifdef THREAD
	RGB2YUV.Bitmap2yuv_THREAD(bgr_frame);
#endif // THREAD

	yuv_from_rgb_frame = RGB2YUV.GetYUV();
	
	uint8_t* yuv_frame;
	uint64_t YUV_imSize1 = YUV_Frame.imageSize();
	uint64_t YUV_imSize2 = RGB_Frame.imageSize();

	while (YUV_Frame.readYUVFrame())
	{
		yuv_frame = YUV_Frame.getFrame();

#ifndef SIMD
		FRAMES_ADD.FrameAdd(yuv_frame, yuv_from_rgb_frame);
#else
		FRAMES_ADD.FrameAdd_SIMD(yuv_frame, yuv_from_rgb_frame);
#endif // SIMD

		NewYUV_Frame.writeYUVFrame(FRAMES_ADD.GetYUV());
	}
    return (1);
}

