#include "StudentPreProcessing.h"

#include "ImageFactory.h"
#include "Kernel.h"

#include <cmath>
#include <iostream>

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
	Kernel::createGuassianKernel(guassian);
	Kernel::apply(image, *edgeImage, 5, 5, guassian);
	
	std::vector<std::vector<float>> picture(edgeImage->getHeight(), std::vector<float>(edgeImage->getWidth(), 0));
	std::vector<std::vector<float>> directions(edgeImage->getHeight(), std::vector<float>(edgeImage->getWidth(), 0));

	Kernel::sobelFilter(*edgeImage, picture, directions);
	Kernel::nonMaxSupp(picture, directions);
	Kernel::doubleThreshold(picture);
	//Kernel::tracking(*nonMaxSuppImage, 50, 255);

	IntensityImage* img = ImageFactory::newIntensityImage(picture[0].size(), picture.size());
	for (int y = 0; y < img->getHeight(); ++y) {
		for (int x = 0; x < img->getWidth(); ++x) {
			img->setPixel(x, y, picture[x][y]);
		}
	}
	return img;
}

IntensityImage * StudentPreProcessing::stepThresholding(const IntensityImage &image) const {
	return nullptr;
}