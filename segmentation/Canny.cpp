/*
 * Canny.cpp
 *
 *  Created on: Oct 25, 2016
 *      Author: linh
 */

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <time.h>

using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Edge.h"
#include "../imageModel/Matrix.h"
#include "../io/Reader.h"
#include "../utils/Converter.h"
#include "Filters.h"
#include "Thresholds.h"

ptr_DoubleMatrix angles;
const float LIMIT_ANGLE_1 = 22.5;
const float LIMIT_ANGLE_2 = 67.5;
const float LIMIT_ANGLE_3 = 112.5;
const float LIMIT_ANGLE_4 = 157.5;

const int WHITE_VALUE = 255;
const int BLACK_VALUE = 0;

#include "Canny.h"

double getBorderValue(ptr_IntMatrix inputMatrix, int x, int y)
{
	int rows = inputMatrix->getRows();
	int cols = inputMatrix->getCols();
	if (x < 0)
	{
		if (y < 0)
		{
			return inputMatrix->getAtPosition(0, 0);
		}
		else
		{
			if (y < cols)
			{
				return inputMatrix->getAtPosition(0, y);
			}
			else
			{
				return inputMatrix->getAtPosition(0, cols - 1);
			}
		}
	}
	else
	{
		if (x < rows)
		{
			if (y < 0)
			{
				return inputMatrix->getAtPosition(x, 0);
			}
			else
			{
				if (y < cols)
				{
					return inputMatrix->getAtPosition(x, y);
				}
				else
				{
					return inputMatrix->getAtPosition(x, cols - 1);
				}
			}
		}
		else
		{
			if (y < 0)
			{
				return inputMatrix->getAtPosition(rows - 1, 0);
			}
			else
			{
				if (y < cols)
				{
					return inputMatrix->getAtPosition(rows - 1, y);
				}
				else
				{
					return inputMatrix->getAtPosition(rows - 1, cols - 1);
				}
			}
		}
	}
}
ptr_IntMatrix gxSobelConvolution(ptr_IntMatrix inputMatrix)
{

	// create kernel
	double x1[] =
	{ -1.0, 0, 1.0 };
	double x2[] =
	{ -2.0, 0, 2.0 };
	double x3[] =
	{ -1.0, 0, 1.0 };
	vector<vector<double> > xfilter(3);
	xfilter[0].assign(x1, x1 + 3);
	xfilter[1].assign(x2, x2 + 3);
	xfilter[2].assign(x3, x3 + 3);
	int size = (int) xfilter.size();
	int ksize = size / 2;

	int rows = inputMatrix->getRows();
	int cols = inputMatrix->getCols();
	ptr_IntMatrix gxConvol = new Matrix<int>(rows, cols, BLACK_VALUE);
	for (int i = -ksize; i < rows - ksize; i++)
	{
		for (int j = -ksize; j < cols - ksize; j++)
		{
			double sumx = 0;

			for (int x = 0; x < size; x++)
			{

				for (int y = 0; y < size; y++)
				{
					double gValue =
						(i + x < 0 || j + y < 0 || i + x >= rows || j + y >= cols) ?
							getBorderValue(inputMatrix, i + x, j + y) :
							(double) inputMatrix->getAtPosition(i + x, j + y);
					sumx += xfilter[x][y] * gValue;

				}

			}
			//set value
			gxConvol->setAtPosition(i + ksize, j + ksize, sumx);
		}
	}
	xfilter.clear();
	return gxConvol;
}

ptr_IntMatrix gySobelConvolution(ptr_IntMatrix inputMatrix)
{

	// create kernel
	double y1[] =
	{ -1.0, -2.0, -1.0 };
	double y2[] =
	{ 0, 0, 0 };
	double y3[] =
	{ 1.0, 2.0, 1.0 };
	vector<vector<double> > yfilter(3);
	yfilter[0].assign(y1, y1 + 3);
	yfilter[1].assign(y2, y2 + 3);
	yfilter[2].assign(y3, y3 + 3);
	int size = (int) yfilter.size();
	int ksize = size / 2;

	int rows = inputMatrix->getRows();
	int cols = inputMatrix->getCols();

	ptr_IntMatrix gyConvol = new Matrix<int>(rows, cols, BLACK_VALUE);
	for (int i = -ksize; i < rows - ksize; i++)
	{
		for (int j = -ksize; j < cols - ksize; j++)
		{
			double sumy = 0;
			for (int x = 0; x < size; x++)
			{
				for (int y = 0; y < size; y++)
				{
					double gValue =
						(i + x < 0 || j + y < 0 || i + x >= rows || j + y >= cols) ?
							getBorderValue(inputMatrix, i + x, j + y) :
							(double) inputMatrix->getAtPosition(i + x, j + y);
					sumy += yfilter[x][y] * gValue;
				}
			}
			//set value
			gyConvol->setAtPosition(i + ksize, j + ksize, sumy);
		}
	}
	yfilter.clear();
	return gyConvol;
}

ptr_IntMatrix sobelOperationCanny(ptr_IntMatrix gaussianImage)
{

	int rows = gaussianImage->getRows();
	int cols = gaussianImage->getCols();

	ptr_IntMatrix filteredImg = new Matrix<int>(rows, cols, WHITE_VALUE);

	angles = new Matrix<double>(rows, cols);

	ptr_IntMatrix dxMatrix = gxSobelConvolution(gaussianImage);
	ptr_IntMatrix dyMatrix = gySobelConvolution(gaussianImage);

	for (int r = 0; r < dxMatrix->getRows(); r++)
	{
		for (int c = 0; c < dxMatrix->getCols(); c++)
		{
			double dx = dxMatrix->getAtPosition(r, c);
			double dy = dyMatrix->getAtPosition(r, c);

			double value = abs(dx) + abs(dy);

			filteredImg->setAtPosition(r, c, value);

			angles->setAtPosition(r, c, (atan2(dy, dx) * 180 / M_PI));

		}
	}
	delete dxMatrix;
	delete dyMatrix;
	return filteredImg;

}

ptr_IntMatrix nonMaxSuppression(ptr_IntMatrix sobelImage)
{
	int rows = sobelImage->getRows();
	int cols = sobelImage->getCols();

	ptr_IntMatrix nonMaxSupped = new Matrix<int>(rows, cols, 0);

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			double tangent = angles->getAtPosition(i, j);

			//Horizontal Edge
			if (((-LIMIT_ANGLE_1 < tangent) && (tangent <= LIMIT_ANGLE_1))
				|| (LIMIT_ANGLE_4 < tangent) || (tangent <= -LIMIT_ANGLE_4))
			{
				int x = (j + 1 >= cols) ? 0 : sobelImage->getAtPosition(i, j + 1);
				int y = (j - 1 < 0) ? 0 : sobelImage->getAtPosition(i, j - 1);
				if ((sobelImage->getAtPosition(i, j) > y)
					&& (sobelImage->getAtPosition(i, j) >= x))
					nonMaxSupped->setAtPosition(i, j, sobelImage->getAtPosition(i, j));

			}
			else
			{
				//Vertical Edge
				if (((-LIMIT_ANGLE_3 < tangent) && (tangent <= -LIMIT_ANGLE_2))
					|| ((LIMIT_ANGLE_2 < tangent) && (tangent <= LIMIT_ANGLE_3)))
				{

					int x = (i + 1 >= rows) ? 0 : sobelImage->getAtPosition(i + 1, j);
					int y = (i - 1 < 0) ? 0 : sobelImage->getAtPosition(i - 1, j);

					if ((sobelImage->getAtPosition(i, j) > y)
						&& (sobelImage->getAtPosition(i, j) >= x))
						nonMaxSupped->setAtPosition(i, j, sobelImage->getAtPosition(i, j));

				}
				else
				{
					//-45 Degree Edge
					if (((-LIMIT_ANGLE_2 < tangent) && (tangent <= -LIMIT_ANGLE_1))
						|| ((LIMIT_ANGLE_3 < tangent) && (tangent <= LIMIT_ANGLE_4)))
					{

						int x =
							(i - 1 < 0 || j + 1 >= cols) ?
								0 : sobelImage->getAtPosition(i - 1, j + 1);
						int y =
							(i + 1 >= rows || j - 1 < 0) ?
								0 : sobelImage->getAtPosition(i + 1, j - 1);
						if ((sobelImage->getAtPosition(i, j) > x)
							&& (sobelImage->getAtPosition(i, j) > y))
							nonMaxSupped->setAtPosition(i, j,
								sobelImage->getAtPosition(i, j));

					}
					else
					{

						//45 Degree Edge
						if (((-LIMIT_ANGLE_4 < tangent) && (tangent <= -LIMIT_ANGLE_3))
							|| ((LIMIT_ANGLE_1 < tangent) && (tangent <= LIMIT_ANGLE_2)))
						{

							int x =
								(i + 1 >= rows || j + 1 >= cols) ?
									0 : sobelImage->getAtPosition(i + 1, j + 1);
							int y =
								(i - 1 < 0 || j - 1 < 0) ?
									0 : sobelImage->getAtPosition(i - 1, j - 1);
							if ((sobelImage->getAtPosition(i, j) > x)
								&& (sobelImage->getAtPosition(i, j) > y))
								nonMaxSupped->setAtPosition(i, j,
									sobelImage->getAtPosition(i, j));
						}
					}
				}
			}
		}
	}

	return nonMaxSupped;
}
ptr_IntMatrix doubleThreshold(ptr_IntMatrix nonMaxImage, int low, int high)
{
	int rows = nonMaxImage->getRows();
	int cols = nonMaxImage->getCols();

	ptr_IntMatrix edgeMatrix = new Matrix<int>(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			edgeMatrix->setAtPosition(i, j, nonMaxImage->getAtPosition(i, j));

			if (nonMaxImage->getAtPosition(i, j) >= high)
				edgeMatrix->setAtPosition(i, j, 255);
			else
			{
				if (nonMaxImage->getAtPosition(i, j) < low)
					edgeMatrix->setAtPosition(i, j, 0);
				else
				{
					//edgeMatrix->setAtPosition(i, j, 0);
					bool anyHigh = false;
					for (int x = i - 1; x < i + 2; x++)
					{
						for (int y = j - 1; y < j + 2; y++)
						{
							if (!(x < 0 || y < 0 || x >= rows || y >= cols))
							{
								if (nonMaxImage->getAtPosition(x, y) >= high)
								{
									edgeMatrix->setAtPosition(i, j, 255);
									nonMaxImage->setAtPosition(i, j, high + 1);
									i = 0;
									j = 0;
									anyHigh = true;
									break;
								}
							}
						}
						if (anyHigh)
							break;
					}
					if (!anyHigh)
						edgeMatrix->setAtPosition(i, j, 0);
				}
			}
		}
	}
	return edgeMatrix;
}
/*
int threshCut(ptr_IntMatrix inputMatrix)
{
	int array[256] =
	{ };
	int rows = inputMatrix->getRows();
	int cols = inputMatrix->getCols();
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int value = inputMatrix->getAtPosition(r, c);
			if (value >= 0 && value <= 255)
			{
				array[value] += 1;
			}
		}
	}
	int max = -1, imax = -1;
	for (int i = 0; i < 256; ++i)
	{
		cout << "\t" << array[i];
		if (array[i] > max)
		{
			max = array[i];
			imax = i;
		}
	}
	int min = max, imin = imax;
	for (int i = imax; i < 256; ++i)
	{
		if (array[i] < min && array[i] != 0)
		{
			min = array[i];
			imin = i;
		}
	}
	return imin;
}
*/
// ========================== Process to find the edges in image =============================================
ptr_IntMatrix cannyProcess(ptr_IntMatrix binaryImage, int lowThreshold,
	int highThreshold, vector<Point> &contourPoints)
{
	ptr_IntMatrix binary2 = postProcess(binaryImage, 255);
	ptr_IntMatrix sobelFilter = sobelOperationCanny(binary2);
	ptr_IntMatrix nonMaxSuppress = nonMaxSuppression(sobelFilter);
	ptr_IntMatrix thresholdImage = doubleThreshold(nonMaxSuppress, lowThreshold,
		highThreshold);
	//delete binary2;
	int rows = thresholdImage->getRows();
	int cols = thresholdImage->getCols();
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			if (thresholdImage->getAtPosition(r, c) == 255)
			{
				contourPoints.push_back(Point(c, r));
			}
		}
	}
	delete sobelFilter;
	delete nonMaxSuppress;

	return thresholdImage;
}
ptr_IntMatrix cannyProcess2(ptr_IntMatrix binaryImage, int lowThreshold,
	int highThreshold, ptr_IntMatrix &gradDirection, vector<Point> &edgePoints)
{
	ptr_IntMatrix binary2 = postProcess(binaryImage, 255);
	ptr_IntMatrix sobelFilter = sobelOperationCanny(binary2);
	ptr_IntMatrix nonMaxSuppress = nonMaxSuppression(sobelFilter);
	ptr_IntMatrix thresholdImage = doubleThreshold(nonMaxSuppress, lowThreshold,
		highThreshold);

	// viet them cho GPH tren pixel
	int rows = binaryImage->getRows();
	int cols = binaryImage->getCols();
	//gradDirection = new Matrix<int>(rows, cols, -1);
	double angle = 0;
	int count = 0;
	edgePoints.clear();
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			if (thresholdImage->getAtPosition(r, c) == 255)
			{
				edgePoints.push_back(Point(c, r));
				angle = angles->getAtPosition(r, c);

				if (angle < 0)
				{
					angle = 360 + angle;
				}
				gradDirection->setAtPosition(r, c, roundToDegree(angle));
				count++;
			}

		}
	}
	cout << "\nTotal canny points: " << count << endl;
	delete binary2;
	delete sobelFilter;
	delete nonMaxSuppress;

	return thresholdImage;
}
