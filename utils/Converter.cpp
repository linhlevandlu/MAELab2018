/*
 * Converter.cpp
 *
 *  Created on: Jan 5, 2017
 *      Author: linh
 */
#include <cstdlib>
#include <ctime>
#include <string>
#include <string.h>
#include <cmath>
#include <sstream>
using namespace std;

#include "Converter.h"

// get a random number between min and max
int random(int min,int max)
{
	srand(time(NULL));
	return rand() % (max - min + 1) + min;
}

// get name from a full name
string getName(string fullPath,string split)
{
	int len = fullPath.length();
	int lIndex = fullPath.find_last_of(split);
	string temp = fullPath.substr(lIndex+1,len - lIndex-1);
	return temp.replace(3,1,"_");
}
int roundToDegree(double value)
{
    int deg = (int)value;
    value = abs(value - deg);
    int min = (int)(value * 60);
    value = value - (double)min / 60;
    int sec = (int)(value * 3600);
    value = value - (double)sec / 3600;
    if(sec >= 30)
    	min += 1;
    if(min >= 30)
    	deg += 1;
    return deg;
}


string numberToString(int number)
{
    stringstream ss;
    ss << number;
    return ss.str();
}

int stringToNumber(const string &text)
{
    stringstream ss(text);
    int result;
    return ss >> result ? result : 0;
}