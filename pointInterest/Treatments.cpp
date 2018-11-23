/*
 * Treatments.cpp
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
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"

#include "../histograms/ShapeHistogram.h"

#include "Treatments.h"

Treatments::Treatments()
{
	// TODO Auto-generated constructor stub

}
Treatments::Treatments(Image img)
{
	refImage = img;
}
Treatments::~Treatments()
{
	// TODO Auto-generated destructor stub
}
void Treatments::setRefImage(Image image)
{
	refImage = image;
}
Image Treatments::getRefImage()
{
	return refImage;
}
