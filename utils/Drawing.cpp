/*
 * Drawing.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: linh
 */
#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;
#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Matrix.h"

#include "Drawing.h"

// draw a line using Breshenham algorithm
vector<Point> detectLine(Line line)
{
	Point begin = line.getBegin();
	Point end = line.getEnd();
	//begin.setColor(color);
	//end.setColor(color);
	int dx = abs(end.getX() - begin.getX());
	int dy = abs(end.getY() - begin.getY());

	int x = begin.getX();
	int y = begin.getY();
	int c = dx - dy;
	int c2 = 2 * c;

	int x_unit = 1, y_unit = 1;

	if (end.getX() - begin.getX() < 0)
		x_unit = -x_unit;
	if (end.getY() - begin.getY() < 0)
		y_unit = -y_unit;
	vector<Point> listOfPoints;
	listOfPoints.push_back(Point(x, y));
	// vertical line
	if (begin.getX() == end.getX())
	{
		while (y != end.getY())
		{
			y += y_unit;
			listOfPoints.push_back(Point(x, y));
		}
	}
	else
	{ // horizontal line
		if (begin.getY() == end.getY())
		{
			while (x != end.getX())
			{
				x += x_unit;
				listOfPoints.push_back(Point(x, y));
			}
		}
		else
		{
			if (begin.getX() != end.getX() && begin.getY() != end.getY())
			{
				while (x != end.getX())
				{
					c2 = 2 * c;
					if (c2 > -dy)
					{
						c = c - dy;
						x += x_unit;
					}
					if (c2 < dx)
					{
						c += dx;
						y += y_unit;
					}

					listOfPoints.push_back(Point(x, y));
				}
			}
		}
	}
	return listOfPoints;
}

vector<Point> put8pixel(Point center, Point p)
{
	int xc = center.getX();
	int yc = center.getY();
	int x = p.getX();
	int y = p.getY();
	vector<Point> eightPoints;
	eightPoints.push_back(Point(x + xc, y + yc));
	eightPoints.push_back(Point(-x + xc, y + yc));
	eightPoints.push_back(Point(x + xc, -y + yc));
	eightPoints.push_back(Point(-x + xc, -y + yc));
	eightPoints.push_back(Point(y + xc, x + yc));
	eightPoints.push_back(Point(-y + xc, x + yc));
	eightPoints.push_back(Point(y + xc, -x + yc));
	eightPoints.push_back(Point(-y + xc, -x + yc));
	return eightPoints;
}
vector<Point> detectCircle(Point center, int radius)
{
	int x = 0;
	int y = radius;
	int f = 1 - radius;
	vector<Point> result;
	vector<Point> drawPoints = put8pixel(center, Point(x, y));
	result.insert(result.end(), drawPoints.begin(), drawPoints.end());
	while (x < y)
	{
		if (f < 0)
		{
			f += 2 * x + 3;
		}
		else
		{
			f += 2 * (x - y) + 5;
			y--;
		}
		x++;
		drawPoints = put8pixel(center, Point(x, y));
		result.insert(result.end(), drawPoints.begin(), drawPoints.end());
	}
	return result;
}

vector<Point> detectFillCircle(Point center, int radius, RGB color)
{
	vector<Point> result;
	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			if (x * x + y * y <= radius * radius)
			{
				result.push_back(
						Point(center.getX() + x, center.getY() + y, color));
			}
		}
	}
	return result;
}
Matrix<RGB> drawingCircle(Matrix<RGB> mat, Point center, int radius, RGB color)
{
	vector<Point> dpoints = detectCircle(center, radius);
	int rows = mat.getRows();
	int cols = mat.getCols();
	Point p1;
	for (size_t k = 0; k < dpoints.size(); k++)
	{
		p1 = dpoints.at(k);
		if (p1.getX() >= 0 && p1.getX() < cols && p1.getY() >= 0
				&& p1.getY() < rows)
		{
			mat.setAtPosition(p1.getY(), p1.getX(), p1.getColor());
		}
	}
	return mat;
}
Matrix<RGB> fillCircle(Matrix<RGB> mat, Point center, int radius, RGB color)
{
	vector<Point> dpoints = detectFillCircle(center, radius, color);
	int rows = mat.getRows();
	int cols = mat.getCols();
	Point p1;
	for (size_t k = 0; k < dpoints.size(); k++)
	{
		p1 = dpoints.at(k);
		if (p1.getX() >= 0 && p1.getX() < cols && p1.getY() >= 0
				&& p1.getY() < rows)
		{
			mat.setAtPosition(p1.getY(), p1.getX(), p1.getColor());
		}
	}
	return mat;
}
Matrix<RGB> drawingLine(Matrix<RGB> mat, Line line, RGB color)
{
	vector<Point> dpoints = detectLine(line);
	int rows = mat.getRows();
	int cols = mat.getCols();
	Point p1;
	for (size_t k = 0; k < dpoints.size(); k++)
	{
		p1 = dpoints.at(k);
		if (p1.getX() >= 0 && p1.getX() < cols && p1.getY() >= 0
				&& p1.getY() < rows)
		{
			mat.setAtPosition(p1.getY(), p1.getX(), color);
		}
	}
	return mat;
}
