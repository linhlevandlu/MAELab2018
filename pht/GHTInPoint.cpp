/*
 * GHTInPoint.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: linh
 */
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <stdio.h>
#include <float.h>
#include <string.h>
using namespace std;

#include "../utils/Converter.h"
#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"
#include "../segmentation/Thresholds.h"
#include "../segmentation/Canny.h"
#include "PCA.h"
#include "GHTInPoint.h"

ptr_IntMatrix getGradientDMatrix(Image grayImage, vector<Point> &edgePoints) {

	int rows = grayImage.getGrayMatrix().getRows();
	int cols = grayImage.getGrayMatrix().getCols();
	ptr_IntMatrix mMatrix = new Matrix<int>(rows, cols, 0);
	*mMatrix = grayImage.getGrayMatrix();
	int mThresholdValue = (int) grayImage.getThresholdValue();
	ptr_IntMatrix mbinMatrix = new Matrix<int>(
			*binaryThreshold(mMatrix, mThresholdValue, 255));
	ptr_IntMatrix mgradirection = new Matrix<int>(rows, cols, -1);
	ptr_IntMatrix mcannyMatrix = new Matrix<int>(
			*cannyProcess2(mbinMatrix, mThresholdValue, 3 * mThresholdValue,
					mgradirection, edgePoints));
	//delete mMatrix;
	//delete mbinMatrix;
//	delete mcannyMatrix;
	return mgradirection;
}
double angleVector(Point p1, Point p2, Point q1, Point q2) {
	Point vp(p2.getX() - p1.getX(), p2.getY() - p1.getY());
	Point vq(q2.getX() - q1.getX(), q2.getY() - q1.getY());

	double lengthVP = sqrt(
			(double) (vp.getX() * vp.getX())
					+ (double) (vp.getY() * vp.getY()));
	double lengthVQ = sqrt(
			(double) (vq.getX() * vq.getX())
					+ (double) (vq.getY() * vq.getY()));

	double vpq = (double) (vp.getX() * vq.getX())
			+ (double) (vp.getY() * vq.getY());

	return acos(vpq / (lengthVP * lengthVQ));
}
RTable rTableConstruct(ptr_IntMatrix gradMatrix, Point center) {
	RTable rtable;
	rtable.center.setX(center.getX());
	rtable.center.setY(center.getY());
	rtable.entriesTable.resize(361);

	// in default, rtable includes 360 rows for 360 degrees of gradient directions.
	int rows = gradMatrix->getRows();
	int cols = gradMatrix->getCols();

	int gd;
	REntry rentry;
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			gd = gradMatrix->getAtPosition(r, c);
			if (gd != -1) {
				PolarValue pv;
				pv.p.setX(c);
				pv.p.setY(r);
				Point hPoint(pv.p.getX() + 100, pv.p.getY());
				pv.angle = angleVector(pv.p, hPoint, pv.p, rtable.center);
				pv.distance = (Line(rtable.center, pv.p)).getLength();
				rentry = rtable.entriesTable.at((gd));

				(rtable.entriesTable.at((gd))).gradient = gd;
				(rtable.entriesTable.at((gd))).polarValues.push_back(pv);
			}
		}
	}
	return rtable;
}

Point houghSpace(ptr_IntMatrix gradMatrix, RTable rentries) {
	int rows = gradMatrix->getRows();
	int cols = gradMatrix->getCols();
	ptr_IntMatrix acc = new Matrix<int>(rows, cols, 0);

	int gradient = 0;
	REntry entry;
	int maxValue = 0, maxXIndex = 0, maxYIndex = 0;
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			gradient = gradMatrix->getAtPosition(r, c);
			if (gradient != -1) {
				entry = rentries.entriesTable.at(gradient);
				if (entry.gradient == gradient) {
					vector<PolarValue> polarValues = entry.polarValues;
					if (polarValues.size() > 0) {
						//PolarValue pvalue;
						double avalue = 0, dvalue = 0;
						int xindex = 0, yindex = 0, accValue = 0;
						for (size_t k = 0; k < polarValues.size(); k++) {
							PolarValue pvalue = polarValues.at(k);
							avalue = pvalue.angle;
							dvalue = pvalue.distance;

							xindex = c + (dvalue * cos(avalue));
							yindex = r + (dvalue * sin(avalue));
							//cout << "\n" << xindex << "\t" << yindex;
							if (xindex >= 0 && yindex >= 0 && xindex < cols
									&& yindex < rows) {
								// increase the value in accumulator
								accValue = acc->getAtPosition(yindex, xindex);
								accValue += 1;
								acc->setAtPosition(yindex, xindex, accValue);
							}
							if (accValue > maxValue && xindex > 0
									&& xindex < cols && yindex > 0
									&& yindex < rows) {
								maxValue = accValue;
								maxXIndex = xindex;
								maxYIndex = yindex;
							}
						}
					}
				}
			}
		}
	}
	cout << "\n center: ";
	rentries.center.toString();
	cout << "\nMax Value:" << maxValue;
	cout << "\nMax X index:" << maxXIndex;
	cout << "\nMax Y index:" << maxYIndex << endl;
	return Point(maxXIndex, maxYIndex);
}

vector<Point> detectLandmarks(Point refPoint, Point ePoint,
		vector<Point> mlandmarks) {
	vector<Point> esLandmarks;
	Point mlm;
	int deltaX = 0, deltaY = 0, xn = 0, yn = 0;
	for (size_t i = 0; i < mlandmarks.size(); i++) {
		mlm = mlandmarks.at(i);
		deltaX = ePoint.getX() - refPoint.getX();
		deltaY = ePoint.getY() - refPoint.getY();

		xn = mlm.getX() + deltaX;
		yn = mlm.getY() + deltaY;
		esLandmarks.push_back(Point(xn, yn));
	}
	return esLandmarks;
}

/*
 * ePoint: centroid's location of original scene
 * mPoint: centroid's location of original model in the scene
 * angle: different angle between scene and model (include direction)
 * translation: translating from scene to model in scene
 * mtranslation: translating of original model and model in scene
 */
vector<Point> ghtWithEntries(RTable rEntries, Line mLine, Point mcPoint,
		ptr_IntMatrix sGradient, vector<Point> mLandmarks, Point &ePoint,
		Point &mPoint, double &angle, Point &translation, Point &mtranslation) {

	Point sPoint = houghSpace(sGradient, rEntries);
	vector<Point> eslm = detectLandmarks(mcPoint, sPoint, mLandmarks);
	Line sLine = principalAxis(sGradient, ePoint);

	// compute angle and rotation direction
	double anglek = mLine.angleLines(sLine); // compute angle difference between two images
	cout << "\nAngle k: " << anglek << endl;
	int dx1 = mcPoint.getX() - ePoint.getX();
	int dy1 = mcPoint.getY() - ePoint.getY();

	Point mBegin = mLine.getBegin();
	sLine.setBegin(mBegin);
	Point sEnd = sLine.getEnd();
	sEnd.setX(sEnd.getX() + dx1);
	sEnd.setY(sEnd.getY() + dy1);
	sLine.setEnd(sEnd);
	// set new location of mLine
	angle = rotateDirection(mLine, sLine, anglek);

	// translate of model
	int dxold = sPoint.getX() - mcPoint.getX();
	int dyold = sPoint.getY() - mcPoint.getY();
	mtranslation.setX(dxold);
	mtranslation.setY(dyold);


	// detect presence of model in scene
	mcPoint = sPoint;
	int dx = mcPoint.getX() - ePoint.getX();
	int dy = mcPoint.getY() - ePoint.getY();
	cout << "\nTranslate: " << dx << "\t" << dy << endl;

	cout << "\nAngle rotation: " << angle << endl;
	cout << "\nLandmarks by GHT: " << eslm.size() << endl;

	// set new location of model centroid
	mPoint.setX(mcPoint.getX());
	mPoint.setY(mcPoint.getY());
	// translate between model and scene
	translation.setX(dx);
	translation.setY(dy);
	return eslm;
}

vector<Point> generalizingHoughTransform(ptr_IntMatrix mGradient,
		ptr_IntMatrix sGradient, vector<Point> mLandmarks, Point &ePoint,
		Point &mPoint, double &angle, Point &translation, Point &mtranslation) {
	int rows = mGradient->getRows();
	int cols = mGradient->getCols();
	//Point center(cols / 2, rows / 2);
	Point mTemp;
	Line mLine = principalAxis(mGradient, mTemp);
	RTable rentries = rTableConstruct(mGradient, mTemp);
	vector<Point> eslm;

	eslm = ghtWithEntries(rentries, mLine, mTemp, sGradient, mLandmarks, ePoint,
			mPoint, angle, translation, mtranslation);

	return eslm;
}
