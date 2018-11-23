/*
 * LBP.h
 *
 *  Created on: Apr 20, 2017
 *      Author: linh
 */

#ifndef LBP_H_
#define LBP_H_
int lbpCalculate(ptr_IntMatrix grayImage, int rowIndex, int colIndex,double &contrast);
void testLBPDescriptor(ptr_IntMatrix grayImage, vector<Point> landmarks, int sPatch);
void testLBPDescriptor2Images(ptr_IntMatrix grayImage1,
	vector<Point> landmarks1, ptr_IntMatrix grayImage2, vector<Point> landmarks2,
	int sPatch);
vector<Point> testLBPDescriptor2ImagesContours(ptr_IntMatrix grayImage1,
	vector<Point> landmarks1, ptr_IntMatrix grayImage2, vector<Point> gray2Contours,
	int sPatch);
#endif /* LBP_H_ */
