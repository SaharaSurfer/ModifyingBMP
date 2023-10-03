#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <fstream>
#include <cmath>

#pragma pack(push, 1)
struct BMPFileHeader {
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

struct BMP {
	BMPFileHeader fileheader;

	unsigned char* read_file(const char* filename)
	{
		FILE* source_image = fopen(filename, "rb");
		if (!source_image)
		{
			std::cout << "File wasn't opened\n";
			return 0;
		}

		// Reading header
		size_t fheader_size = fread(&fileheader, sizeof(char), sizeof(BMPFileHeader), source_image);
		if (fheader_size != sizeof(BMPFileHeader))
		{
			fclose(source_image);
			std::cout << "BMPFileHeader was read wrong (size issue)!\n";
			return 0;
		}
		std::cout << fheader_size << " = size of header\n";

		int width = fileheader.width;
		int height = fileheader.height;
		//int padded_width_m3 = (3 * width + 3) & (-4);
		//std::cout << width << " - actual width, " << padded_width_m3 / 3 << " - corrected width\n";

		// Reading info about pixels.
		unsigned char* pixel_info = new unsigned char[3 * width * height];
		size_t bytes_read = fread(pixel_info, sizeof(char), 3 * width * height, source_image);
		if (bytes_read != 3 * width * height)
		{
			delete[] pixel_info;
			fclose(source_image);
			std::cout << bytes_read << " - bytes read, " << 3 * width * height << " bytes should be\n";
			std::cout << "Pixel information read wrongly\n";
			return 0;
		}
		fclose(source_image);
		return pixel_info;
	}

	unsigned char* write_file(const char* filename, BMPFileHeader fheader, unsigned char* data)
	{
		FILE* image_write = fopen(filename, "wb");
		if (!image_write)
		{
			std::cout << "File wasn't opened\n";
			return 0;
		}

		size_t fheader_size = fwrite(&fheader, sizeof(char), sizeof(BMPFileHeader), image_write);
		if (fheader_size != sizeof(BMPFileHeader))
		{
			fclose(image_write);
			std::cout << "Header is written wrongly\n";
			return 0;
		}

		//int padded_width_m3 = (3 * fheader.width + 3) & (-4); // для некратных 4

		size_t written_bytes = fwrite(data, sizeof(char), 3 * fheader.width * fheader.height, image_write);
		if (written_bytes != 3 * fheader.width * fheader.height)
		{
			std::cout << written_bytes << " / " << 3 * fheader.width * fheader.height << "bytes written\n";
			fclose(image_write);

		}
		else { std::cout << "File written successfully\n"; }

		fclose(image_write);
		return 0;
	}

	unsigned char* turn_left(BMPFileHeader fheader, unsigned char* data)
	{
		int w = fheader.width, h = fheader.height;
		unsigned char* new_data = new unsigned char[3 * w * h];
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				new_data[(y * w + x) * 3] = data[((w - 1 - x) * h + y) * 3];
				new_data[(y * w + x) * 3 + 1] = data[((w - 1 - x) * h + y) * 3 + 1];
				new_data[(y * w + x) * 3 + 2] = data[((w - 1 - x) * h + y) * 3 + 2];
			}
		}

		return new_data;
	}

	unsigned char* turn_right(BMPFileHeader fheader, unsigned char* data)
	{
		int w = fheader.width, h = fheader.height;
		unsigned char* new_data = new unsigned char[3 * w * h];
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				new_data[(y * w + x) * 3] = data[(x * h + h - 1 - y) * 3];
				new_data[(y * w + x) * 3 + 1] = data[(x * h + h - 1 - y) * 3 + 1];
				new_data[(y * w + x) * 3 + 2] = data[(x * h + h - 1 - y) * 3 + 2];
			}
		}

		return new_data;
	}

	unsigned char* gaussian_blur(BMPFileHeader fheader, unsigned char* data)
	{
		const int radius = 10, matrix_size = 2 * radius + 1;
		const double pi = 3.14159, sigma = radius / 3;
		double sum = 0.0;

		// Filling the filter
		double Gaus_Kernel[matrix_size][matrix_size];
		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				Gaus_Kernel[y + radius][x + radius] = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * pi * sigma * sigma);
				sum += Gaus_Kernel[y + radius][x + radius];
			}
		}
		std::cout << "Gauss matrix sum: " << sum << std::endl;
		// Normalizing matrix
		for (int y = 0; y < matrix_size; y++)
		{
			for (int x = 0; x < matrix_size; x++)
			{
				Gaus_Kernel[y][x] /= sum;
			}
		}

		// Applying the filter
		unsigned char* new_data = new unsigned char[3 * fheader.width * fheader.height];
		for (size_t y = 0; y < fheader.height; y++)
		{
			for (size_t x = 0; x < fheader.width; x++)
			{
				double r = 0, g = 0, b = 0;
				for (int shift_h = 0; shift_h < matrix_size; shift_h++)
				{
					for (int shift_w = 0; shift_w < matrix_size; shift_w++)
					{
						if (y + shift_h - radius > 0 and y + shift_h - radius < fheader.height and x + shift_w - radius > 0 and x + shift_w - radius < fheader.width)
						{
							r += Gaus_Kernel[shift_h][shift_w] * data[((y + shift_h - radius) * fheader.width + x + shift_w - radius) * 3];
							g += Gaus_Kernel[shift_h][shift_w] * data[((y + shift_h - radius) * fheader.width + x + shift_w - radius) * 3 + 1];
							b += Gaus_Kernel[shift_h][shift_w] * data[((y + shift_h - radius) * fheader.width + x + shift_w - radius) * 3 + 2];
						}
						else
						{
							r += Gaus_Kernel[shift_h][shift_w] * data[(y * fheader.width + x) * 3];
							g += Gaus_Kernel[shift_h][shift_w] * data[(y * fheader.width + x) * 3 + 1];
							b += Gaus_Kernel[shift_h][shift_w] * data[(y * fheader.width + x) * 3 + 2];
						}
					}
				}
				new_data[(y * fheader.width + x) * 3] = (int)r;
				new_data[(y * fheader.width + x) * 3 + 1] = (int)g;
				new_data[(y * fheader.width + x) * 3 + 2] = (int)b;
			}
		}

		return new_data;
	}

};

int main() {
	BMP image;
	unsigned char* pixel_info = image.read_file("scenery.bmp");
	std::cout << image.fileheader.file_size << " bytes = file size\n";

	//Rotating image to the left
	BMPFileHeader leftR_header = image.fileheader;
	std::swap(leftR_header.width, leftR_header.height);
	unsigned char* leftR_data = image.turn_left(leftR_header, pixel_info);
	image.write_file("Turned_left.bmp", leftR_header, leftR_data);
	delete[] leftR_data;

	//Rotating image to the right
	BMPFileHeader rightR_header = image.fileheader;
	std::swap(rightR_header.width, rightR_header.height);
	unsigned char* rightR_data = image.turn_right(rightR_header, pixel_info);
	image.write_file("Turned_right.bmp", rightR_header, rightR_data);
	delete[] rightR_data;

	//Putting on gauss filter
	BMPFileHeader gaus_header = image.fileheader;
	unsigned char* gaus_data = image.gaussian_blur(gaus_header, pixel_info);
	image.write_file("Gaussian_filter.bmp", image.fileheader, gaus_data);
	delete[] gaus_data;

	delete[] pixel_info;
	return 0;
}