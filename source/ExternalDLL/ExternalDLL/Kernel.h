#pragma once

#include "IntensityImage.h"
#include <vector>
#include <array>

namespace Kernel {
	std::vector<std::vector<double>> applyGuassian(const IntensityImage &image);
	void sobelFilter(const std::vector<std::vector<double>> &sourceImage, std::vector<std::vector<double>> &destImage, std::vector<std::vector<double>> &directionImage);
	void nonMaxSupp(std::vector<std::vector<double>> &image, std::vector<std::vector<double>> &directions);
	void doubleThreshold(std::vector<std::vector<double>> &image, const Intensity& lowThreshold, const Intensity& highThreshold, const Intensity& strong, const Intensity& weak);
	void tracking(std::vector<std::vector<double>> &image, const Intensity &weak, const Intensity &strong);
	void toHistogram(const std::vector<std::vector<double>> &image, std::array<double, 256> &histogram);
	double otsu(const std::vector<std::vector<double>> &image, std::array<double, 256>& histogram);

};
