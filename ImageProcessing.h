#pragma once
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <stdint.h>
#include "emmintrin.h"
#include "tmmintrin.h"

using namespace std;

class ImageProcessing
{
private:
	uint8_t* YUV;
	uint8_t* u;
	uint8_t* v;

	uint16_t width;
	uint16_t height;

	uint64_t image_size;
	uint64_t uv_size;
	uint64_t YUV_frameSize;

public:
	ImageProcessing(uint16_t, uint16_t);
	~ImageProcessing();

	void Bitmap2Yuv420p(uint8_t *bgr, uint8_t *yuv, uint64_t bgr_offset, uint32_t upos_offset, uint32_t vpos_offset, uint16_t height_p_th);
	uint8_t Bitmap2yuv_SMID(uint8_t *bgr, uint32_t upos, uint32_t vpos);
	static uint8_t FrameAdd(uint8_t *frame1, uint64_t frame1_size, uint8_t *frame2, uint64_t frame2_size);
	static uint8_t FrameAdd_SMID(uint8_t *frame1, uint64_t frame1_size, uint8_t *frame2, uint64_t frame2_size);
	uint8_t* GetYUV();
};

