/*
 * TPSReader.cpp
 *
 *  Created on: Oct 6, 2016
 *      Author: linh
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <string>
using namespace std;

#include "../imageModel/Point.h"
#include "TPSReader.h"

std::vector<Point> readTPS(const char* filename)
{
	ifstream openFile(filename);
	vector<Point> mLandmarks;
	string lineText;
	if (openFile.is_open())
	{
		getline(openFile, lineText);
		size_t poseq = lineText.find_first_of("=");
		string nop = lineText.substr(poseq + 1, lineText.size() - 1 - poseq);

		int nPoints = atoi(nop.c_str());
		//cout<<"\n number points: "<<nPoints;
		int i = 0;
		while (i < nPoints)
		{
			getline(openFile, lineText);
			int pos = lineText.find(" ");
			string l1 = lineText.substr(0, pos);
			string l2 = lineText.substr(pos + 1, lineText.length() - pos - 1);
			mLandmarks.push_back(Point(atoi(l1.c_str()), atoi(l2.c_str())));
			i++;
		}
	}
	else
	{
		cout << endl << "Cannot open this file !";
	}
	openFile.close();
	return mLandmarks;
}
std::vector<Point> readTPSWithDifference(const char* filename,
	string &difference)
{
	ifstream openFile(filename);
	vector<Point> mLandmarks;
	string lineText;
	if (openFile.is_open())
	{
		getline(openFile, lineText);
		size_t poseq = lineText.find_first_of("=");
		string nop = lineText.substr(poseq + 1, lineText.size() - 1 - poseq);

		int nPoints = atoi(nop.c_str());
		int i = 0;
		while (i < nPoints)
		{
			getline(openFile, lineText);
			int pos = lineText.find(" ");
			string l1 = lineText.substr(0, pos);
			string l2 = lineText.substr(pos + 1, lineText.length() - pos - 1);
			mLandmarks.push_back(Point(atoi(l1.c_str()), atoi(l2.c_str())));
			i++;
		}
		getline(openFile, difference);
	}
	else
	{
		cout << endl << "Cannot open this file !";
	}
	openFile.close();
	return mLandmarks;
}
