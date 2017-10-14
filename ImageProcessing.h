#pragma once
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <thread>
#include "emmintrin.h"
#include "tmmintrin.h"


class ImageProcessing
{
private:
	uint8_t* YUV;
	uint8_t* u;
	uint8_t* v;

	uint16_t width;
	uint16_t height;
	uint16_t width1;
	uint16_t height1;

	uint64_t image_size;
	uint64_t uv_size;
	uint64_t YUV_frameSize;
	uint64_t image_size1;
	uint64_t uv_size1;
	uint64_t YUV_frameSize1;

public:
	ImageProcessing(uint16_t, uint16_t);
	ImageProcessing(uint16_t, uint16_t, uint16_t, uint16_t);
	~ImageProcessing();

	uint8_t Bitmap2Yuv420p(uint8_t *bgr, uint8_t *yuv, uint64_t bgr_offset, uint32_t upos_offset, uint32_t vpos_offset, uint16_t height_p_th);
	uint8_t ImageProcessing::Bitmap2yuv_THREAD(uint8_t *bgr);
	uint8_t Bitmap2yuv_SIMD(uint8_t *bgr, uint32_t uvpos);
	uint8_t FrameAdd(uint8_t *frame1, uint8_t *frame2);
	uint8_t FrameAdd_SIMD(uint8_t *frame1, uint8_t *frame2);
	uint8_t* GetYUV();
};

