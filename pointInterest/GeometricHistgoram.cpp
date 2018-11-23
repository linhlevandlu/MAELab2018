/*
 * GeometricHistgoram.cpp
 *
 *  Created on: Dec 2, 2016
 *      Author: linh
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <cmath>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Edge.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"

#include "../histograms/ShapeHistogram.h"

#include "Treatments.h"
#include "GeometricHistgoram.h"

double bhattacharyyaMetric(ShapeHistogram refHist, ShapeHistogram sceneHist)
{
	double ref_size = refHist.getEntries();
	double scene_size = sceneHist.getEntries();
	double distance = 0;

	ptr_IntMatrix refMatrix = refHist.getMatrix();
	ptr_IntMatrix sceneMatrix = sceneHist.getMatrix();

	for (int r = 0; r < refMatrix->getRows(); r++)
	{
		for (int c = 0; c < refMatrix->getCols(); c++)
		{
			double value_1 = sqrt(refMatrix->getAtPosition(r, c) / ref_size);
			double value_2 = sqrt(sceneMatrix->getAtPosition(r, c) / scene_size);
			distance += value_1 * value_2;
		}
	}

	return distance;
}

GeometricHistgoram::GeometricHistgoram()
{
	// TODO Auto-generated constructor stub

}

GeometricHistgoram::~GeometricHistgoram()
{
	// TODO Auto-generated destructor stub
}
ShapeHistogram GeometricHistgoram::gmHistogram(Image image,
	AngleAccuracy angleAcc, int cols)
{
	ShapeHistogram shapeHist;
	vector<Line> lines = image.getListOfLines();
	vector<LocalHistogram> listLocalHists = shapeHist.constructPGH(lines);
	shapeHist.constructPGHMatrix(listLocalHists, angleAcc, cols);
	return shapeHist;
}
ShapeHistogram GeometricHistgoram::geomHistogram(AngleAccuracy angleAcc,
	int cols)
{
	return gmHistogram(Treatments::refImage, angleAcc, cols);
}
double GeometricHistgoram::bhattacharyyaDistance(Image sceneImage,
	AngleAccuracy angleAcc, int cols)
{
	ShapeHistogram refHistogram = gmHistogram(Treatments::refImage, angleAcc,
		cols);
	ShapeHistogram sceneHistogram = gmHistogram(sceneImage, angleAcc, cols);
	return bhattacharyyaMetric(refHistogram, sceneHistogram);
}
