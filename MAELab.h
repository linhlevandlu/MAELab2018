/*
 * Analysis.h
 *
 *  Created on: Dec 7, 2016
 *      Author: linh
 */

#ifndef MAELAB_H_
#define MAELAB_H_

ptr_IntMatrix mae_get_Gray_Histogram(Image* inputImage);
ptr_RGBMatrix mae_get_RGB_Histogram(Image* inputImage);
ptr_IntMatrix mae_Binary_Threshold(Image* inputImage);
vector<Point> mae_Canny_Algorithm(Image* inputImage);
vector<Edge> mae_Suzuki_Algorithm(Image* inputImage);
vector<Line> mae_Line_Segment(Image* inputImage);
Matrix<RGB> mae_Gaussian_Filter(Image* inputImage, Matrix<double> kernel);
Matrix<int> mae_Robert_Filter(Image* inputImage);
Matrix<double> mae_Sobel_Filter(Image* inputImage);
Matrix<int> mae_Erode(Image* inputImage);
Matrix<int> mae_Dilate(Image* inputImage);
Matrix<int> mae_Open_Binary(Image* inputImage);
Matrix<int> mae_Close_Binary(Image* inputImage);

vector<Line> segment(ptr_Treatments treatment, Image sceneImage,
	int minDistance);
ShapeHistogram geomHistogram(ptr_Treatments treatment, Image image,
	AngleAccuracy angleAcc, int cols);
double bhattacharyyaDistance(ptr_Treatments treatment, Image sceneImage,
	AngleAccuracy angleAcc, int cols);
PHoughTransform phtEntriesTable(ptr_Treatments treatment, Image image);
vector<Point> estimatedLandmarks(ptr_Treatments treatment, Image sceneImage,
	AngleAccuracy acc, int cols, int templSize, int sceneSize,Point &ePoint, double angleDiff);
double measureCentroidPoint(vector<Point> landmarks, Point &ebary);
#endif /* ANALYSIS_H_ */
