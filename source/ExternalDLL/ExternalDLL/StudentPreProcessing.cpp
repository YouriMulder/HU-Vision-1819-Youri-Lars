#include "StudentPreProcessing.h"

#include "ImageFactory.h"
#include "EdgeDetection.h"

#include <vector>
#include <array>

IntensityImage * StudentPreProcessing::stepToIntensityImage(const RGBImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepScaleImage(const IntensityImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepEdgeDetection(const IntensityImage &image) const {
	auto initialImage = EdgeDetection::imageVectorFromIntensityImage(image);
	
	// test settings
	//double sigma = 1; double tresholdFactor = 1;
	//double sigma = 1; double tresholdFactor = 0.5;
	//double sigma = 1; double tresholdFactor = 0.25;
	
	//double sigma = 1.5; double tresholdFactor = 1;
	//double sigma = 1.5; double tresholdFactor = 0.5;
	//double sigma = 1.5; double tresholdFactor = 0.25;
	
	//double sigma = 2; double tresholdFactor = 1;
	//double sigma = 2; double tresholdFactor = 0.5;
	double sigma = 0.5; double tresholdFactor = 0.25;


	// gaussian
	EdgeDetection::imageVector gaussian = EdgeDetection::applyGaussian(initialImage, sigma);
	
	// sobel
	EdgeDetection::imageVector directions(gaussian.size(), std::vector<double>(gaussian[0].size(), 0));
	EdgeDetection::imageVector sobel(gaussian.size(), std::vector<double>(gaussian[0].size(), 0));
	EdgeDetection::sobelFilter(gaussian, sobel, directions);

	// non max
	EdgeDetection::nonMaxSupp(sobel, directions);
	
	const Intensity strong = 255;
	const Intensity weak = 100;
	
	std::array<double, 256> histogram;
	EdgeDetection::toHistogram(sobel, histogram);
	
	const Intensity highTres = EdgeDetection::otsu(sobel, histogram) * tresholdFactor;

	
	const Intensity lowTres = highTres * 0.5;
	//const Intensity highTres = 25;
	//const Intensity lowTres = 10;
	EdgeDetection::doubleThreshold(sobel, lowTres, highTres, strong, weak);
	
	EdgeDetection::tracking(sobel, strong, weak);

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