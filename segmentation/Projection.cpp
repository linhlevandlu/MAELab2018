/*
 * Projection.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: linh
 */

#include <iostream>
#include <vector>
#include <math.h>
#include <cmath>
#include <string.h>
#include <queue>

using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"
#include "../io/Reader.h"
#include "Projection.h"

/*
 * viet 4 ham
 * 0. Quantization image
 * 1. horizontal projection
 * 2. vetical projection
 * 3. analysis histogram
 * 4. detecting bounding box
 */

// gray level -> number of bits which used to store value 1<= graylevel <= 8
ptr_IntMatrix quantization(ptr_IntMatrix grayMatrix, int graylevel)
{
	int rows = grayMatrix->getRows();
	int cols = grayMatrix->getCols();
	ptr_IntMatrix result = new Matrix<int>(rows, cols, 0);
	int mask = pow(2.0, 8 - graylevel);
	if (graylevel == 8)
		return grayMatrix;
	int tvalue = 0;
	int newvalue = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			tvalue = grayMatrix->getAtPosition(r, c);
			newvalue = tvalue / mask;
			result->setAtPosition(r, c, newvalue);
		}
	}
	return result;
}

// projection on columns
int* histogramProjection(ptr_IntMatrix binaryMatrix, PROJECTION_TYPE pType,
	int &size)
{
	int brows = binaryMatrix->getRows(); // number of rows of input matrix
	int bcols = binaryMatrix->getCols(); // number of columns of input matrix

	int rows, cols; // rows and cols for process
	if (pType == Vertical_Projection) // vertical projection
	{
		rows = bcols;
		cols = brows;
	}
	else // horizontal projection (default)
	{
		rows = brows;
		cols = bcols;
	}

	int* colProj = new int[cols];
	memset(colProj, 0, sizeof(colProj));
	size = cols;
	int tvalue = 0;
	int count = 0;
	for (int c = 0; c < cols; ++c)
	{
		count = 0;
		for (int r = 0; r < rows; ++r)
		{
			if (pType == Horizontal_Projection)
			{
				tvalue = binaryMatrix->getAtPosition(r, c);

			}
			else
			{
				tvalue = binaryMatrix->getAtPosition(c, r);
			}
			if (tvalue == 0)
			{
				count++;
			}
		}
		colProj[c] = count;
	}
	return colProj;
}

void analysisProjection(int* projection, int psize, int &leftLimit,
	int &rightLimit)
{
	int maxValue = projection[0];
	int imax = 0;
	int pValue = 0;
	// find the max location and value
	for (int i = 1; i < psize; i++)
	{
		pValue = projection[i];
		if (pValue > maxValue)
		{
			maxValue = pValue;
			imax = i;
		}
	}
	//cout<<"\nMax value: "<<maxValue;
	// find the left part
	int currentMax = maxValue;
	int lIndex = imax;
	int flIndex = imax; // store the left limti position
	for (int l = imax - 1; l >= 0; l--)
	{
		pValue = projection[l];
		/*if (pValue < currentMax)
		 {
		 currentMax = pValue;
		 lIndex = l;
		 } else
		 {
		 if (pValue == currentMax)
		 {
		 flIndex = lIndex;
		 }
		 }*/
		if (pValue == 0)
		{
			flIndex = l;
			break;
		}
	}
	//cout << "\nLeft limit: " << flIndex;
	leftLimit = flIndex;
	// find the right part
	currentMax = maxValue;
	int rIndex = imax;
	int frIndex = imax; // store the right limit position
	for (int r = imax; r < psize; r++)
	{
		pValue = projection[r];
		if (pValue == 0)
		{
			frIndex = r;
			break;
		}
		/*if (pValue < currentMax)
		 {
		 currentMax = pValue;
		 rIndex = r;
		 } else
		 {
		 if (pValue == currentMax)
		 {
		 frIndex = rIndex;
		 }
		 }*/
	}
	//cout << "\nRight limit: " << frIndex;
	rightLimit = frIndex;
}

vector<Point> boundingBoxDetection(int* hProjection, int hSize,
	int* vProjection, int vSize)
{
	vector<Point> corners;

	int left = 0, right = 0;
	analysisProjection(hProjection, hSize, left, right);

	int top = 0, bottom = 0;
	analysisProjection(vProjection, vSize, top, bottom);

	Point topleft(left, top);
	Point topright(right, top);
	Point bottomleft(left, bottom);
	Point bottomright(right, bottom);
	corners.push_back(topleft);
	corners.push_back(topright);
	corners.push_back(bottomleft);
	corners.push_back(bottomright);

	return corners;
}

vector<Point> boundingBoxDetection(ptr_IntMatrix grayMatrix)
{
	vector<Point> corners;
	ptr_IntMatrix quanMatrix = quantization(grayMatrix, 1);
	int hpsize = 0;
	int* hProjection = histogramProjection(quanMatrix, Horizontal_Projection, hpsize);
	int left = 0, right = 0;
	analysisProjection(hProjection, hpsize, left, right);

	int vpsize = 0;
	int* vProjection = histogramProjection(quanMatrix, Vertical_Projection, vpsize);
	int top = 0, bottom = 0;
	analysisProjection(vProjection, vpsize, top, bottom);

	Point topleft(left, top);
	Point topright(right, top);
	Point bottomleft(left, bottom);
	Point bottomright(right, bottom);
	corners.push_back(topleft);
	corners.push_back(topright);
	corners.push_back(bottomleft);
	corners.push_back(bottomright);

	// free memory
	delete quanMatrix;

	return corners;
}

/*
 * Chia hinh anh thanh nhieu hinh anh nho voi cung chieu rong
 */
Matrix<int> splitImage(ptr_IntMatrix binMatrix, int rowsSize)
{
	vector<Matrix<int> > results;
	int rows = binMatrix->getRows();
	int cols = binMatrix->getCols();
	int k = 0;
	while (k * rowsSize < rows)
	{
		int begin = k * rowsSize;
		int size = 0;
		if (rows - begin > rowsSize)
			size = rowsSize;
		else
			size = rows - begin;
		Matrix<int> subMatrix(size, cols, 255);
		int m = 0;
		for (int r = begin; r < (k + 1) * rowsSize; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				if (r < rows && c < cols)
				{
					int value = binMatrix->getAtPosition(r, c);
					subMatrix.setAtPosition(m, c, value);
				}
			}
			m++;
		}
		results.push_back(subMatrix);
		k++;
	}
	//cout << "\nNumber of sub-matrix: " << results.size() << endl;
	int l = 0;
	Matrix<int> mResult(rows, cols, 255);
	for (int i = 0; i < results.size(); i++)
	{
		Matrix<int> mt = results.at(i);
		//cout << "\nSize of matrix: " << mt.getRows() << "\t" << mt.getCols();
		checkRegion(mt);
		for (int rs = 0; rs < mt.getRows(); rs++)
		{
			for (int cl = 0; cl < mt.getCols(); cl++)
			{
				int value = mt.getAtPosition(rs, cl);
				mResult.setAtPosition(l, cl, value);
			}
			l++;
		}

	}
	//saveGrayScale("text.jpg",&mResult);
	//checkRegions(&results.at(0));

	return mResult;
}

Matrix<int> splitImageCols(ptr_IntMatrix binMatrix, int colsSize)
{
	vector<Matrix<int> > results;
	int rows = binMatrix->getRows();
	int cols = binMatrix->getCols();
	int k = 0;
	while (k * colsSize < cols)
	{
		int begin = k * colsSize;
		int size = 0;
		if (cols - begin > colsSize)
			size = colsSize;
		else
			size = cols - begin;
		Matrix<int> subMatrix(size, cols, 255);
		int m = 0;
		for (int r = 0; r < rows; r++)
		{
			for (int c = begin; c < (k + 1) * colsSize; c++)
			{
				if (r < rows && c < cols)
				{
					int value = binMatrix->getAtPosition(r, c);
					subMatrix.setAtPosition(r, m, value);
				}
				m++;
			}
			m = 0;
		}
		results.push_back(subMatrix);
		k++;
	}
	saveGrayScale("test.jpg", &results.at(0));
	return *binMatrix;
}

/*
 * Kiem tra cac vung diem anh va xoa
 */
void checkRegion(Matrix<int> &binMatrix)
{
	int rows = binMatrix.getRows();
	int cols = binMatrix.getCols();
	Matrix<int> checkMatrix(rows, cols, 0);

	Point left(0, 0), right(0, 0);
	vector<Line> lines;
	vector<vector<Point> > regions;
	int maxRegion = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			queue<Point> qPoints;
			if (binMatrix.getAtPosition(r, c) == 0
				&& binMatrix.getAtPosition(r, c - 1) == 255)
			{
				if (checkMatrix.getAtPosition(r, c) == 0) // chua check
				{
					qPoints.push(Point(c, r));
					vector<Point> region;
					while (!qPoints.empty())
					{
						Point p = qPoints.front();
						qPoints.pop();
						if (checkMatrix.getAtPosition(p.getY(), p.getX()) == 0)
						{
							region.push_back(p);
							checkMatrix.setAtPosition(p.getY(), p.getX(), 1); // da check
							// kiem tra 8-connected va dua vao danh sach doi
							for (int r1 = p.getY() - 1; r1 <= p.getY() + 1; r1++)
							{
								for (int c1 = p.getX() - 1; c1 <= p.getX() + 1; c1++)
								{
									if (r1 >= 0 && r1 < rows && c1 >= 0 && c1 < cols)
									{
										if (binMatrix.getAtPosition(r1, c1) == 0
											&& checkMatrix.getAtPosition(r1, c1) == 0) // mau den va chua check
										{
											//cout<<"\nabc: "<<c1<<"\t"<<r1;
											qPoints.push(Point(c1, r1));
										}
									}
								}
							}
						}
					}
					if (region.size() > maxRegion)
						maxRegion = region.size();
					//cout << "\nRegion: " << region.size();
					regions.push_back(region);
				}
			}
		}
	}
	for (int k = 0; k < regions.size(); k++)
	{
		vector<Point> region = regions.at(k);
		if (region.size() < maxRegion)
		{
			Point p;
			for (int l = 0; l < region.size(); l++)
			{
				p = region.at(l);
				binMatrix.setAtPosition(p.getY(), p.getX(), 255);
			}
		}
	}
	//cout << "\n Number of regions: " << regions.size() << endl;
}

/*
 * Tao horizontal va vertical projection
 */
void binProjection(ptr_IntMatrix binImage, ptr_IntMatrix &hProjection,
	ptr_IntMatrix &vProjection)
{
	int rows = binImage->getRows();
	int cols = binImage->getCols();
	hProjection = new Matrix<int>(rows, cols, 255);
	vProjection = new Matrix<int>(rows, cols, 255);
	int ri = 0, ci = 0;
	for (int r = 0; r < rows; r++)
	{
		ri = 0;
		for (int c = 0; c < cols; c++)
		{
			if (binImage->getAtPosition(r, c) == 0)
			{
				ri++;
			}
		}
		for (int k = 0; k < ri; k++)
		{
			hProjection->setAtPosition(r, k, 0);
		}
	}
	for (int c = 0; c < cols; c++)
	{
		ci = 0;
		for (int r = 0; r < rows; r++)
		{
			if (binImage->getAtPosition(r, c) == 0)
			{
				ci++;
			}
		}
		for (int k = 0; k < ci; k++)
		{
			vProjection->setAtPosition(rows - k - 1, c, 0);
		}
	}
}
// phan tich horizontal projection
void analysisHistogram(ptr_IntMatrix &histogram, int type, int sizeRegion) // type = 0 - horizontal, 1 - vertical
{
	int rows = histogram->getRows();
	int cols = histogram->getCols();
	int cs = 0, ls = 0;
	int arrSize = 0;
	if (type == 0)
	{
		arrSize = rows;
		ls = rows;
		cs = cols;
	}
	else
	{
		arrSize = cols;
		ls = cols;
		cs = rows;
	}
	int array[arrSize];
	for (int r = 0; r < ls; r++)
	{
		int count = 0;
		for (int c = 0; c < cs; c++)
		{
			if (type == 0)
			{
				if (histogram->getAtPosition(r, c) == 0)
					count++;
			}
			else
			{
				if (histogram->getAtPosition(c, r) == 0)
					count++;
			}
		}
		array[r] = count;
		//cout << "\t" << count;
	}
	// analysis the projection
	vector<int> peakIndex;
	vector<int> valleyIndex;
	if (sizeRegion % 2 == 0)
		sizeRegion += 1;
	int hregion = sizeRegion / 2;
	int peaks = 0, valley = 0;
	for (int i = hregion; i < arrSize - hregion; i++)
	{
		int valuei = array[i];
		int count1 = 0, count2 = 0;
		for (int k = i - hregion; k <= i + hregion; k++)
		{
			if (k != i)
			{
				int valuek = 0;
				if (k < 0 || k >= arrSize)
				{
					valuek = 0;
				}
				else
				{
					valuek = array[k];
				}
				if (valuek < valuei)
				{
					count1++;
				}
				if (valuek > valuei)
				{
					count2++;
				}
				if (valuek == valuei)
				{
					if (count1 == 0 && count2 != 0)
						count2++;
					if (count1 != 0 && count2 == 0)
						count1++;
				}
			}
		}
		if (count1 == 0 && count2 == sizeRegion - 1)
		{
			valley++;
			valleyIndex.push_back(i);
		}
		if (count1 == sizeRegion - 1 && count2 == 0)
		{
			peaks++;
			peakIndex.push_back(i);
		}
	}
	cout << endl << "Number of peaks: " << peaks << endl;
	cout << endl << "Number of valleys: " << valley << endl;
	int maxDistance = 0, maxDistance2 = 0;
	int index = 0, index2 = 0;
	for (int i = 1; i < valleyIndex.size(); i++)
	{
		if (type == 0)
		{
			for (int r = 0; r < cs; r++)
			{
				histogram->setAtPosition(valleyIndex.at(i), r, 0);
			}
		}
		else
		{
			for (int r = 0; r < cs; r++)
			{
				histogram->setAtPosition(r, valleyIndex.at(i), 0);
			}
		}
	}
}
