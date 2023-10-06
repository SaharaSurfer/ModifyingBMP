#include <iostream>
#include "BMP.h"

int main() {
	BMP image;
	unsigned char* pixel_info = image.read_file("test1.bmp");
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
