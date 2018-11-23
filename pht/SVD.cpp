/*
 * SVD.cpp
 *
 *  Created on: Feb 2, 2017
 *      Author: linh
 */
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <fstream>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <pthread.h>
#include <stdlib.h>
using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Edge.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"

#include "../pht/GHTInPoint.h"

#include "../io/Reader.h"
#include "../segmentation/Thresholds.h"

#include "SVD.h"

struct pDescriptor
{
	Point center;
	double LBP;
	double contrast;
};

struct comparextheny
{
	/*bool operator()(Point p1, Point p2)
	 {
	 if (p1.getX() < p2.getX())
	 return true;
	 else
	 {
	 if (p1.getX() == p2.getX())
	 {
	 if (p1.getY() < p2.getY())
	 {
	 return true;
	 }
	 else
	 {
	 return false;
	 }
	 }
	 }
	 return false;
	 }*/
	bool operator()(pDescriptor p1, pDescriptor p2)
	{
		if (p1.LBP < p2.LBP)
			return true;
		else
		{
			if (p1.LBP == p2.LBP)
			{
				if (p1.contrast < p2.contrast)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		return false;
	}
} xtheny;

struct comparex
{
	bool operator()(Point p1, Point p2)
	{
		return p1.getX() < p2.getX();
	}

} cX;
struct comparey
{
	bool operator()(Point p1, Point p2)
	{
		return p1.getY() < p2.getY();
	}
} cY;
pDescriptor calpDescriptor(Matrix<int> patch, Point p)
{
	int rows = patch.getRows();
	int cols = patch.getCols();
	Matrix<int> binary(rows, cols, 0);
	int max = 0, min = DBL_MAX;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int value = patch.getAtPosition(r, c);
			if (value > max)
				max = value;
			if (value < min)
				min = value;
		}
	}
	double cont = (max - min) / (max + min);
	double lbp = 0.0;
	int cvalue = patch.getAtPosition(rows / 2, cols / 2);
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int value = patch.getAtPosition(r, c);
			if (value >= cvalue && r != rows / 2 && c != cols / 2)
			{
				lbp += value;
			}
		}
	}
	pDescriptor pd;
	pd.center.setX(p.getX());
	pd.center.setY(p.getY());
	pd.LBP = lbp;
	pd.contrast = cont;
	return pd;
}
Matrix<int> createPatch(Matrix<int> grayMatrix, Point center, int size)
{
	if (size % 2 == 0)
		size += 1;
	int rows = grayMatrix.getRows();
	int cols = grayMatrix.getCols();
	int hsize = size / 2;
	int x1 = center.getX() - hsize;
	int x2 = center.getX() + hsize;
	int y1 = center.getY() - hsize;
	int y2 = center.getY() + hsize;
	Matrix<int> patch(size, size, 0);
	int i = 0, j = 0;
	for (int r = y1; r <= y2; r++)
	{
		j = 0;
		for (int c = x1; c <= x2; c++)
		{
			if (r >= 0 && r < rows && c >= 0 && c < cols)
			{
				int value = grayMatrix.getAtPosition(r, c);
				patch.setAtPosition(i, j, value);
			}
			j++;
		}
		i++;
	}
	return patch;
}
vector<pDescriptor> createListPatches(Matrix<int> grayMatrix, Point begin,
	Point end, int size)
{
	cout << "\nCreate list of patches" << endl;
	vector<pDescriptor> pds;
	double distance = 0.0, minDistance = DBL_MAX;
	for (int r = begin.getY(); r < end.getY(); r++)
	{
		for (int c = begin.getX(); c < end.getX(); c++)
		{
			Matrix<int> patch = createPatch(grayMatrix, Point(c, r), size);
			pDescriptor pd = calpDescriptor(patch, Point(c, r));
			pds.push_back(pd);
		}
	}
	cout << "\nSorting..." << endl;
	std::sort(pds.begin(), pds.end(), xtheny);
	return pds;
}
vector<pDescriptor> createListPatches2(Matrix<int> grayMatrix, Point begin,
	Point end, int size)
{
	cout << "\nCreate list of patches" << endl;
	vector<pDescriptor> pds;
	double distance = 0.0, minDistance = DBL_MAX;
	int rows = (end.getY() - begin.getY()) / size;
	int cols = (end.getX() - begin.getX()) / size;
	for (int r = begin.getY(); r < begin.getY() + size; r++)
	{
		for (int c = begin.getX(); c < begin.getX() + size; c++)
		{
			Matrix<int> patch = createPatch(grayMatrix, Point(c, r), size);
			pDescriptor pd = calpDescriptor(patch, Point(c, r));
			pds.push_back(pd);
			int i = 0, j = 1;
			int row = i * size + r;
			int col = j * size + c;
			while (i <= rows)
			{
				while (j <= cols)
				{
					col = j * size + c;
					Matrix<int> patch = createPatch(grayMatrix, Point(row, col), size);
					pDescriptor pd = calpDescriptor(patch, Point(row, col));
					pds.push_back(pd);
					j++;
				}
				i++;
				row = i * size + r;
				j = 0;
			}
		}
	}
	cout << "\nSorting..." << endl;
	std::sort(pds.begin(), pds.end(), xtheny);

	return pds;
}
double distancePD(pDescriptor dp1, pDescriptor dp2)
{
	double distance = sqrt(
		pow(dp1.LBP - dp2.LBP, 2) + pow(dp1.contrast - dp2.contrast, 2));
	return distance;
}
pDescriptor binarySearch(vector<pDescriptor> pds, pDescriptor pd)
{
	cout << "\nSearching..." << endl;
	size_t l = 0, r = pds.size() - 1;
	size_t m;
	pDescriptor pdm, pdl, pdr;
	vector<pDescriptor> result;
	while (l < r)
	{
		pdl = pds.at(l);
		pdr = pds.at(r);
		m = floor((l + r) / 2);
		pdm = pds.at(m);

		if (pdm.LBP > pd.LBP)
			r = m - 1;
		if (pdm.LBP < pd.LBP)
			l = m + 1;
		if (pdm.LBP == pd.LBP)
		{
			result.push_back(pdm);
			if (m - l > r - m)
			{
				l += 1;
			}
			else
			{
				r -= 1;
			}
		}
	}
	//duyet lai
	double distance = 0, minDistance = DBL_MAX;
	for (int i = 0; i < result.size(); i++)
	{
		pDescriptor pi = result.at(i);
		distance = sqrt(
			pow(pi.LBP - pd.LBP, 2) + pow(pi.contrast - pd.contrast, 2));
		if (distance < minDistance)
		{
			minDistance = distance;
			pdm = pi;
		}
	}
	return pdm;
}
vector<Point> test(Image modelImage, Image sceneImage,
	vector<Point> mnLandmarks)
{
	vector<Point> result;
	int rows = sceneImage.getGrayMatrix().getRows();
	int cols = sceneImage.getGrayMatrix().getCols();
	int size = 9;

	/*vector<Point> scenePoints;
	 getGradientDMatrix(sceneImage, scenePoints);
	 Point p1, p2;
	 std::sort(scenePoints.begin(), scenePoints.end(), cX);
	 p1.setX(scenePoints.at(0).getX() - size);
	 p2.setX(scenePoints.at(scenePoints.size() - 1).getX() + size);
	 std::sort(scenePoints.begin(), scenePoints.end(), cY);
	 p1.setY(scenePoints.at(0).getY() - size);
	 p2.setY(scenePoints.at(scenePoints.size() - 1).getY() + size);*/

	Point lmi;

	/*vector<pDescriptor> pds = createListPatches(*sceneImage.getGrayMatrix(), p1,
	 p2, size);*/
	vector<pDescriptor> pds;
	vector<Point> sceneMLM = sceneImage.getListOfManualLandmarks();
	for (int k = 0; k < sceneMLM.size(); k++)
	{
		Point p = sceneMLM.at(k);
		Point p1 = p - Point(20, 20);
		Point p2 = p + Point(20, 20);
		vector<pDescriptor> pdsk = createListPatches2(sceneImage.getGrayMatrix(),
			p1, p2, size);
		pds.insert(pds.end(), pdsk.begin(), pdsk.end());
	}
	cout << "\nNumber of descriptors: " << pds.size() << endl;
	std::sort(pds.begin(), pds.end(), xtheny);
	for (int i = 0; i < mnLandmarks.size(); i++)
	{
		lmi = mnLandmarks.at(i);
		Matrix<int> mpatch = createPatch(modelImage.getGrayMatrix(), lmi, size);
		pDescriptor mpd = calpDescriptor(mpatch, lmi);
		pDescriptor lm = binarySearch(pds, mpd);
		result.push_back(lm.center);
	}
	return result;
}

/*ptr_IntMatrix getContour(ptr_IntMatrix grayMatrix)
 {

 saveGrayScale("hbcMatrix.jpg",hMatrix);
 saveGrayScale("vbcMatrix.jpg",vMatrix);
 saveGrayScale("dbcMatrix.jpg",dMatrix);
 return SobelOperation(grayMatrix);
 }*/

vector<Edge> test2(Image image)
{
	Matrix<RGB> rgbData = image.getRGBMatrix();
	ptr_RGBMatrix rgbmatrix = &rgbData;
	ptr_IntMatrix greenChannel = new Matrix<int>(rgbmatrix->getRows(),
		rgbmatrix->getCols(), 0);
	for (int r = 0; r < rgbmatrix->getRows(); r++)
	{
		for (int c = 0; c < rgbmatrix->getCols(); c++)
		{
			RGB color = rgbmatrix->getAtPosition(r, c);
			greenChannel->setAtPosition(r, c, color.G);
		}
	}
	//getContour(greenChannel);
	Image image2;
	image2.setGrayMatrix(*greenChannel);
	vector<Point> cPoints;
	vector<Edge> edges = image2.cannyAlgorithm(cPoints);
	return edges;
}
// ========================== Test method =======================

ptr_RGBMatrix chooseHighest(ptr_RGBMatrix inputImage, int type) // 0 = R, 1 = G, 2 = B
{
	int rows = inputImage->getRows();
	int cols = inputImage->getCols();
	RGB color;
	color = 0;
	ptr_RGBMatrix result = new Matrix<RGB>(rows, cols, color);
	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			color = inputImage->getAtPosition(r, c);
			if (type == 0)
			{
				if (color.R >= color.G && color.R >= color.B)
				{
					result->setAtPosition(r, c, color);
				}
			}
			if (type == 1)
			{
				if (color.G >= color.R && color.G >= color.B)
				{
					result->setAtPosition(r, c, color);
				}
			}
			if (type == 2)
			{
				if (color.B >= color.G && color.B >= color.R)
				{
					result->setAtPosition(r, c, color);
				}
			}
		}
	}
	return result;
}

ptr_IntMatrix splitChannel(ptr_RGBMatrix inputImage, int channel) // 0 = R, 1 = G, 2 = B
{
	int rows = inputImage->getRows();
	int cols = inputImage->getCols();
	RGB color;
	color = 0;
	ptr_IntMatrix result = new Matrix<int>(rows, cols, 0);
	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			color = inputImage->getAtPosition(r, c);
			if (channel == 0)
			{
				result->setAtPosition(r, c, color.R);
			}
			if (channel == 1)
			{
				result->setAtPosition(r, c, color.G);

			}
			if (channel == 2)
			{
				result->setAtPosition(r, c, color.B);

			}
		}
	}
	return result;
}

Matrix<RGB> extractAPatch(ptr_RGBMatrix rgbMatrix, int wpatch, int hpatch,
	Point center)
{
	RGB color;
	color.R = color.G = color.B = 0;
	return rgbMatrix->extractPatch(wpatch, hpatch, center.getY(), center.getX(),
		color);
}
void extractPatches(ptr_RGBMatrix rgbMatrix, int wpatch, int hpatch,
	string foldersave)
{
	int rows = rgbMatrix->getRows();
	int cols = rgbMatrix->getCols();
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			Matrix<RGB> onePatch = extractAPatch(rgbMatrix, wpatch, hpatch,
				Point(c, r));
			stringstream sname;
			sname << "patch_" << r << "_" << c << ".jpg";
			string name = sname.str();
			saveRGB((foldersave + name).c_str(), &onePatch);
		}
	}
}
