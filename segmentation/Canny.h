/*
 * Canny.h
 *
 *  Created on: Oct 25, 2016
 *      Author: linh
 */

#ifndef CANNY_H_
#define CANNY_H_
double getBorderValue(ptr_IntMatrix inputMatrix, int x, int y);
ptr_IntMatrix cannyProcess(ptr_IntMatrix binaryImage, int low, int high, vector<Point> &contourPoints);
ptr_IntMatrix cannyProcess2(ptr_IntMatrix binaryImage, int lowThreshold,
	int highThreshold, ptr_IntMatrix &gradDirection,vector<Point> &edgePoints);
ptr_IntMatrix postProcess(ptr_IntMatrix binaryMatrix, int maxValue);
#endif /* CANNY2_H_ */
