/*
 *
 *
 * Test file
 */
#include <iostream>
#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <time.h>
#include <cstdlib>
#include <dirent.h>

using namespace std;

#include "imageModel/Point.h"
#include "imageModel/Line.h"
#include "imageModel/Edge.h"
#include "imageModel/Matrix.h"
#include "imageModel/Image.h"
#include "utils/Drawing.h"
#include "io/Reader.h"

#include "histograms/ShapeHistogram.h"
#include "pht/PHTEntry.h"
#include "pht/PHoughTransform.h"
#include "correlation/CrossCorrelation.h"
#include "pointInterest/Treatments.h"
#include "pointInterest/Segmentation.h"
#include "pointInterest/GeometricHistgoram.h"
#include "pointInterest/ProHoughTransform.h"
#include "pointInterest/LandmarkDetection.h"

#include "MAELab.h"

int main(int argc, char* argv[])
{
	cout << "\n MAELab without graphic user interface !!!" << endl;
	string modeljpg, modeltps, scenejpg, scenetps, method, savejpg, savetps;
	int firstSize, secondSize;
	if (argc == 1)
	{
		cout << "\nAutomatic landmarks with template matching: " << argv[0] << endl;
		modeljpg = "data/Md 054.JPG";
		modeltps = "data/landmarks/Md 054.TPS";
		scenejpg = "data/md028.jpg";
		scenetps = "data/landmarks/Md 028.TPS";
		method = "2"; // 1 is template matching, 2 is SIFT
		firstSize = 9;
		secondSize = 36;
		savejpg = "results/Md_028_GHT.jpg";
		savetps = "results/Md_028_GHT.TPS";
	}
	else
	{
		cout << "\nAutomatic landmarks with SIFT: " << endl;
		modeljpg = argv[1];
		modeltps = argv[2];
		scenejpg = argv[3];
		scenetps = argv[4];
		method = argv[5];
		firstSize = std::atoi(argv[6]);
		secondSize = std::atoi(argv[7]);
		savejpg = argv[8];
		savetps = argv[9];
	}
	vector<Point> estLandmarks;
	Image model(modeljpg);
	model.readManualLandmarks(modeltps);
	Image scene(scenejpg);
	scene.readManualLandmarks(scenetps);

	LandmarkDetection lmDetection;
	lmDetection.setRefImage(model);

	int rows = scene.getRGBMatrix()->getRows();
	int cols = scene.getRGBMatrix()->getCols();

	if (std::atoi(method.c_str()) == 1) // template matching
	{
		estLandmarks = lmDetection.landmarksAutoDectect2(scene, firstSize,
			secondSize);
	}
	else
	{
		cout<<"\nMethod 2"<<endl;
		if (std::atoi(method.c_str()) == 2) // SIFT
		{
			estLandmarks = lmDetection.landmarksWithSIFT(scene, firstSize,
				secondSize);
		}
	}

	RGB color;
	color.R = 255;
	color.G = color.B = 0;
	// draw manual landmarks
	vector<Point> sceneManual = scene.getListOfManualLandmarks();
	Point pi;
	for (size_t i = 0; i < sceneManual.size(); i++)
	{
		pi = sceneManual.at(i);
		fillCircle(*scene.getRGBMatrix(), pi, 3, color);
	}
	// draw estimated landmarks
	color.G = 255;
	for (size_t i = 0; i < estLandmarks.size(); i++)
	{
		pi = estLandmarks.at(i);
		fillCircle(*scene.getRGBMatrix(), pi, 3, color);
	}

	saveRGB(savejpg.c_str(), scene.getRGBMatrix());
	// save TPS
	ofstream inFile(savetps.c_str());
	inFile << "LM=" << estLandmarks.size() << "\n";
	Point epk;
	double totalDiff1=0, totalDiff2 = 0;
	vector<double> distances;
	for (size_t k = 0; k < estLandmarks.size(); k++)
	{
		epk = estLandmarks.at(k);
		Line line(epk, sceneManual.at(k));
		inFile << epk.getX() << "\t" << rows - epk.getY() << "\t"
			<< line.getLength() << "\n";
		distances.push_back(line.getLength());
		totalDiff1 += line.getLength();
	}
	for (int m = 0; m < distances.size(); m++) {
		inFile <<distances.at(m)<<"\t";
		totalDiff2 += distances.at(m);
	}
	if(totalDiff1 == totalDiff2)
		inFile<<"\nTotal difference: "<<totalDiff1<<"\n";
	inFile << "IMAGE=" << savejpg << "\n";
	inFile.close();
	return 0;

}
