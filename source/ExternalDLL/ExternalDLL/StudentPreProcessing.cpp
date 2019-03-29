#include "StudentPreProcessing.h"

#include "ImageFactory.h"
#include "Kernel.h"

#include <iostream>

IntensityImage * StudentPreProcessing::stepToIntensityImage(const RGBImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepScaleImage(const IntensityImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepEdgeDetection(const IntensityImage &image) const {
	// gaussian
	std::vector<std::vector<double>> guassian = Kernel::applyGuassian(image);
	
	// sobel
	std::vector<std::vector<double>> directions(guassian.size(), std::vector<double>(guassian[0].size(), 0));
	std::vector<std::vector<double>> sobel(guassian.size(), std::vector<double>(guassian[0].size(), 0));
	Kernel::sobelFilter(guassian, sobel, directions);

	// non max
	Kernel::nonMaxSupp(sobel, directions);
	
	const Intensity strong = 255;
	const Intensity weak = 100;
	
	std::array<double, 256> histogram;
	Kernel::toHistogram(sobel, histogram);
	const Intensity highTres = Kernel::otsu(sobel, histogram) * 0.25;
	const Intensity lowTres = highTres * 0.5;
	//const Intensity highTres = 25;
	//const Intensity lowTres = 10;
	Kernel::doubleThreshold(sobel, lowTres, highTres, strong, weak);
	
	Kernel::tracking(sobel, strong, weak);

	IntensityImage* img = ImageFactory::newIntensityImage(sobel[0].size(), sobel.size());
	for (int y = 0; y < img->getHeight(); ++y) {
		for (int x = 0; x < img->getWidth(); ++x) {
			img->setPixel(x, y, sobel[y][x]);
		}
	}
	return img;
}

IntensityImage * StudentPreProcessing::stepThresholding(const IntensityImage &image) const {
	IntensityImage* thresHolding = ImageFactory::newIntensityImage(image);
	
	for (int y = 0; y < thresHolding->getHeight(); ++y) {
		for (int x = 0; x < thresHolding->getWidth(); ++x) {
			thresHolding->setPixel(x, y,
				std::abs(thresHolding->getPixel(x, y) - 255)
			);
		}
	}
	return thresHolding;
}