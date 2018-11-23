/*
 * ProHoughTransform.cpp
 *
 *  Created on: Dec 2, 2016
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
#include <algorithm>
using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Edge.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"

#include "../segmentation/Canny.h"
#include "../segmentation/Thresholds.h"

#include "../pht/PHTEntry.h"
#include "../pht/PHoughTransform.h"
#include "../pht/GHTInPoint.h"
#include "../io/Reader.h"
#include "../utils/Drawing.h"
#include "../pht/PCA.h"
#include "../correlation/DescriptorDistance.h"
#include "Treatments.h"
#include "ProHoughTransform.h"

vector<Point> findLandmarks(Point refPoint, Point esPoint,
	vector<Point> refLandmarks, int width, int height, int &positive)
{
	vector<Point> esLandmarks;
	positive = 0;
	Point temp, lm;
	for (size_t t = 0; t < refLandmarks.size(); t++)
	{
		temp = refLandmarks.at(t);

		lm.setX(temp.getX());
		lm.setY(temp.getY());

		int px = refPoint.getX() - esPoint.getX();
		int py = refPoint.getY() - esPoint.getY();
		int x;
		int y;
		x = lm.getX() - px;
		if (py > 0)
			y = lm.getY() - py;
		else
			y = lm.getY() + py;
		esLandmarks.push_back(Point(x, y));
		if (x >= 0 && x <= width && y >= 0 && y <= height)
		{
			positive += 1;
		}
	}
	return esLandmarks;
}

Point refPointInScene(PHTEntry entry, vector<Line> matchLines,
	double &angleDiff, vector<Point> refLandmarks, int width, int height)
{

	Point inter(0, 0);
	Point refPoint(width / 2, height / 2);

	if (matchLines.size() <= 0)
		return inter;
	Line objl1 = matchLines.at(0);
	Line objl2 = matchLines.at(1);

	Line lineEntry1 = entry.getRefLine();
	Line lineEntry2 = entry.getObjLine();

	HoughSpace hs1, hs2;
	hs1 = entry.getListHoughSpace().at(0);
	hs2 = entry.getListHoughSpace().at(1);

	// display the angle
	double angle3 = lineEntry1.angleLines(objl1);
	double angle4 = lineEntry1.angleLines(objl2);

	vector<Point> intersects1 = objl1.interParallel(objl1, objl2, hs1.distance,
		hs2.distance, width, height);
	int max = 0;
	vector<Point> estLM;
	vector<double> angles;
	Point esPoint;
	vector<Point> lms;
	int positive = 0;
	for (size_t i = 0; i < intersects1.size(); i++)
	{
		esPoint = intersects1.at(i);
		positive = 0;
		lms = findLandmarks(refPoint, esPoint, refLandmarks, width, height,
			positive);
		if (positive > max)
		{
			estLM.clear();
			estLM.push_back(esPoint);
			angles.clear();
			angles.push_back(angle3);
			max = positive;
		}
		else
		{
			if (positive == max)
			{
				estLM.push_back(esPoint);
				angles.push_back(angle3);
			}
		}
	}

	vector<Point> intersects2 = objl1.interParallel(objl1, objl2, hs2.distance,
		hs1.distance, width, height);

	for (size_t i = 0; i < intersects2.size(); i++)
	{
		esPoint = intersects2.at(i);
		positive = 0;
		lms = findLandmarks(refPoint, esPoint, refLandmarks, width, height,
			positive);
		if (positive > max)
		{
			estLM.clear();
			estLM.push_back(esPoint);
			angles.clear();
			angles.push_back(angle4);
			max = positive;
		}
		else
		{
			if (positive == max)
			{
				estLM.push_back(esPoint);
				angles.push_back(angle4);
			}
		}
	}

	intersects1.clear();
	intersects2.clear();

	if (estLM.size() == 1)
	{
		inter = estLM.at(0);
		angleDiff = angles.at(0);
	}
	else
	{
		double angleds = 180;
		Point es;
		for (size_t i = 0; i < estLM.size(); i++)
		{
			es = estLM.at(i);
			double angle = angles.at(i);
			if (angle <= angleds)
			{
				angleds = angle;
				angleDiff = angleds;
				inter = es;
			}
		}
	}
	cout << "\n Reference point in scene: " << inter.getX() << ", "
		<< inter.getY();

	return inter;
}
bool similarPairLines(Line ref1, Line ref2, Line scene1, Line scene2)
{
	int cond1 = 1;
	int cond2 = 1;
	int cond3 = 2;

	double refAngle = ref1.angleLines(ref2);
	double rd1 = ref1.perpendicularDistance(ref2.getBegin());
	double rd2 = ref1.perpendicularDistance(ref2.getEnd());
	double rd = rd1 + rd2;

	double sceneAngle = scene1.angleLines(scene2);
	double sd1 = scene1.perpendicularDistance(scene2.getBegin());
	double sd2 = scene1.perpendicularDistance(scene2.getEnd());
	double sd = sd1 + sd2;

	if (abs(refAngle - sceneAngle) < cond1
		&& (abs(
			(ref1.getLength() / scene1.getLength())
				- (ref2.getLength() / scene2.getLength())) < cond2)
		&& (abs(rd - sd) < cond3))
	{
		return true;
	}
	return false;
}

PHTEntry findHoughSpace(vector<PHTEntry> entryTable, Line line1, Line line2)
{
	PHTEntry entry;
	Line ref1, ref2;
	for (size_t i = 0; i < entryTable.size(); i++)
	{
		ref1 = entryTable.at(i).getRefLine();
		ref2 = entryTable.at(i).getObjLine();
		if (similarPairLines(ref1, ref2, line1, line2))
		{
			entry = entryTable.at(i);
		}
	}
	return entry;
}

PHTEntry matchingInScene(vector<PHTEntry> entryTable, vector<Line> sceneLines,
	int width, int height, vector<Line> &maxVector)
{
	ptr_IntMatrix accumulator = new Matrix<int>(
		floor(sqrt(width * width + height * height)), 361);
	int maxValue = 0;
	PHTEntry maxEntry;
	Line objLine1;
	Line objLine2;
	PHTEntry entry;
	vector<HoughSpace> hspace;
	for (size_t i = 0; i < sceneLines.size(); i++)
	{
		objLine1 = sceneLines.at(i);
		for (size_t j = 0; j < sceneLines.size(); j++)
		{
			objLine2 = sceneLines.at(j);
			if (i != j && closetLine(objLine2, objLine1))
			{
				entry = findHoughSpace(entryTable, objLine1, objLine2);
				hspace = entry.getListHoughSpace();
				if (hspace.size() != 0)
				{
					HoughSpace hsp;
					for (size_t k = 0; k < hspace.size(); k++)
					{
						hsp = hspace.at(k);
						int angle = round(hsp.angle);
						int distance = round(hsp.distance);
						if (!isnan(angle) && !isnan(distance) && angle >= 0
							&& distance >= 0)
						{
							int value = accumulator->getAtPosition(distance, angle);
							accumulator->setAtPosition(distance, angle, value + 1);
							if (accumulator->getAtPosition(distance, angle) > maxValue)
							{
								maxVector.clear();
								maxVector.push_back(objLine1);
								maxVector.push_back(objLine2);
								maxValue = accumulator->getAtPosition(distance, angle);
								maxEntry.setRefLine(entry.getRefLine());
								maxEntry.setObjLine(entry.getObjLine());
								maxEntry.setListHoughSpace(entry.getListHoughSpace());
								//maxEntry= entry;
							}
							else
							{
								if (k == 0
									&& accumulator->getAtPosition(distance, angle) == maxValue)
								{
									maxVector.push_back(objLine1);
									maxVector.push_back(objLine2);
								}
							}
						}
					}
				}
			}
		}
	}

	delete accumulator;
	return maxEntry;
}
vector<Point> phtLandmarks(vector<PHTEntry> entriesTable, Point refPoint,
	vector<Line> sceneLines, int width, int height, vector<Point> mLandmarks,
	double &angleDiff, Point &ePoint)
{
	vector<Point> eLandmarks;
	vector<Line> maxVector;
	PHTEntry entry = matchingInScene(entriesTable, sceneLines, width, height,
		maxVector);
	if (maxVector.size() > 0)
	{

		ePoint = refPointInScene(entry, maxVector, angleDiff, mLandmarks, width,
			height);
		double angle1 = entry.getRefLine().angleLines(entry.getObjLine());
		double angle2 = maxVector.at(0).angleLines(maxVector.at(1));
		angleDiff += abs(angle1 - angle2);
		int positive = 0;
		eLandmarks = findLandmarks(refPoint, ePoint, mLandmarks, width, height,
			positive);
	}
	maxVector.clear();

	return eLandmarks;
}
ProHoughTransform::ProHoughTransform()
{
	// TODO Auto-generated constructor stub

}

ProHoughTransform::~ProHoughTransform()
{
	// TODO Auto-generated destructor stub
}
PHoughTransform ProHoughTransform::constructPHT()
{
	ptr_IntMatrix grayImage;
	grayImage = (Matrix<int> *) malloc(sizeof(Matrix<int> ));
	*grayImage = Treatments::refImage.getGrayMatrix();
	//grayImage = Treatments::refImage.getGrayMatrix();
	int width = grayImage->getCols();
	int height = grayImage->getRows();

	PHoughTransform pht;
	pht.setRefPoint(Point(width / 2, height / 2));
	pht.constructPHTTable(Treatments::refImage.getListOfLines());

	delete grayImage;
	return pht;
}

vector<Point> ProHoughTransform::estimateLandmarks(Image sImage,
	double &angleDiff, Point &ePoint)
{
	vector<Point> eLandmarks;
	Matrix<int> gMatrix = Treatments::refImage.getGrayMatrix();
	ptr_IntMatrix mMatrix = &gMatrix;
	int width = mMatrix->getCols();
	int height = mMatrix->getRows();

	vector<Point> mLandmarks = Treatments::refImage.getListOfManualLandmarks();
	vector<Line> mLines = Treatments::refImage.getListOfLines();
	vector<Line> sLines = sImage.getListOfLines();

	Point mPoint(width / 2, height / 2);

	PHoughTransform pht;
	pht.setRefPoint(mPoint);
	vector<PHTEntry> entryTable = pht.constructPHTTable(mLines);

	eLandmarks = phtLandmarks(entryTable, mPoint, sLines, width, height,
		mLandmarks, angleDiff, ePoint);

	return eLandmarks;

}

void alignSegmentation(int minOx, int minOy, int maxOx, int maxOy, int addition,
	vector<Point> &newModelPoints, vector<Point> &newScenePoints,
	vector<Point> &eslm, Point &mPoint)
{
	Point mi;
	if (minOx != 0)
	{
		mPoint.setX(mPoint.getX() - minOx + addition);
		for (size_t i = 0; i < newScenePoints.size(); i++)
		{
			mi = newScenePoints.at(i);
			newScenePoints.at(i).setX(mi.getX() - minOx + addition);
		}
		for (size_t i = 0; i < eslm.size(); i++)
		{
			mi = eslm.at(i);
			eslm.at(i).setX(mi.getX() - minOx + addition);
		}
		for (size_t i = 0; i < newModelPoints.size(); i++)
		{
			mi = newModelPoints.at(i);
			newModelPoints.at(i).setX(mi.getX() - minOx + addition);
		}
	}
	if (maxOx != 0)
	{
		mPoint.setX(mPoint.getX() - maxOx - addition);
		for (size_t i = 0; i < newScenePoints.size(); i++)
		{
			mi = newScenePoints.at(i);
			newScenePoints.at(i).setX(mi.getX() - maxOx - addition);
		}
		for (size_t i = 0; i < eslm.size(); i++)
		{
			mi = eslm.at(i);
			eslm.at(i).setX(mi.getX() - maxOx - addition);
		}
		for (size_t i = 0; i < newModelPoints.size(); i++)
		{
			mi = newModelPoints.at(i);
			newModelPoints.at(i).setX(mi.getX() - maxOx - addition);
		}
	}
	if (minOy != 0)
	{
		mPoint.setY(mPoint.getY() - minOy + addition);
		for (size_t i = 0; i < newScenePoints.size(); i++)
		{
			mi = newScenePoints.at(i);
			newScenePoints.at(i).setY(mi.getY() - minOy + addition);
		}
		for (size_t i = 0; i < eslm.size(); i++)
		{
			mi = eslm.at(i);
			eslm.at(i).setY(mi.getY() - minOy + addition);
		}
		for (size_t i = 0; i < newModelPoints.size(); i++)
		{
			mi = newModelPoints.at(i);
			newModelPoints.at(i).setY(mi.getY() - minOy + addition);
		}
	}
	if (maxOy != 0)
	{
		mPoint.setY(mPoint.getY() - maxOy - addition);
		for (size_t i = 0; i < newScenePoints.size(); i++)
		{
			mi = newScenePoints.at(i);
			newScenePoints.at(i).setY(mi.getY() - maxOy - addition);
		}
		for (size_t i = 0; i < eslm.size(); i++)
		{
			mi = eslm.at(i);
			eslm.at(i).setY(mi.getY() - maxOy - addition);
		}
		for (size_t i = 0; i < newModelPoints.size(); i++)
		{
			mi = newModelPoints.at(i);
			newModelPoints.at(i).setY(mi.getY() - maxOy - addition);
		}
	}
}
void checkInSize(vector<Point> points, int rows, int cols, int &minOx,
	int &minOy, int &maxOx, int &maxOy)
{
	minOx = minOy = maxOx = maxOy = 0;
	Point pi;
	for (size_t i = 0; i < points.size(); i++)
	{
		pi = points.at(i);
		if (pi.getX() < 0)
		{
			int temp = pi.getX() - 0;
			if (temp < minOx)
				minOx = temp;
		}
		if (pi.getX() >= cols)
		{
			int temp = pi.getX() - cols;
			if (temp > maxOx)
				maxOx = temp;
		}
		if (pi.getY() < 0)
		{
			int temp = pi.getY() - 0;
			if (temp < minOy)
				minOy = temp;
		}
		if (pi.getY() >= rows)
		{
			int temp = pi.getY() - rows;
			if (temp > maxOy)
				maxOy = temp;
		}
	}
}
struct comparey
{
	bool operator()(Point p1, Point p2)
	{
		return p1.getY() < p2.getY();
	}
} yDirection;
vector<Point> ProHoughTransform::generalTransform(Image &sImage, double &angle,
	Point &ePoint, Point &mPoint, ptr_IntMatrix &newSceneGray, Point &translation,
	vector<Point> &modelSeg, vector<Point> &sceneSeg,
	vector<Point> &newScenePoints, double &scaleX, double &scaleY,
	Point &moveScale)
{
	int rows = sImage.getGrayMatrix().getRows();
	int cols = sImage.getGrayMatrix().getCols();
	Image mImage = Treatments::refImage;
	ptr_IntMatrix mgradirection = new Matrix<int>(rows, cols, -1);
	vector<Point> modelPoints;
	*mgradirection = *(getGradientDMatrix(mImage, modelPoints));
	vector<Point> mLandmarks = mImage.getListOfManualLandmarks();

	ptr_IntMatrix gradirection = new Matrix<int>(rows, cols, -1);
	vector<Point> scenePoints;
	*gradirection = *(getGradientDMatrix(sImage, scenePoints));

	vector<Point> eslm;
	translation.setX(0);
	translation.setY(0);
	Point mtranslation;

	/*
	 * ePoint: centroid's location of original scene
	 * mPoint: centroid's location of original model in the scene
	 * angle: different angle between scene and model (include direction)
	 * translation: translating from scene to model in scene
	 * mtranslation: translating of original model and model in scene
	 */

	eslm = generalizingHoughTransform(mgradirection, gradirection, mLandmarks,
		ePoint, mPoint, angle, translation, mtranslation);
	int dx = translation.getX();
	int dy = translation.getY();
	// rotation the scene points
	Point mi;
	newScenePoints.clear();

	for (size_t i = 0; i < scenePoints.size(); i++)
	{
		mi = scenePoints.at(i);
		int xnew = mi.getX() + dx;
		int ynew = mi.getY() + dy;
		rotateAPoint(xnew, ynew, mPoint, angle, 1, xnew, ynew);
		newScenePoints.push_back(Point(xnew, ynew));
	}
	// new location of model points
	vector<Point> newModelPoints;
	Point nmi;

	for (size_t i = 0; i < modelPoints.size(); i++)
	{
		nmi = modelPoints.at(i);
		int x = nmi.getX() + mtranslation.getX();
		int y = nmi.getY() + mtranslation.getY();
		newModelPoints.push_back(Point(x, y));
	}
	// move new scene and new model into the image
	Point osPoint, nsPoint;
	vector<Point> newScene = PCAIPoints(newModelPoints, mPoint, newScenePoints,
		abs(angle));
	newScenePoints = newScene;
	int minOx = 0, minOy = 0, maxOx = 0, maxOy = 0;
	int mminOx = 0, mminOy = 0, mmaxOx = 0, mmaxOy = 0;
	checkInSize(newScenePoints, rows, cols, minOx, minOy, maxOx, maxOy);
	alignSegmentation(minOx, minOy, maxOx, maxOy, 30, newModelPoints,
		newScenePoints, eslm, mPoint);
	checkInSize(newModelPoints, rows, cols, mminOx, mminOy, mmaxOx, mmaxOy);
	alignSegmentation(mminOx, mminOy, mmaxOx, mmaxOy, 30, newModelPoints,
		newScenePoints, eslm, mPoint);

	Line olineScene = principalAxis(scenePoints, osPoint);
	Line nlineScene = principalAxis(newScenePoints, nsPoint);
	double sangle = nlineScene.angleLines(olineScene);
	Point smove = nsPoint - osPoint;
	olineScene.setBegin(nlineScene.getBegin());
	Point eoScene = olineScene.getEnd();
	eoScene.setX(eoScene.getX() + smove.getX());
	eoScene.setY(eoScene.getY() + smove.getY());
	olineScene.setEnd(eoScene);
	sangle = rotateDirection(nlineScene, olineScene, sangle);
// move the scene gray to the same and rotate the model
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int value = sImage.getGrayMatrix().getAtPosition(r, c);
			int xnew = c + smove.getX();
			int ynew = r + smove.getY();
			rotateAPoint(xnew, ynew, mPoint, sangle, 1, xnew, ynew);
			if (xnew >= 0 && ynew >= 0 && ynew < rows && xnew < cols)
			{
				newSceneGray->setAtPosition(ynew, xnew, value);
			}
		}
	}
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int t1 = 0, t2 = 0, t3 = 0, t4 = 0;
			int t = newSceneGray->getAtPosition(r, c);
			if (r - 1 >= 0 && r - 1 < rows)
			{
				t1 = newSceneGray->getAtPosition(r - 1, c);
			}
			if (r + 1 >= 0 && r + 1 < rows)
			{
				t2 = newSceneGray->getAtPosition(r + 1, c);
			}
			if (c - 1 >= 0 && c - 1 < cols)
			{
				t3 = newSceneGray->getAtPosition(r, c - 1);
			}
			if (c + 1 >= 0 && c + 1 < cols)
			{
				t4 = newSceneGray->getAtPosition(r, c + 1);
			}
			if (t == 0)
			{
				newSceneGray->setAtPosition(r, c, (t1 + t2 + t3 + t4) / 4);
			}
		}
	}
	//saveGrayScale("abc.jpg", newSceneGray);
// export two images of segmentation
	modelSeg = modelPoints;
	sceneSeg = scenePoints;

	RGB color;
	color.R = 255;
	color.G = 0;
	color.B = 0;
	// draw mot so diem
	Point sLeft(cols, rows), sRight(0, 0);
	for (size_t i = 0; i < newScenePoints.size(); i++)
	{
		mi = newScenePoints.at(i);
		int x = mi.getX();
		int y = mi.getY();
		if (x >= 0 && y >= 0 && y < rows && x < cols)
		{
			//sImage.getRGBMatrix()->setAtPosition(y, x, color);
			if (x < sLeft.getX())
				sLeft.setX(x);
			if (y < sLeft.getY())
				sLeft.setY(y);
			if (x > sRight.getX())
				sRight.setX(x);
			if (y > sRight.getY())
				sRight.setY(y);
		}
	}
	//draw the bounding box
	/*drawingLine(*sImage.getRGBMatrix(),
	 Line(sLeft, Point(sRight.getX(), sLeft.getY())), color);
	 drawingLine(*sImage.getRGBMatrix(),
	 Line(sLeft, Point(sLeft.getX(), sRight.getY())), color);
	 drawingLine(*sImage.getRGBMatrix(),
	 Line(sRight, Point(sLeft.getX(), sRight.getY())), color);
	 drawingLine(*sImage.getRGBMatrix(),
	 Line(sRight, Point(sRight.getX(), sLeft.getY())), color);*/
	Point sceneCenter;
	sceneCenter.setX((sLeft.getX() + sRight.getX()) / 2);
	sceneCenter.setY((sLeft.getY() + sRight.getY()) / 2);
	fillCircle(sImage.getRGBMatrix(), sceneCenter, 5, color);
	color.G = 0;
	Point mLeft, mRight;
	mLeft.setX(cols);
	mLeft.setY(rows);
	mRight.setX(0);
	mRight.setY(0);
	for (int k = 0; k < newModelPoints.size(); k++)
	{
		mi = newModelPoints.at(k);
		int x = mi.getX();
		int y = mi.getY();
		if (x >= 0 && y >= 0 && y < rows && x < cols)
		{
			sImage.getRGBMatrix().setAtPosition(y, x, color);
			if (x < mLeft.getX())
				mLeft.setX(x);
			if (y < mLeft.getY())
				mLeft.setY(y);
			if (x > mRight.getX())
				mRight.setX(x);
			if (y > mRight.getY())
				mRight.setY(y);
		}
	}
	Point modelCenter;
	modelCenter.setX((mLeft.getX() + mRight.getX()) / 2);
	modelCenter.setY((mLeft.getY() + mRight.getY()) / 2);
	fillCircle(sImage.getRGBMatrix(), modelCenter, 5, color);
	// draw bounding box
	/*drawingLine(*sImage.getRGBMatrix(),
	 Line(mLeft, Point(mRight.getX(), mLeft.getY())), color);
	 drawingLine(*sImage.getRGBMatrix(),
	 Line(mLeft, Point(mLeft.getX(), mRight.getY())), color);
	 drawingLine(*sImage.getRGBMatrix(),
	 Line(mRight, Point(mLeft.getX(), mRight.getY())), color);
	 drawingLine(*sImage.getRGBMatrix(),
	 Line(mRight, Point(mRight.getX(), mLeft.getY())), color);*/

	Line mX(mLeft, Point(mRight.getX(), mLeft.getY()));
	Line mY(mLeft, Point(mLeft.getX(), mRight.getY()));
	Line sX(sLeft, Point(sRight.getX(), sLeft.getY()));
	Line sY(sLeft, Point(sLeft.getX(), sRight.getY()));
	scaleX = mX.getLength() / sX.getLength();
	scaleY = mY.getLength() / sY.getLength();
	color.G = 255;
	for (size_t i = 0; i < newScenePoints.size(); i++)
	{
		mi = newScenePoints.at(i);
		int x = round(mi.getX() * scaleX);
		int y = round(mi.getY() * scaleY);
		newScenePoints.at(i).setX(x);
		newScenePoints.at(i).setY(y);
		/*if (x >= 0 && y >= 0 && y < rows && x < cols)
		 {
		 sImage.getRGBMatrix()->setAtPosition(y, x, color);
		 }*/
	}

	std::sort(newModelPoints.begin(), newModelPoints.end(), yDirection);
	std::sort(newScenePoints.begin(), newScenePoints.end(), yDirection);
	Point diffSort = newModelPoints.at(0) - newScenePoints.at(0);
	moveScale.setX(diffSort.getX());
	moveScale.setY(diffSort.getY());
	for (size_t i = 0; i < newScenePoints.size(); i++)
	{
		mi = newScenePoints.at(i);
		int x = mi.getX() + diffSort.getX();
		int y = mi.getY() + diffSort.getY();
		newScenePoints.at(i).setX(x);
		newScenePoints.at(i).setY(y);
		if (x >= 0 && y >= 0 && y < rows && x < cols)
		{
			sImage.getRGBMatrix().setAtPosition(y, x, color);
		}
	}
	Point pi;
	for (size_t k = 0; k < eslm.size(); k++)
	{
		pi = eslm.at(k);
		Point ci = nearestPoint(newScenePoints, pi);
		eslm.at(k).setX(ci.getX());
		eslm.at(k).setY(ci.getY());
		fillCircle(sImage.getRGBMatrix(), ci, 5, color);
	}
	Matrix<RGB> savergb = sImage.getRGBMatrix();
	saveRGB("color.jpg", &savergb);
// ================================================================================
	angle = -sangle;
	translation.setX(-smove.getX());
	translation.setY(-smove.getY());
	ePoint = nsPoint;
	return eslm;
}
