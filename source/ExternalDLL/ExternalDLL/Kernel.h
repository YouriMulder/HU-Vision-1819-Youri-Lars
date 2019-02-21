#pragma once

#include "IntensityImage.h"

namespace Kernel {
	template<typename T>
	void apply(
		const IntensityImage &sourceImage,
		IntensityImage &destinationImage,
		int kWidth,
		int kHeight,
		T kernel
	) {
		/*
	 https://en.wikipedia.org/wiki/Kernel_(image_processing)
	 for each image row in input image:
		 for each pixel in image row:

			 set accumulator to zero

			 for each kernel row in kernel:
				 for each element in kernel row:

			 if element position  corresponding* to pixel position then
				 multiply element value  corresponding* to pixel value
				 add result to accumulator
			 endif

			 set output image pixel to accumulator
	 */

		double accumulator = 0;

		int kOffsetX = kWidth / 2;
		int kOffsetY = kHeight / 2;

		for (int y = kOffsetY; y <= sourceImage.getHeight() - kOffsetY; ++y) {
			for (int x = kOffsetX; x <= sourceImage.getWidth() - kOffsetX; ++x) {
				int topLeftX = x - kOffsetX;
				int topLeftY = y - kOffsetY;

				accumulator = 0;

				for (int kernelY = 0; kernelY < kHeight; ++kernelY) {
					for (int kernelX = 0; kernelX < kWidth; ++kernelX) {
						accumulator += sourceImage.getPixel(topLeftY + kernelY, topLeftX + kernelX) * kernel[kernelY][kernelX];
					}
				}

				destinationImage.setPixel(y, x, Intensity(accumulator));
			}
		}
	}
 
	void createGuassianKernel(double GKernel[][5]);
	void sobelFilter(const IntensityImage &sourceImage, IntensityImage &destImage);
};

