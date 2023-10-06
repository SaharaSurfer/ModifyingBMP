#include <iostream>
#include "BMP.h"

int main() {
	BMP image;
	image.read_file("test3.bmp");
	std::cout << image.fileheader.file_size << " bytes = file size\n";
	
	//Rotating image to the left
	BMP turned_left = image.turn_left();
	turned_left.write_file("Turned_left.bmp");
	//delete& turned_left;

	//Rotating image to the right
	BMP turned_right = image.turn_right();
	turned_right.write_file("Turned_right.bmp");
	//delete& turned_right;

	//Putting on gauss filter
	BMP gaussian_filter = image.gaussian_blur();
	gaussian_filter.write_file("Gaussian_filter.bmp");
	//delete& gaussian_filter;

	//delete& image;

	return 0;
}
