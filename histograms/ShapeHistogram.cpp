/*
 * LocalHistogram.cpp
 *
 *  Created on: Oct 18, 2016
 *      Author: linh
 */
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <fstream>
using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"

// the minutes per bin corresponds with angle accuracy
const int m_bin_30 = 120;
const int m_bin_60 = 60; // each bin is
const int m_bin_120 = 30;
const int m_bin_240 = 15;
const int m_bin_360 = 10;
const int m_bin_720 = 5;
const int m_bin_3600 = 1;
#include "ShapeHistogram.h"

GFeature pairwiseHistogram(Line refLine, Line objLine)
{
	GFeature pgh;
	pgh.angle = refLine.angleLines(objLine);

	double distance1 = refLine.perpendicularDistance(objLine.getBegin());
	double distance2 = refLine.perpendicularDistance(objLine.getEnd());
	pgh.dmin = ((distance1 < distance2 ? distance1 : distance2));
	pgh.dmax = ((distance1 < distance2 ? distance2 : distance1));
	return pgh;
}

double addGFeature(LocalHistogram &lhist, GFeature feature)
{
	lhist.features.push_back(feature);
	if (feature.dmax > lhist.maxDistance)
	{
		lhist.maxDistance = feature.dmax;
		return feature.dmax;
	}
	return lhist.maxDistance;
}
int heightOfAngleAxis(AngleAccuracy angleAcc)
{
	if (angleAcc == HaftDegree)
		return 90;
	return angleAcc * 180;
}

int distanceOffset(double maxDistance, double distance, int cols)
{
	double binSize = maxDistance / cols;
	double bin = round(distance / binSize);
	return (bin > 0) ? (bin - 1) : bin;
}
int convertAngleToMinute(double angle)
{
	double degree;
	modf(angle, &degree);
	int minute = (angle - degree) * 60;
	int second = (angle - degree - minute / 60) * 3600;
	if (second >= 30)
		minute += 1;
	return (degree * 60) + minute;

}
int angleOffset(double angle, AngleAccuracy angleAcc)
{
	int m = m_bin_60;
	switch (angleAcc)
	{
	case HaftDegree:
		m = m_bin_30;
		break;
	case Degree:
		m = m_bin_60;
		break;
	case TwoTimeDegree:
		m = m_bin_120;
		break;
	case FourTimeDegree:
		m = m_bin_240;
		break;
	case SixTimeDegree:
		m = m_bin_360;
		break;
	case TwelveTimeDegree:
		m = m_bin_720;
		break;
	case SixtyTimeDegree:
		m = m_bin_3600;
		break;
	default:
		m = m_bin_60;
		break;
	}
	int minutes = convertAngleToMinute(angle);
	double bins = minutes / m;
	double bin = 0;
	double mod = modf(bins, &bin);
	if (mod > 0)
		bin += 1;
	return bin;
}
ShapeHistogram::ShapeHistogram()
{
	//matrix = (ptr_IntMatrix) malloc(sizeof(Matrix<int>));
}
ShapeHistogram::~ShapeHistogram()
{
	//delete matrix;
	//listLocalHistogram.clear();
}
double ShapeHistogram::getEntries()
{
	return totalEntries;
}
void ShapeHistogram::setEntries(double entries)
{
	totalEntries = entries;
}
ptr_IntMatrix ShapeHistogram::getMatrix()
{
	return matrix;
}
void ShapeHistogram::setMatrix(ptr_IntMatrix pghmatrix)
{
	//matrix = (Matrix<int> *) malloc(sizeof(Matrix<int>));
	matrix = pghmatrix;
}
void ShapeHistogram::setMaxDistance(double maxDist)
{
	max_distance = maxDist;
}
double ShapeHistogram::getMaxDistance()
{
	return max_distance;
}
void ShapeHistogram::setLocalHistogram(vector<LocalHistogram> localHist)
{
	listLocalHistogram = localHist;
}
vector<LocalHistogram> ShapeHistogram::getLocalHistogram()
{
	return listLocalHistogram;
}
vector<LocalHistogram> ShapeHistogram::constructPGH(vector<Line> listOfLines)
{
	vector<LocalHistogram> result;
	double mDistance = 0;

	Line refLine, objLine;
	for (size_t t = 0; t < listOfLines.size(); t++)
	{
		refLine = listOfLines.at(t);
		LocalHistogram lh;
		lh.maxDistance = 0;
		for (size_t i = 0; i < listOfLines.size(); i++)
		{
			if (t != i)
			{
				objLine = listOfLines.at(i);
				GFeature pgh = pairwiseHistogram(refLine, objLine);
				addGFeature(lh, pgh);
			}
		}
		result.push_back(lh);
		if (lh.maxDistance > mDistance)
			mDistance = lh.maxDistance;
	}
	setMaxDistance(mDistance);
	setLocalHistogram(result);
	return result;
}

/*
 * Construct a matrix for geometric histogram by normalize followed the angle (rows) and distance (cols)
 */
ptr_IntMatrix ShapeHistogram::constructPGHMatrix(
		vector<LocalHistogram> localHists, AngleAccuracy angleAcc, int cols)
{
	int rows = heightOfAngleAxis(angleAcc);
	double entries = 0;
	ptr_IntMatrix result = new Matrix<int>(rows, cols, 0);

	for (size_t t = 0; t < localHists.size(); t++)
	{
		LocalHistogram lh = localHists.at(t);
		vector<GFeature> lsFeatures = lh.features;
		for (size_t i = 0; i < lsFeatures.size(); i++)
		{
			GFeature feature = lsFeatures.at(i);
			int dmin = distanceOffset(max_distance, feature.dmin, cols);
			int dmax = distanceOffset(max_distance, feature.dmax, cols);
			int rowindex = angleOffset(feature.angle, angleAcc);
			if (!isnan(rowindex))
			{
				entries += (dmax - dmin) + 1;
				for (int k = dmin; k <= dmax; k++)
				{
					if (rowindex >= 0 && k < cols)
					{
						int value = result->getAtPosition(rowindex, k);
						result->setAtPosition(rowindex, k, value + 1);
					}
				}
			}
		}
	}
	setEntries(entries);
	setMatrix(result);
	return result;
}

//================================================================= Public method ===================================

