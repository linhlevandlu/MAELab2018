/*
 * PCA.cpp
 *
 *  Created on: Feb 7, 2017
 *      Author: linh
 */
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <algorithm>
using namespace std;
#include "../utils/Converter.h"
#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"
#include "../segmentation/Thresholds.h"
#include "../segmentation/Canny.h"
#include "../correlation/CrossCorrelation.h"
#include "../correlation/DescriptorDistance.h"
#include "../utils/Drawing.h"
#include "../io/Reader.h"
#include "GHTInPoint.h"
#include "PCA.h"
double avgDistance(vector<Point> listPoints, Line axis)
{
	double totalDist = 0;
	size_t nPoints = listPoints.size();
	for (size_t j = 0; j < nPoints; j++)
	{
		Point pj = listPoints.at(j);
		double distance = axis.perpendicularDistance(pj);
		totalDist += distance;
	}
	return totalDist / (int) nPoints;
}

// compute centroid point from the gradient matrix of image
// output: centroid point and list points of edge.
Point centroidPoint(ptr_IntMatrix gradMatrix, vector<Point> &listPoints)
{
	int rows = gradMatrix->getRows();
	int cols = gradMatrix->getCols();
	int totalX = 0, totalY = 0, count = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			if (gradMatrix->getAtPosition(r, c) != -1)
			{
				listPoints.push_back(Point(c, r));
				totalX += c;
				totalY += r;
				count++;
			}
		}
	}
	return Point(totalX / count, totalY / count);
}

Line principalAxis(vector<Point> listOfPoints, Point &cPoint)
{
	double minAvgDist = DBL_MAX;
	Point sPoint;
	size_t nPoints = listOfPoints.size();
	int totalX = 0, totalY = 0;
	Point pi;
	for (size_t i = 0; i < nPoints; i++)
	{
		pi = listOfPoints.at(i);
		totalX += pi.getX();
		totalY += pi.getY();
	}
	cPoint.setX(totalX / (int) nPoints);
	cPoint.setY(totalY / (int) nPoints);
	pi.setX(0);
	pi.setY(0);
	for (size_t i = 0; i < nPoints; i++)
	{
		pi = listOfPoints.at(i);
		Line l(cPoint, pi);
		double avgDist = avgDistance(listOfPoints, l);
		if (avgDist < minAvgDist)
		{
			minAvgDist = avgDist;
			sPoint.setX(pi.getX());
			sPoint.setY(pi.getY());
		}
	}

	return Line(cPoint, sPoint);
}
Line principalAxis(ptr_IntMatrix gradMatrix, Point &cPoint)
{
	vector<Point> listOfPoints;
	cPoint = centroidPoint(gradMatrix, listOfPoints);
	return principalAxis(listOfPoints, cPoint);

}
// negative for clockwise, positive for counterclockwise
// two lines have the same origin
double rotateDirection(Line refLine, Line objLine, double angle)
{
	int xnew = 0, ynew = 0;
	Point origin = refLine.getBegin();
	Point rpoint = objLine.getEnd();
	rotateAPoint(rpoint.getX(), rpoint.getY(), origin, angle, 1, xnew, ynew);
	double distance1 = refLine.perpendicularDistance(Point(xnew, ynew));
	xnew = ynew = 0;
	rotateAPoint(rpoint.getX(), rpoint.getY(), origin, -angle, 1, xnew, ynew);
	double distance2 = refLine.perpendicularDistance(Point(xnew, ynew));
	if (distance2 < distance1)
		angle = -angle;
	return angle;
}
// estimated by using PCAI
struct comparey
{
	bool operator()(Point p1, Point p2)
	{
		return p1.getY() < p2.getY();
	}
} yComparation;
struct comparex
{
	bool operator()(Point p1, Point p2)
	{
		return p1.getX() < p2.getX();
	}
} xComparation;
vector<Point> PCAIPoints(vector<Point> modelPoints, Point mPoint,
	vector<Point> scenePoints, double angle)
{

	size_t nlimit = modelPoints.size() / 2;
	std::sort(modelPoints.begin(), modelPoints.end(), yComparation);
	vector<Point> newModel(modelPoints.begin(), modelPoints.begin() + nlimit);
	vector<Point> sceneTemp(scenePoints); // copy the points of scene
	int index = 0;
	double minAngle = 360;
	cout << "\nFirst angle: " << angle << endl;
	while (angle > 1.5)
	{
		index++;
		std::sort(sceneTemp.begin(), sceneTemp.end(), yComparation);
		vector<Point> newScene(sceneTemp.begin(), sceneTemp.begin() + nlimit);
		Point cPoint1, cPoint2;
		Line l1 = principalAxis(newModel, cPoint1);
		Line l2 = principalAxis(newScene, cPoint2);
		angle = l1.angleLines(l2);
		if (index > 20 || angle < 1.5)
		{
			if (angle < minAngle)
				minAngle = angle;
		}
		Point diff = cPoint1 - cPoint2;
		l2.setBegin(l1.getBegin());
		l2.setEnd(
			Point(l2.getEnd().getX() + diff.getX(),
				l2.getEnd().getY() + diff.getY()));
		double angleR2 = rotateDirection(l1, l2, l1.angleLines(l2));

		Point psn;
		for (size_t i = 0; i < sceneTemp.size(); i++)
		{
			psn = sceneTemp.at(i);
			int xnew = psn.getX() + diff.getX(), ynew = psn.getY() + diff.getY();
			rotateAPoint(xnew, ynew, mPoint, angleR2, 1, xnew, ynew);
			sceneTemp.at(i).setX(xnew);
			sceneTemp.at(i).setY(ynew);
		}
		if (index > 30)
		{
			if ((int) angle == (int) minAngle)
			{
				break;
			}
		}
	}
	cout << "\nMin angle: " << minAngle << endl;
	if (minAngle > 10)
	{
		sceneTemp = scenePoints;
	}
	return sceneTemp;
}

Point checkOneDirection(vector<Point> contours, Point pi, Point closest)
{
	int mIndex = -1;
	Point p(0, 0);
	for (int k = 0; k < contours.size(); k++)
	{
		if (closest == contours.at(k))
		{
			mIndex = k;
			break;
		}
	}
	if (mIndex != -1)
	{

		Point pyNext(0, 0), pyPrev(0, 0);

		Point p1(0, 0), p2(0, 0);
		if (mIndex < contours.size() - 1)
		{
			pyNext = contours.at(mIndex + 1);
			// check line 1
			vector<Point> pOnLine1 = detectLine(Line(pyNext, closest));
			bool onLine = checkPointInList(pOnLine1, pi);
			if (onLine)
			{
				p1 = pi;
			}
			else
			{
				Point pi1 = nearestPoint(pOnLine1, pi);
				p1 = pi1;
			}
		}
		if (mIndex != 0)
		{
			pyPrev = contours.at(mIndex - 1);
			// check line 2
			vector<Point> pOnLine2 = detectLine(Line(pyPrev, closest));
			bool onLine2 = checkPointInList(pOnLine2, pi);
			if (onLine2)
			{
				p2 = pi;
			}
			else
			{
				Point pi2 = nearestPoint(pOnLine2, pi);
				p2 = pi2;
			}
		}
		Line l1(pi, p1);
		Line l2(pi, p2);
		if (l1.getLength() <= l2.getLength())
			p = p1;
		else
			p = p2;
	}
	return p;
}
vector<Point> refine(vector<Point> contours, vector<Point> estlm)
{
	vector<Point> results;
	for (size_t i = 0; i < estlm.size(); i++)
	{
		Point pi = estlm.at(i);
		bool onCurve = checkPointInList(contours, pi);
		if (!onCurve)
		{
			Point mPoint = nearestPoint(contours, pi);
			std::sort(contours.begin(), contours.end(), yComparation);
			Point py = checkOneDirection(contours, pi, mPoint);

			std::sort(contours.begin(), contours.end(), yComparation);
			Point px = checkOneDirection(contours, pi, mPoint);

			Line lx(pi, px);
			Line ly(pi, py);
			if (lx.getLength() <= ly.getLength())
				results.push_back(px);
			else
				results.push_back(py);
		}
		else
		{
			results.push_back(pi);
		}
	}
	return results;
}
vector<Point> PCAI(vector<Point> modelPoints, Image &sceneGray,
	vector<Point> mnLandmarks)
{
	int rows = sceneGray.getGrayMatrix().getRows();
	int cols = sceneGray.getGrayMatrix().getCols();
	ptr_IntMatrix lastModel = new Matrix<int>(rows, cols, 0);
	ptr_IntMatrix lastScene = new Matrix<int>(rows, cols, 0);

	Point mPoint;
	Line mAxis = principalAxis(modelPoints, mPoint);

	vector<Point> scenePoints;
	ptr_IntMatrix sceneGrandient = new Matrix<int>(rows, cols, -1);
	*sceneGrandient = *(getGradientDMatrix(sceneGray, scenePoints));
	vector<Point> scenePoints2(scenePoints); // keep the original points of the scene
	Point sPoint;
	Line sAxis = principalAxis(sceneGrandient, sPoint);
	Point send = sAxis.getEnd();
	double angle = mAxis.angleLines(sAxis);

	// difference between two centroids
	int dx = mPoint.getX() - sPoint.getX();
	int dy = mPoint.getY() - sPoint.getY();

	// move the scene to the model
	sAxis.setBegin(mPoint);
	send.setX(send.getX() + dx);
	send.setY(send.getY() + dy);
	sAxis.setEnd(send);
	Point pi;
	// Detecting the rotated direction
	double angleR = rotateDirection(mAxis, sAxis, angle);

	RGB color;
	color.B = 255;
	color.R = color.G = 255;

	// rotate and translate the scene to match with the model
	color.R = color.G = color.B = 0;
	for (size_t i = 0; i < scenePoints.size(); i++)
	{
		pi = scenePoints.at(i);
		int xnew = 0, ynew = 0;
		rotateAPoint(pi.getX() + dx, pi.getY() + dy, mPoint, angleR, 1, xnew, ynew);
		scenePoints.at(i).setX(xnew);
		scenePoints.at(i).setY(ynew);
	}
	color.R = 255;
	color.B = 0;
	// hien thu ket qua lan thu nhat cua model
	for (size_t i = 0; i < modelPoints.size(); i++)
	{
		pi = modelPoints.at(i);
		if (pi.getX() >= 0 && pi.getX() < cols && pi.getY() >= 0
			&& pi.getY() < rows)
		{
			lastModel->setAtPosition(pi.getY(), pi.getX(), 255);
		}
	}
	vector<Point> sceneTemp;
	sceneTemp = PCAIPoints(modelPoints, mPoint, scenePoints, angle);
	// display scene points
	color.R = 0;
	color.B = 255;
	for (size_t i = 0; i < sceneTemp.size(); i++)
	{
		pi = sceneTemp.at(i);
		if (pi.getX() >= 0 && pi.getX() < cols && pi.getY() >= 0
			&& pi.getY() < rows)
		{
			lastScene->setAtPosition(pi.getY(), pi.getX(), 255);
		}
	}


// indicate the bounding box of the edges
	Point mLeft, mRight;
	std::sort(modelPoints.begin(), modelPoints.end(), xComparation);
	mLeft.setX(modelPoints.at(0).getX());
	mRight.setX(modelPoints.at(modelPoints.size() - 1).getX());
	std::sort(modelPoints.begin(), modelPoints.end(), yComparation);
	mLeft.setY(modelPoints.at(0).getY());
	mRight.setY(modelPoints.at(modelPoints.size() - 1).getY());

	Point sLeft, sRight;
	std::sort(sceneTemp.begin(), sceneTemp.end(), xComparation);
	sLeft.setX(sceneTemp.at(0).getX());
	sRight.setX(sceneTemp.at(sceneTemp.size() - 1).getX());
	std::sort(sceneTemp.begin(), sceneTemp.end(), yComparation);
	sLeft.setY(sceneTemp.at(0).getY());
	sRight.setY(sceneTemp.at(sceneTemp.size() - 1).getY());
	Line mX(mLeft, Point(mRight.getX(), mLeft.getY()));
	Line mY(mLeft, Point(mLeft.getX(), mRight.getY()));
	Line sX(sLeft, Point(sRight.getX(), sLeft.getY()));
	Line sY(sLeft, Point(sLeft.getX(), sRight.getY()));
	double scaleX = mX.getLength() / sX.getLength();
	double scaleY = mY.getLength() / sY.getLength();
	vector<Point> scenePointbk = sceneTemp;
	for (size_t i = 0; i < sceneTemp.size(); i++)
	{
		Point mi = sceneTemp.at(i);
		int x = round(mi.getX() * scaleX);
		int y = round(mi.getY() * scaleY);
		sceneTemp.at(i).setX(x);
		sceneTemp.at(i).setY(y);
		/*if (x >= 0 && y >= 0 && y < rows && x < cols)
		 {
		 sceneGray.getRGBMatrix()->setAtPosition(y, x, color);
		 }*/
	}
	std::sort(modelPoints.begin(), modelPoints.end(), yComparation);
	std::sort(sceneTemp.begin(), sceneTemp.end(), yComparation);
	Point diffSort = modelPoints.at(0) - sceneTemp.at(0);
	color.R = 0;
	for (size_t i = 0; i < sceneTemp.size(); i++)
	{
		Point mi = sceneTemp.at(i);
		int x = mi.getX() + diffSort.getX();
		int y = mi.getY() + diffSort.getY();
		sceneTemp.at(i).setX(x);
		sceneTemp.at(i).setY(y);
		/*if (x >= 0 && y >= 0 && y < rows && x < cols)
		 {
		 sceneGray.getRGBMatrix()->setAtPosition(y, x, color);
		 }*/
	}
	vector<Point> eslm;
	for (size_t k = 0; k < mnLandmarks.size(); k++)
	{
		Point pi = mnLandmarks.at(k);
		Point ci = nearestPoint(sceneTemp, pi);
		eslm.push_back(ci);
		//fillCircle(*(sceneGray.getRGBMatrix()), ci, 3, color);
	}
// end indicate the bounding box of the edges
	vector<Point> result;
	if (mnLandmarks.size() > 0)
	{
		//result = verifyLandmarks2(lastModel, lastScene, mnLandmarks, mnLandmarks,
		// 100, 300);
		result = verifyDescriptors2(lastModel, lastScene, sceneTemp, mnLandmarks,
			eslm, 9, 36);
	}

	// compute the difference between original scene and rotated scene
	// to move-back the landmarks
	Point lsPoint1, lsPoint2;
	Line sl1 = principalAxis(scenePoints2, lsPoint1);
	Line sl2 = principalAxis(scenePointbk, lsPoint2);
	Point mback = lsPoint1 - lsPoint2;
	double mangle = sl1.angleLines(sl2);
	sl2.setBegin(sl1.getBegin());
	sl2.setEnd(
		Point(sl2.getEnd().getX() + mback.getX(),
			sl2.getEnd().getY() + mback.getY()));
	mangle = rotateDirection(sl1, sl2, mangle);
	for (size_t i = 0; i < result.size(); i++)
	{
		pi = result.at(i);
		pi.setX((pi.getX() - diffSort.getX()) / scaleX);
		pi.setY((pi.getY() - diffSort.getY()) / scaleY);
		int xnew = pi.getX() + mback.getX(), ynew = pi.getY() + mback.getY();
		rotateAPoint(xnew, ynew, lsPoint1, mangle, 1, xnew, ynew);
		pi.setX(xnew);
		pi.setY(ynew);
		result.at(i).setX(xnew);
		result.at(i).setY(ynew);
	}

	result = refine(scenePoints2, result);
	//delete sceneGrandient;
	return result;
}
vector<Point> PCAI(Image modelGray, Image &sceneGray, vector<Point> mnLandmarks)
{
	// PCA
	int rows = modelGray.getGrayMatrix().getRows();
	int cols = modelGray.getGrayMatrix().getCols();
	vector<Point> modelPoints;
	ptr_IntMatrix modelGrandient = new Matrix<int>(rows, cols, -1);
	*modelGrandient = *(getGradientDMatrix(modelGray, modelPoints));

	vector<Point> result = PCAI(modelPoints, sceneGray, mnLandmarks);

	delete modelGrandient;
	//delete sceneGrandient;
	return result;
}
