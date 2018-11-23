/*
 * Filters.cpp
 *
 *  Created on: Apr 7, 2017
 *      Author: linh
 */
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <time.h>
#include <float.h>

using namespace std;
#include "../imageModel/Point.h"
#include "../imageModel/Matrix.h"
#include "Canny.h"
#include "Filters.h"

Matrix<double> getGaussianKernel(int ksize, double sigma)
{
	Matrix<double> kernel(ksize, ksize, 0.0);
	if (sigma <= 0)
	{
		sigma = 0.3 * ((ksize - 1) * 0.5 - 1) + 0.8;
	}
	double mean = ksize / 2;
	double scale = (2 * sigma * sigma);
	double evalue;
	double sum = 0;
	for (int i = 0; i < ksize; i++)
	{
		for (int j = 0; j < ksize; j++)
		{
			evalue = 0.0;
			double r = pow(i - mean, 2) + pow(j - mean, 2);
			evalue = exp(-r / scale) / (scale * M_PI);
			kernel.setAtPosition(i, j, evalue);
			sum += evalue;
		}
	}
	// normalized
	for (int i = 0; i < ksize; i++)
	{
		for (int j = 0; j < ksize; j++)
		{
			kernel.setAtPosition(i, j, kernel.getAtPosition(i, j) / sum);
			//cout << "\t" << kernel.getAtPosition(i, j);
		}
		//cout << endl;
	}
	return kernel;
}

Matrix<int> gaussianBlur(Matrix<int> inputMatrix, Matrix<double> kernel)
{
	int rows = inputMatrix.getRows();
	int cols = inputMatrix.getCols();
	int krows = kernel.getRows();
	int kcols = kernel.getCols();

	int begin = krows / 2;
	//Matrix<int> result(rows, cols);
	for (int r = -begin; r < rows - begin; r++)
	{
		for (int c = -begin; c < cols - begin; c++)
		{
			double sum = 0;
			for (int i = 0; i < krows; i++)
			{
				for (int j = 0; j < kcols; j++)
				{
					double kvalue = kernel.getAtPosition(i, j);
					double gValue =
						(i + r < 0 || j + c < 0 || i + r >= rows || j + c >= cols) ?
							getBorderValue(&inputMatrix, i + r, j + c) :
							(double) inputMatrix.getAtPosition(i + r, j + c);
					sum += kvalue * gValue;
				}
			}
			inputMatrix.setAtPosition(r + begin, c + begin, sum);
		}
	}
	return inputMatrix;
}
Matrix<double> gaussianBlur_Double(Matrix<int> inputMatrix, Matrix<double> kernel)
{
	int rows = inputMatrix.getRows();
	int cols = inputMatrix.getCols();
	int krows = kernel.getRows();
	int kcols = kernel.getCols();

	int begin = krows / 2;
	Matrix<double> result(rows, cols);
	for (int r = -begin; r < rows - begin; r++)
	{
		for (int c = -begin; c < cols - begin; c++)
		{
			double sum = 0;
			for (int i = 0; i < krows; i++)
			{
				for (int j = 0; j < kcols; j++)
				{
					double kvalue = kernel.getAtPosition(i, j);
					double gValue =
						(i + r < 0 || j + c < 0 || i + r >= rows || j + c >= cols) ?
							getBorderValue(&inputMatrix, i + r, j + c) :
							(double) inputMatrix.getAtPosition(i + r, j + c);
					sum += kvalue * gValue;
				}
			}
			result.setAtPosition(r + begin, c + begin, sum);
		}
	}
	return result;
}
Matrix<int> RobertOperation(ptr_IntMatrix grayMatrix)
{
	int rows = grayMatrix->getRows();
	int cols = grayMatrix->getCols();
	Matrix<int> dMatrix(rows, cols, 0);
	int v = 0, v1 = 0, v2 = 0;
	int v3 = 0, v4 = 0;
	int gx = 0, gy = 0, gxy = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			v1 = grayMatrix->getAtPosition(r, c);
			if (r + 1 < rows)
			{
				v2 = grayMatrix->getAtPosition(r + 1, c);
				if (c + 1 < cols)
				{
					v3 = grayMatrix->getAtPosition(r + 1, c + 1);
				}
			}
			if (c + 1 < cols)
			{
				v4 = grayMatrix->getAtPosition(r, c + 1);
			}
			gx = v1 - v3;
			gy = v4 - v2;
			gxy = abs(gx) + abs(gy);
			//gxy = pow(pow(gx, 2) + pow(gy, 2), 0.5);
			dMatrix.setAtPosition(r, c, gxy);
		}
	}
	return dMatrix;
}

Matrix<int> SobelOperation(ptr_IntMatrix grayMatrix)
{
	int rows = grayMatrix->getRows();
	int cols = grayMatrix->getCols();
	Matrix<int> dMatrix(rows, cols, 0);
	int v1 = 0, v2 = 0, v3 = 0;
	int v4 = 0, v5 = 0, v6 = 0;
	int v7 = 0, v8 = 0, v9 = 0;
	int gx = 0, gy = 0, gxy = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			v1 = v2 = v3 = v4 = v5 = v6 = 0;
			v7 = v8 = v9 = gx = gy = gxy = 0;

			v5 = grayMatrix->getAtPosition(r, c);
			if (r - 1 >= 0)
			{
				v2 = grayMatrix->getAtPosition(r - 1, c);
				if (c - 1 >= 0)
				{
					v1 = grayMatrix->getAtPosition(r - 1, c - 1);
				}
				if (c + 1 < cols)
				{
					v3 = grayMatrix->getAtPosition(r - 1, c + 1);
				}
			}
			if (r + 1 < rows)
			{
				v8 = grayMatrix->getAtPosition(r + 1, c);
				if (c - 1 >= 0)
				{
					v7 = grayMatrix->getAtPosition(r + 1, c - 1);
				}
				if (c + 1 < cols)
				{
					v9 = grayMatrix->getAtPosition(r + 1, c + 1);
				}
			}
			if (c - 1 >= 0)
			{
				v4 = grayMatrix->getAtPosition(r, c - 1);
			}
			if (c + 1 < cols)
			{
				v6 = grayMatrix->getAtPosition(r, c + 1);
			}
			gx = ((v7 + 2 * v8 + v9) - (v1 + 2 * v2 + v3)) / 4;
			gy = ((v3 + 2 * v6 + v9) - (v1 + 2 * v4 + v7)) / 4;
			gxy = abs(gx) + abs(gy);
			dMatrix.setAtPosition(r, c, gxy);
		}
	}
	return dMatrix;
}
Matrix<double> SobelOperation_Double(ptr_IntMatrix grayMatrix)
{
	int rows = grayMatrix->getRows();
	int cols = grayMatrix->getCols();
	cout<<"rows - cols: "<<rows <<"\t"<<cols<<endl;
	Matrix<double> dMatrix(rows, cols, 0.0);
	int v1 = 0, v2 = 0, v3 = 0;
	int v4 = 0, v5 = 0, v6 = 0;
	int v7 = 0, v8 = 0, v9 = 0;
	double gx = 0, gy = 0, gxy = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			v1 = v2 = v3 = v4 = v5 = v6 = 0;
			v7 = v8 = v9 = gx = gy = gxy = 0;

			v5 = grayMatrix->getAtPosition(r, c);
			if (r - 1 >= 0)
			{
				v2 = grayMatrix->getAtPosition(r - 1, c);
				if (c - 1 >= 0)
				{
					v1 = grayMatrix->getAtPosition(r - 1, c - 1);
				}
				if (c + 1 < cols)
				{
					v3 = grayMatrix->getAtPosition(r - 1, c + 1);
				}
			}
			if (r + 1 < rows)
			{
				v8 = grayMatrix->getAtPosition(r + 1, c);
				if (c - 1 >= 0)
				{
					v7 = grayMatrix->getAtPosition(r + 1, c - 1);
				}
				if (c + 1 < cols)
				{
					v9 = grayMatrix->getAtPosition(r + 1, c + 1);
				}
			}
			if (c - 1 >= 0)
			{
				v4 = grayMatrix->getAtPosition(r, c - 1);
			}
			if (c + 1 < cols)
			{
				v6 = grayMatrix->getAtPosition(r, c + 1);
			}
			gx = (double)(((v7 + 2 * v8 + v9) - (v1 + 2 * v2 + v3))) / 4;
			gy = double(((v3 + 2 * v6 + v9) - (v1 + 2 * v4 + v7))) / 4;
			gxy = abs(gx) + abs(gy);
			dMatrix.setAtPosition(r, c, gxy);
			cout<<"\t"<<round(atan(gy/gx) * 180/M_PI);
		}
		cout<<"\n";
	}
	return dMatrix;
}

Matrix<int> postFilter(Matrix<int> sobelResult)
{
	int rows = sobelResult.getRows();
	int cols = sobelResult.getCols();
	/*
	 * Inverse the result
	 */
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int value = sobelResult.getAtPosition(r, c);
			if (value == 0)
			{
				sobelResult.setAtPosition(r, c, 0);
			}
			else
			{
				sobelResult.setAtPosition(r, c, 255);
			}
		}
	}

	return sobelResult;
}

double intraClasses(double pro[256], int threshold)
{
	double weight1 = 0, weight2 = 0;
	for (int i = 0; i < 256; i++)
	{
		double p = pro[i];
		if (i < threshold)
		{
			weight1 += p;
		}
		else
		{
			weight2 += p;
		}
	}
	//cout<<"\nWeights: "<<weight1<<"\t"<<weight2<<endl;
	double muy1 = 0, muy2 = 0;
	for (int i = 0; i < 256; i++)
	{
		double p = pro[i];
		if (i < threshold)
		{
			muy1 += (i *(p/weight1));
		}
		else
		{
			muy2 += (i *(p/weight2));
		}
	}
	double sigma = weight1 * weight2 * pow((muy1 - muy2),2);
	return sigma;
}

int thresholdOtsu(Matrix<int> sobelResult)
{
	int rows = sobelResult.getRows();
	int cols = sobelResult.getCols();
	int values = rows * cols;
	int hist[256] =
	{ };
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int value = sobelResult.getAtPosition(r, c);
			hist[value] += 1;
		}
	}
	double pro[256] =
	{ };

	for (int i = 0; i < 256; i++)
	{
		pro[i] = (double)hist[i] / (double)values;
	}
	double sigmaMax = 0;
	int index = 0;
	for (int i = 0; i < 256; i++) {
		double sigma = intraClasses(pro,i);
		if(sigma > sigmaMax)
		{
			sigmaMax = sigma;
			index = i;
		}
	}
	return index;
}

// =================================== Some binary operations =============================
ptr_IntMatrix erode(ptr_IntMatrix binMatrix, int kernelSize)
{
	if (kernelSize % 2 == 0)
		kernelSize += 1;
	int hSize = kernelSize / 2;
	int rows = binMatrix->getRows();
	int cols = binMatrix->getCols();
	ptr_IntMatrix result = new Matrix<int>(rows, cols, 0);
	*result = *binMatrix;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int countValue = 0;
			for (int rs = r - hSize; rs <= r + hSize; rs++)
			{
				for (int cs = c - hSize; cs <= c + hSize; cs++)
				{
					int value;
					if (rs < 0 || rs >= rows || cs < 0 || cs >= cols)
					{
						value = 0;
					}
					else
					{
						value = binMatrix->getAtPosition(rs, cs);
					}
					if (value == 0)
						countValue++;
				}
			}
			if (countValue != (kernelSize * kernelSize))
				result->setAtPosition(r, c, 255);
		}
	}
	return result;
}
ptr_IntMatrix dilate(ptr_IntMatrix binMatrix, int kernelSize)
{
	if (kernelSize % 2 == 0)
		kernelSize += 1;
	int hSize = kernelSize / 2;
	int rows = binMatrix->getRows();
	int cols = binMatrix->getCols();
	ptr_IntMatrix result = new Matrix<int>(rows, cols, 0);
	*result = *binMatrix;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int countValue = 0;
			for (int rs = r - hSize; rs <= r + hSize; rs++)
			{
				for (int cs = c - hSize; cs <= c + hSize; cs++)
				{
					int value;
					if (rs < 0 || rs >= rows || cs < 0 || cs >= cols)
					{
						value = 0;
					}
					else
					{
						value = binMatrix->getAtPosition(rs, cs);
					}
					if (value == 0)
						countValue++;
				}
			}
			if (countValue != 0)
				result->setAtPosition(r, c, 0);
		}
	}
	return result;
}

ptr_IntMatrix openBinary(ptr_IntMatrix binMatrix, int kernelSize)
{
	ptr_IntMatrix erosion = erode(binMatrix, kernelSize);
	return dilate(erosion, kernelSize);
}

ptr_IntMatrix closeBinary(ptr_IntMatrix binMatrix, int kernelSize)
{
	ptr_IntMatrix dilation = dilate(binMatrix, kernelSize);
	return erode(dilation, kernelSize);
}
