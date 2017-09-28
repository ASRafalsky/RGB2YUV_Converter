#include "improc.h"

static uint8_t y_calc(uint8_t *bgr, uint64_t bgr_offset, uint8_t *y, uint16_t width, uint16_t height)
{
	uint64_t i = bgr_offset;
	for (uint64_t line = 0; line < height; ++line)
	{
			for (uint64_t x = 0; x < width; x += 1)
			{
				uint8_t r = bgr[3 * i + 2];
				uint8_t g = bgr[3 * i + 1];
				uint8_t b = bgr[3 * i];

				y[i++] = ((77 * r + 150 * g + 29 * b) >> 8);
			}
	}
	return (1);
}

static int uv_calc(uint8_t *bgr, uint64_t bgr_offset, uint8_t *uv, uint64_t upos_offset, uint64_t vpos_offset, uint32_t width, uint32_t height)
{
	uint64_t i = bgr_offset;
	for (uint32_t line = 0; line < height; line += 2)
	{
		for (uint32_t x = 0; x < width; x += 2)
		{
			uint8_t r = bgr[3 * i + 2];
			uint8_t g = bgr[3 * i + 1];
			uint8_t b = bgr[3 * i];

			uv[upos_offset++] = (((-43 * r + -84 * g + 127 * b) >> 8) + 128);
			uv[vpos_offset++] = (((127 * r + -106 * g + -21 * b) >> 8) + 128);
			i += 2;
		}
		i += width;
	}
	return (1);
}

/*uint8_t Bitmap2Yuv420p(uint8_t *bgr, uint64_t bgr_offset, uint8_t *yuv, uint16_t width, uint16_t height, uint32_t upos_offset, uint32_t vpos_offset)
{
	y_calc(bgr, bgr_offset, yuv, width, height);
	uv_calc(bgr, bgr_offset, yuv, upos_offset, vpos_offset, width, height);
	return (1);
}*/

uint8_t Bitmap2Yuv420p_v2(uint8_t *bgr, uint64_t bgr_offset, uint8_t *yuv, uint16_t width, uint16_t height, uint32_t upos_offset, uint32_t vpos_offset)
{
	uint32_t i = bgr_offset;
	for (uint16_t line = 0; line < height; line += 2)
	{
		for (uint16_t x = 0; x < width; x += 2)
		{
			uint8_t r = bgr[3 * i + 2];
			uint8_t g = bgr[3 * i + 1];
			uint8_t b = bgr[3 * i];

			yuv[i++] = ((77 * r + 150 * g + 29 * b) >> 8);

			yuv[upos_offset++] = (((-43 * r + -84 * g + 127 * b) >> 8) + 128);
			yuv[vpos_offset++] = (((127 * r + -106 * g + -21 * b) >> 8) + 128);

			r = bgr[3 * i + 2];
			g = bgr[3 * i + 1];
			b = bgr[3 * i];

			yuv[i++] = ((77 * r + 150 * g + 29 * b) >> 8);
		}
		for (uint16_t x = 0; x < width; x += 1)
		{
			uint8_t r = bgr[3 * i + 2];
			uint8_t g = bgr[3 * i + 1];
			uint8_t b = bgr[3 * i];

			yuv[i++] = ((77 * r + 150 * g + 29 * b) >> 8);
		}
	}
	return (1);
}

uint8_t Bitmap2yuv_SMID(uint8_t *bgr, uint8_t *y, uint8_t *u, uint8_t *v, uint64_t imsize, uint64_t uv_size, uint64_t YUV_frameSize, uint16_t width, uint16_t height, uint32_t upos, uint32_t vpos)
{
	uint64_t i = 0;

	__m128i r2y = _mm_set1_epi16(77);
	__m128i g2y = _mm_set1_epi16(150);
	__m128i b2y = _mm_set1_epi16(29);

	__m128i r2u = _mm_set1_epi16(-43);
	__m128i g2u = _mm_set1_epi16(-84);
	__m128i b2u = _mm_set1_epi16(127);

	__m128i r2v = _mm_set1_epi16(127);
	__m128i g2v = _mm_set1_epi16(-106);
	__m128i b2v = _mm_set1_epi16(-21);

	__m128i uv_bias = _mm_set1_epi16(128);

	__m128i ssse3_blue_indeces_0 = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 9, -1, 6, -1, 3, -1, 0);
	__m128i ssse3_blue_indeces_1 = _mm_set_epi8(-1, 9, -1,  6, -1,  3, -1,  0, -1, -1, -1, -1,-1, -1,-1, -1);
	__m128i ssse3_green_indeces_0 = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, 7, -1, 4, -1, 1);
	__m128i ssse3_green_indeces_1 = _mm_set_epi8(-1, 10, -1, 7,	 -1, 4,	 -1, 1,  -1, -1, -1,-1,	-1,	-1,-1,-1);
	__m128i ssse3_red_indeces_0	  = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 11, -1, 8, -1, 5, -1, 2);
	__m128i ssse3_red_indeces_1   = _mm_set_epi8(-1, 11, -1, 8, -1, 5, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1);
	__m128i ssse3_yout = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 14, 12, 10, 8, 6, 4, 2, 0);
	__m128i ssse3_uvout = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0);

	for (uint16_t line = 0; line < height; line += 2)
	{
			for (uint16_t x = 0; x < width; x += 8)
			{
				__m128i chunk0 = _mm_loadu_si128((__m128i*)bgr);
				__m128i chunk1 = _mm_loadu_si128((__m128i*)(bgr + 12));

				bgr += 24;

				__m128i red_chunk = _mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_red_indeces_0), _mm_shuffle_epi8(chunk1, ssse3_red_indeces_1));
				__m128i green_chunk = _mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_green_indeces_0), _mm_shuffle_epi8(chunk1, ssse3_green_indeces_1));
				__m128i blue_chunk = _mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_blue_indeces_0), _mm_shuffle_epi8(chunk1, ssse3_blue_indeces_1));

				_mm_storeu_si128((__m128i*)(y + i),
					(_mm_shuffle_epi8((_mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(red_chunk, r2y), _mm_mullo_epi16(green_chunk, g2y)), _mm_mullo_epi16(blue_chunk, b2y)), 8)), ssse3_yout)));

				_mm_storeu_si128((__m128i*)(u + upos),
					(_mm_shuffle_epi8(_mm_add_epi16(_mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(red_chunk, r2u), _mm_mullo_epi16(green_chunk, g2u)), _mm_mullo_epi16(blue_chunk, b2u)), 8), uv_bias), ssse3_uvout)));
				upos += 4;

				_mm_storeu_si128((__m128i*)(v + vpos),
					(_mm_shuffle_epi8(_mm_add_epi16(_mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(red_chunk, r2v), _mm_mullo_epi16(green_chunk, g2v)), _mm_mullo_epi16(blue_chunk, b2v)), 8), uv_bias), ssse3_uvout)));
				vpos += 4;

				i += 8;
			}
			for (uint16_t x = 0; x < width; x += 8)
			{
				__m128i chunk0 = _mm_loadu_si128((__m128i*)bgr);
				__m128i chunk1 = _mm_loadu_si128((__m128i*)(bgr + 12));

				bgr += 24;

				__m128i red_chunk = _mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_red_indeces_0), _mm_shuffle_epi8(chunk1, ssse3_red_indeces_1));
				__m128i green_chunk = _mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_green_indeces_0), _mm_shuffle_epi8(chunk1, ssse3_green_indeces_1));
				__m128i blue_chunk = _mm_or_si128(_mm_shuffle_epi8(chunk0, ssse3_blue_indeces_0), _mm_shuffle_epi8(chunk1, ssse3_blue_indeces_1));
				
				_mm_storeu_si128((__m128i*)(y + i),
					(_mm_shuffle_epi8((_mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(red_chunk, r2y), _mm_mullo_epi16(green_chunk, g2y)), _mm_mullo_epi16(blue_chunk, b2y)), 8)), ssse3_yout)));

				i += 8;
			}
		}
	memcpy_s(y + imsize, YUV_frameSize, u, uv_size);
	memcpy_s(y + imsize + uv_size, YUV_frameSize, v, uv_size);

	return (1);
}
