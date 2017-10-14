#pragma once
#include <fstream>
#include <iostream>

class Frame
{
private:
	uint32_t width;
	uint32_t height;
	uint64_t rgb_frame_size;
	uint64_t yuv_frame_size;
	const char *filename;
	FILE* f;
	uint8_t* frame;

public:
	Frame(uint32_t, uint32_t, const char*, const char*);
	~Frame();

	uint64_t RGBframeSize();
	uint64_t YUVframeSize();
	uint64_t imageSize();

	uint8_t readBMP();
	uint8_t readYUVFile();
	uint8_t readYUVFrame();
	uint8_t writeYUVFrame(uint8_t* new_frame);

	uint8_t* getFrame();
};

