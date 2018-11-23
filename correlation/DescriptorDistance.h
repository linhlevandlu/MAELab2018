/*
 * DescriptorDistance.h
 *
 *  Created on: Feb 28, 2017
 *      Author: linh
 */

#ifndef DESCRIPTORDISTANCE_H_
#define DESCRIPTORDISTANCE_H_
Point nearestPoint(vector<Point> lsPoints, Point p);
vector<double> SIFTDescriptor(ptr_IntMatrix imgMatrix, Point center, int size);
vector<Point> verifyDescriptors(ptr_IntMatrix model, ptr_IntMatrix scene,
	vector<Point> manualLM, vector<Point> esLandmarks, int templSize,
	int sceneSize);
vector<Point> verifyDescriptors2(ptr_IntMatrix model, ptr_IntMatrix scene,
	vector<Point> scenePoints, vector<Point> manualLM, vector<Point> esLandmarks,
	int templSize, int sceneSize);
vector<Point> verifyDescriptors3(ptr_IntMatrix model, ptr_IntMatrix scene,
	vector<Point> contourPoints, vector<Point> manualLM, int patchSize);
vector<Point> verifyDescriptors4(ptr_IntMatrix model, ptr_IntMatrix scene,
	vector<Point> contourPoints, vector<Point> manualLM, int patchSize);
vector<Point> testSIFTOnRGB(ptr_RGBMatrix model, vector<Point> mLandmarks,
	ptr_RGBMatrix scene,vector<Point> contourPoints, int pSize);
#endif /* DESCRIPTORDISTANCE_H_ */
