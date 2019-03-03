#include "Kernel.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include "ImageFactory.h"
/*void Kernel::apply(const IntensityImage &sourceImage, IntensityImage *destinationImage, int kWidth, int kHeight, double kernel[5][5]) {
	 
}*/

void Kernel::createGuassianKernel(double GKernel[][5]) {
	// intialising standard deviation to 1.0 
	double sigma = 1.4;
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

void Kernel::sobelFilter(const IntensityImage &sourceImage, std::vector<std::vector<float>> &destImage, std::vector<std::vector<float>> &directionImage) {
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

	int kOffsetX = KERNEL_X / 2;
	int kOffsetY = KERNEL_Y / 2;

	for (int y = kOffsetY; y <= sourceImage.getHeight() - kOffsetY; ++y) {
		for (int x = kOffsetX; x <= sourceImage.getWidth() - kOffsetX; ++x) {

			int topLeftX = x - kOffsetX;
			int topLeftY = y - kOffsetY;

			for (int kernelY = 0; kernelY < KERNEL_Y; ++kernelY) {
				for (int kernelX = 0; kernelX < KERNEL_X; ++kernelX) {
					accumulatorVertical += sourceImage.getPixel(topLeftY + kernelY, topLeftX + kernelX) * kernelVertical[kernelY][kernelX];
					accumulatorHorizontal += sourceImage.getPixel(topLeftY + kernelY, topLeftX + kernelX) * kernelHorizontal[kernelY][kernelX];
				}
			}

			double gradient = std::sqrt((accumulatorHorizontal * accumulatorHorizontal) + (accumulatorVertical * accumulatorVertical));
			double direction = std::atan2(accumulatorVertical, accumulatorHorizontal);
			accumulatorVertical = 0;
			accumulatorHorizontal = 0;
			
			destImage[y][x] = gradient;
			directionImage[y][x] = direction;
		}
	}
	float max = destImage[0][0];
	float oldMax = max;


	for (const auto& row : destImage) {
		for (const auto& value : row) {
			oldMax = max;
			max = std::max(max, value);
			
		}
	}
	std::cout << oldMax << "\n";

	for (auto& row : destImage) {
		for (auto& value : row) {
			//value = value / max * 255);
			//std::cout << value << "\n";
		}
	}
}

double Kernel::getDegrees(double radians) {
	return (radians * 180) / 3.14;
}

double Kernel::roundAngle(double angleDeg) {
	if ((0 <= angleDeg && angleDeg < 22.5) || (157.5 <= angleDeg && angleDeg < 180)) {
		angleDeg = 0;
	} else if (22.5 <= angleDeg && angleDeg < 67.5) {
		angleDeg = 45;
	} else if (67.5 <= angleDeg && angleDeg < 112.5) {
		angleDeg = 90;
	} else if (112.5 <= angleDeg && angleDeg < 157.5) {
		angleDeg = 135;
	}
	return angleDeg;
}

void Kernel::nonMaxSupp(std::vector<std::vector<float>> &image, std::vector<std::vector<float>> &angle) {
	for (auto& row : angle) {
		for (auto& angle : row) {
			angle *= 180 / 3.14;
			if (angle < 0) {
				angle += 180;
			}
		}
	}

	auto height = image.size() > angle.size() ? image.size() : angle.size();
	auto width = image[0].size() > angle[0].size() ? image[0].size() : angle[0].size();
	std::vector<std::vector<float>> dest(image.size(), std::vector<float>(image.size(), 0));
	for (int y = 1; y < height; ++y) {
		for (int x = 1; x < width; ++x) {
			int q = 255;
			int r = 255;

			if ((0 <= angle[y][x] < 22.5) || (157.5 <= angle[y][x] <= 180)) {
				q = image[y][x + 1];
				r = image[y][x - 1];
			}
			//angle 45
			else if (22.5 <= angle[y][x] < 67.5) {
				q = image[y + 1][x - 1];
				r = image[y - 1][x + 1];

			}
			//angle 90
			else if (67.5 <= angle[y][x] < 112.5) {
				q = image[y + 1][x];
				r = image[y - 1][x];
			}
			//angle 135
			else if (112.5 <= angle[y][x] < 157.5) {
				q = image[y - 1][x - 1];
				r = image[y + 1][x + 1];
			}
			
			if(!(image[y][x] >= q) && (image[y][x] >= r)) {
				image[y][x] = 0;
			}
		}
	}

}

void Kernel::doubleThreshold(std::vector<std::vector<float>> &image) {
	double lowerThreshold = 70;
	double upperThreshold = 45;

	for (int y = 0; y < image.size(); ++y) {
		for (int x = 0; x < image[y].size(); ++x) {
			auto value = image[y][x];
			if (value > upperThreshold) {
				image[y][x] = 255;
			} else if (value >= lowerThreshold && value <= upperThreshold) {
				image[y][x] = 255;
			} else {
				image[y][x] = 0;
			}
		}
	}
}

void Kernel::tracking(IntensityImage &image, const Intensity &weak, const Intensity &strong) {
	for (int y = 1; y < image.getHeight(); ++y) {
		for (int x = 1; x < image.getWidth(); ++x) {
			auto value = image.getPixel(x, y);
			if (value == weak) {
				if (image.getPixel(x + 1, y) == strong 
					|| image.getPixel(x - 1, y) == strong
					|| image.getPixel(x, y + 1) == strong 
					|| image.getPixel(x, y - 1) == strong
					|| image.getPixel(x + 1, y + 1) == strong 
					|| image.getPixel(x - 1, y - 1) == strong) { 
							image.setPixel(x, y, strong);
				} else {
					image.setPixel(x, y, 0);
				}
			}
		}
	}
}
