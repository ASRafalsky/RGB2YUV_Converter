#include "stdafx.h"
#include "ImageProcessing.h"


ImageProcessing::ImageProcessing(uint16_t Width, uint16_t Height)
{
	width = Width;
	height = Height;

	image_size = width * height;
	uv_size = image_size / 4;

	YUV_frameSize = image_size + image_size / 2;

	/*YUV = new uint8_t[YUV_frameSize];
	u = new uint8_t[uv_size + 12];
	v = new uint8_t[uv_size + 12];*/

	vYUV.reserve(YUV_frameSize);
	vu.reserve(uv_size + 12);
	vv.reserve(uv_size + 12);

	vYUV.resize(YUV_frameSize);
	vu.resize(uv_size + 12);
	vv.resize(uv_size + 12);

	YUV = vYUV.data();
	u = vu.data();
	v = vv.data();
}

ImageProcessing::ImageProcessing(uint16_t Width1, uint16_t Height1, uint16_t Width2, uint16_t Height2)
{
	width = Width1;
	height = Height1;

	width1 = Width2;
	height1 = Height2;

	image_size = width * height;
	image_size1 = width1 * height1;
	uv_size = image_size / 4;
	uv_size1 = image_size1 / 4;

	YUV_frameSize = image_size + image_size / 2;
	YUV_frameSize1 = image_size1 + image_size1 / 2;

	if (image_size > image_size1)
	{
		/*YUV = new uint8_t[YUV_frameSize];
		u = new uint8_t[uv_size + 8];
		v = new uint8_t[uv_size + 8];*/

		vYUV.reserve(YUV_frameSize);
		vu.reserve(uv_size + 8);
		vv.reserve(uv_size + 8);

		vYUV.resize(YUV_frameSize);
		vu.resize(uv_size + 8);
		vv.resize(uv_size + 8);

		YUV = vYUV.data();
		u = vu.data();
		v = vv.data();
	}
	else
	{
		/*YUV = new uint8_t[YUV_frameSize1];
		u = new uint8_t[uv_size1 + 8];
		v = new uint8_t[uv_size1 + 8];*/

		vYUV.reserve(YUV_frameSize1);
		vu.reserve(uv_size1 + 8);
		vv.reserve(uv_size1 + 8);

		vYUV.resize(YUV_frameSize1);
		vu.resize(uv_size1 + 8);
		vv.resize(uv_size1 + 8);

		YUV = vYUV.data();
		u = vu.data();
		v = vv.data();
	}
}

ImageProcessing::~ImageProcessing()
{
	delete[] YUV;
	delete[] u;
	delete[] v;
}

void ImageProcessing::Bitmap2Yuv420p(uint8_t* bgr, uint8_t* yuv, uint64_t bgr_offset, uint32_t upos_offset, uint32_t vpos_offset, uint16_t height_p_th)
{
	uint64_t i = bgr_offset;
	for (uint16_t line = 0; line < height_p_th; line += 2)
	{
		for (uint16_t x = 0; x < width; x += 2)
		{
			uint8_t r = *(bgr + 3 * i + 2);
			uint8_t g = *(bgr + 3 * i + 1);
			uint8_t b = *(bgr + 3 * i);

			yuv[i++] = ((77 * r + 150 * g + 29 * b) >> 8);

			yuv[upos_offset++] = (((-43 * r + -84 * g + 127 * b) >> 8) + 128);
			yuv[vpos_offset++] = (((127 * r + -106 * g + -21 * b) >> 8) + 128);

			r = *(bgr + 3 * i + 2);
			g = *(bgr + 3 * i + 1);
			b = *(bgr + 3 * i);

			yuv[i++] = ((77 * r + 150 * g + 29 * b) >> 8);
		}
		for (uint16_t x = 0; x < width; x += 1)
		{
			uint8_t r = *(bgr + 3 * i + 2);
			uint8_t g = *(bgr + 3 * i + 1);
			uint8_t b = *(bgr + 3 * i);

			yuv[i++] = ((77 * r + 150 * g + 29 * b) >> 8);
		}
	}
}

uint8_t ImageProcessing::Bitmap2yuv_SIMD(uint8_t *bgr, uint32_t upos, uint32_t vpos)
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
	__m128i ssse3_blue_indeces_1 = _mm_set_epi8(-1, 9, -1, 6, -1, 3, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1);
	__m128i ssse3_green_indeces_0 = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, 7, -1, 4, -1, 1);
	__m128i ssse3_green_indeces_1 = _mm_set_epi8(-1, 10, -1, 7, -1, 4, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1);
	__m128i ssse3_red_indeces_0 = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, 11, -1, 8, -1, 5, -1, 2);
	__m128i ssse3_red_indeces_1 = _mm_set_epi8(-1, 11, -1, 8, -1, 5, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1);
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

			_mm_storeu_si128((__m128i*)(YUV + i),
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

			_mm_storeu_si128((__m128i*)(YUV + i),
				(_mm_shuffle_epi8((_mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(_mm_mullo_epi16(red_chunk, r2y), _mm_mullo_epi16(green_chunk, g2y)), _mm_mullo_epi16(blue_chunk, b2y)), 8)), ssse3_yout)));

			i += 8;
		}
	}
	memcpy_s(YUV + image_size, YUV_frameSize, u, uv_size);
	memcpy_s(YUV + image_size + uv_size, YUV_frameSize, v, uv_size);

	return (1);
}

uint8_t ImageProcessing::FrameAdd(uint8_t *frame1, uint8_t *frame2)
{
	uint64_t cnt = image_size;
	uint64_t uv_cnt = 0;
	uint64_t out_frame_size = YUV_frameSize1;
	uint64_t out_uv_size = uv_size1;

	uint64_t v_off1 = image_size + uv_size;
	uint64_t v_off2 = image_size1 + uv_size1;

	if (image_size > image_size1)
	{
		cnt = image_size1;
		out_frame_size = YUV_frameSize;
		out_uv_size = uv_size;
	}

	for (uint64_t n = 0; n < cnt; n += 4)
	{
		*(YUV + n) = (*(frame1 + n) + *(frame2 + n)) / 2;
		*(YUV + n + 1) = (*(frame1 + n + 1) + *(frame2 + n + 1)) / 2;
		*(YUV + n + 2) = (*(frame1 + n + 2) + *(frame2 + n + 2)) / 2;
		*(YUV + n + 3) = (*(frame1 + n + 3) + *(frame2 + n + 3)) / 2;

		*(YUV + cnt + uv_cnt) = (*(frame1 + image_size + uv_cnt) + *(frame2 + image_size1 + uv_cnt)) / 2;
		*(YUV + cnt + out_uv_size + uv_cnt) = (*(frame1 + v_off1 + uv_cnt) + *(frame2 + v_off2 + uv_cnt)) / 2;

		++uv_cnt;
	}
	return (1);
}

uint8_t ImageProcessing::FrameAdd_SIMD(uint8_t *frame1, uint8_t *frame2)
{
	uint64_t cnt = image_size;
	uint64_t uv_cnt = 0;
	uint64_t out_frame_size = YUV_frameSize1;
	uint64_t out_uv_size = uv_size1;

	uint64_t v_off1 = image_size + uv_size;
	uint64_t v_off2 = image_size1 + uv_size1;

	__m128i zero = _mm_set1_epi8(0);
	__m128i ssse3_youtl = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, 14, 12, 10, 8, 6, 4, 2, 0);

	if (image_size > image_size1)
	{
		cnt = image_size1;
		out_frame_size = YUV_frameSize;
		out_uv_size = uv_size;
	}

	for (uint64_t n = 0; n < cnt; n += 64)
	{
		__m128i y_chunk00 = _mm_loadu_si128((__m128i*)(frame1 + n));
		__m128i y_chunk01 = _mm_loadu_si128((__m128i*)(frame1 + n + 16));
		__m128i y_chunk02 = _mm_loadu_si128((__m128i*)(frame1 + n + 32));
		__m128i y_chunk03 = _mm_loadu_si128((__m128i*)(frame1 + n + 48));

		__m128i y_chunk10 = _mm_loadu_si128((__m128i*)(frame2 + n));
		__m128i y_chunk11 = _mm_loadu_si128((__m128i*)(frame2 + n + 16));
		__m128i y_chunk12 = _mm_loadu_si128((__m128i*)(frame2 + n + 32));
		__m128i y_chunk13 = _mm_loadu_si128((__m128i*)(frame2 + n + 48));

		__m128i u_chunk1 = _mm_loadu_si128((__m128i*)(frame1 + image_size + uv_cnt));

		__m128i u_chunk2 = _mm_loadu_si128((__m128i*)(frame2 + image_size1 + uv_cnt));

		__m128i v_chunk1 = _mm_loadu_si128((__m128i*)(frame1 + v_off1 + uv_cnt));

		__m128i v_chunk2 = _mm_loadu_si128((__m128i*)(frame2 + v_off2 + uv_cnt));

		_mm_storeu_si128((__m128i*)(YUV + n), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpacklo_epi8(y_chunk00, zero), _mm_unpacklo_epi8(y_chunk10, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(YUV + 8 + n), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpackhi_epi8(y_chunk00, zero), _mm_unpackhi_epi8(y_chunk10, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(YUV + 16 + n), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpacklo_epi8(y_chunk01, zero), _mm_unpacklo_epi8(y_chunk11, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(YUV + 24 + n), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpackhi_epi8(y_chunk01, zero), _mm_unpackhi_epi8(y_chunk11, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(YUV + 32 + n), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpacklo_epi8(y_chunk02, zero), _mm_unpacklo_epi8(y_chunk12, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(YUV + 40 + n), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpackhi_epi8(y_chunk02, zero), _mm_unpackhi_epi8(y_chunk12, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(YUV + 48 + n), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpacklo_epi8(y_chunk03, zero), _mm_unpacklo_epi8(y_chunk13, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(YUV + 56 + n), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpackhi_epi8(y_chunk03, zero), _mm_unpackhi_epi8(y_chunk13, zero)), 1), ssse3_youtl)));

		_mm_storeu_si128((__m128i*)(u + uv_cnt), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpacklo_epi8(u_chunk1, zero), _mm_unpacklo_epi8(u_chunk2, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(u + 8 + uv_cnt), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpackhi_epi8(u_chunk1, zero), _mm_unpackhi_epi8(u_chunk2, zero)), 1), ssse3_youtl)));

		_mm_storeu_si128((__m128i*)(v + uv_cnt), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpacklo_epi8(v_chunk1, zero), _mm_unpacklo_epi8(v_chunk2, zero)), 1), ssse3_youtl)));
		_mm_storeu_si128((__m128i*)(v + 8 + uv_cnt), (_mm_shuffle_epi8(_mm_srli_epi16(_mm_add_epi16(_mm_unpackhi_epi8(v_chunk1, zero), _mm_unpackhi_epi8(v_chunk2, zero)), 1), ssse3_youtl)));

		uv_cnt += 16;
	}
	memcpy_s(YUV + cnt, out_frame_size, u, out_uv_size);
	memcpy_s(YUV + cnt + out_uv_size, out_frame_size, v, out_uv_size);
	return (1);
}

uint8_t* ImageProcessing::GetYUV()
{
	return (YUV);
}

uint8_t ImageProcessing::GetYUV(std::vector<uint8_t> &v)
{
	std::lock_guard<std::recursive_mutex> locker(mtx);

	if (qYUV.empty())
	{
		return (0);
	}

	v = qYUV.front();
	qYUV.pop();

	return (1);
}