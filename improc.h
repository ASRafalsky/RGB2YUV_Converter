#ifndef __IMPROC_H__

#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <stdint.h>
#include <vector>
#include "emmintrin.h"
#include "tmmintrin.h"

//uint8_t Bitmap2Yuv420p(uint8_t *bgr, uint64_t bgr_offset, uint8_t *yuv, uint16_t width, uint16_t height, uint32_t upos_offset, uint32_t vpos_offset);

uint8_t Bitmap2Yuv420p_v2(uint8_t *bgr, uint64_t bgr_offset, uint8_t *yuv, uint16_t width, uint16_t height, uint32_t upos_offset, uint32_t vpos_offset);

uint8_t Bitmap2yuv_SMID(uint8_t *bgr, uint8_t *y, uint8_t *u, uint8_t *v, uint64_t imsize, uint64_t uv_size, uint64_t YUV_frameSize, uint16_t width, uint16_t height, uint32_t upos, uint32_t vpos);

#endif /* __IMPROC_H__ */