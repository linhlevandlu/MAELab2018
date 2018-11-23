/*
 * Projection.h
 *
 *  Created on: Mar 22, 2017
 *      Author: linh
 */

#ifndef PROJECTION_H_
#define PROJECTION_H_

enum PROJECTION_TYPE
{
	Horizontal_Projection = 1,
	Vertical_Projection = 2
};
ptr_IntMatrix quantization(ptr_IntMatrix grayMatrix, int graylevel);

int* histogramProjection(ptr_IntMatrix binaryMatrix, PROJECTION_TYPE pType,
		int &size);
void analysisProjection(int* projection, int psize,int &leftLimit, int &rightLimit);
vector<Point> boundingBoxDetection(int* hProjection, int hSize, int* vProjection, int vSize);
vector<Point> boundingBoxDetection(ptr_IntMatrix grayMatrix);


void checkRegion(Matrix<int> &binMatrix);
Matrix<int> splitImage(ptr_IntMatrix binMatrix, int rowsSize);
Matrix<int> splitImageCols(ptr_IntMatrix binMatrix, int colsSize);
void binProjection(ptr_IntMatrix binImage, ptr_IntMatrix &hProjection,
	ptr_IntMatrix &vProjection);
void analysisHistogram(ptr_IntMatrix &histogram, int type, int sizeRegion);
#endif /* PROJECTION_H_ */
