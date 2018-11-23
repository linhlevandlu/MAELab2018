/*
 * Image.h
 *
 *  Created on: Sep 16, 2016
 *      Author: linh
 */

#ifndef IMAGE_H_
#define IMAGE_H_
#include "Edge.h"

const int MAX_GRAY_VALUE = 255;

class Image
{
private:
	std::string fileName;
	vector<Line> listOfLines;
	vector<Point> manualLandmarks;
	vector<Point> autoLandmarks;
	Matrix<int> grayMatrix;
	Matrix<RGB> imgMatrix;
	Matrix<int> grayHistogram;
	Matrix<RGB> rgbHistogram;
	float medianHistogram;
	float meanHistogram;
	float thresholdValue;

	void calcHistogram ();
	void calThresholdValue();

public:
	Image();
	Image(const Image &cpimage);
	~Image();
	Image(std::string);
	void setFileName(std::string);
	std::string getFileName();
	string getName();
	void setMLandmarks(string);
	void setRGBMatrix(Matrix<RGB>);
	void setGrayMatrix(Matrix<int>);
	void setAutoLandmarks(vector<Point>);
	Matrix<int> getGrayMatrix();
	Matrix<RGB> getRGBMatrix();
	Matrix<int> getGrayHistogram();
	Matrix<RGB> getRGBHistogram();
	float getMedianHistogram();
	float getMeanHistogram();
	float getThresholdValue();
	vector<Line> getListOfLines();
	vector<Point> getListOfManualLandmarks();
	vector<Point> getListOfAutoLandmarks();

	vector<Edge> cannyAlgorithm(vector<Point> &cPoints);
	vector<Line> getApproximateLines(double minDistance);
	vector<Point> readManualLandmarks(string fileName);
	vector<Matrix<int> > splitChannels();
	Matrix<RGB> mergeChannels(vector<Matrix<int> > channels);
	/*ptr_DoubleMatrix getRotationMatrix2D(Point center, double angle,
		double scale);*/
	void rotate(Point center, double angle, double scale);
};
typedef Image* ptr_Image;
#endif /* IMAGE_H_ */
