/*
 * Thresholds.h
 *
 *  Created on: Nov 26, 2016
 *      Author: linh
 */

#ifndef THRESHOLDS_H_
#define THRESHOLDS_H_

ptr_IntMatrix binaryThreshold(ptr_IntMatrix inputMatrix, int tValue, int maxValue);
ptr_IntMatrix postProcess(ptr_IntMatrix binaryMatrix, int maxValue);
ptr_RGBMatrix colorThreshold(ptr_RGBMatrix rgbImage, ptr_RGBMatrix colorHistogram);
#endif /* THRESHOLDS_H_ */
