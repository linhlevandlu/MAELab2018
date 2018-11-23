/*
 * PHoughTransform.cpp
 *
 *  Created on: Nov 30, 2016
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

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"

#include "PHTEntry.h"

#include "PHoughTransform.h"

bool closetLine(Line line1, Line line2)
{
	int cond1 = 60;
	int cond2 = 15;
	int cond3 = 5;

	double distance1 = line2.perpendicularDistance(line1.getBegin());
	double distance2 = line2.perpendicularDistance(line1.getEnd());
	if (line1.getLength() > cond1 && line2.getLength() > cond1
		&& line1.angleLines(line2) >= cond2
		&& (distance1 <= cond3 || distance2 <= cond3))
		return true;
	return false;
}
double computeDistance(Line objLine, Point rPoint)
{
	return objLine.perpendicularDistance(rPoint);
}

Point closestPoint(Line objLine, Point origin)
{
	vector<double> equation = objLine.getEquation();
	double a = equation.at(0);
	double b = equation.at(1);
	double c = equation.at(2);
	double x, y;

	if (a == 0 && b == 1)
	{
		x = origin.getX();
		y = c;
	}
	else
	{
		if (a == 1 && b == 0)
		{
			x = c;
			y = origin.getY();
		}
		else
		{
			x = (b * (b * origin.getX() - a * origin.getY()) - (a * c))
				/ (a * a + b * b);
			y = (a * (-b * origin.getX() + a * origin.getY()) - (b * c))
				/ (a * a + b * b);
		}
	}
	return Point(x, y);
}

double computeAngle(Line objLine, Point rPoint)
{
	Line oX(rPoint,
		Point(rPoint.getX() + 100, rPoint.getY()));
	Point pCloset = closestPoint(objLine, rPoint);
	Line distanceLine(rPoint, pCloset);
	double theta = oX.angleLines(distanceLine);

	if (pCloset.getY() > rPoint.getY())
	{
		return 360 - theta;

	}
	return theta;
}

//==================================== Methods in class ==================================================
PHoughTransform::PHoughTransform()
{
	// TODO Auto-generated constructor stub

}

PHoughTransform::~PHoughTransform()
{
	// TODO Auto-generated destructor stub
}

void PHoughTransform::setRefPoint(Point rpoint)
{
	refPoint = rpoint;
}
Point PHoughTransform::getRefPoint()
{

	return refPoint;
}
vector<PHTEntry> PHoughTransform::getPHTEntries()
{
	//if (!phtEntries.empty())
	return phtEntries;
	//return null;
}
vector<PHTEntry> PHoughTransform::constructPHTTable(vector<Line> lines)
{

	vector<PHTEntry> entries;
	/*ofstream ofl("lineValues2.txt");

	for (size_t k = 0; k < lines.size(); k++)
	{
		ptr_Line line = lines.at(k);
		ofl << line->getBegin()->getX() << "\t" << line->getBegin()->getY() << "\n";
		ofl << line->getEnd()->getX() << "\t" << line->getEnd()->getY() << "\n";

	}
	ofl.close();*/
	Line line1,line2;
	for (size_t i = 0; i < lines.size(); i++)
	{
		line1 = lines.at(i);
		HoughSpace hs1;
		hs1.angle = computeAngle(line1, refPoint);
		hs1.distance = computeDistance(line1, refPoint);

		for (size_t j = 0; j < lines.size(); j++)
		{
			line2 = lines.at(j);
			if (i != j && closetLine(line1, line2))
			{
				HoughSpace hs2;
				hs2.angle = computeAngle(line2, refPoint);
				hs2.distance = computeDistance(line2, refPoint);

				PHTEntry entry;// = new PHTEntry();
				entry.setRefLine(line1);
				entry.setObjLine(line2);
				entry.addHoughSpace(hs1);
				entry.addHoughSpace(hs2);

				entries.push_back(entry);
			}
		}
	}
	phtEntries = entries;

	return entries;
}
