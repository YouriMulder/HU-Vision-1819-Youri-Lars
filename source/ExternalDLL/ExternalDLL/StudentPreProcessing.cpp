#include "StudentPreProcessing.h"

#include "ImageFactory.h"
#include "Kernel.h"

#include <cmath>
#include <iostream>
#include <array>

IntensityImage * StudentPreProcessing::stepToIntensityImage(const RGBImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepScaleImage(const IntensityImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepEdgeDetection(const IntensityImage &image) const {
	// input scaled intensity image 
	// output Edge-detected image

	IntensityImage* edgeImage = ImageFactory::newIntensityImage(image);

	// gausian filter
	double guassian[5][5];
	Kernel::createGuassianKernel(guassian, 0.33f);
	//Kernel::apply(image, *edgeImage, 5, 5, guassian);
	
	std::vector<std::vector<float>> picture(edgeImage->getHeight(), std::vector<float>(edgeImage->getWidth(), 0));
	std::vector<std::vector<float>> directions(edgeImage->getHeight(), std::vector<float>(edgeImage->getWidth(), 0));

	Kernel::sobelFilter(*edgeImage, picture, directions);
	std::array<float, 256> histogram;

	Kernel::toHistogram(picture, histogram);
	const Intensity strong = 255;
	const Intensity weak = 100;
	const Intensity highTres = Kernel::otsu(picture, histogram);
	const Intensity lowTres = highTres/2;

	std::cout << "Treshold : " << double(highTres) << "\n";
	//Kernel::nonMaxSupp(picture, directions);
	Kernel::doubleThreshold(picture, lowTres, highTres, strong, weak);
	Kernel::tracking(picture, weak, strong);

	IntensityImage* img = ImageFactory::newIntensityImage(picture[0].size(), picture.size());
	for (int y = 0; y < img->getHeight(); ++y) {
		for (int x = 0; x < img->getWidth(); ++x) {
			img->setPixel(x, y, picture[y][x]);
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