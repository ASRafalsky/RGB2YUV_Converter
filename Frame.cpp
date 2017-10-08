#include "stdafx.h"
#include "Frame.h"


Frame::Frame(uint32_t Width, uint32_t Height, const char* Filename, const char* type, const char* mode)
{
	width = Width;
	height = Height;
	filename = Filename;
	rgb_frame_size = RGBframeSize();
	yuv_frame_size = YUVframeSize();
	f = NULL;

	if (mode == "rb")
	{
		if (type == "rgb")
		{
			vIn.reserve(rgb_frame_size);
			vIn.resize(rgb_frame_size);
		}
		else if (type == "yuv")
		{
			vIn.reserve(yuv_frame_size);
			vIn.resize(yuv_frame_size);
		}
		else
		{
			;
		}

		frame = new uint8_t[rgb_frame_size];
	}
	else
	{
		;
	}

	fopen_s(&f, filename, mode);
	if (f == NULL)
	{
		fprintf(stderr, "%s", "Cannot open file\n", Filename);
	}
	else
	{
		;
	}
		
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
	std::lock_guard<std::recursive_mutex> locker(mtx);
	
	fseek(f, 54, SEEK_SET); //set pointer above header
	//fread(frame, sizeof(uint8_t), rgb_frame_size, f); // read the rest of the data at once
	fread(&vIn[0], sizeof(uint8_t), rgb_frame_size, f); // read the rest of the data at once
	
	qFrame.push(vIn);

	return (1);
}

uint8_t Frame::readYUVFrame(uint64_t framenum)
{
	std::lock_guard<std::recursive_mutex> locker(mtx);
	uint64_t frameStartNum = framenum * yuv_frame_size;
	fseek(f, frameStartNum, SEEK_SET);
	//size_t res = fread(frame, sizeof(unsigned char), yuv_frame_size, f); // read the rest of the data at once
	size_t res = fread(&vIn[0], sizeof(unsigned char), yuv_frame_size, f); // read the rest of the data at once

	qFrame.push(vIn);

	if (res != yuv_frame_size)
	{
		fprintf(stdout, "End of file was reached\n");
		fclose(f);
		return (0);
	}
	return (1);
}

uint8_t Frame::readYUVFile()
{
	size_t res = 0;
	uint64_t frameStartNum = 0;
	uint64_t framenum = 0;

	do
	{
		frameStartNum = framenum * yuv_frame_size;
		++framenum;
		fseek(f, frameStartNum, SEEK_SET);
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

uint8_t Frame::getFrame(std::vector<uint8_t> &v)
{
	std::lock_guard<std::recursive_mutex> locker(mtx);

	if (qFrame.empty())
	{
		return (0);
	}

	v = qFrame.front();
	qFrame.pop();

	return (1);
}