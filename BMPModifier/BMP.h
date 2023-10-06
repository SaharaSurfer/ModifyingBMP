#pragma once

#pragma pack(push, 1)
struct BMPFileHeader 
{
	uint16_t file_type{ 0x4D42 };
	uint32_t file_size{ 0 };
	uint16_t reserved1{ 0 };
	uint16_t reserved2{ 0 };
	uint32_t offset_data{ 0 };

	uint32_t size{ 0 };
	int32_t width{ 0 };
	int32_t height{ 0 };
	uint16_t planes{ 1 };
	uint16_t bit_count{ 0 };
	uint32_t compression{ 0 };
	uint32_t size_image{ 0 };
	int32_t x_pixels_per_meter{ 0 };
	int32_t y_pixels_per_meter{ 0 };
	uint32_t colors_used{ 0 };
	uint32_t colors_important{ 0 };

	uint32_t red_mask{ 0x00ff0000 };
	uint32_t green_mask{ 0x0000ff00 };
	uint32_t blue_mask{ 0x000000ff };
	uint32_t alpha_mask{ 0xff000000 };
	uint32_t color_space_type{ 0x73524742 };
	uint32_t unused[16]{ 0 };
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

	unsigned char* read_file(const char* filename);

	unsigned char* write_file(const char* filename);

	BMP* turn_left();

	BMP* turn_right();
	
	BMP* gaussian_blur();

	~BMP() 
	{
		delete[] pixel_info;
	}
};