#pragma once

#pragma pack(push, 1)
struct BMPFileHeader 
{
	uint16_t file_type{ 0x4D42 };
	uint32_t file_size{ 0 };

	uint16_t unused_1[2]{ 0 };
	uint32_t unused_2[2]{ 0 };
	
	int32_t width{ 0 };
	int32_t height{ 0 };
	
	uint16_t unused_3[2]{ 1, 0 };
	uint32_t unused_4[2]{ 0 };
	int32_t unused_5[2]{ 0 };
	uint32_t unused_6[23]{ 0 };
};
#pragma pack(pop)

struct BMP 
{
	BMPFileHeader fileheader;
	unsigned char* pixel_info;

	BMP(BMPFileHeader fheader, unsigned char* pixel_data)
	{
		fileheader = fheader;
		pixel_info = pixel_data;
	}
	BMP() {}

	int read_file(const char* filename);

	int write_file(const char* filename);

	BMP turn_left();

	BMP turn_right();
	
	BMP gaussian_blur();

	~BMP() 
	{
		delete[] pixel_info;
	}
};