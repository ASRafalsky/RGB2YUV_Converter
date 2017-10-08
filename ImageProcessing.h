#pragma once
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <stdint.h>
#include <queue>
#include <vector>
#include <mutex>
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

	std::vector<uint8_t> vYUV;
	std::vector<uint8_t> vu;
	std::vector<uint8_t> vv;

	std::queue <std::vector<uint8_t>> qYUV;
	std::recursive_mutex mtx;

public:
	ImageProcessing(uint16_t, uint16_t);
	ImageProcessing(uint16_t, uint16_t, uint16_t, uint16_t);
	~ImageProcessing();

	void Bitmap2Yuv420p(uint8_t *bgr, uint8_t *yuv, uint64_t bgr_offset, uint32_t upos_offset, uint32_t vpos_offset, uint16_t height_p_th);
	uint8_t Bitmap2yuv_SIMD(uint8_t *bgr, uint32_t upos, uint32_t vpos);
	uint8_t FrameAdd(uint8_t *frame1, uint8_t *frame2);
	uint8_t FrameAdd_SIMD(uint8_t *frame1, uint8_t *frame2);
	uint8_t* GetYUV();
	uint8_t GetYUV(std::vector<uint8_t> &v);
};

