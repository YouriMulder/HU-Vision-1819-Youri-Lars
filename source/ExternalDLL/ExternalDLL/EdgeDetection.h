#pragma once

#include "IntensityImage.h"
#include <vector>
#include <array>

namespace EdgeDetection {
	typedef std::vector<std::vector<double>> imageVector;
	
	/*
	@brief Stores the values of an IntensityImage into an imageVector.
	@param image An Intensity image you want to convert to a imageVector.
	@return An imageVector containing the pixels of the image.
	@details
	View the implementation plan chapter 1.5.1 for more details.
	*/
	imageVector imageVectorFromIntensityImage(const IntensityImage& image);
	
	/*
	@brief Creates and applies gaussian filter to an imageVector using a given sigma.
	@param image The image you want to perform a guassian filter on.
	@param sigma The sigma you want to use to create the guassian kernel.
	@return An imageVector containing the image after applying the guassian filter.
	@details
	View the implementation plan chapter 1.5.2 for more details.
	*/
	imageVector applyGaussian(const imageVector &image, double sigma);
	
	/*
	@brief Applies sobel filter to an imageVector and stores the result in destImage and directionImage.
	@param sourceImage The image you want to perform a sobel filter on.
	@param destImage The image you want to store the intensities in after applying the sobel filter.
	@param directionImage The image you want to store the gradient directions in.
	@details
	View the implementation plan chapter 1.5.3 for more details.
	@warming
	Watch out the size of the output vectors matter. No size checking is done.
	*/
	void sobelFilter(const imageVector &sourceImage, imageVector &destImage, imageVector &directionImage);
	
	/*
	@brief Applies non-maximum supression to an imageVector using the gradient directions.
	@param image The image you want to perform non-maximum supression on.
	@param directions The gradient directions of the given pixel.
	@param directionImage The image you want to store the gradient directions in.
	@details
	View the implementation plan chapter 1.5.4 for more details.
	@warming
	The gradient directions must be those of the given image.
	*/
	void nonMaxSupp(imageVector &image, const imageVector &directions);
	
	/*
	@brief Applies double thresholding to an imageVector.
	@param lowThreshold The lowest value of the threshold.
	@param highThreshold The highest value of the threshold.
	@param strong The value the pixel must be set to when higher than the highThreshold.
	@param weak The value the pixel must be set to when between the lowThreshold and highThreshold.
	@details
	View the implementation plan chapter 1.5.5 for more details.
	*/
	void doubleThreshold(imageVector &image, const Intensity& lowThreshold, const Intensity& highThreshold, const Intensity& strong, const Intensity& weak);

	/*
	@brief Weak edges are checked and set to strong edge if connected to one.
	@param image The image containing all the strong and weak edges.
	@param strong The value of a weak edge.
	@param weak The value of a strong edge.
	@details
	View the implementation plan chapter 1.5.6 for more details.
	*/
	void tracking(imageVector &image, const Intensity &strong, const Intensity &weak);
	
	/*
	@brief Creates histogram using an imageVector.
	@param image The image you want to create a histogram of.
	@param histogram The place you want to store the histogram.
	@details View the implementation plan chapter 1.5.7 for more details.
	*/
	void toHistogram(const imageVector &image, std::array<double, 256> &histogram);
	
	/*
	@brief Applies otsu to an image using the histogram.
	@param totalAmountOfPixels The totalAmountOfPixels of the original image in the histogram.
	@param histogram The histogram of the image.
	@details
	View the implementation plan chapter 1.5.8 for more details.
	*/
	double otsu(int totalAmountOfPixels, std::array<double, 256>& histogram);
};
