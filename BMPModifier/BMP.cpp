#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <fstream>
#include <cmath>
#include "BMP.h"

unsigned char* BMP::read_file(const char* filename)
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
	int padding = (4 - width * 3 % 4) % 4; // In BMP images the line width is rounded to multiples of 4

	// Reading info about pixels.
	unsigned char* pixel_info = new unsigned char[(3 * width + padding) * height];
	size_t bytes_read = fread(pixel_info, sizeof(char), (3 * width + padding) * height, source_image);
	if (bytes_read != (3 * width + padding) * height)
	{
		delete[] pixel_info;
		fclose(source_image);
		std::cout << bytes_read << " - bytes read, " << (3 * width + padding) * height << " bytes should be\n";
		std::cout << "Pixel information read wrongly\n";
		return 0;
	}
	fclose(source_image);
	return pixel_info;
}

unsigned char* BMP::write_file(const char* filename, BMPFileHeader fheader, unsigned char* data)
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

	int padding = (4 - fheader.width * 3 % 4) % 4;

	size_t written_bytes = fwrite(data, sizeof(char), (3 * fheader.width + padding) * fheader.height, image_write);
	if (written_bytes != (3 * fheader.width + padding) * fheader.height)
	{
		std::cout << written_bytes << " / " << (3 * fheader.width + padding) * fheader.height << "bytes written\n";
		fclose(image_write);
	}
	else
	{
		std::cout << "File written successfully\n";
	}

	fclose(image_write);
	return 0;
}

unsigned char* BMP::turn_left(BMPFileHeader fheader, unsigned char* data)
{
	int w = fheader.width, h = fheader.height;
	int padding_curr = (4 - 3 * w % 4) % 4, padding_source = (4 - 3 * h % 4) % 4;
	unsigned char* new_data = new unsigned char[(3 * w + padding_curr) * h];
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			new_data[(y * w + x) * 3 + y * padding_curr] = data[((w - 1 - x) * h + y) * 3 + (w - 1 - x) * padding_source];
			new_data[(y * w + x) * 3 + 1 + y * padding_curr] = data[((w - 1 - x) * h + y) * 3 + 1 + (w - 1 - x) * padding_source];
			new_data[(y * w + x) * 3 + 2 + y * padding_curr] = data[((w - 1 - x) * h + y) * 3 + 2 + (w - 1 - x) * padding_source];
		}
	}

	return new_data;
}

unsigned char* BMP::turn_right(BMPFileHeader fheader, unsigned char* data)
{
	int w = fheader.width, h = fheader.height;
	int padding_curr = (4 - 3 * w % 4) % 4, padding_source = (4 - 3 * h % 4) % 4;
	unsigned char* new_data = new unsigned char[(3 * w + padding_curr) * h];
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			new_data[(y * w + x) * 3 + y * padding_curr] = data[(x * h + h - 1 - y) * 3 + x * padding_source];
			new_data[(y * w + x) * 3 + 1 + y * padding_curr] = data[(x * h + h - 1 - y) * 3 + 1 + x * padding_source];
			new_data[(y * w + x) * 3 + 2 + y * padding_curr] = data[(x * h + h - 1 - y) * 3 + 2 + x * padding_source];
		}
	}

	return new_data;
}

unsigned char* BMP::gaussian_blur(BMPFileHeader fheader, unsigned char* data)
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
	int padding = (4 - 3 * fheader.width % 4) % 4;
	unsigned char* new_data = new unsigned char[(3 * fheader.width + padding) * fheader.height];
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
						r += Gaus_Kernel[shift_h][shift_w] * data[((y + shift_h - radius) * fheader.width + x + shift_w - radius) * 3 + padding * (y + shift_h - radius)];
						g += Gaus_Kernel[shift_h][shift_w] * data[((y + shift_h - radius) * fheader.width + x + shift_w - radius) * 3 + 1 + padding * (y + shift_h - radius)];
						b += Gaus_Kernel[shift_h][shift_w] * data[((y + shift_h - radius) * fheader.width + x + shift_w - radius) * 3 + 2 + padding * (y + shift_h - radius)];
					}
					else
					{
						r += Gaus_Kernel[shift_h][shift_w] * data[(y * fheader.width + x) * 3 + y * padding];
						g += Gaus_Kernel[shift_h][shift_w] * data[(y * fheader.width + x) * 3 + 1 + y * padding];
						b += Gaus_Kernel[shift_h][shift_w] * data[(y * fheader.width + x) * 3 + 2 + y * padding];
					}
				}
			}
			new_data[(y * fheader.width + x) * 3 + y * padding] = (int)r;
			new_data[(y * fheader.width + x) * 3 + 1 + y * padding] = (int)g;
			new_data[(y * fheader.width + x) * 3 + 2 + y * padding] = (int)b;
		}
	}

	return new_data;
}