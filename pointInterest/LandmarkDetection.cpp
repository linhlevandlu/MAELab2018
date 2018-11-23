/*
 * LandmarkDetection.cpp
 *
 *  Created on: Dec 7, 2016
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
#include <float.h>

using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Edge.h"
#include "../imageModel/Matrix.h"
#include "../imageModel/Image.h"

#include "../segmentation/Canny.h"
#include "../segmentation/Thresholds.h"

#include "../histograms/ShapeHistogram.h"
#include "../pht/PHTEntry.h"
#include "../pht/PHoughTransform.h"
#include "../pht/GHTInPoint.h"
#include "../correlation/CrossCorrelation.h"
#include "../correlation/DescriptorDistance.h"
#include "../utils/Drawing.h"
#include "../io/Reader.h"

#include "Treatments.h"
#include "Segmentation.h"
#include "GeometricHistgoram.h"
#include "ProHoughTransform.h"
#include "LandmarkDetection.h"

LandmarkDetection::LandmarkDetection()
{
	// TODO Auto-generated constructor stub

}

LandmarkDetection::~LandmarkDetection()
{
	// TODO Auto-generated destructor stub
}
vector<Point> LandmarkDetection::refineLandmarks(vector<Point> estLandmarks,
	vector<Point> segmentation)
{
	vector<Point> result;
	Point pi;
	for (size_t i = 0; i < estLandmarks.size(); i++)
	{
		pi = estLandmarks.at(i);
		bool onCurve = checkPointInList(segmentation, pi);
		if (onCurve)
		{
			result.push_back(pi);
		}
		else
		{
			Point mPoint = nearestPoint(segmentation, pi);

			result.push_back(mPoint);
		}
	}
	return result;
}
vector<Point> LandmarkDetection::landmarksAutoDectect(Image sceneImage,
	AngleAccuracy acc, int cols, int templSize, int sceneSize, Point &ePoint,
	double &angleDiff)
{
	vector<Point> result;
	Image modelImage = Treatments::refImage;
	vector<Point> manualLMs = modelImage.getListOfManualLandmarks();
	vector<Line> mLines = modelImage.getListOfLines();
	vector<Line> sLines = sceneImage.getListOfLines();
	int width = modelImage.getGrayMatrix().getCols();
	int height = modelImage.getGrayMatrix().getRows();

	ShapeHistogram mHistogram;
	vector<LocalHistogram> mLocalHist = mHistogram.constructPGH(mLines);
	mHistogram.constructPGHMatrix(mLocalHist, acc, cols);

	ShapeHistogram sHistogram;
	vector<LocalHistogram> sLocalHist = sHistogram.constructPGH(sLines);
	sHistogram.constructPGHMatrix(sLocalHist, acc, cols);

	double bhatt = bhattacharyyaMetric(mHistogram, sHistogram);
	cout << "\nBhattacharrya: " << bhatt << endl;
	//if (bhatt > 0.9)
	//{
	PHoughTransform mpht;
	mpht.setRefPoint(Point(width / 2, height / 2));

	vector<PHTEntry> entriesTable = mpht.constructPHTTable(mLines);
	vector<Point> phtEsLM = phtLandmarks(entriesTable, mpht.getRefPoint(), sLines,
		width, height, manualLMs, angleDiff, ePoint);

	cout << "\n Number of landmarks (pht): " << phtEsLM.size();
	cout << "\nAngle difference: " << angleDiff << endl;
	if (phtEsLM.size() > 0)
	{
		result = verifyLandmarks(modelImage, sceneImage, manualLMs, phtEsLM,
			templSize, sceneSize, angleDiff, ePoint);
		//result = phtEsLM;

	}
	entriesTable.clear();
	phtEsLM.clear();
	//}

	return result;
}

vector<Point> LandmarkDetection::landmarksAutoDectect2(Image &sceneImage,
	int templSize, int sceneSize)
{
	Image modelImage = Treatments::refImage;
	vector<Point> manualLMs = modelImage.getListOfManualLandmarks();
	ProHoughTransform proHT;
	proHT.setRefImage(modelImage);
	double angle = 0;
	Point ePoint, mPoint;
	int rows = sceneImage.getGrayMatrix().getRows();
	int cols = sceneImage.getGrayMatrix().getCols();
	ptr_IntMatrix newScene = new Matrix<int>(rows, cols, 0);
	vector<Point> modelPoints, scenePoints, newScenePoints;
	Point translation, scaleMove;
	double scaleX = 0.0, scaleY = 0.0;
	vector<Point> phtEsLM = proHT.generalTransform(sceneImage, angle, ePoint,
		mPoint, newScene, translation, modelPoints, scenePoints, newScenePoints,
		scaleX, scaleY, scaleMove);
	ptr_IntMatrix modelSeg = new Matrix<int>(rows, cols, 0);
	ptr_IntMatrix sceneSeg = new Matrix<int>(rows, cols, 0);
	Point mi;
	for (size_t i = 0; i < modelPoints.size(); i++)
	{
		mi = modelPoints.at(i);
		if (mi.getX() >= 0 && mi.getY() >= 0 && mi.getY() < rows
			&& mi.getX() < cols)
		{
			modelSeg->setAtPosition(mi.getY(), mi.getX(), 255);
		}
	}
	for (size_t i = 0; i < newScenePoints.size(); i++)
	{
		mi = newScenePoints.at(i);
		if (mi.getX() >= 0 && mi.getY() >= 0 && mi.getY() < rows
			&& mi.getX() < cols)
		{
			sceneSeg->setAtPosition(mi.getY(), mi.getX(), 255);
		}
	}
	vector<Point> result;
	if (phtEsLM.size() > 0)
	{
		result = verifyLandmarks2(modelSeg, sceneSeg, manualLMs, phtEsLM, templSize,
			sceneSize);
	}
	phtEsLM.clear();
	delete newScene;
	delete modelSeg;
	delete sceneSeg;

	// reverse the coordinate of estimated landmarks
	Point pi;
	int dx = translation.getX();
	int dy = translation.getY();
	for (size_t i = 0; i < result.size(); i++)
	{
		pi = result.at(i);
		int xnew = pi.getX(), ynew = pi.getY();
		rotateAPoint(xnew, ynew, ePoint, angle, 1, xnew, ynew);
		result.at(i).setX(xnew + dx);
		result.at(i).setY(ynew + dy);
	}
	return result;
}

vector<Point> LandmarkDetection::landmarksWithSIFT(Image &sceneImage,
	int templSize, int sceneSize)
{
	Image modelImage = Treatments::refImage;
	vector<Point> manualLMs = modelImage.getListOfManualLandmarks();
	ProHoughTransform proHT;
	proHT.setRefImage(modelImage);
	double angle = 0;
	Point ePoint, mPoint;
	int rows = sceneImage.getGrayMatrix().getRows();
	int cols = sceneImage.getGrayMatrix().getCols();
	ptr_IntMatrix newScene = new Matrix<int>(rows, cols, 0);
	vector<Point> modelPoints, scenePoints, newScenePoints;
	Point translation, scaleMove;
	double scaleX = 0, scaleY =0;
	vector<Point> phtEsLM = proHT.generalTransform(sceneImage, angle, ePoint,
		mPoint, newScene, translation, modelPoints, scenePoints, newScenePoints,
		scaleX, scaleY, scaleMove);
	ptr_IntMatrix modelSeg = new Matrix<int>(rows, cols, 0);
	ptr_IntMatrix sceneSeg = new Matrix<int>(rows, cols, 0);
	Point mi;
	for (size_t i = 0; i < modelPoints.size(); i++)
	{
		mi = modelPoints.at(i);
		if (mi.getX() >= 0 && mi.getY() >= 0 && mi.getY() < rows
			&& mi.getX() < cols)
		{
			modelSeg->setAtPosition(mi.getY(), mi.getX(), 255);
		}
	}
	for (size_t i = 0; i < newScenePoints.size(); i++)
	{
		mi = newScenePoints.at(i);
		if (mi.getX() >= 0 && mi.getY() >= 0 && mi.getY() < rows
			&& mi.getX() < cols)
		{
			sceneSeg->setAtPosition(mi.getY(), mi.getX(), 255);
		}
	}
	vector<Point> result;
	if (phtEsLM.size() > 0)
	{
		/*result = verifyDescriptors(modelImage.getGrayMatrix(), newScene,
		 manualLMs, phtEsLM, templSize, sceneSize);*/
		result = verifyDescriptors(modelSeg, sceneSeg, manualLMs, phtEsLM,
			templSize, sceneSize);
	}
	//result = phtEsLM;
	phtEsLM.clear();
	delete newScene;
	delete modelSeg;
	delete sceneSeg;
	for (size_t i = 0; i < result.size(); i++)
	{
		mi = result.at(i);
		int x = mi.getX() - scaleMove.getX();
		int y = mi.getY() - scaleMove.getY();
		result.at(i).setX(x/ scaleX);
		result.at(i).setY(y/scaleY);
	}

	// reverse the coordinate of estimated landmarks
	Point pi;
	int dx = translation.getX();
	int dy = translation.getY();
	for (size_t i = 0; i < result.size(); i++)
	{
		pi = result.at(i);
		int xnew = pi.getX(), ynew = pi.getY();
		rotateAPoint(xnew, ynew, ePoint, angle, 1, xnew, ynew);
		result.at(i).setX(xnew + dx);
		result.at(i).setY(ynew + dy);
	}
	result = refineLandmarks(result, scenePoints);
	return result;
}
void LandmarkDetection::landmarksOnDir(string modelName, string folderScene,
	vector<string> sceneImages, AngleAccuracy acc, int cols, int templSize,
	int sceneSize, Point &ePoint, double &angleDiff, string saveFolder)
{

	Image modelImage = Treatments::refImage;
	vector<Point> manualLMs = modelImage.getListOfManualLandmarks();
	vector<Line> mLines = modelImage.getListOfLines();
	int width = modelImage.getGrayMatrix().getCols();
	int height = modelImage.getGrayMatrix().getRows();

	ShapeHistogram mHistogram;
	vector<LocalHistogram> mLocalHist = mHistogram.constructPGH(mLines);
	mHistogram.constructPGHMatrix(mLocalHist, acc, cols);

	ShapeHistogram sHistogram;
	vector<LocalHistogram> sLocalHist;
	Image *sceneImage;
	for (size_t i = 0; i < sceneImages.size(); i++)
	{
		string sceneName = sceneImages.at(i);
		cout << "\n==============================================" << sceneName;
		vector<Point> result;
		angleDiff = 0;
		sceneImage = new Image(folderScene + "/" + sceneName);
		vector<Line> sLines = sceneImage->getListOfLines();

		sLocalHist = sHistogram.constructPGH(sLines);
		sHistogram.constructPGHMatrix(sLocalHist, acc, cols);

		double bhatt = bhattacharyyaMetric(mHistogram, sHistogram);
		cout << "\nBhattacharrya: " << bhatt << endl;
		PHoughTransform mpht;
		mpht.setRefPoint(Point(width / 2, height / 2));

		vector<PHTEntry> entriesTable = mpht.constructPHTTable(mLines);
		vector<Point> phtEsLM = phtLandmarks(entriesTable, mpht.getRefPoint(),
			sLines, width, height, manualLMs, angleDiff, ePoint);

		cout << "\n Number of landmarks (pht): " << phtEsLM.size();
		cout << "\nAngle difference: " << angleDiff << endl;

		if (phtEsLM.size() > 0)
		{
			result = verifyLandmarks(modelImage, *sceneImage, manualLMs, phtEsLM,
				templSize, sceneSize, angleDiff, ePoint);

		}
		entriesTable.clear();
		phtEsLM.clear();

		string saveFile = saveFolder + "/" + modelName + "_" + sceneName + ".TPS";
		ofstream inFile(saveFile.c_str());
		inFile << "LM=" << result.size() << "\n";
		Point pk;
		for (size_t k = 0; k < result.size(); k++)
		{
			pk = result.at(k);
			inFile << pk.getX() << " " << pk.getY() << "\n";
		}
		inFile << "IMAGE=" << saveFile << "\n";
		inFile.close();
		delete sceneImage;
	}

}
void LandmarkDetection::landmarksOnDir2(string modelName, string folderScene,
	vector<string> sceneImages, string saveFolder)
{

	Image modelImage = Treatments::refImage;

	int rows = modelImage.getGrayMatrix().getRows();
	int cols = modelImage.getGrayMatrix().getCols();
	ptr_IntMatrix mgradirection = new Matrix<int>(rows, cols, -1);
	vector<Point> modelPoints;
	*mgradirection = *(getGradientDMatrix(modelImage, modelPoints));
	vector<Point> mLandmarks = modelImage.getListOfManualLandmarks();
	Point center(cols / 2, rows / 2);
	RTable rentries = rTableConstruct(mgradirection, center);
	Point mTemp;
	Line mLine = principalAxis(mgradirection, mTemp);
	//sceneImages.size()
	for (size_t i = 0; i < 20; i++)
	{
		Image *sceneImage;
		string sceneName = sceneImages.at(i);
		cout << "\n==============================================" << sceneName;

		sceneImage = new Image(folderScene + "/" + sceneName);

		ptr_IntMatrix gradirection = new Matrix<int>(rows, cols, -1);
		vector<Point> scenePoints;
		*gradirection = *(getGradientDMatrix(*sceneImage, scenePoints));

		Point mPoint, ePoint, translation, mtranslation;
		double angle;
		//vector<Point> eslm = generalizingHoughTransform(mgradirection, gradirection,
		//	mLandmarks, ePoint, mPoint, angle, translation);

		vector<Point> eslm = ghtWithEntries(rentries, mLine, mTemp, gradirection,
			mLandmarks, ePoint, mPoint, angle, translation, mtranslation);

		ptr_IntMatrix newScene = new Matrix<int>(rows, cols, 0);
		// move the model to the same and rotate the scene
		int dx = translation.getX();
		int dy = translation.getY();
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				int value = sceneImage->getGrayMatrix().getAtPosition(r, c);
				int xnew = c + dx;
				int ynew = r + dy;
				rotateAPoint(c + dx, r + dy, mPoint, angle, 1, xnew, ynew);
				if (xnew >= 0 && ynew >= 0 && ynew < rows && xnew < cols)
				{
					newScene->setAtPosition(ynew, xnew, value);
				}
			}
		}

		vector<Point> estLandmarks;

		if (eslm.size() > 0)
		{
			cout << "\nAngle difference: " << angle << endl;
			cout << "\n Number of landmarks (ght): " << eslm.size();
			Matrix<int> mgData = modelImage.getGrayMatrix();
			estLandmarks = verifyLandmarks2(&mgData, newScene,
				mLandmarks, eslm, 100, 300);

		}

		cout << "\n Number of landmarks (matching): " << estLandmarks.size();

		// reverse the coordinate of estimated landmarks
		Point pi;
		int dx2 = ePoint.getX() - mPoint.getX();
		int dy2 = ePoint.getY() - mPoint.getY();
		for (size_t i = 0; i < estLandmarks.size(); i++)
		{
			pi = estLandmarks.at(i);
			int xnew = 0, ynew = 0;
			rotateAPoint(pi.getX(), pi.getY(), mPoint, -angle, 1, xnew, ynew);
			xnew += dx2;
			ynew += dy2;
			estLandmarks.at(i).setX(xnew);
			estLandmarks.at(i).setY(ynew);
		}

		string saveFile = saveFolder + "/" + modelName + "_" + sceneName + ".TPS";
		ofstream inFile(saveFile.c_str());
		inFile << "LM=" << estLandmarks.size() << "\n";
		Point epk;
		RGB color;
		color.R = color.G = 255;
		color.B = 0;
		for (size_t k = 0; k < estLandmarks.size(); k++)
		{
			epk = estLandmarks.at(k);
			if (epk.getX() >= 0 && epk.getX() < cols && epk.getY() >= 0
				&& epk.getY() < rows)
			{
				fillCircle(sceneImage->getRGBMatrix(), epk, 5, color);
			}

			inFile << epk.getX() << " " << rows - epk.getY() << "\n";
		}
		inFile << "IMAGE=" << saveFile << "\n";
		inFile.close();

		string imgFile = saveFolder + "/" + modelName + "_" + sceneName;
		Matrix<RGB> sData = sceneImage->getRGBMatrix();
		saveRGB(imgFile.c_str(), &sData);

		estLandmarks.clear();
		eslm.clear();

		delete gradirection;
		delete sceneImage;
		delete newScene;
	}

}
void LandmarkDetection::landmarksOnDir3(string modelName, string folderScene,
	vector<string> sceneImages, string saveFolder)
{

	Image modelImage = Treatments::refImage;

	int rows = modelImage.getGrayMatrix().getRows();
	int cols = modelImage.getGrayMatrix().getCols();
	ptr_IntMatrix mgradirection = new Matrix<int>(rows, cols, -1);
	vector<Point> modelPoints;
	*mgradirection = *(getGradientDMatrix(modelImage, modelPoints));
	vector<Point> mLandmarks = modelImage.getListOfManualLandmarks();
	Point center(cols / 2, rows / 2);
	RTable rentries = rTableConstruct(mgradirection, center);
	Point mTemp;
	Line mLine = principalAxis(mgradirection, mTemp);
	string saveFile = saveFolder + "/" + modelName + "_angle.TPS";
	ofstream inFile(saveFile.c_str());
//sceneImages.size()
	for (size_t i = 260; i < sceneImages.size(); i++)
	{
		Image *sceneImage;
		string sceneName = sceneImages.at(i);
		cout << "\n==============================================" << sceneName
			<< endl;

		sceneImage = new Image(folderScene + "/" + sceneName);

		ptr_IntMatrix gradirection = new Matrix<int>(rows, cols, -1);
		vector<Point> scenePoints;
		*gradirection = *(getGradientDMatrix(*sceneImage, scenePoints));

		Point mPoint, ePoint, translation, mtranslation;
		double angle;
		//vector<Point> eslm = generalizingHoughTransform(mgradirection, gradirection,
		//	mLandmarks, ePoint, mPoint, angle, translation);

		vector<Point> eslm = ghtWithEntries(rentries, mLine, mTemp, gradirection,
			mLandmarks, ePoint, mPoint, angle, translation, mtranslation);
		inFile << sceneName << "\t" << angle << "\n";

		eslm.clear();
		delete gradirection;
		delete sceneImage;
	}
	delete mgradirection;
	inFile.close();
}
void LandmarkDetection::landmarksOnDir4(string modelName, string folderScene,
	vector<string> sceneImages, string saveFolder, string folderLandmarks,
	vector<string> landmarks)
{

	Image modelImage = Treatments::refImage;
	vector<Point> mLandmarks = modelImage.getListOfManualLandmarks();
	int rows = modelImage.getGrayMatrix().getRows();
	int cols = modelImage.getGrayMatrix().getCols();
	for (size_t i = 20; i < 40; i++)
	{

		string sceneName = sceneImages.at(i);
		cout << "\n==============================================" << sceneName;
		Image sceneImage(folderScene + "/" + sceneName);
		string lmFile = folderLandmarks + "/" + landmarks.at(i);
		sceneImage.readManualLandmarks(lmFile);
		vector<Point> mnLandmarks = sceneImage.getListOfManualLandmarks();

		vector<Point> estLandmarks = landmarksAutoDectect2(sceneImage, 9, 36);
		cout << "\n Number of landmarks (matching): " << estLandmarks.size();

		string saveFile = saveFolder + "/" + modelName + "_" + sceneName + ".TPS";
		ofstream inFile(saveFile.c_str());
		inFile << "LM=" << estLandmarks.size() << "\n";
		Point epk;
		RGB color;
		color.R = color.G = 255;
		color.B = 0;
		for (size_t k = 0; k < estLandmarks.size(); k++)
		{
			epk = estLandmarks.at(k);
			if (epk.getX() >= 0 && epk.getX() < cols && epk.getY() >= 0
				&& epk.getY() < rows)
			{
				color.G = 255;
				fillCircle(sceneImage.getRGBMatrix(), epk, 3, color);
				color.G = 0;
				fillCircle(sceneImage.getRGBMatrix(), mnLandmarks.at(k), 3, color);
			}
			Line line(epk, mnLandmarks.at(k));

			inFile << epk.getX() << " " << rows - epk.getY() << "\t"
				<< line.getLength() << "\n";
		}
		inFile << "IMAGE=" << saveFile << "\n";
		inFile.close();

		string imgFile = saveFolder + "/" + modelName + "_" + sceneName;
		Matrix<RGB> saveData = sceneImage.getRGBMatrix();
		saveRGB(imgFile.c_str(), &saveData);

		estLandmarks.clear();

	}
}
