/*
 * Analysis.cpp
 *
 *  Created on: Dec 7, 2016
 *      Author: linh
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <cmath>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "imageModel/Point.h"
#include "imageModel/Line.h"
#include "imageModel/Edge.h"
#include "imageModel/Matrix.h"
#include "imageModel/Image.h"

#include "histograms/ShapeHistogram.h"
#include "pht/PHTEntry.h"
#include "pht/PHoughTransform.h"
#include "correlation/CrossCorrelation.h"

#include "segmentation/Thresholds.h"
#include "segmentation/Canny.h"
#include "segmentation/Suzuki.h"
#include "segmentation/Filters.h"

#include "pointInterest/Treatments.h"
#include "pointInterest/Segmentation.h"
#include "pointInterest/GeometricHistgoram.h"
#include "pointInterest/ProHoughTransform.h"
#include "pointInterest/LandmarkDetection.h"

#include "MAELab.h"

ptr_IntMatrix mae_get_Gray_Histogram(Image* inputImage)
{
	Matrix<int> grayHist = inputImage->getGrayHistogram();
	ptr_IntMatrix result = new Matrix<int>(grayHist.getRows(),
		grayHist.getCols());
	result->setData(grayHist.getData());
	return result;
}

ptr_RGBMatrix mae_get_RGB_Histogram(Image* inputImage)
{
	Matrix<RGB> rgbHistData = inputImage->getRGBHistogram();
	ptr_RGBMatrix rgbHist = &rgbHistData;
	ptr_RGBMatrix result = new Matrix<RGB>(rgbHist->getRows(),
		rgbHist->getCols());
	for (int i = 0; i < rgbHist->getCols(); i++)
	{
		result->setAtPosition(0, i, rgbHist->getAtPosition(0, i));
	}
	return result;
}
ptr_IntMatrix mae_Binary_Threshold(Image* inputImage)
{
	Matrix<int> inputData = inputImage->getGrayMatrix();
	ptr_IntMatrix inputMatrix = &inputData;
	float thresh_value = inputImage->getThresholdValue();

	ptr_IntMatrix binaryMatrix = new Matrix<int>(inputMatrix->getRows(),
		inputMatrix->getCols(), MAX_GRAY_VALUE);
	*binaryMatrix = *(binaryThreshold(inputMatrix, thresh_value, MAX_GRAY_VALUE));
	ptr_IntMatrix result = new Matrix<int>(inputMatrix->getRows(),
		inputMatrix->getCols(), MAX_GRAY_VALUE);
	*result = *binaryMatrix;
	delete binaryMatrix;
	return result;
}
vector<Point> mae_Canny_Algorithm(Image* inputImage)
{

	float thresh_value = inputImage->getThresholdValue();
	Matrix<int> grayImage = inputImage->getGrayMatrix();
	ptr_IntMatrix binMatrix = binaryThreshold(&grayImage, (int) thresh_value,
		MAX_GRAY_VALUE);
	vector<Point> cPoints;
	ptr_IntMatrix cannyMatrix = cannyProcess(binMatrix, (int) thresh_value,
		3 * (int) thresh_value, cPoints);
	delete binMatrix;
	delete cannyMatrix;
	return cPoints;
}
vector<Edge> mae_Suzuki_Algorithm(Image* inputImage)
{
	float thresh_value = inputImage->getThresholdValue();
	Matrix<int> grayImage = inputImage->getGrayMatrix();
	ptr_IntMatrix binMatrix = binaryThreshold(&grayImage, (int) thresh_value,
		MAX_GRAY_VALUE);
	vector<Point> cPoints;
	ptr_IntMatrix cannyMatrix = cannyProcess(binMatrix, (int) thresh_value,
		3 * (int) thresh_value, cPoints);
	vector<Edge> listOfEdges;
	listOfEdges = suzuki(cannyMatrix);
	delete binMatrix;
	delete cannyMatrix;
	return listOfEdges;
}
vector<Line> mae_Line_Segment(Image* inputImage)
{
	return inputImage->getListOfLines();
}

Matrix<RGB> mae_Gaussian_Filter(Image* inputImage, Matrix<double> kernel)
{
	vector<Matrix<int> > channels = inputImage->splitChannels();
	for (size_t i = 0; i < channels.size(); i++)
	{
		Matrix<int> gau = gaussianBlur(channels[i], kernel);
		channels[i] = gau;
	}
	return inputImage->mergeChannels(channels);
}
Matrix<int> mae_Robert_Filter(Image* inputImage)
{
	Matrix<int> grayData = inputImage->getGrayMatrix();
	Matrix<int> rbResult = RobertOperation(&grayData);
	rbResult = postFilter(rbResult);
	return rbResult;
}
Matrix<double> mae_Sobel_Filter(Image* inputImage)
{
	Matrix<int> grayData = inputImage->getGrayMatrix();
	//Matrix<int> sbResult = SobelOperation(&grayData);
	Matrix<double> sbResult = SobelOperation_Double(&grayData);
	//sbResult = postFilter(sbResult);
	return sbResult;
}
Matrix<int> mae_Erode(Image* inputImage)
{
	Matrix<int> sbResult = *(mae_Binary_Threshold(inputImage));
	ptr_IntMatrix erResult = erode(&sbResult, KERNEL_SIZE_DEFAULT);
	return *erResult;
}
Matrix<int> mae_Dilate(Image* inputImage)
{
	Matrix<int> sbResult = *(mae_Binary_Threshold(inputImage));
	ptr_IntMatrix dlResult = dilate(&sbResult, KERNEL_SIZE_DEFAULT);
	return *dlResult;
}
Matrix<int> mae_Open_Binary(Image* inputImage)
{
	Matrix<int> sbResult = *(mae_Binary_Threshold(inputImage));
	ptr_IntMatrix opResult = openBinary(&sbResult, KERNEL_SIZE_DEFAULT);
	return *opResult;
}
Matrix<int> mae_Close_Binary(Image* inputImage)
{
	Matrix<int> sbResult = *(mae_Binary_Threshold(inputImage));
	ptr_IntMatrix clResult = closeBinary(&sbResult, KERNEL_SIZE_DEFAULT);
	return *clResult;
}
//=================================== old version ============================================
vector<Line> segment(ptr_Treatments treatment, Image image, int minDistance)
{

	Segmentation sg;
	treatment = &sg;
	treatment->setRefImage(image);

	vector<Line> lines = sg.segment();
	return lines;
}

ShapeHistogram geomHistogram(ptr_Treatments treatment, Image image,
	AngleAccuracy angleAcc, int cols)
{
	GeometricHistgoram geoHistogram;
	treatment = &geoHistogram;
	treatment->setRefImage(image);
	return geoHistogram.geomHistogram(angleAcc, cols);
}
double bhattacharyyaDistance(ptr_Treatments treatment, Image sceneImage,
	AngleAccuracy angleAcc, int cols)
{
	GeometricHistgoram geoHistogram;
	treatment = &geoHistogram;
	return geoHistogram.bhattacharyyaDistance(sceneImage, angleAcc, cols);
}
PHoughTransform phtEntriesTable(ptr_Treatments treatment, Image image)
{
	ProHoughTransform pht;
	treatment = &pht;
	treatment->setRefImage(image);
	return pht.constructPHT();
}
vector<Point> estimatedLandmarks(ptr_Treatments treatment, Image sceneImage,
	AngleAccuracy acc, int cols, int templSize, int sceneSize, Point &ePoint,
	double angleDiff)
{
	LandmarkDetection lmd;
	lmd.setRefImage(treatment->getRefImage());
	//treatment = &lmd;
	return lmd.landmarksAutoDectect(sceneImage, acc, cols, templSize, sceneSize,
		ePoint, angleDiff);
}
double measureCentroidPoint(vector<Point> landmarks, Point &ebary)
{
	ebary.setX(0);
	ebary.setY(0);

	int totalX = 0;
	int totalY = 0;
	size_t lmSize = landmarks.size();

	Point pi;
	for (size_t i = 0; i < lmSize; i++)
	{
		pi = landmarks.at(i);
		totalX += pi.getX();
		totalY += pi.getY();
	}

	if (lmSize > 0)
	{
		ebary.setX(totalX / lmSize);
		ebary.setY(totalY / lmSize);
	}
	double totalDistance = 0;
	Point lm;
	for (size_t j = 0; j < lmSize; j++)
	{
		lm = landmarks.at(j);
		//ptr_Line line = new Line(ebary,lm);
		double distance = distancePoints(ebary, landmarks.at(j));
		//cout<<"\nDistance: \t"<<distance;
		totalDistance += (distance * distance);

	}
	//cout<<"\n"<<sqrt(totalDistance);
	return sqrt(totalDistance);
}

