#include "stdafx.h"
#include "Frame.h"


Frame::Frame(uint32_t Width, uint32_t Height, const char* Filename, const char* mode)
{
	width = Width;
	height = Height;
	filename = Filename;
	rgb_frame_size = RGBframeSize();
	yuv_frame_size = YUVframeSize();
	f = NULL;

	if(mode == "rb")
		frame = new uint8_t[rgb_frame_size];

	fopen_s(&f, filename, mode);
	if (f == NULL)
		fprintf(stderr, "%s", "Cannot open file\n", Filename);
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

uint8_t Frame::readBMP()
{
	fseek(f, 54, SEEK_SET); //set pointer above header
	fread(frame, sizeof(uint8_t), rgb_frame_size, f); // read the rest of the data at once

	return (1);
}

uint8_t Frame::readYUVFrame()
{
	size_t res = fread(frame, sizeof(unsigned char), yuv_frame_size, f); // read the rest of the data at once
	return (res == yuv_frame_size);
}

uint8_t Frame::readYUVFile()
{
	size_t res = 0;
	do
	{
		res = fread(frame, sizeof(unsigned char), yuv_frame_size, f); // read the rest of the data at once
	} while (yuv_frame_size == res);
	return (1);
}

uint8_t Frame::writeYUVFrame(uint8_t* new_frame)
{
	fwrite(new_frame, sizeof(uint8_t), yuv_frame_size, f);
	return (1);
}

uint8_t* Frame::getFrame()
{
	return (frame);
}
