/*
 * ImageConvert.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: linh
 */

#include <QtGui/qimage.h>
#include "../imageModel/Point.h"
#include "../imageModel/Matrix.h"

#include "ImageConvert.h"

QImage ptrIntToQImage(Matrix<int> inputMatrix)
{
	static QVector<QRgb> sColorTable;

	// only create our color table once
	if (sColorTable.isEmpty())
	{
		for (int i = 0; i < 256; i++)
			sColorTable.push_back(qRgb(i, i, i));
	}
	int width = inputMatrix.getCols();
	int height = inputMatrix.getRows();

	QImage image(width, height, QImage::Format_Indexed8);
	image.setColorTable(sColorTable);
	for (int r = 0; r < height; r++)
	{
		for (int c = 0; c < width; c++)
		{
			int value = inputMatrix.getAtPosition(r, c);
			image.setPixel(c, r, value);
		}
	}

	return image;
}
QImage ptrDoubleToQImage(Matrix<double> inputMatrix)
{
	static QVector<QRgb> sColorTable;

	// only create our color table once
	if (sColorTable.isEmpty())
	{
		for (int i = 0; i < 256; i++)
			sColorTable.push_back(qRgb(i, i, i));
	}
	int width = inputMatrix.getCols();
	int height = inputMatrix.getRows();

	QImage image(width, height, QImage::Format_Indexed8);
	image.setColorTable(sColorTable);
	for (int r = 0; r < height; r++)
	{
		for (int c = 0; c < width; c++)
		{
			int value = inputMatrix.getAtPosition(r, c);
			image.setPixel(c, r, value);
		}
	}

	return image;
}

QImage ptrRGBToQImage(Matrix<RGB> inputMatrix)
{
	int width = inputMatrix.getCols();
	int height = inputMatrix.getRows();

	QImage image(width, height, QImage::Format_RGB888);
	for (int r = 0; r < height; r++)
	{
		for (int c = 0; c < width; c++)
		{
			RGB value = inputMatrix.getAtPosition(r, c);
			image.setPixel(c, r, qRgb(value.R,value.G,value.B));
		}
	}
	return image;
}
