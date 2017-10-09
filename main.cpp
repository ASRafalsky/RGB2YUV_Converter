// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include <thread>
#include "Frame.h"
#include "ImageProcessing.h"

//#define THREAD
#ifndef THREAD
	#define SIMD
#endif // !THREAD

//#define TEST_EN

const char	*yuvfile	= "park_joy_1080p50.yuv";
const char	*NewYUV		= "NEW.yuv";
const char	*bitmapfile = "test_colorbars_1920x1080.bmp";

const uint32_t RGB_Width	= 1920;
const uint32_t RGB_Height	= 1080;
const uint32_t YUV_Width	= 1920;
const uint32_t YUV_Height	= 1080;

#ifdef TEST_EN
// в случае несовпадения значений в массивах, выводятся номер элементов, и разность их значений.
// когда проверили соответствие всех элементов, возвращаем кол-во несовпадающих значений.
//sample - указатель на эталонный массив
//result - указатель на проверяемый массив
uint8_t test_conv(uint8_t *RGB_bmp, uint8_t* result, uint64_t imSize, uint16_t height)
{
	ImageProcessing TEST_PROC(YUV_Width, YUV_Height);
	uint8_t *YUV_sample = new uint8_t[imSize + imSize / 2];
	if (YUV_sample == NULL)
	{
		fprintf(stderr, "Cannot create YUV_sample array\n");
		return (0);
	}
	TEST_PROC.Bitmap2Yuv420p(RGB_bmp, YUV_sample, 0, imSize, imSize + imSize / 4, height);

	uint64_t pix_err_cnt = 0;
	uint8_t pix_err_value = 0;

	for (uint64_t i = 0; i < imSize + imSize / 2; i++)
	{
		pix_err_value = (*(YUV_sample + i) - *(result + i));
		if ((*(YUV_sample + i) != *(result + i)))
		{
			fprintf(stderr, "YUV conversion error!!! \r\n");
			printf("pix num = %d, val =, %d", i, pix_err_value);
			printf("%s", "\r\n");
			++pix_err_cnt;
		}
	}
	delete[] YUV_sample;
	if(pix_err_cnt)
	{
		printf("pix error counter = %d", pix_err_cnt);
		return (0);
	}
	return (1);
}

uint8_t test_add(uint8_t *frame1, uint8_t *frame2, uint8_t* result, uint64_t YUV_frameSize)
{
	ImageProcessing TEST_ADD_PROC(YUV_Width, YUV_Height, RGB_Width, RGB_Height);
	TEST_ADD_PROC.FrameAdd(frame1, frame2);

	uint64_t pix_err_cnt = 0;
	uint8_t pix_err_value = 0;

	for (uint64_t i = 0; i < YUV_frameSize; i++)
	{
		pix_err_value = (*(TEST_ADD_PROC.GetYUV() + i) - *(result + i));
		if ((*(TEST_ADD_PROC.GetYUV() + i) != *(result + i)))
		{
			fprintf(stderr, "Frame adding error!!! \r\n");
			printf("pix num = %d, val =, %d", i, pix_err_value);
			printf("%s", "\r\n");
			++pix_err_cnt;
		}
	}
	if (pix_err_cnt)
	{
		printf("pix error counter = %d", pix_err_cnt);
		return (0);
	}
	return (1);
}
#endif // TEST_EN

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
	RGB2YUV.Bitmap2yuv_SIMD(bgr_frame, 0, 0);
	yuv_from_rgb_frame = RGB2YUV.GetYUV();
#endif // SMID

#ifdef THREAD
	yuv_from_rgb_frame = new uint8_t[YUV_Frame.YUVframeSize()];

	uint64_t imSize = RGB_Frame.imageSize();
	uint64_t yuv1_st_1 = imSize / 4;
	uint64_t yuv1_st_2 = imSize / 2;
	uint64_t yuv1_st_3 = imSize / 4 + imSize / 2;

	uint64_t RGB_bmp_st_0 = 0;
	uint64_t RGB_bmp_st_1 = imSize / 4;
	uint64_t RGB_bmp_st_2 = RGB_bmp_st_1 * 2;
	uint64_t RGB_bmp_st_3 = RGB_bmp_st_2 + RGB_bmp_st_1;

	uint64_t upos_st_0 = imSize;
	uint64_t upos_st_1 = imSize + imSize / 16;
	uint64_t upos_st_2 = upos_st_1 + imSize / 16;
	uint64_t upos_st_3 = upos_st_2 + imSize / 16;

	uint64_t vpos_st_0 = imSize + imSize / 4;
	uint64_t vpos_st_1 = vpos_st_0 + imSize / 16;
	uint64_t vpos_st_2 = vpos_st_1 + imSize / 16;
	uint64_t vpos_st_3 = vpos_st_2 + imSize / 16;

	uint16_t height_per_thread = RGB_Height / 4;

	std::thread t0(&ImageProcessing::Bitmap2Yuv420p, &RGB2YUV, std::ref(bgr_frame), std::ref(yuv_from_rgb_frame), RGB_bmp_st_0, upos_st_0, vpos_st_0, height_per_thread);
	std::thread t1(&ImageProcessing::Bitmap2Yuv420p, &RGB2YUV, std::ref(bgr_frame), std::ref(yuv_from_rgb_frame), RGB_bmp_st_1, upos_st_1, vpos_st_1, height_per_thread);
	std::thread t2(&ImageProcessing::Bitmap2Yuv420p, &RGB2YUV, std::ref(bgr_frame), std::ref(yuv_from_rgb_frame), RGB_bmp_st_2, upos_st_2, vpos_st_2, height_per_thread);
	std::thread t3(&ImageProcessing::Bitmap2Yuv420p, &RGB2YUV, std::ref(bgr_frame), std::ref(yuv_from_rgb_frame), RGB_bmp_st_3, upos_st_3, vpos_st_3, height_per_thread);

	t0.join();
	t1.join();
	t2.join();
	t3.join();

#endif // THREAD
	
	uint8_t* yuv_frame;
	uint64_t YUV_imSize1 = YUV_Frame.imageSize();
	uint64_t YUV_imSize2 = RGB_Frame.imageSize();
	while (YUV_Frame.readYUVFrame())
	{
		yuv_frame = YUV_Frame.getFrame();
#ifndef SIMD

		FRAMES_ADD.FrameAdd(yuv_frame, yuv_from_rgb_frame);

#ifdef TEST_EN
		if (!test_conv(bgr_frame, yuv_from_rgb_frame, YUV_Frame.imageSize(), RGB_Height))
			fprintf(stderr, "conversation test error!!! \r\n");
		system("pause");
		return (1);
#endif // TEST_EN

#else

		FRAMES_ADD.FrameAdd_SIMD(yuv_frame, yuv_from_rgb_frame);

#ifdef TEST_EN
		if (!test_conv(bgr_frame, yuv_from_rgb_frame, YUV_Frame.imageSize(), RGB_Height))
			fprintf(stderr, "conversation test error!!! \r\n");

		if(!test_add(yuv_frame, yuv_from_rgb_frame, FRAMES_ADD.GetYUV(), YUV_Frame.YUVframeSize()))
			fprintf(stderr, "frame adding test error!!! \r\n");
		system("pause");
		return (1);
#endif // TEST_EN

#endif // SIMD
		NewYUV_Frame.writeYUVFrame(FRAMES_ADD.GetYUV());
	}

#ifdef THREAD
	delete[] yuv_from_rgb_frame;
#endif // THREAD

    return (1);
}

