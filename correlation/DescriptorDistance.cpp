/*
 * DescriptorDistance.cpp
 *
 *  Created on: Feb 28, 2017
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
#include <float.h>
#include <iomanip>
using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"
#include "../segmentation/Filters.h"

#include "DescriptorDistance.h"

Matrix<double> createDescriptor(ptr_IntMatrix mImage, Point lefttop,
	Point rightbot, Matrix<double> &gradient)
{
	int rows = mImage->getRows();
	int cols = mImage->getCols();
	int rpatch = rightbot.getY() - lefttop.getY() + 1;
	int cpatch = rightbot.getX() - lefttop.getX() + 1;
	Matrix<double> orientation(rpatch, cpatch, 0.0);
	int llocation = 0, rlocation = 0, alocation = 0, blocation = 0;
	int i = 0, j = 0;
	for (int r = lefttop.getY(); r <= rightbot.getY(); r++)
	{
		//cout<<"\n";
		for (int c = lefttop.getX(); c <= rightbot.getX(); c++)
		{
			//cout<<mImage->getAtPosition(r,c)<<"\t";
			llocation = rlocation = alocation = blocation = 0;

			if (r - 1 >= 0 && r - 1 < rows)
			{
				alocation = mImage->getAtPosition(r - 1, c);
			}
			if (r + 1 >= 0 && r + 1 < rows)
			{
				blocation = mImage->getAtPosition(r + 1, c);
			}
			if (c - 1 >= 0 && c - 1 < cols)
			{
				llocation = mImage->getAtPosition(r, c - 1);
			}
			if (c + 1 >= 0 && c + 1 < cols)
			{
				rlocation = mImage->getAtPosition(r, c + 1);
			}
			double dy = (double) blocation - (double) alocation;
			double dx = (double) rlocation - (double) llocation;
			double angle= atan2(dy,dx) * 180/M_PI;
			if (angle < 0)
					angle += 360;
				orientation.setAtPosition(i, j, angle);
			/*if (d2 != 0)
			{
				//double angle = atan(d1 / d2) * 180 / M_PI;
				double angle = atan2(d2, d1) * 180 / M_PI;
				if (angle < 0)
					angle += 360;
				orientation.setAtPosition(i, j, angle);

			}
			else
			{
				orientation.setAtPosition(i, j, 0.0);
			}*/
			double magnutide = (dx * dx) + (dy * dy);
			gradient.setAtPosition(i, j, sqrt(magnutide));
			j++;
		}
		i++;
		j = 0;
	}

	return orientation;
}

Matrix<double> createDescriptor_Double(ptr_DoubleMatrix mImage, Point lefttop,
	Point rightbot, Matrix<double> &gradient)
{
	int rows = mImage->getRows();
	int cols = mImage->getCols();
	int rpatch = rightbot.getY() - lefttop.getY() + 1;
	int cpatch = rightbot.getX() - lefttop.getX() + 1;
	Matrix<double> orientation(rpatch, cpatch, 0.0);
	double llocation = 0, rlocation = 0, alocation = 0, blocation = 0;
	int i = 0, j = 0;
	for (int r = lefttop.getY(); r <= rightbot.getY(); r++)
	{
		//cout<<"\n";
		for (int c = lefttop.getX(); c <= rightbot.getX(); c++)
		{
			//cout<<mImage->getAtPosition(r,c)<<"\t";
			llocation = rlocation = alocation = blocation = 0;

			if (r - 1 >= 0 && r - 1 < rows)
			{
				alocation = mImage->getAtPosition(r - 1, c);
			}
			if (r + 1 >= 0 && r + 1 < rows)
			{
				blocation = mImage->getAtPosition(r + 1, c);
			}
			if (c - 1 >= 0 && c - 1 < cols)
			{
				llocation = mImage->getAtPosition(r, c - 1);
			}
			if (c + 1 >= 0 && c + 1 < cols)
			{
				rlocation = mImage->getAtPosition(r, c + 1);
			}
			double dy = (double) alocation - (double) blocation;
			double dx = (double) rlocation - (double) llocation;

			double angle= atan2(dy,dx) * 180/M_PI;
			if (angle < 0)
				angle += 360;
			orientation.setAtPosition(i, j, angle);
			//cout<<"\t"<<dx <<"\t"<<dy<<"\t"<<angle<<endl;
			/*if (d2 != 0)
			{
				//double angle = atan(d1 / d2) * 180 / M_PI;
				double angle = atan2(d2, d1) * 180 / M_PI;
				if (angle < 0)
					angle += 360;
				orientation.setAtPosition(i, j, angle);

			}
			else
			{
				orientation.setAtPosition(i, j, 0.0);
			}*/
			double magnutide = (dx * dx) + (dy * dy);
			gradient.setAtPosition(i, j, sqrt(magnutide));
			j++;
		}
		i++;
		j = 0;
	}

	return orientation;
}

// Compute the vector descriptor for a small patch 4x4
// Output: the vector with 8-dimensions of the descriptor
vector<double> orientHist4(Matrix<double> gradient, Matrix<double> orientation,
	int rbegin, int cbegin, int size) // default size = 4
{
	vector<double> histograms;
	histograms.resize(8, 0.0);
	double grad = 0.0, orien = 0.0;
	//cout<<"\n";
	for (int r = rbegin; r < rbegin + size; r++)
	{
		for (int c = cbegin; c < cbegin + size; c++)
		{
			if (r >= 0 && c >= 0 && r < gradient.getRows() && c < gradient.getCols())
			{
				grad = gradient.getAtPosition(r, c);
				orien = orientation.getAtPosition(r, c);
				//cout<<"\t"<<orien;
				int bin = ceil(orien / 45);
				bin = (bin == 0) ? 0 : (bin - 1);
				histograms.at(bin) += grad;
			}
		}
	}
	//cout<<"\n";
	return histograms;
}

// Compute the descriptor for whole the patch (includes many 4x4 patches)
vector<double> orientHist16(Matrix<double> gradient, Matrix<double> orientation,
	int subsize)
{
	vector<double> histograms, hist;
	for (int r = 0; r < gradient.getRows(); r += subsize)
	{
		for (int c = 0; c < gradient.getCols(); c += subsize)
		{
			
			hist.resize(8, 0.0);
			hist = orientHist4(gradient, orientation, r, c, subsize);
			histograms.insert(histograms.end(), hist.begin(), hist.end());
		}
		
	}
	// normalize the vector (twice)
	double totalValue = 0;
	for (size_t i = 0; i < histograms.size(); i++)
	{
		totalValue += (histograms.at(i) * histograms.at(i));
	}
	totalValue = sqrt(totalValue);
	//  normalizing and theshold < 0.2
	if (totalValue > 0)
	{
		double totalValue2 = 0.0;
		for (int i = 0; i < histograms.size(); i++)
		{
			histograms.at(i) /= totalValue;
			if (histograms.at(i) < 0.2)
				histograms.at(i) = 0;
			totalValue2 += (histograms.at(i) * histograms.at(i));
		}
		//re-normalize
		for (size_t i = 0; i < histograms.size(); i++)
		{
			histograms.at(i) /= totalValue2;
		}
	}

	return histograms;
}

// Compute the L2 distance between two vectors
double l2Distance(vector<double> sourceTarget, vector<double> targetHist)
{
	double distance = 0;
	size_t size = sourceTarget.size();
	if (size == targetHist.size())
	{
		double temp = 0;
		for (size_t i = 0; i < size; i++)
		{
			temp += pow(sourceTarget.at(i) - targetHist.at(i), 2.0);
		}
		distance = sqrt(temp);
	}
	return distance;
}

// Extract a patch in a int_matrix with psize and center point at center
// The method return the top-left and bottom-right corner of patch
Point createPatch(ptr_IntMatrix imageMatrix, int psize, Point center,
	Point &right)
{
	Point left(0, 0);
	right = left;
	int hsize = psize / 2;
	int cx = center.getX();
	int cy = center.getY();
	int rows = imageMatrix->getRows();
	int cols = imageMatrix->getCols();

	int lx = (cx - hsize) < 0 ? 0 : (cx - hsize);
	int ly = (cy - hsize) < 0 ? 0 : (cy - hsize);
	left.setX(lx);
	left.setY(ly);

	int tempx = cx + hsize;
	int tempy = cy + hsize;
	tempx = (psize % 2 != 0) ? tempx : tempx - 1;
	tempy = (psize % 2 != 0) ? tempy : tempy - 1;
	int rx = tempx >= cols ? cols - 1 : tempx;
	int ry = tempy >= rows ? rows - 1 : tempy;
	right.setX(rx);
	right.setY(ry);
	return left;
}
Point createPatch_Double(ptr_DoubleMatrix imageMatrix, int psize, Point center,
	Point &right)
{
	Point left(0, 0);
	right = left;
	int hsize = psize / 2;
	int cx = center.getX();
	int cy = center.getY();
	int rows = imageMatrix->getRows();
	int cols = imageMatrix->getCols();

	int lx = (cx - hsize) < 0 ? 0 : (cx - hsize);
	int ly = (cy - hsize) < 0 ? 0 : (cy - hsize);
	left.setX(lx);
	left.setY(ly);

	int tempx = cx + hsize;
	int tempy = cy + hsize;
	tempx = (psize % 2 != 0) ? tempx : tempx - 1;
	tempy = (psize % 2 != 0) ? tempy : tempy - 1;
	int rx = tempx >= cols ? cols - 1 : tempx;
	int ry = tempy >= rows ? rows - 1 : tempy;
	right.setX(rx);
	right.setY(ry);
	return left;
}
// Find the point in lsPoints that nearest with p
Point nearestPoint(vector<Point> lsPoints, Point p)
{
	double minDistance = DBL_MAX;
	Point ePoint, mPoint;
	for (size_t j = 0; j < lsPoints.size(); j++)
	{
		ePoint = lsPoints.at(j);
		Line ltemp(p, ePoint);
		if (ltemp.getLength() < minDistance)
		{
			minDistance = ltemp.getLength();
			mPoint.setX(ePoint.getX());
			mPoint.setY(ePoint.getY());
		}
	}
	return mPoint;
}

vector<double> SIFTDescriptor(ptr_IntMatrix imgMatrix, Point center, int size)
{
	Point mright(0, 0);
// Apply gaussian blur before calculating the SIFT descriptor
	/*Matrix<double> kernelGaussian = getGaussianKernel(5,1.6);
	Matrix<double> gausMatrix = gaussianBlur_Double(*imgMatrix, kernelGaussian);
	Point mleft = createPatch_Double(&gausMatrix, size, center, mright);
	Matrix<double> mgradient(mright.getY() - mleft.getY() + 1,
		mright.getX() - mleft.getX() + 1, 0.0);
	Matrix<double> mOrient = createDescriptor_Double(&gausMatrix, mleft, mright,
		mgradient);
	vector<double> mHistogram = orientHist16(mgradient, mOrient, 3);*/

	Point mleft = createPatch(imgMatrix, size, center, mright);
	Matrix<double> mgradient(mright.getY() - mleft.getY() + 1,
		mright.getX() - mleft.getX() + 1, 0.0);
	Matrix<double> mOrient = createDescriptor(imgMatrix, mleft, mright,
		mgradient);
	vector<double> mHistogram = orientHist16(mgradient, mOrient, 3);
	return mHistogram;
}

// sceneSize and templSize should be is multiple of 16

vector<Point> verifyDescriptors(ptr_IntMatrix model, ptr_IntMatrix scene,
	vector<Point> manualLM, vector<Point> esLandmarks, int templSize,
	int sceneSize)
{
	vector<Point> result;
	int width = model->getCols();
	int height = model->getRows();
	size_t limit = 0;
	if (manualLM.size() < esLandmarks.size())
		limit = manualLM.size();
	else
		limit = esLandmarks.size();
	cout << "\nLimit: " << limit << endl;
	Point mpi, epi;
	for (size_t i = 0; i < limit; i++)
	{
		Point mleft(0, 0), mright(0, 0), sleft(0, 0), sright(0, 0);
		epi = esLandmarks.at(i);
		mpi = manualLM.at(i);
		if (epi.getX() >= 0 && epi.getY() >= 0 && epi.getX() < width
			&& epi.getY() < height)
		{

			/*mleft = createPatch(model, templSize, mpi, mright);
			 Matrix<double> mgradient(mright.getY() - mleft.getY() + 1,
			 mright.getX() - mleft.getX() + 1, 0.0);
			 Matrix<double> mOrient = createDescriptor(model, mleft, mright,
			 mgradient);
			 vector<double> mHistogram = orientHist16(mgradient, mOrient, templSize);*/
			vector<double> mHistogram;
			mHistogram = SIFTDescriptor(model, mpi, templSize);
			double minDistance = DBL_MAX, maxDistance = 0.0;
			Point minPoint(0, 0), maxPoint(0, 0);
			//mpi.toString();
			sleft = createPatch(scene, sceneSize, epi, sright);
			int count = 0;
			for (int r = sleft.getY(); r < sright.getY(); r++)
			{
				for (int c = sleft.getX(); c < sright.getX(); c++)
				{
					if (scene->getAtPosition(r, c) == 255)
					{
						Point p(c, r);
						/*Point ssleft(0, 0), ssright(0, 0);
						 ssleft = createPatch(scene, templSize, p, ssright);
						 Matrix<double> sgradient(ssright.getY() - ssleft.getY() + 1,
						 ssright.getX() - ssleft.getX() + 1, 0.0);
						 Matrix<double> sOrient = createDescriptor(scene, ssleft, ssright,
						 sgradient);
						 vector<double> sHistogram = orientHist16(sgradient, sOrient,
						 templSize);*/
						vector<double> sHistogram = SIFTDescriptor(scene, p, templSize);
						double distance = l2Distance(mHistogram, sHistogram);
						//cout << "\nDistance: " << distance << endl;
						if (distance > maxDistance)
						{
							maxDistance = distance;
							maxPoint.setX(c);
							maxPoint.setY(r);
						}
						if (distance == minDistance)
						{
							count++;
						}
						if (distance < minDistance)
						{
							minDistance = distance;
							minPoint.setX(c);
							minPoint.setY(r);
						}
					}
				}
			}
			//cout << "\nDuplicate: " << count << endl;
			result.push_back(minPoint);
		}
	}
	return result;
}

/*
 * Create the descriptor for each estimated landmark.
 * But firstly is find out the location of closest point in the contours.
 */
vector<Point> verifyDescriptors2(ptr_IntMatrix model, ptr_IntMatrix scene,
	vector<Point> scenePoints, vector<Point> manualLM, vector<Point> esLandmarks,
	int templSize, int sceneSize)
{
	vector<Point> result;
	int width = model->getCols();
	int height = model->getRows();
	size_t limit = 0;
	if (manualLM.size() < esLandmarks.size())
		limit = manualLM.size();
	else
		limit = esLandmarks.size();
	Point mpi, epi;
	for (size_t i = 0; i < limit; i++)
	{
		Point mleft(0, 0), mright(0, 0), sleft(0, 0), sright(0, 0);
		epi = esLandmarks.at(i);
		mpi = manualLM.at(i);
		if (epi.getX() >= 0 && epi.getY() >= 0 && epi.getX() < width
			&& epi.getY() < height)
		{

			vector<double> mHistogram = SIFTDescriptor(model, mpi, templSize);
			/*mleft = createPatch(model, templSize, mpi, mright);
			 Matrix<double> mgradient(mright.getY() - mleft.getY() + 1,
			 mright.getX() - mleft.getX() + 1, 0.0);
			 Matrix<double> mOrient = createDescriptor(model, mleft, mright,
			 mgradient);
			 vector<double> mHistogram = orientHist16(mgradient, mOrient, templSize);*/
			epi = nearestPoint(scenePoints, epi); // find the closest point with estimated landmark
			sleft = createPatch(scene, sceneSize, epi, sright);
			double minDistance = DBL_MAX;
			Point minPoint(0, 0), maxPoint(0, 0);
			//
			int count = 0;
			for (int r = sleft.getY(); r < sright.getY(); r++)
			{
				for (int c = sleft.getX(); c < sright.getX(); c++)
				{
					Point p(c, r);
					/*Point ssleft(0, 0), ssright(0, 0);
					 ssleft = createPatch(scene, templSize, p, ssright);
					 Matrix<double> sgradient(ssright.getY() - ssleft.getY() + 1,
					 ssright.getX() - ssleft.getX() + 1, 0.0);
					 Matrix<double> sOrient = createDescriptor(scene, ssleft, ssright,
					 sgradient);
					 vector<double> sHistogram = orientHist16(sgradient, sOrient,
					 templSize);*/
					vector<double> sHistogram = SIFTDescriptor(scene, p, templSize);
					double distance = l2Distance(mHistogram, sHistogram);
					//cout << "\nDistance: " << distance << endl;
					if (distance == minDistance)
					{
						count++;
					}
					if (distance < minDistance)
					{
						minDistance = distance;
						minPoint.setX(c);
						minPoint.setY(r);
					}

				}
			}
			//cout << "\nDuplicate: " << count << endl;
			result.push_back(minPoint);
		}
	}
	return result;
}

// estimated the landmarks on list of points belong to the contours
vector<Point> verifyDescriptors3(ptr_IntMatrix model, ptr_IntMatrix scene,
	vector<Point> contourPoints, vector<Point> manualLM, int patchSize)
{
	vector<Point> result;
	int width = model->getCols();
	int height = model->getRows();
	Point mpi(0, 0), epi(0, 0), minPoint(0, 0);

	for (int i = 0; i < manualLM.size(); i++)
	{
		mpi = manualLM.at(i);
		minPoint.reset();
		double minDistance = DBL_MAX;
		vector<double> mHistogram = SIFTDescriptor(model, mpi, patchSize);
		for (int j = 0; j < contourPoints.size(); j++)
		{
			epi = contourPoints.at(j);
			vector<double> sHistogram = SIFTDescriptor(scene, epi, patchSize);
			double distance = l2Distance(mHistogram, sHistogram);
			if (distance < minDistance)
			{
				minDistance = distance;
				minPoint.setX(epi.getX());
				minPoint.setY(epi.getY());
			}
		}
		result.push_back(minPoint);
	}
	return result;
}

double bhatScore(ptr_IntMatrix patch1, ptr_IntMatrix patch2)
{
	int rows1 = patch1->getRows();
	int cols1 = patch1->getCols();
	int rows2 = patch2->getRows();
	int cols2 = patch2->getCols();
	if (rows1 != rows2 || cols1 != cols2)
		return 0;
	double values1 = 0, values2 = 0;
	for (int r = 0; r < rows1; r++)
	{
		for (int c = 0; c < cols1; c++)
		{
			values1 += patch1->getAtPosition(r, c);
		}
	}
	for (int r = 0; r < rows2; r++)
	{
		for (int c = 0; c < cols2; c++)
		{
			values2 += patch2->getAtPosition(r, c);
		}
	}
	for (int r = 0; r < rows2; r++)
	{
		for (int c = 0; c < cols2; c++)
		{
			values2 += patch2->getAtPosition(r, c);
		}
	}
	double distance = 0;
	for (int r = 0; r < rows1; r++)
	{
		for (int c = 0; c < cols1; c++)
		{
			double value_1 = sqrt(patch1->getAtPosition(r, c) / values1);
			double value_2 = sqrt(patch2->getAtPosition(r, c) / values2);
			distance += value_1 * value_2;
		}
	}
	return distance;
}

ptr_IntMatrix extractPatchGray(ptr_IntMatrix grayImage, int pSize, Point center)
{
	if (pSize % 2 == 0)
		pSize += 1;
	int hSize = pSize / 2;
	ptr_IntMatrix patch = new Matrix<int>(pSize, pSize, 0);
	int rows = grayImage->getRows();
	int cols = grayImage->getCols();
	int rbelow = center.getY() - hSize;
	int rabow = center.getY() + hSize;
	int cleft = center.getX() - hSize;
	int cright = center.getX() + hSize;
	if (rbelow < 0 || rabow >= rows || cleft < 0 || cright >= cols)
		return patch;

	int i = 0, j = 0;
	for (int r = rbelow; r <= rabow; r++)
	{
		j = 0;
		for (int c = cleft; c <= cright; c++)
		{
			int value = grayImage->getAtPosition(r, c);
			patch->setAtPosition(i, j, value);
			j++;
		}
		i++;
	}
	return patch;
}

vector<Point> verifyDescriptors4(ptr_IntMatrix model, ptr_IntMatrix scene,
	vector<Point> contourPoints, vector<Point> manualLM, int patchSize)
{
	vector<Point> result;
	int width = model->getCols();
	int height = model->getRows();
	Point mpi(0, 0), epi(0, 0), minPoint(0, 0);

	for (int i = 0; i < manualLM.size(); i++)
	{
		mpi = manualLM.at(i);
		minPoint.reset();
		double maxDistance = -1;
		ptr_IntMatrix mPatch = extractPatchGray(model, patchSize, mpi);

		for (int j = 0; j < contourPoints.size(); j++)
		{
			epi = contourPoints.at(j);
			ptr_IntMatrix sPatch = extractPatchGray(scene, patchSize, epi);

			double distance = bhatScore(mPatch, sPatch);
			if (distance > maxDistance)
			{
				maxDistance = distance;
				minPoint.setX(epi.getX());
				minPoint.setY(epi.getY());
			}
		}
		result.push_back(minPoint);
	}
	return result;
}

