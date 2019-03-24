#include "Kernel.h"

#include "ImageFactory.h"

#include <vector>

std::vector<std::vector<double>> Kernel::applyGuassian(const IntensityImage &image) {
	const int kernelSize = 5;
	double gaussianKernel[kernelSize][kernelSize] = {};
	const double pi = 3.14159265358979323846;
	// set standard deviation to 1.0
	double sigma = 1.6;
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
	std::vector<std::vector<double>> output(image.getHeight() + 1 - kernelSize, std::vector<double>(image.getWidth() + 1 - kernelSize, 0));
	for(int pixelX = size; pixelX < image.getWidth() - size; ++pixelX) {
		for(int pixelY = size; pixelY < image.getHeight() - size; ++pixelY) {
			double sum = 0;

			for (int x = 0; x < kernelSize; ++x) {
				for (int y = 0; y < kernelSize; ++y) {
					sum += gaussianKernel[x][y] * (double)(image.getPixel(pixelX + x - size, pixelY + y - size));
				}
			}
			output[pixelY - size][pixelX - size] = sum;
		}
	}
	return std::move(output);
}

void Kernel::sobelFilter(const std::vector<std::vector<double>> &sourceImage, std::vector<std::vector<double>> &destImage, std::vector<std::vector<double>> &directionImage) {
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

	double max = 0;

	for (int y = 1; y < sourceImage.size() - 1; ++y) {
		for (int x = 1; x < sourceImage[0].size() - 1; ++x) {
			accumulatorVertical = 0;
			accumulatorHorizontal = 0;
			int topLeftX = x - kOffsetX;
			int topLeftY = y - kOffsetY;

			for (int kernelY = 0; kernelY < KERNEL_Y; ++kernelY) {
				for (int kernelX = 0; kernelX < KERNEL_X; ++kernelX) {
					accumulatorVertical += sourceImage[topLeftY + kernelY][topLeftX + kernelX] * kernelVertical[kernelY][kernelX];
					accumulatorHorizontal += sourceImage[topLeftY + kernelY][topLeftX + kernelX] * kernelHorizontal[kernelY][kernelX];
				}
			}

			double gradient = std::sqrt((accumulatorHorizontal * accumulatorHorizontal) + (accumulatorVertical * accumulatorVertical));
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

void Kernel::nonMaxSupp(std::vector<std::vector<double>> &image, std::vector<std::vector<double>> &angle) {

	auto height = image.size() > angle.size() ? image.size() : angle.size();
	auto width = image[0].size() > angle[0].size() ? image[0].size() : angle[0].size();
	std::vector<std::vector<double>> dest(image.size(), std::vector<double>(image[0].size(), 0));
	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width - 1; ++x) {
			double theta = (angle[y][x] * 180) / 3.14 + 180;
			//std::cout << theta << "\n";
			// 0 degrees
			if ((theta >= 0 && theta <= 22.5) ||
				(theta >= 337.5 && theta <= 360)) {
				if ((image[y][x] >= image[y][x - 1]) && (image[y][x] >= image[y][x + 1])) {
					dest[y][x] = image[y][x];
				}
			}
			// 45 degrees
			else if ((theta >= 22.5 && theta <= 67.5) ||
				(theta >= 202.5 && theta <= 247.5)) {
				if ((image[y][x] >= image[y - 1][x + 1]) && (image[y][x] >= image[y + 1][x - 1])) {
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
				if ((image[y][x] >= image[y - 1][x + 1]) && (image[y][x] >= image[y + 1][x + 1])) {
					dest[y][x] = image[y][x];
				}
			}

		}
	}
	image = dest;
}

void Kernel::doubleThreshold(std::vector<std::vector<double>> &image, const Intensity& lowThreshold, const Intensity& highThreshold, const Intensity& strong, const Intensity& weak) {
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

void Kernel::tracking(std::vector<std::vector<double>> &image, const Intensity &weak, const Intensity &strong) {
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

void Kernel::toHistogram(const std::vector<std::vector<double>> &image, std::array<double, 256>& histogram) {
	histogram.fill(0);
	for (const auto row : image) {
		for (const int value : row) {
			++histogram[value];
		}
	}
}

double Kernel::otsu(const std::vector<std::vector<double>> &image, std::array<double, 256>& histogram) {
	int threshold = 0;

	int total = image.size() * image[0].size();

	double sum = 0;
	double sumB = 0;
	int wB = 0;
	double maximum = 0;

	for (int i = 0; i < histogram.size(); ++i) {
		sum += i * histogram[i];
	}

	for (int i = 0; i < histogram.size(); ++i) {
		wB += histogram[i];
		int wF = total - wB;
		if (wB == 0 || wF == 0) continue;

		sumB += i * histogram[i];
		auto mF = (sum - sumB) / wF;
		double between = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
		if (between >= maximum) {
			threshold = i;
			maximum = between;
		}
	}

	return threshold;
}
