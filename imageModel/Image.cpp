/*
 * Image.cpp
 *
 *  Created on: Sep 16, 2016
 *      Author: linh
 */

#include <iostream>
#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <time.h>
#include <string>
#include <algorithm>
using namespace std;

#include "Point.h"
#include "Line.h"
#include "Edge.h"
#include "Matrix.h"
#include "../io/Reader.h"
#include "../segmentation/Thresholds.h"
#include "../segmentation/Canny.h"
#include "../segmentation/Suzuki.h"

#include "Image.h"

const int BIN_SIZE = 256;
const int MEAN_GRAY = 120;
const int DECREASE_25 = 25;
const int DECREASE_5 = 5;

const double RED_COEFFICIENT = 0.299f;
const double GREEN_COEFFICIENT = 0.587f;
const double BLUE_COEFFICIENT = 0.114f;


/*
 * The equations to convert from RGB or BGR to Grayscale: Gvalue = 0.299*R + 0.587*G + 0.114*B
 *
 */
//================================================= Utils methods =================================================
Matrix<int> convertRGBToGray(Matrix<RGB> rgbMatrix)
{

	int width = rgbMatrix.getCols();
	int height = rgbMatrix.getRows();
	Matrix<int> grayMatrix(height,width);
	//grayMatrix = new Matrix<int>(height, width);
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{

			grayMatrix.setAtPosition(h, w,
				round(
					((double) rgbMatrix.getAtPosition(h, w).R * RED_COEFFICIENT)
						+ ((double) rgbMatrix.getAtPosition(h, w).G * GREEN_COEFFICIENT)
						+ ((double) rgbMatrix.getAtPosition(h, w).B * BLUE_COEFFICIENT)));
		}
	}

	return grayMatrix;
}

// ================================================== End utils methods =============================================
//===================================================== Constructor =================================================
Image::Image()
{
	// TODO Auto-generated constructor stub
	medianHistogram = 0;
	meanHistogram = 0;
	thresholdValue = 0;
}
Image::Image(const Image &cpimage)
{
	fileName = cpimage.fileName;
	listOfLines = cpimage.listOfLines;
	manualLandmarks = cpimage.manualLandmarks;
	autoLandmarks = cpimage.autoLandmarks;
	imgMatrix = cpimage.imgMatrix;
	grayMatrix = cpimage.grayMatrix;
	grayHistogram = cpimage.grayHistogram;
	medianHistogram = cpimage.medianHistogram;
	meanHistogram = cpimage.meanHistogram;
	thresholdValue = cpimage.thresholdValue;
}
Image::~Image()
{
	/*listOfLines.clear();
	 manualLandmarks.clear();
	 autoLandmarks.clear();
	 delete grayMatrix;
	 delete imgMatrix;
	 delete grayHistogram;*/
}
;
Image::Image(std::string filePath)
{
	medianHistogram = 0;
	meanHistogram = 0;
	thresholdValue = 0;

	fileName = filePath;
	int rows = 0, cols = 0;
	imgMatrix = readJPGToRGB(filePath.c_str(), rows, cols);

	grayMatrix = convertRGBToGray(imgMatrix);
	grayHistogram.setRows(1);
	grayHistogram.setCols(BIN_SIZE);
	grayHistogram.Init();

	rgbHistogram.setRows(1);
	rgbHistogram.setCols(BIN_SIZE);
	rgbHistogram.Init();

	calcHistogram();

	calThresholdValue();

	//cout << endl << "Threshold value: " << thresholdValue;
}

//===================================================== End constructor ================================================

//================================================ Public methods ======================================================
void Image::setFileName(std::string filePath)
{
	fileName = filePath;
}
std::string Image::getFileName()
{
	return fileName;
}
string Image::getName()
{
	size_t found1 = fileName.find_last_of("/");
	size_t found2 = fileName.find_last_of(".");
	string str = fileName.substr(found1 + 1, found2 - found1 - 1);
	size_t len = str.length();
	size_t i = 0;
	while (i < len)
	{
		char c = str.at(i);
		if (c == ' ')
		{
			str.replace(i, 1, "_");
		}
		i++;
	}
	return str+".JPG";
}
void Image::setMLandmarks(string tpsFile)
{
	manualLandmarks = readManualLandmarks(tpsFile);
}
void Image::setRGBMatrix(Matrix<RGB> rgbMatrix)
{
	imgMatrix = rgbMatrix;
	grayMatrix = convertRGBToGray(imgMatrix);
}
void Image::setGrayMatrix(Matrix<int> graymatrix)
{
	grayMatrix = graymatrix;
}
void Image::setAutoLandmarks(vector<Point> eLandmarks)
{
	autoLandmarks = eLandmarks;
}
Matrix<int> Image::getGrayMatrix()
{
	return grayMatrix;
}
Matrix<RGB> Image::getRGBMatrix()
{
	return imgMatrix;
}
vector<Line> Image::getListOfLines()
{
	if (listOfLines.size() <= 0)
		getApproximateLines(3);
	return listOfLines;
}
float Image::getMedianHistogram()
{
	if (medianHistogram == 0)
		calcHistogram();
	return medianHistogram;

}
Matrix<int> Image::getGrayHistogram()
{
	return grayHistogram;
}
Matrix<RGB> Image::getRGBHistogram()
{
	return rgbHistogram;
}
float Image::getMeanHistogram()
{
	if (meanHistogram == 0)
		calcHistogram();
	return meanHistogram;
}
float Image::getThresholdValue()
{
	if (thresholdValue == 0)
		calThresholdValue();
	return thresholdValue;
}

vector<Line> Image::getApproximateLines(double minDistance)
{
	vector<Point> cPoints;
	vector<Edge> listOfEdges = cannyAlgorithm(cPoints);
	vector<Line> totalLines;

	Edge ed;
	for (size_t i = 0; i < listOfEdges.size(); i++)
	{
		ed = listOfEdges.at(i);
		vector<Point> breakPoints = ed.segment(minDistance);
		vector<Line> lines = ed.getLines(breakPoints);
		totalLines.insert(totalLines.end(), lines.begin(), lines.end());

		breakPoints.clear();
		lines.clear();
	}

	cout << "\n Min distance: " << minDistance;
	cout << "\n Total lines after segment the edge: " << totalLines.size()<<endl;
	listOfLines = totalLines;
	return totalLines;
}
vector<Point> Image::readManualLandmarks(string fileName)
{
	manualLandmarks.clear();
	vector<Point> mLandmarks = readTPSFile(fileName.c_str());
	int rows = grayMatrix.getRows();
	Point temp;
	Point p;
	for (size_t t = 0; t < mLandmarks.size(); t++)
	{
		temp = mLandmarks.at(t);
		p.setX(temp.getX());
		p.setY(rows - temp.getY());
		manualLandmarks.push_back(p);
	}

	mLandmarks.clear();

	return manualLandmarks;
}
vector<Point> Image::getListOfManualLandmarks()
{
	return manualLandmarks;
}
vector<Point> Image::getListOfAutoLandmarks()
{
	return autoLandmarks;
}
//================================================ End public methods ==================================================

//================================================ Private methods =====================================================
void Image::calcHistogram()
{

	if (grayMatrix.getCols() != 0)
	{
		float total = 0;
		float pi = 0;
		int array[BIN_SIZE] =
		{ 0 };

		int redArray[BIN_SIZE] = {0};
		int greenArray[BIN_SIZE] = {0};
		int blueArray[BIN_SIZE] = {0};
		RGB color;
		color.R = color.G = color.B = 0;
		for (int r = 0; r < grayMatrix.getRows(); r++)
		{
			for (int c = 0; c < grayMatrix.getCols(); c++)
			{
				int k = grayMatrix.getAtPosition(r, c);
				array[k]++;

				RGB ccolor = imgMatrix.getAtPosition(r,c);
				redArray[ccolor.R]++;
				greenArray[ccolor.G]++;
				blueArray[ccolor.B]++;
			}
		}

		for (int k = 0; k < BIN_SIZE; k++)
		{
			// save and calculate the mean of gray histogram
			grayHistogram.setAtPosition(0, k, array[k]);
			total += array[k];
			pi += (k * array[k]);

			// save the rgb histogram
			color.R = redArray[k];
			color.G = greenArray[k];
			color.B = blueArray[k];
			rgbHistogram.setAtPosition(0,k,color);
		}

		// calculate the mean of histogram
		meanHistogram = (pi / total);
		// calculate the median of histogram
		float avm = total / 2;
		float temp = 0;
		for (int m = 0; m < BIN_SIZE; m++)
		{
			temp += array[m];
			if (temp >= avm)
			{
				medianHistogram = m;
				break;
			}
		}
	}

}

void Image::calThresholdValue()
{
	if (medianHistogram == 0 || meanHistogram == 0)
		calcHistogram();
	int limit1 =
		meanHistogram > medianHistogram ? medianHistogram : meanHistogram;
	limit1 = (limit1 >= 120) ? (limit1 - DECREASE_25) : (limit1 - DECREASE_5);
	int imax1 = -1, max1 = -1;
	for (int index = 0; index < limit1; index++)
	{
		int temp = grayHistogram.getAtPosition(0, index);
		if (temp > max1)
		{
			max1 = temp;
			imax1 = index;
		}
	}
	int limit2 =
		meanHistogram > medianHistogram ? meanHistogram : medianHistogram;
	int imin = -1, min = max1;
	for (int k = imax1; k < limit2; k++)
	{
		int temp = grayHistogram.getAtPosition(0, k);
		if (temp < min)
		{
			min = temp;
			imin = k;
		}
	}
	int max2 = -1, imax2 = -1;
	for (int k = limit2; k < BIN_SIZE; k++)
	{
		int temp = grayHistogram.getAtPosition(0, k);
		if (temp > max2)
		{
			max2 = temp;
			imax2 = k;
		}
	}
	float mid1 = (imin + imax1) / 2;
	float mid2 = (imin + imax2) / 2;
	thresholdValue = (mid1 + mid2) / 2;
}

vector<Edge> Image::cannyAlgorithm(vector<Point> &cPoints)
{
	if (thresholdValue == 0)
		calThresholdValue();

	ptr_IntMatrix binMatrix = binaryThreshold(&grayMatrix, (int) thresholdValue,
		MAX_GRAY_VALUE);
	ptr_IntMatrix cannyMatrix = cannyProcess(binMatrix, (int) thresholdValue,
		3 * (int) thresholdValue, cPoints);

	vector<Edge> listOfEdges;
	listOfEdges = suzuki(cannyMatrix);

	delete cannyMatrix;
	delete binMatrix;

	return listOfEdges;
}

// angle in degree
/*ptr_DoubleMatrix Image::getRotationMatrix2D(Point center, double angle,
 double scale)
 {
 cout << "\n"<<angle;
 //if (angle > 0)
 //	angle = -angle;

 double alpha = cos(angle * M_PI / 180) * scale;
 double beta = sin(angle * M_PI / 180) * scale;

 ptr_DoubleMatrix rotateM = new Matrix<double>(2, 3, 0);

 rotateM->setAtPosition(0, 0, alpha);
 rotateM->setAtPosition(0, 1, beta);
 rotateM->setAtPosition(0, 2,
 (1 - alpha) * center.getX() - beta * center.getY());
 rotateM->setAtPosition(1, 0, -beta);
 rotateM->setAtPosition(1, 1, alpha);
 rotateM->setAtPosition(1, 2,
 beta * center.getX() + (1 - alpha) * center.getY());

 return rotateM;
 }*/

void Image::rotate(Point center, double angle, double scale)
{
	/*ptr_DoubleMatrix rotationMatrix = getRotationMatrix2D(center, angle, scale);
	 ptr_IntMatrix source = grayMatrix;*/
	RGB color;
	color.R = color.B = color.G = 0;
	grayMatrix.rotation(center, angle, scale, 0);
	imgMatrix.rotation(center, angle, scale, color);

	//*grayMatrix = gray;
	//*imgMatrix = rgb;

	/*int rows = source->getRows();
	 int cols = source->getCols();

	 double a00 = rotationMatrix->getAtPosition(0, 0);
	 double a01 = rotationMatrix->getAtPosition(0, 1);
	 double b00 = rotationMatrix->getAtPosition(0, 2);
	 double a10 = rotationMatrix->getAtPosition(1, 0);
	 double a11 = rotationMatrix->getAtPosition(1, 1);
	 double b10 = rotationMatrix->getAtPosition(1, 2);
	 ptr_RGBMatrix rgbSource = imgMatrix;
	 RGB color;
	 color.R = color.G = color.B = 0;
	 ptr_RGBMatrix rgbDest = new Matrix<RGB>(rows, cols, color);
	 ptr_IntMatrix result = new Matrix<int>(rows, cols, 0);
	 for (int row = 0; row < rows; row++)
	 {
	 for (int col = 0; col < cols; col++)
	 {
	 int value = source->getAtPosition(row, col);
	 color = rgbSource->getAtPosition(row, col);

	 int xnew = round(a00 * col + a01 * row + b00);
	 int ynew = round(a10 * col + a11 * row + b10);

	 if (xnew >= 0 && xnew < cols && ynew >= 0 && ynew < rows)
	 {
	 result->setAtPosition(ynew, xnew, value);
	 rgbDest->setAtPosition(ynew, xnew, color);
	 }
	 }
	 }
	 for (int row = 0; row < rows; row++)
	 {
	 for (int col = 1; col < cols - 1; col++)
	 {
	 int value = result->getAtPosition(row, col);
	 int leftValue = result->getAtPosition(row, col - 1);
	 int rightValue = result->getAtPosition(row, col + 1);
	 if (value == 0 && leftValue > 0 && rightValue >= 0)
	 {
	 result->setAtPosition(row, col, (leftValue + rightValue) / 2);
	 }

	 RGB color = rgbDest->getAtPosition(row, col);
	 RGB leftColor = rgbDest->getAtPosition(row, col - 1);
	 RGB rightColor = rgbDest->getAtPosition(row, col + 1);
	 if (color.R == 0 && color.G == 0 && color.B == 0
	 && (leftColor.R != 0 || leftColor.G != 0 || leftColor.B != 0)
	 && (rightColor.R != 0 || rightColor.G != 0 || rightColor.B != 0))
	 {
	 RGB newColor;
	 newColor.R = (leftColor.R + rightColor.R) / 2;
	 newColor.G = (leftColor.G + rightColor.G) / 2;
	 newColor.B = (leftColor.B + rightColor.B) / 2;
	 rgbDest->setAtPosition(row, col, newColor);
	 }
	 }
	 }
	 imgMatrix = rgbDest;
	 grayMatrix = result;*/

	//delete rotationMatrix;
	//delete source;
	//delete rgbSource;
	//delete rgbDest;
	//return result;
}
vector<Matrix<int> > Image::splitChannels()
{
	vector<Matrix<int> > channels;
	int rows = imgMatrix.getRows();
	int cols = imgMatrix.getCols();
	Matrix<int> red_Channel(rows,cols,0);
	Matrix<int> green_Channel(rows,cols,0);
	Matrix<int> blue_Channel(rows,cols,0);
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			RGB color = imgMatrix.getAtPosition(r,c);
			red_Channel.setAtPosition(r,c,color.R);
			green_Channel.setAtPosition(r,c,color.G);
			blue_Channel.setAtPosition(r,c,color.B);
		}
	}
	channels.push_back(red_Channel);
	channels.push_back(green_Channel);
	channels.push_back(blue_Channel);
	return channels;
}

// Merge 3 channels (R, G, B) into RGB matrix
Matrix<RGB> Image::mergeChannels(vector<Matrix<int> > channels)
{
	Matrix<RGB> mergeMatrix;
	if(channels.size() == 3) // 
	{
		int rows = channels.at(0).getRows();
		int cols = channels.at(0).getCols();
		mergeMatrix.setRows(rows);
		mergeMatrix.setCols(cols);
		mergeMatrix.Init();
		RGB color;
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				color.R = channels.at(0).getAtPosition(r,c);
				color.G = channels.at(1).getAtPosition(r,c);
				color.B = channels.at(2).getAtPosition(r,c);
				mergeMatrix.setAtPosition(r,c,color);
			}
		}
		imgMatrix = mergeMatrix;
	}
	return mergeMatrix;
}
//================================================ End private methods =====================================================

