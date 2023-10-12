#include <iostream>
#include "BMP.h"

int main() {
	BMP image = BMP();
	image.read_file("test1.bmp");
	std::cout << image.fileheader.file_size << " bytes = file size\n";
	
	//Rotating image to the left
	BMP turned_left = image.turn_left();
	turned_left.write_file("Turned_left.bmp");

	//Rotating image to the right
	BMP turned_right = image.turn_right();
	turned_right.write_file("Turned_right.bmp");

	//Putting on gauss filter
	BMP gaussian_filter = image.gaussian_blur(10);
	gaussian_filter.write_file("Gaussian_filter.bmp");

	return 0;
}
