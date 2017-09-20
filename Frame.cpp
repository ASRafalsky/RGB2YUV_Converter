#include "stdafx.h"
#include "Frame.h"


Frame::Frame(uint32_t Width, uint32_t Height, const char* Filename)
{
	width = Width;
	height = Height;
	filename = Filename;
	rgb_frame_size = RGBframeSize();
	yuv_frame_size = YUVframeSize();
	f = NULL;
	frame = new uint8_t[rgb_frame_size];
}

Frame::~Frame()
{
	if(f)
		fclose(f);

	delete[] frame;
}

uint64_t Frame::RGBframeSize()
{
	return (imageSize() * 3);
}

uint64_t Frame::YUVframeSize()
{
	uint64_t imsize = imageSize();
	return (imsize + imsize / 2);
}

uint64_t Frame::imageSize()
{
	return (width * height);
}

uint8_t* Frame::readBMP()
{
	uint64_t i = 0;
	fopen_s(&f, filename, "rb");
	if (f == NULL)
	{
		fprintf(stderr, "Cannot open bitmapfile\n");
		return (0);
	}

	fseek(f, 54, SEEK_SET); //set pointer above header
	fread(frame, sizeof(uint8_t), rgb_frame_size, f); // read the rest of the data at once
	fclose(f);

	return (frame);
}

uint8_t* Frame::readYUV(uint16_t framenum)
{
	fopen_s(&f, filename, "rb");
	if (f == NULL)
	{
		fprintf(stderr, "Cannot open yuvfile\n");
		return (0);
	}
	uint64_t frameStartNum = framenum * yuv_frame_size;

	fseek(f, frameStartNum, SEEK_SET);
	fread(frame, sizeof(unsigned char), yuv_frame_size, f); // read the rest of the data at once
	fclose(f);

	return (frame);
}