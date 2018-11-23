/*
 * PHTEntry.cpp
 *
 *  Created on: Nov 30, 2016
 *      Author: linh
 */
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"

#include "PHTEntry.h"

//============================== Method of class ==========================
PHTEntry::PHTEntry()
{
	// TODO Auto-generated constructor stub

}

PHTEntry::~PHTEntry()
{
	// TODO Auto-generated destructor stub
}
vector<HoughSpace> PHTEntry::getListHoughSpace()
{
	return listHoughSpace;
}

void PHTEntry::setListHoughSpace(vector<HoughSpace> listHoughSpace)
{
	this->listHoughSpace = listHoughSpace;
}

Line PHTEntry::getObjLine()
{
	return oLine;
}

void PHTEntry::setObjLine(Line line)
{
	oLine = line;
}

Line PHTEntry::getRefLine()
{
	return rLine;
}

void PHTEntry::setRefLine(Line line)
{
	rLine = line;
}
void PHTEntry::addHoughSpace(HoughSpace hSpace)
{
	listHoughSpace.push_back(hSpace);
}
