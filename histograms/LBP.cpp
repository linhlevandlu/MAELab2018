/*
 * LBP.cpp
 *
 *  Created on: Apr 20, 2017
 *      Author: linh
 */
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cmath>
#include <fstream>
using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"
#include "LBP.h"

/*
 * Calculate the LBP and contrast for one pixels by its neighbor intensities
 */
int lbpCalculate(ptr_IntMatrix grayImage, int rowIndex, int colIndex,
	double &contrast)
{
	int rows = grayImage->getRows();
	int cols = grayImage->getCols();
	int i = 0;
	int vIndex = grayImage->getAtPosition(rowIndex, colIndex);
	int lbpSum = 0;
	int countl0 = 0, countg0 = 0, suml0 = 0, sumg0 = 0;
	for (int r = rowIndex - 1; r <= rowIndex + 1; r++)
	{
		for (int c = colIndex - 1; c <= colIndex + 1; c++)
		{
			int value;
			if (r < 0 || c < 0 || r >= rows || c >= cols)
			{
				value = 0;
			}
			else
			{
				value = grayImage->getAtPosition(r, c);
			}
			if (r != rowIndex || c != colIndex)
			{
				int bValue;
				if (value >= vIndex)
				{
					bValue = 1;
					countg0++;
					sumg0 += value;
				}
				else
				{
					bValue = 0;
					countl0++;
					suml0 += value;
				}
				lbpSum += (bValue * pow(2, i));
				i++;
			}
		}
	}
	contrast = ((double) sumg0 / (double) countg0)
		- ((double) suml0 / (double) countl0);
	return lbpSum;
}

/*
 * Compute the LBP and Contrast for all pixels in a patch
 */
ptr_IntMatrix LBPDescriptor(ptr_IntMatrix grayImage, ptr_DoubleMatrix &contrast)
{
	int rows = grayImage->getRows();
	int cols = grayImage->getCols();
	ptr_IntMatrix result = new Matrix<int>(rows, cols, 0);

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			double cont = 0.0;
			int lbpValue = lbpCalculate(grayImage, r, c, cont);
			result->setAtPosition(r, c, lbpValue);
			contrast->setAtPosition(r, c, cont);
		}
	}
	return result;
}
ptr_IntMatrix quantizier(ptr_DoubleMatrix contrast)
{
	int rows = contrast->getRows();
	int cols = contrast->getCols();
	double minValue = DBL_MAX, maxValue = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			double value = contrast->getAtPosition(r, c);
			if (value > maxValue)
			{
				maxValue = value;
			}
			if (value < minValue)
			{
				minValue = value;
			}
		}
	}
	double step = (maxValue - minValue) / 8;
	ptr_IntMatrix result = new Matrix<int>(rows, cols, 0);
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			double value = contrast->getAtPosition(r, c);
			int index = round((value - minValue) / step);
			if (index < 0)
				index = 0;
			if (index >= 8)
				index = 7;
			result->setAtPosition(r, c, index);
		}
	}
	return result;
}
ptr_IntMatrix LBPContrastHistogram(ptr_IntMatrix lbpMatrix,
	ptr_IntMatrix contrast)
{
	int rows = lbpMatrix->getRows();
	int cols = lbpMatrix->getCols();
	ptr_IntMatrix result = new Matrix<int>(8, 256, 0);
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int lbp = lbpMatrix->getAtPosition(r, c);
			if (lbp < 0)
				lbp = 0;
			if (lbp >= 256)
				lbp = 256;
			int ct = contrast->getAtPosition(r, c);
			if (ct < 0)
				ct = 0;
			if (ct >= 8)
				ct = 7;
			int value = result->getAtPosition(ct, lbp);
			result->setAtPosition(ct, lbp, value + 1);
		}
	}

	// normalize
	/*int total = 0;
	 for (int r = 0; r < 8; r++)
	 {
	 for (int c = 0; c < 256; c++)
	 {
	 total += result->getAtPosition(r, c);
	 }
	 }
	 for (int r = 0; r < 8; r++)
	 {
	 for (int c = 0; c < 256; c++)
	 {
	 int v = result->getAtPosition(r, c);
	 result->setAtPosition(r,c,v/total);
	 }
	 }*/

	return result;
}
void LBPHistogram(ptr_IntMatrix lbpMatrix, double (&histogram)[256])
{
	int rows = lbpMatrix->getRows();
	int cols = lbpMatrix->getCols();
	//double histCount[256] =
	//{ };
	//hist = {};
	// compute the histogram
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int value = lbpMatrix->getAtPosition(r, c);
			histogram[value] += 1;
		}
	}

	// normalize the histogram
	int total = rows * cols;
	for (int i = 0; i < 256; i++)
	{
		histogram[i] /= total;
	}
}
double measureDistance(double hist1[256], double hist2[256])
{
	double distance = 0.0, temp = 0.0;
	for (int i = 0; i < 256; ++i)
	{
		double v1 = hist1[i];
		double v2 = hist2[i];
		temp += pow(v1 - v2, 2.0);
	}
	distance = sqrt(temp);
	return distance;
}
double measureDistance2(ptr_IntMatrix histogram1, ptr_IntMatrix histogram2)
{
	int rows = histogram1->getRows();
	int cols = histogram1->getCols();
	double distance = 0.0, temp = 0.0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int v1 = histogram1->getAtPosition(r, c);
			int v2 = histogram2->getAtPosition(r, c);
			temp += pow(v1 - v2, 2.0);
		}
	}
	distance = sqrt((double) temp);
	return distance;
}

void testLBPDescriptor(ptr_IntMatrix grayImage, vector<Point> landmarks,
	int sPatch)
{
	for (int i = 0; i < landmarks.size(); i++)
	{
		Point pi = landmarks.at(i);
		Matrix<int> patch = grayImage->extractPatch(sPatch, sPatch, pi.getY(),
			pi.getX(), 0);
		ptr_DoubleMatrix contrast = new Matrix<double>(patch.getRows(),
			patch.getCols(), 0.0);
		ptr_IntMatrix lbp1 = LBPDescriptor(&patch, contrast);
		ptr_IntMatrix ctQuan = quantizier(contrast);
		ptr_IntMatrix hist1 = LBPContrastHistogram(lbp1, ctQuan);

		for (int j = 0; j < landmarks.size(); j++)
		{

			Point pk = landmarks.at(j);
			Matrix<int> patch2 = grayImage->extractPatch(sPatch, sPatch, pk.getY(),
				pk.getX(), 0);
			ptr_DoubleMatrix contrast2 = new Matrix<double>(patch2.getRows(),
				patch2.getCols(), 0.0);
			ptr_IntMatrix lbp2 = LBPDescriptor(&patch2, contrast2);
			ptr_IntMatrix ctQuan2 = quantizier(contrast2);
			ptr_IntMatrix hist2 = LBPContrastHistogram(lbp2, ctQuan2);
			double dist = measureDistance2(hist1, hist2);
			cout << "\n Pair: " << i << " - " << j << ": " << dist << endl;
		}
	}
}

void testLBPDescriptor2Images(ptr_IntMatrix grayImage1,
	vector<Point> landmarks1, ptr_IntMatrix grayImage2, vector<Point> landmarks2,
	int sPatch)
{
	for (int i = 0; i < landmarks1.size(); i++)
	{
		Point pi = landmarks1.at(i);
		Matrix<int> patch = grayImage1->extractPatch(sPatch, sPatch, pi.getY(),
			pi.getX(), 0);
		ptr_DoubleMatrix contrast = new Matrix<double>(patch.getRows(),
			patch.getCols(), 0.0);
		ptr_IntMatrix lbp1 = LBPDescriptor(&patch, contrast);
		ptr_IntMatrix ctQuan = quantizier(contrast);
		ptr_IntMatrix hist1 = LBPContrastHistogram(lbp1, ctQuan);
		for (int j = 0; j < landmarks2.size(); j++)
		{

			Point pk = landmarks2.at(j);
			Matrix<int> patch2 = grayImage2->extractPatch(sPatch, sPatch, pk.getY(),
				pk.getX(), 0);
			ptr_DoubleMatrix contrast2 = new Matrix<double>(patch2.getRows(),
				patch2.getCols(), 0.0);
			ptr_IntMatrix lbp2 = LBPDescriptor(&patch2, contrast2);
			ptr_IntMatrix ctQuan2 = quantizier(contrast2);
			ptr_IntMatrix hist2 = LBPContrastHistogram(lbp2, ctQuan2);
			double dist = measureDistance2(hist1, hist2);
			cout << "\n Pair: " << i << " - " << j << ": " << dist << endl;
		}
	}
}

vector<Point> testLBPDescriptor2ImagesContours(ptr_IntMatrix grayImage1,
	vector<Point> landmarks1, ptr_IntMatrix grayImage2,
	vector<Point> gray2Contours, int sPatch)
{
	vector<Point> result;
	for (int i = 6; i < landmarks1.size(); i++)
	{
		Point pi = landmarks1.at(i);
		Matrix<int> patch = grayImage1->extractPatch(sPatch, sPatch, pi.getY(),
			pi.getX(), 0);
		ptr_DoubleMatrix contrast = new Matrix<double>(patch.getRows(),
			patch.getCols(), 0.0);
		ptr_IntMatrix lbp1 = LBPDescriptor(&patch, contrast);
		ptr_IntMatrix ctQuan = quantizier(contrast);
		ptr_IntMatrix hist1 = LBPContrastHistogram(lbp1, ctQuan);
		double minDistance = DBL_MAX;
		Point cpoint(0, 0);
		for (int j = 0; j < gray2Contours.size(); j++)
		{
			Point pk = gray2Contours.at(j);
			Matrix<int> patch2 = grayImage2->extractPatch(sPatch, sPatch, pk.getY(),
				pk.getX(), 0);
			ptr_DoubleMatrix contrast2 = new Matrix<double>(patch2.getRows(),
				patch2.getCols(), 0.0);
			ptr_IntMatrix lbp2 = LBPDescriptor(&patch2, contrast2);
			ptr_IntMatrix ctQuan2 = quantizier(contrast2);
			ptr_IntMatrix hist2 = LBPContrastHistogram(lbp2, ctQuan2);
			double dist = measureDistance2(hist1, hist2);
			if (dist < minDistance)
			{
				cpoint.setX(pk.getX());
				cpoint.setY(pk.getY());
				minDistance = dist;
			}
			delete lbp2;
			delete ctQuan2;
			delete hist2;
			//cout << "\n Pair: " << i << " - " << j << ": " << dist << endl;
		}
		delete lbp1;
		delete ctQuan;
		delete hist1;
		cpoint.toString();
		result.push_back(cpoint);
		break;
	}
	return result;
}

