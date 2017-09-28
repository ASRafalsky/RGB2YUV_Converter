#pragma once
#include <iostream>
using namespace std;

class Frame
{
private:
	uint32_t width;
	uint32_t height;
	uint64_t rgb_frame_size;
	uint64_t yuv_frame_size;
	uint8_t *frame;
	const char *filename;
	FILE* f;

public:
	Frame(uint32_t, uint32_t, const char*);
	~Frame();

	uint64_t RGBframeSize();
	uint64_t YUVframeSize();
	uint64_t imageSize();

	uint8_t* readBMP();
	uint8_t* readYUV(uint64_t framenum);
};

