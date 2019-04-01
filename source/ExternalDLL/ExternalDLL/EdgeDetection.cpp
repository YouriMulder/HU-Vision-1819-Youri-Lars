#include "EdgeDetection.h"

#include "IntensityImage.h"

#include <vector>
#include <iostream>
#include <cmath>


EdgeDetection::imageVector EdgeDetection::imageVectorFromIntensityImage(const IntensityImage& image) {
	EdgeDetection::imageVector output(image.getHeight(), std::vector<double>(image.getWidth(), 0));
	for (int pixelY = 0; pixelY < image.getHeight(); ++pixelY) {
		for (int pixelX = 0; pixelX < image.getWidth(); ++pixelX) {
			output[pixelY][pixelX] = image.getPixel(pixelX, pixelY);
		}
	}
	return std::move(output);
}

EdgeDetection::imageVector EdgeDetection::applygaussian(const EdgeDetection::imageVector &image) {
	const int kernelSize = 5;
	double gaussianKernel[kernelSize][kernelSize] = {};
	const double pi = 3.14159265358979323846;
	// set standard deviation to 1.0
	double sigma = 1;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization
	double sum = 0.0;

	// generate 5x5 kernel
	for (int x = -2; x <= 2; x++) {
		for (int y = -2; y <= 2; y++) {
			r = sqrt(x*x + y * y);
			gaussianKernel[x + 2][y + 2] = (exp(-(r*r) / s)) / (pi * s);
			sum += gaussianKernel[x + 2][y + 2];
		}
	}

	// normalize the Kernel
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			gaussianKernel[i][j] /= sum;
		}
	}

	int size = kernelSize/2;
	EdgeDetection::imageVector output(image.size() + 1 - kernelSize, std::vector<double>(image[0].size() + 1 - kernelSize, 0));
	for(int pixelY = size; pixelY < image.size() - size; ++pixelY) {
		for(int pixelX = size; pixelX < image[pixelY].size() - size; ++pixelX) {
			double sum = 0;

			for (int x = 0; x < kernelSize; ++x) {
				for (int y = 0; y < kernelSize; ++y) {
					sum += gaussianKernel[x][y] * image[pixelY + y - size][pixelX + x - size];
				}
			}
			output[pixelY - size][pixelX - size] = sum;
		}
	}
	return std::move(output);
}

void EdgeDetection::sobelFilter(const EdgeDetection::imageVector &sourceImage, EdgeDetection::imageVector &destImage, EdgeDetection::imageVector &directionImage) {
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

	double Gy = 0;
	double Gx = 0;

	int kOffsetX = KERNEL_X / 2;
	int kOffsetY = KERNEL_Y / 2;

	double max = 0;

	for (int y = 1; y < sourceImage.size() - 1; ++y) {
		for (int x = 1; x < sourceImage[0].size() - 1; ++x) {
			Gy = 0;
			Gx = 0;
			int topLeftX = x - kOffsetX;
			int topLeftY = y - kOffsetY;

			for (int kernelY = 0; kernelY < KERNEL_Y; ++kernelY) {
				for (int kernelX = 0; kernelX < KERNEL_X; ++kernelX) {
					Gy += sourceImage[topLeftY + kernelY][topLeftX + kernelX] * kernelVertical[kernelY][kernelX];
					Gx += sourceImage[topLeftY + kernelY][topLeftX + kernelX] * kernelHorizontal[kernelY][kernelX];
				}
			}

			double gradient = std::sqrt((Gx * Gx) + (Gy * Gy));
			destImage[y][x] = gradient;

			max = destImage[y][x] > max ? destImage[y][x] : max;
			directionImage[y][x] = std::atan2(Gy, Gx) ;
		}
	}

	for (auto& row : destImage) {
		for (auto& value : row) {
			value = 255.0f * value / max;
		}
	}
}

void EdgeDetection::nonMaxSupp(EdgeDetection::imageVector &image, EdgeDetection::imageVector &angle) {

	auto height = image.size() > angle.size() ? image.size() : angle.size();
	auto width = image[0].size() > angle[0].size() ? image[0].size() : angle[0].size();
	auto dest = image;
	int min = 0;
	int max = 0;
	for (int y = 1; y < dest.size() - 1; ++y) {
		for (int x = 1; x < dest[y].size() - 1; ++x) {
			int angleDeg = (angle[y][x] * 180 / 3.14);
			if (angleDeg < 0) {
				angleDeg += 180;
			}
			if (angleDeg > max) {
				max = angleDeg;
			}
			if (angleDeg < min) {
				min = angleDeg;
			}

			// horizontal
			if ((angleDeg >= 0 && angleDeg <= 22.5) || (angleDeg >= 157.5 && angleDeg <= 180)) {
				if (image[y - 1][x] > image[y][x] || image[y + 1][x] > image[y][x]) {
					dest[y][x] = 0;
				}
			// vertical
			} else if (angleDeg >= 67.5 && angleDeg <= 112.5) {
				if (image[y][x - 1] > image[y][x] || image[y][x + 1] > image[y][x]) {
					dest[y][x] = 0;
				}
				
			// diagonal left
			} else if (angleDeg >= 112.5 && angleDeg <= 157.5) {
				if (image[y + 1][x - 1] > image[y][x] || image[y - 1][x + 1] > image[y][x]) {
					dest[y][x] = 0;
				}
			// diagonal right
			} else if (angleDeg >= 22.5 && angleDeg <= 67.5) {
				if (image[y - 1][x - 1] > image[y][x] || image[y + 1][x + 1] > image[y][x]) {
					dest[y][x] = 0;
				}
			}
		}
	}

	image = dest;
}

void EdgeDetection::doubleThreshold(EdgeDetection::imageVector &image, const Intensity& lowThreshold, const Intensity& highThreshold, const Intensity& strong, const Intensity& weak) {
	for (int y = 0; y < image.size(); ++y) {
		for (int x = 0; x < image[y].size(); ++x) {
			auto value = image[y][x];
			if (value > highThreshold) {
				image[y][x] = strong;
			}
			else if (value >= lowThreshold && value <= highThreshold) {
				image[y][x] = weak;
			}
			else {
				image[y][x] = 0;
			}
		}
	}
}

void EdgeDetection::tracking(EdgeDetection::imageVector &image, const Intensity &strong, const Intensity &weak) {
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
				}
				else {
					image[y][x] = 0;
				}
			}
		}
	}
}

void EdgeDetection::toHistogram(const EdgeDetection::imageVector &image, std::array<double, 256>& histogram) {
	histogram.fill(0);
	for (const auto row : image) {
		for (const int value : row) {
			++histogram[value];
		}
	}
}

double EdgeDetection::otsu(const EdgeDetection::imageVector &image, std::array<double, 256>& histogram) {
	int threshold = 0;

	int total = image.size() * image[0].size();

	double totalIntensity = 0;
	double totalIntensityBackground = 0;
	int weightBackground = 0;
	double maximum = 0;

	for (int i = 0; i < histogram.size(); ++i) {
		totalIntensity += i * histogram[i];
	}

	for (int i = 0; i < histogram.size(); ++i) {
		weightBackground += histogram[i];
		int weightForeground = total - weightBackground;
		if (weightBackground == 0 || weightForeground == 0) continue;

		totalIntensityBackground += i * histogram[i];
		auto meanForeground = (totalIntensity - totalIntensityBackground) / weightForeground;
		double between = weightBackground * weightForeground * ((totalIntensityBackground / weightBackground) - meanForeground) * ((totalIntensityBackground / weightBackground) - meanForeground);
		if (between >= maximum) {
			threshold = i;
			maximum = between;
		}
	}

	return threshold;
}
