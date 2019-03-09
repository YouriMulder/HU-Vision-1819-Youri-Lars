#include "Kernel.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <array>

#include "ImageFactory.h"
/*void Kernel::apply(const IntensityImage &sourceImage, IntensityImage *destinationImage, int kWidth, int kHeight, double kernel[5][5]) {
	 
}*/

void Kernel::createGuassianKernel(double GKernel[][5], float sigma) {
	// intialising standard deviation to 1.0 
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

	float max = 0;

	for (int y = 1; y < sourceImage.getHeight()-1; ++y) {
		for (int x = 1; x < sourceImage.getWidth()-1; ++x) {
			accumulatorVertical = 0;
			accumulatorHorizontal = 0;
			int topLeftX = x - kOffsetX;
			int topLeftY = y - kOffsetY;

			for (int kernelY = 0; kernelY < KERNEL_Y; ++kernelY) {
				for (int kernelX = 0; kernelX < KERNEL_X; ++kernelX) {
					accumulatorVertical += sourceImage.getPixel(topLeftX + kernelX, topLeftY + kernelY) * kernelVertical[kernelY][kernelX];
					accumulatorHorizontal += sourceImage.getPixel(topLeftX + kernelX, topLeftY + kernelY) * kernelHorizontal[kernelY][kernelX];
				}
			}

			float gradient = std::sqrt((accumulatorHorizontal * accumulatorHorizontal) + (accumulatorVertical * accumulatorVertical));
			destImage[y][x] = gradient;
			
			max = destImage[y][x] > max ? destImage[y][x] : max;
			directionImage[y][x] = std::atan2(accumulatorVertical, accumulatorHorizontal);
		}
	}

	for (auto& row : destImage) {
		for (auto& value : row) {
			value = 255.0f * value / max;
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

	auto height = image.size() > angle.size() ? image.size() : angle.size();
	auto width = image[0].size() > angle[0].size() ? image[0].size() : angle[0].size();
	std::vector<std::vector<float>> dest(image.size(), std::vector<float>(image[0].size(), 0));
	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width - 1; ++x) {
			float theta = getDegrees(angle[y][x]) + 180;
			//std::cout << theta << "\n";
			// 0 degrees
			if ((theta >= 0 && theta <= 22.5) ||
				(theta >= 337.5 && theta <= 360)) {
				if ( (image[y][x] >= image[y][x - 1]) && (image[y][x] >= image[y][x + 1]) ) {
					dest[y][x] = image[y][x];
				}
			}
			// 45 degrees
			else if ((theta >= 22.5 && theta <= 67.5) ||
				(theta >= 202.5 && theta <= 247.5)) {
				if ((image[y][x] >= image[y - 1][x+1]) && (image[y][x] >= image[y+1][x-1])) {
					dest[y][x] = image[y][x];
				}
			}
			// 90 degrees
			else if ((theta >= 67.5 && theta <= 112.5) ||
				(theta >= 247.5 && theta <= 292.5)) {
				if ((image[y][x] >= image[y - 1][x]) && (image[y][x] >= image[y + 1][x])) {
					dest[y][x] = image[y][x];
				}
			}
			// 135 degrees
			else if ((theta >= 112.5 && theta <= 157.5) ||
				(theta >= 292.5 && theta <= 337.5)) {
				if ((image[y][x] >= image[y-1][x+1]) && (image[y][x] >= image[y + 1][x + 1])) {
					dest[y][x] = image[y][x];
				}
			}

		}
	}
	image = dest;
}

void Kernel::doubleThreshold(std::vector<std::vector<float>> &image, const Intensity& lowThreshold, const Intensity& highThreshold, const Intensity& strong, const Intensity& weak) {
	for (int y = 0; y < image.size(); ++y) {
		for (int x = 0; x < image[y].size(); ++x) {
			auto value = image[y][x];
			if (value > highThreshold) {
				image[y][x] = strong;
			} else if (value >= lowThreshold && value <= highThreshold) {
				image[y][x] = weak;
			} else {
				image[y][x] = 0;
			}
		}
	}
}

void Kernel::tracking(std::vector<std::vector<float>> &image, const Intensity &weak, const Intensity &strong) {
	for (int y = 1; y < image.size() - 1; ++y) {
		for (int x = 1; x < image[y].size() - 1; ++x) {
			auto value = image[y][x];
			if (value == weak) {
				if (image[y][x + 1] == strong 
					|| image[y][x - 1] == strong
					|| image[y + 1][x] == strong
					|| image[y - 1][x] == strong
					|| image[y + 1][x + 1] == strong
					|| image[y - 1][x - 1] == strong) {
							image[y][x] = strong;
				} else {
					image[y][x] = 0;
				}
			}
		}
	}
}

void Kernel::toHistogram(const std::vector<std::vector<float>> &image, std::array<float, 256>& histogram) {
	histogram.fill(0);
	for (const auto row : image) {
		for (const int value : row) {
			++histogram[value];
		}
	}
}

double Kernel::otsu(const std::vector<std::vector<float>> &image, std::array<float, 256>& histogram) {
	int threshold = 0;

	int total = image.size() * image[0].size();
	
	float sum = 0;
	float sumB = 0;
	int wB = 0;
	float maximum = 0;

	for (int i = 0; i < histogram.size(); ++i) {
		sum += i * histogram[i];
	}

	for (int i = 0; i < histogram.size(); ++i) {
		wB += histogram[i];
		int wF = total - wB;
		if (wB == 0 || wF == 0) continue;

		sumB += i * histogram[i];
		auto mF = (sum - sumB) / wF;
		float between = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
		if (between >= maximum) {
			threshold = i;
			maximum = between;
		}
	}
	
	return threshold;
}
