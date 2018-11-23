/*
 * SVD.h
 *
 *  Created on: Feb 2, 2017
 *      Author: linh
 */

#ifndef SVD_H_
#define SVD_H_

vector<Point> test(Image modelImage, Image sceneImage, vector<Point> mnLandmarks);
vector<Edge> test2(Image image);
//ptr_IntMatrix getContour(ptr_IntMatrix grayMatrix);
ptr_RGBMatrix chooseHighest(ptr_RGBMatrix inputImage, int type);
void extractPatches(ptr_RGBMatrix rgbMatrix, int wpatch, int hpatch,
	string foldersave);
#endif /* SVD_H_ */
