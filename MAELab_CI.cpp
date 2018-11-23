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

#include "histograms/ShapeHistogram.h"
#include "pht/PHTEntry.h"
#include "pht/PHoughTransform.h"
#include "correlation/CrossCorrelation.h"
#include "pointInterest/Treatments.h"
#include "pointInterest/Segmentation.h"
#include "pointInterest/GeometricHistgoram.h"
#include "pointInterest/ProHoughTransform.h"
#include "pointInterest/LandmarkDetection.h"

#include <QtGui/QApplication>
#include "ui/ImageViewer.h"

#include "MAELab.h"

int main(int argc, char* argv[])
{
	cout << "\n MAELab with graphic user interface !!!" << endl;
	QApplication app(argc, argv);
	ImageViewer imageViewer;
#if defined(Q_OS_SYMBIAN)
	imageViewer.showMaximized();
#else
	imageViewer.show();
#endif
	return app.exec();
	return 0;

}
