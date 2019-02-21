#include "Kernel.h"

#include <cmath>
#include <iomanip>
#include <iostream>

#include "ImageFactory.h"
/*void Kernel::apply(const IntensityImage &sourceImage, IntensityImage *destinationImage, int kWidth, int kHeight, double kernel[5][5]) {
	 
}*/

void Kernel::createGuassianKernel(double GKernel[][5]) {
	// intialising standard deviation to 1.0 
	double sigma = 1.5;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization 
	double sum = 0.0;

	// generating 5x5 kernel 
	for (int x = -2; x <= 2; x++) {
		for (int y = -2; y <= 2; y++) {
			r = std::sqrt(x * x + y * y);
			GKernel[x + 2][y + 2] = (std::exp(-(r * r) / s)) / (3.1415296 * s);
			sum += GKernel[x + 2][y + 2];
		}
	}

	// normalising the Kernel 
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
			GKernel[i][j] /= sum;
}

void Kernel::sobelFilter(const IntensityImage &sourceImage, IntensityImage &destImage) {
	for (int y = 0; y < sourceImage.getHeight(); ++y) {
		for (int x = 0; x < sourceImage.getWidth(); ++x) {
			destImage.setPixel(x, y, 0);
		}
	}
	
	int convRow = 0, convCol = 0;
	int kernelRow = 0, kernelCol = 0;
	int convDstRow = 1, convDstCol = 1;
	int gradient = 0;

	int kernelAreaDotProd_X = 0;
	int kernelAreaDotProd_Y = 0;

	const int KERNEL_X = 3;
	const int KERNEL_Y = 3;

	const int kernelVertical[KERNEL_X][KERNEL_Y] = {
		{ -1, 0, 1 },
		{ -2, 0, 2 },
		{ -1, 0, 1 }
	}; // initialize sobel vertical kernel

	const int kernelHorizontal[KERNEL_X][KERNEL_Y] = {
		{ -1, -2, -1 },
		{ 0, 0, 0 },
		{ 1, 2, 1 }
	}; // initialize sobel horizontal kernel

	double accumulatorVertical = 0;
	double accumulatorHorizontal = 0;

	int maxX = 0;
	int maxY = 0;

	int kOffsetX = KERNEL_X / 2;
	int kOffsetY = KERNEL_Y / 2;

	for (int y = kOffsetY; y <= sourceImage.getHeight() - kOffsetY; ++y) {
		for (int x = kOffsetX; x <= sourceImage.getWidth() - kOffsetX; ++x) {
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;

			int topLeftX = x - kOffsetX;
			int topLeftY = y - kOffsetY;

			for (int kernelY = 0; kernelY < KERNEL_Y; ++kernelY) {
				for (int kernelX = 0; kernelX < KERNEL_X; ++kernelX) {
					accumulatorVertical += sourceImage.getPixel(topLeftY + kernelY, topLeftX + kernelX) * kernelVertical[kernelY][kernelX];
					accumulatorHorizontal += sourceImage.getPixel(topLeftY + kernelY, topLeftX + kernelX) * kernelHorizontal[kernelY][kernelX];
				}
			}

			Intensity gradient = std::sqrt((accumulatorHorizontal * accumulatorHorizontal) + (accumulatorVertical * accumulatorVertical));
			accumulatorVertical = 0;
			accumulatorHorizontal = 0;
			if (gradient > 70) {
				gradient = 255;
			} else {
				gradient = 0;
			}
			
			destImage.setPixel(y, x, gradient);
		}
	}
}