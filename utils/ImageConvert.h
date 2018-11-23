/*
 * ImageConvert.h
 *
 *  Created on: Dec 19, 2016
 *      Author: linh
 */

#ifndef IMAGECONVERT_H_
#define IMAGECONVERT_H_

QImage ptrIntToQImage(Matrix<int> inputMatrix);
QImage ptrDoubleToQImage(Matrix<double> inputMatrix);
QImage ptrRGBToQImage(Matrix<RGB> inputMatrix);
#endif /* IMAGECONVERT_H_ */
