/*
 * Drawing.h
 *
 *  Created on: Dec 18, 2016
 *      Author: linh
 */

#ifndef DRAWING_H_
#define DRAWING_H_

vector<Point> detectLine(Line line);

Matrix<RGB> drawingCircle(Matrix<RGB> mat, Point center, int radius, RGB color);
Matrix<RGB> fillCircle(Matrix<RGB> mat, Point center, int radius, RGB color);
Matrix<RGB> drawingLine(Matrix<RGB> mat, Line line, RGB color);
#endif /* DRAWING_H_ */
