/*
 * Point.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: linh
 */
#include <iostream>
using namespace std;
#include "Point.h"

/*
 * Constructor the Point
 */
Point::Point()
{
	x = 0;
	y = 0;
	color.R = color.G = color.B = 0;
}
Point::Point(const Point &cpPoint)
{
	x = cpPoint.x;
	y = cpPoint.y;
	color = cpPoint.color;
}
Point::~Point()
{
	// TODO Auto-generated destructor stub
}

RGB Point::getColor()
{
	return color;
}
void Point::setColor(RGB rgb)
{
	color.R = rgb.R;
	color.G = rgb.G;
	color.B = rgb.B;
}

void Point::toString()
{
	cout << "\n" << x << "\t" << y;
}

void Point::reset()
{
	x = y = 0;
}
bool Point::operator<(const Point &cPoint)
{
	if (this->x < cPoint.x && this->y < cPoint.y)
		return true;
	return false;
}
bool Point::operator>(const Point &cPoint)
{
	if (this->x > cPoint.x && this->y > cPoint.y)
		return true;
	return false;
}
bool Point::operator==(const Point &cPoint)
{
	if (this->x == cPoint.x && this->y == cPoint.y)
		return true;
	return false;
}
bool Point::operator!=(int value)
{
	if (this->x != value && this->y != value)
		return true;
	return false;
}
Point Point::operator-(const Point cPoint)
{
	return Point(this->x - cPoint.x, this->y - cPoint.y);
}
Point Point::operator+(const Point cPoint)
{
	return Point(this->x + cPoint.x, this->y + cPoint.y);
}
