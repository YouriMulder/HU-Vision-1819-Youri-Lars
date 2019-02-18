#include "StudentPreProcessing.h"

#include "ImageFactory.h"
#include "Kernel.h"

IntensityImage * StudentPreProcessing::stepToIntensityImage(const RGBImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepScaleImage(const IntensityImage &image) const {
	return nullptr;
}

IntensityImage * StudentPreProcessing::stepEdgeDetection(const IntensityImage &image) const {	
	// input scaled intensity image 
	// output Edge-detected image

	/* 
	void set(const int width, const int height);

	void setPixel(int x, int y, Intensity pixel);
	void setPixel(int i, Intensity pixel);

	Intensity getPixel(int x, int y) const;
	Intensity getPixel(int i) const1;
	
	*/
	IntensityImage* edgeImage = ImageFactory::newIntensityImage(image);
	
	// gausian filter
	double guassian[5][5];
	Kernel::FilterCreation(guassian);
	Kernel::apply(image, edgeImage, 5, 5, guassian);
	return edgeImage;
}

IntensityImage * StudentPreProcessing::stepThresholding(const IntensityImage &image) const {
	return nullptr;
}