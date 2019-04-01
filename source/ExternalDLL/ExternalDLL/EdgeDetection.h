#pragma once

#include "IntensityImage.h"
#include <vector>
#include <array>

namespace EdgeDetection {
	typedef std::vector<std::vector<double>> imageVector;
	
	imageVector imageVectorFromIntensityImage(const IntensityImage& image);
	imageVector applyGaussian(const imageVector &image, double sigma);
	void sobelFilter(const imageVector &sourceImage, imageVector &destImage, imageVector &directionImage);
	void nonMaxSupp(imageVector &image, imageVector &directions);
	void doubleThreshold(imageVector &image, const Intensity& lowThreshold, const Intensity& highThreshold, const Intensity& strong, const Intensity& weak);
	void tracking(imageVector &image, const Intensity &strong, const Intensity &weak);
	void toHistogram(const imageVector &image, std::array<double, 256> &histogram);
	double otsu(const imageVector &image, std::array<double, 256>& histogram);
};
