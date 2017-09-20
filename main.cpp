// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include <thread>
#include "improc.h"
#include "Frame.h"
#include "ImageProcessing.h"

#define THREAD
//#define SMID
//#define TEST_EN

using namespace std;

const char	*yuvfile = "park_joy_1080p50.yuv";
const char	*NewYUV = "NEW.yuv";
const char	*bitmapfile = "test_colorbars_1920x1080.bmp";

const uint32_t RGB_Width = 1920;
const uint32_t RGB_Height = 1080;
const uint32_t YUV_Width = 1920;
const uint32_t YUV_Height = 1080;

#ifdef TEST_EN
// в случае несовпадения значений в массивах, выводятся номер элементов, и разность их значений.
// когда проверили соответствие всех элементов, возвращаем кол-во несовпадающих значений.
//sample - указатель на эталонный массив
//result - указатель на проверяемый массив
uint64_t test(uint8_t *RGB_bmp, uint8_t* result, uint64_t YUV_frameSize, uint16_t width, uint16_t height)
{
	uint8_t *YUV_sample = (uint8_t*) malloc(YUV_frameSize);
	if (YUV_sample == NULL)
	{
		fprintf(stderr, "Cannot create YUV_sample array\n");
		return (0);
	}

	//Bitmap2Yuv420p(RGB_bmp, RGB_bmp_st_0, YUV_sample, 1920, 1080, upos_st_0, vpos_st_0);
	Bitmap2Yuv420p_v2(RGB_bmp, 0, YUV_sample, width, height, width*height, width*height + width*height/4);

	uint64_t pix_err_cnt = 0;
	uint32_t pix_err_value = 0;

	for (uint64_t i = 0; i < width*height + width*height / 2; i++)
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

	free(YUV_sample);
	return (pix_err_cnt);
}
#endif // TEST_EN

int main()
{
	uint64_t i = 0;
	/*
	uint64_t imSize = (1920 * 1080);
	uint64_t uvSize = imSize/4;
	uint64_t YUV_frameSize = ((imSize) + (imSize) / 2);

	uint8_t *RGB_bmp = (uint8_t*)malloc(imSize * 3);
	if (RGB_bmp == NULL)
	{
		fprintf(stderr, "Cannot create RGB array\n");
		return (0);
	}
	uint8_t *YUV1 = (uint8_t*)malloc(YUV_frameSize);//массив для хранения преобразованного bmp
	if (YUV1 == NULL)
	{
		fprintf(stderr, "Cannot create YUV1 array\n");
		return (0);
	}
	uint8_t *YUV2 = (uint8_t*)malloc(YUV_frameSize);//массив для наложения кадров и последующего вывода
	if (YUV1 == NULL)
	{
		fprintf(stderr, "Cannot create YUV2 array\n");
		return (0);
	}

	uint8_t *u = (uint8_t*)malloc(uvSize + 12);
	if (u == NULL)
	{
		fprintf(stderr, "Cannot create u array\n");
		return (0);
	}

	uint8_t *v = (uint8_t*)malloc(uvSize + 12);
	if (v == NULL)
	{
		fprintf(stderr, "Cannot create v array\n");
		return (0);
	}
	
	readBMP(bitmapfile, RGB_bmp);
	
	//преобразование bmp to yuv
#ifdef SMID
	Bitmap2yuv_SMID(RGB_bmp, YUV1, u, v, imSize, uvSize, YUV_frameSize, 1920, 1080, 0, 0);
#endif // SMID

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

#ifdef THREAD
	thread t0(Bitmap2Yuv420p_v2, ref(RGB_bmp), RGB_bmp_st_0, ref(YUV1), 1920, 1080 / 4, upos_st_0, vpos_st_0);
	thread t1(Bitmap2Yuv420p_v2, ref(RGB_bmp), RGB_bmp_st_1, ref(YUV1), 1920, 1080 / 4, upos_st_1, vpos_st_1);
	thread t2(Bitmap2Yuv420p_v2, ref(RGB_bmp), RGB_bmp_st_2, ref(YUV1), 1920, 1080 / 4, upos_st_2, vpos_st_2);
	thread t3(Bitmap2Yuv420p_v2, ref(RGB_bmp), RGB_bmp_st_3, ref(YUV1), 1920, 1080 / 4, upos_st_3, vpos_st_3);

	t0.join();
	t1.join();
	t2.join();
	t3.join();
#endif // THREAD

#ifdef TEST_EN
	uint64_t n = test(RGB_bmp, YUV1, YUV_frameSize, 1920, 1080);
	free(RGB_bmp);
	free(YUV1);
	free(YUV2);
	free(u);
	free(v);

	return (n);
#endif // TEST_EN

	
	FILE* f = NULL;
	fopen_s(&f, NewYUV, "wb");
	if (f == NULL)
	{
		fprintf(stderr, "Cannot open yuvfile\n");
		return (0);
	}

	//читаем yuv и накладываем картинку
	//кол-во кадров можно посчитать, но для отладки так было удобнее.)
	while (i < 500)
	{
		readYUVFrame(yuvfile, YUV2, 1920, 1080, YUV_frameSize, i);
		for (uint64_t n = 0; n < YUV_frameSize; n++)
		{
			*(YUV2 + n) = (*(YUV2 + n) + *(YUV1 + n)) / 2;
		}
		fwrite(YUV2, sizeof(uint8_t), YUV_frameSize, f);
		i++;
	}
	fclose(f);

	free(RGB_bmp);
	free(YUV1);
	free(YUV2);
	free(u);
	free(v);
	*/


	Frame RGB_Frame(RGB_Width, RGB_Height, bitmapfile);
	Frame YUV_Frame(YUV_Width, YUV_Height, yuvfile);
	ImageProcessing RGB2YUV(YUV_Width, YUV_Height);

	uint8_t* bgr_frame = RGB_Frame.readBMP();

	uint8_t* yuv_from_rgb_frame = NULL;

#ifdef SMID
	yuv_from_rgb_frame = RGB2YUV.Bitmap2yuv_SMID(bgr_frame, 0, 0);
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

	thread t0(Bitmap2Yuv420p_v2, ref(bgr_frame), RGB_bmp_st_0, ref(yuv_from_rgb_frame), RGB_Width, RGB_Height / 4, upos_st_0, vpos_st_0);
	thread t1(Bitmap2Yuv420p_v2, ref(bgr_frame), RGB_bmp_st_1, ref(yuv_from_rgb_frame), RGB_Width, RGB_Height / 4, upos_st_1, vpos_st_1);
	thread t2(Bitmap2Yuv420p_v2, ref(bgr_frame), RGB_bmp_st_2, ref(yuv_from_rgb_frame), RGB_Width, RGB_Height / 4, upos_st_2, vpos_st_2);
	thread t3(Bitmap2Yuv420p_v2, ref(bgr_frame), RGB_bmp_st_3, ref(yuv_from_rgb_frame), RGB_Width, RGB_Height / 4, upos_st_3, vpos_st_3);

	t0.join();
	t1.join();
	t2.join();
	t3.join();
#endif // THREAD

#ifdef TEST_EN
	uint64_t n = test(bgr_frame, yuv_from_rgb_frame, YUV_Frame.YUVframeSize(), RGB_Width, RGB_Height);
	return (n);
#endif // TEST_EN

	//читаем yuv и накладываем картинку
	FILE* f = NULL;
	fopen_s(&f, NewYUV, "wb");
	if (f == NULL)
	{
		fprintf(stderr, "Cannot open yuvfile\n");
		return (0);
	}
	//кол-во кадров можно посчитать, но для отладки так было удобнее.)
	uint8_t* yuv_frame;
	uint64_t YUV_frameSize = YUV_Frame.YUVframeSize();
	while (i < 1)
	{
		yuv_frame = YUV_Frame.readYUV(i);
		for (uint64_t n = 0; n < YUV_frameSize; n++)
		{
			*(yuv_frame + n) = (*(yuv_frame + n) + *(yuv_from_rgb_frame + n)) / 2;
		}
		fwrite(yuv_frame, sizeof(uint8_t), YUV_frameSize, f);
		i++;
	}
	fclose(f);

#ifdef THREAD
	delete[] yuv_from_rgb_frame;
#endif // THREAD

    return (1);
}

