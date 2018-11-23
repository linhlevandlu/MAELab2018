#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <time.h>

using namespace std;

#include "../imageModel/Point.h"
#include "../imageModel/Line.h"
#include "../imageModel/Edge.h"
#include "../imageModel/Matrix.h"
#include "../io/Reader.h"
#include "Thresholds.h"

#include "Suzuki.h"

/*
 * The direction of neighbor pixel
 */
enum Direction
{
	EAST = 1,
	NORTHEAST = 2,
	NORTH = 3,
	NORTHWEST = 4,
	WEST = 5,
	SOUTHWEST = 6,
	SOUTH = 7,
	SOUTHEAST = 8
};

/*
 * Indicate the direction of a neighbor pixel (ni,nj)
 * of a pixel (i,j)
 */
Direction getPosition(int i, int j, int ni, int nj)
{

	if (i == ni && j < nj)
		return EAST;
	if (i > ni && j < nj)
		return NORTHEAST;
	if (i > ni && j == nj)
		return NORTH;
	if (i > ni && j > nj)
		return NORTHWEST;
	if (i == ni && j > nj)
		return WEST;
	if (i < ni && j > nj)
		return SOUTHWEST;
	if (i < ni && j == nj)
		return SOUTH;
	if (i < ni && j < nj)
		return SOUTHEAST;
	return SOUTHEAST;
}

void clockWiseCheck(ptr_IntMatrix inputImage, int i, int j, int ci, int cj,
	int &iOut, int &jOut)
{
	int rows = inputImage->getRows();
	int cols = inputImage->getCols();

	if (inputImage->getAtPosition(ci, cj) != 0)
	{
		iOut = ci;
		jOut = cj;
		return;
	}
	else
	{
		Direction pos = getPosition(i, j, ci, cj);

		if (pos == EAST)
		{
			for (int k = j + 1; k >= j - 1; k--)
			{
				if (k >= 0 && k < cols && i + 1 < rows && i + 1 >= 0
					&& inputImage->getAtPosition(i + 1, k) != 0)
				{
					iOut = i + 1;
					jOut = k;
					return;
				}
			}
			if (j - 1 >= 0 && j - 1 < cols
				&& inputImage->getAtPosition(i, j - 1) != 0)
			{
				iOut = i;
				jOut = j - 1;
				return;
			}
			for (int k = j - 1; k <= j + 1; k++)
			{
				if (k >= 0 && k < cols && i - 1 >= 0 && i - 1 < rows
					&& inputImage->getAtPosition(i - 1, k) != 0)
				{
					iOut = i - 1;
					jOut = k;
					return;
				}
			}

		}
		if (pos == SOUTHEAST)
		{
			if (i + 1 < rows && i + 1 >= 0
				&& inputImage->getAtPosition(i + 1, j) != 0)
			{
				iOut = i + 1;
				jOut = j;
				return;
			}
			for (int r = i + 1; r >= i - 1; r--)
			{
				if (r >= 0 && r < rows && j - 1 >= 0 && j - 1 < cols
					&& inputImage->getAtPosition(r, j - 1) != 0)
				{
					iOut = r;
					jOut = j - 1;
					return;
				}
			}
			if (i - 1 >= 0 && inputImage->getAtPosition(i - 1, j) != 0)
			{
				iOut = i - 1;
				jOut = j;
				return;
			}
			for (int r = i - 1; r < i + 1; r++)
			{
				if (r >= 0 && r < rows && j + 1 < cols && j + 1 >= 0
					&& inputImage->getAtPosition(r, j + 1) != 0)
				{
					iOut = r;
					jOut = j + 1;
					return;
				}
			}
		}

		if (pos == SOUTH)
		{
			for (int r = i + 1; r >= i - 1; r--)
			{
				if (r >= 0 && r < rows && j - 1 >= 0 && j - 1 < cols
					&& inputImage->getAtPosition(r, j - 1) != 0)
				{
					iOut = r;
					jOut = j - 1;
					return;
				}
			}
			if (i - 1 >= 0 && inputImage->getAtPosition(i - 1, j) != 0)
			{
				iOut = i - 1;
				jOut = j;
				return;
			}
			for (int r = i - 1; r <= i + 1; r++)
			{
				if (r >= 0 && r < rows && j + 1 < cols && j + 1 >= 0
					&& inputImage->getAtPosition(r, j + 1) != 0)
				{
					iOut = r;
					jOut = j + 1;
					return;
				}
			}
		}

		if (pos == SOUTHWEST)
		{
			if (j - 1 >= 0 && inputImage->getAtPosition(i, j - 1) != 0)
			{
				iOut = i;
				jOut = j - 1;
				return;
			}
			for (int k = j - 1; k <= j + 1; k++)
			{
				if (k >= 0 && k < cols && i - 1 >= 0 && i - 1 < rows
					&& inputImage->getAtPosition(i - 1, k) != 0)
				{
					iOut = i - 1;
					jOut = k;
					return;
				}
			}
			if (j + 1 < cols && j + 1 >= 0
				&& inputImage->getAtPosition(i, j + 1) != 0)
			{
				iOut = i;
				jOut = j + 1;
				return;
			}
			for (int k = j + 1; k > j - 1; k--)
			{
				if (k >= 0 && k < cols && i + 1 < rows && i + 1 >= 0
					&& inputImage->getAtPosition(i + 1, k) != 0)
				{
					iOut = i + 1;
					jOut = k;
					return;
				}
			}
		}

		if (pos == WEST)
		{
			for (int k = j - 1; k <= j + 1; k++)
			{
				if (k >= 0 && k < cols && i - 1 >= 0 && i - 1 < rows
					&& inputImage->getAtPosition(i - 1, k) != 0)
				{
					iOut = i - 1;
					jOut = k;
					return;
				}
			}
			if (j + 1 < cols && j + 1 >= 0
				&& inputImage->getAtPosition(i, j + 1) != 0)
			{
				iOut = i;
				jOut = j + 1;
				return;
			}
			for (int k = j + 1; k >= j - 1; k--)
			{
				if (k >= 0 && k < cols && i + 1 < rows && i + 1 >= 0
					&& inputImage->getAtPosition(i + 1, k) != 0)
				{
					iOut = i + 1;
					jOut = k;
					return;
				}
			}
		}

		if (pos == NORTHWEST)
		{
			if (i - 1 >= 0 && i - 1 < rows
				&& inputImage->getAtPosition(i - 1, j) != 0)
			{
				iOut = i - 1;
				jOut = j;
				return;
			}

			for (int r = i - 1; r <= i + 1; r++)
			{
				if (r >= 0 && r < rows && j + 1 < cols && j + 1 >= 0
					&& inputImage->getAtPosition(r, j + 1) != 0)
				{
					iOut = r;
					jOut = j + 1;
					return;
				}
			}

			if (i + 1 < rows && i + 1 >= 0
				&& inputImage->getAtPosition(i + 1, j) != 0)
			{
				iOut = i + 1;
				jOut = j;
				return;
			}
			for (int r = i + 1; r > i - 1; r--)
			{
				if (r >= 0 && r < rows && j - 1 >= 0 && j - 1 < cols
					&& inputImage->getAtPosition(r, j - 1) != 0)
				{
					iOut = r;
					jOut = j - 1;
					return;
				}
			}

		}

		if (pos == NORTH)
		{
			for (int r = i - 1; r <= i + 1; r++)
			{
				if (r >= 0 && r < rows && j + 1 < cols && j + 1 >= 0
					&& inputImage->getAtPosition(r, j + 1) != 0)
				{
					iOut = r;
					jOut = j + 1;
					return;
				}
			}
			if (i + 1 < rows && i + 1 >= 0
				&& inputImage->getAtPosition(i + 1, j) != 0)
			{
				iOut = i + 1;
				jOut = j;
				return;
			}
			for (int r = i + 1; r >= i - 1; r--)
			{
				if (r >= 0 && r < rows && j - 1 >= 0 && j - 1 < cols
					&& inputImage->getAtPosition(r, j - 1) != 0)
				{
					iOut = r;
					jOut = j - 1;
					return;
				}
			}

		}

		if (pos == NORTHEAST)
		{
			if (j + 1 < cols && j + 1 >= 0
				&& inputImage->getAtPosition(i, j + 1) != 0)
			{
				iOut = i;
				jOut = j + 1;
				return;
			}
			for (int k = j + 1; k >= j - 1; k--)
			{
				if (k >= 0 && k < cols && i + 1 < rows && i + 1 >= 0
					&& inputImage->getAtPosition(i + 1, k) != 0)
				{
					iOut = i + 1;
					jOut = k;
					return;
				}
			}
			if (j - 1 >= 0 && j - 1 < cols
				&& inputImage->getAtPosition(i, j - 1) != 0)
			{
				iOut = i;
				jOut = j - 1;
				return;
			}
			for (int k = j - 1; k < j + 1; k++)
			{
				if (k >= 0 && k < cols && i - 1 >= 0 && i - 1 < rows
					&& inputImage->getAtPosition(i - 1, k) != 0)
				{
					iOut = i - 1;
					jOut = k;
					return;
				}
			}

		}

		{
			iOut = -1;
			jOut = -1;
			return;
		}
	}
}
void counterClockWiseCheck(ptr_IntMatrix inputImage, int i, int j, int ci,
	int cj, int &iOut, int &jOut, bool &flag)
{

	Direction pos = getPosition(i, j, ci, cj);
	int rows = inputImage->getRows();
	int cols = inputImage->getCols();
	flag = false;

	if (pos == EAST)
	{
		for (int k = j + 1; k >= j - 1; k--)
		{
			if (k >= 0 && k < cols && i - 1 >= 0 && i - 1 < rows)
			{
				if (inputImage->getAtPosition(i - 1, k) != 0)
				{
					iOut = i - 1;
					jOut = k;
					return;
				}
				else
				{
					if (inputImage->getAtPosition(i - 1, k) == 0)
					{
						flag = true;
					}
				}
			}
		}
		if (j - 1 >= 0 && j - 1 < cols && i < rows)
		{
			if (inputImage->getAtPosition(i, j - 1) != 0)
			{
				iOut = i;
				jOut = j - 1;
				return;
			}
		}
		for (int k = j - 1; k <= j + 1; k++)
		{
			if (k >= 0 && k < cols && i + 1 < rows && i + 1 >= 0)
			{
				if (inputImage->getAtPosition(i + 1, k) != 0)
				{
					iOut = i + 1;
					jOut = k;

					return;
				}
			}
		}
		if (j + 1 < cols && j + 1 >= 0 && i < rows)
		{
			if (inputImage->getAtPosition(i, j + 1) != 0)
			{
				iOut = i;
				jOut = j + 1;

				return;
			}
		}
	}
	if (pos == SOUTHEAST)
	{
		if (j + 1 < cols && j + 1 >= 0 && i < rows)
		{
			if (inputImage->getAtPosition(i, j + 1) != 0)
			{
				iOut = i;
				jOut = j + 1;
				return;
			}
			else
			{
				if (inputImage->getAtPosition(i, j + 1) == 0)
				{
					flag = true;
				}
			}

		}
		for (int k = j + 1; k >= j - 1; k--)
		{
			if (k >= 0 && k < cols && i - 1 >= 0 && i - 1 < rows)
			{
				if (inputImage->getAtPosition(i - 1, k) != 0)
				{
					iOut = i - 1;
					jOut = k;
					return;
				}
			}
		}
		if (j - 1 >= 0 && j - 1 < cols && i < rows)
		{
			if (inputImage->getAtPosition(i, j - 1) != 0)
			{
				iOut = i;
				jOut = j - 1;
				return;
			}
		}
		for (int k = j - 1; k <= j + 1; k++)
		{
			if (k >= 0 && k < cols && i + 1 < rows && i + 1 >= 0)
			{
				if (inputImage->getAtPosition(i + 1, k) != 0)
				{
					iOut = i + 1;
					jOut = k;
					return;
				}
			}
		}
	}

	if (pos == SOUTH)
	{
		for (int r = i + 1; r >= i - 1; r--)
		{
			if (r >= 0 && r < rows && j + 1 < cols && j + 1 >= 0)
			{
				if (r == i && inputImage->getAtPosition(r, j + 1) == 0)
				{
					flag = true;
				}
				if (inputImage->getAtPosition(r, j + 1) != 0)
				{
					iOut = r;
					jOut = j + 1;
					return;
				}
			}
		}
		if (i - 1 >= 0 && i - 1 < rows && j < cols)
		{
			if (inputImage->getAtPosition(i - 1, j) != 0)
			{
				iOut = i - 1;
				jOut = j;
				return;
			}
		}
		for (int r = i - 1; r <= i + 1; r++)
		{
			if (r >= 0 && r < rows && j - 1 >= 0 && j - 1 < cols)
			{
				if (inputImage->getAtPosition(r, j - 1) != 0)
				{
					iOut = r;
					jOut = j - 1;
					return;
				}
			}
		}
		if (i + 1 < rows && i + 1 >= 0 && j < cols)
		{
			if (inputImage->getAtPosition(i + 1, j) != 0)
			{
				iOut = i + 1;
				jOut = j;
				return;
			}
		}
	}

	if (pos == SOUTHWEST)
	{
		if (i + 1 < rows && i + 1 >= 0 && j < cols)
		{
			if (inputImage->getAtPosition(i + 1, j) != 0)
			{
				iOut = i + 1;
				jOut = j;
				return;
			}
		}
		for (int r = i + 1; r >= i - 1; r--)
		{
			if (r >= 0 && r < rows && j + 1 < cols && j + 1 >= 0)
			{
				if (r == i && inputImage->getAtPosition(r, j + 1) == 0)
				{
					flag = true;
				}
				if (inputImage->getAtPosition(r, j + 1) != 0)
				{
					iOut = r;
					jOut = j + 1;

					return;
				}
			}
		}
		if (i - 1 >= 0 && i - 1 < rows && j < cols)
		{
			if (inputImage->getAtPosition(i - 1, j) != 0)
			{
				iOut = i - 1;
				jOut = j;
				return;
			}
		}
		for (int r = i - 1; r <= i + 1; r++)
		{
			if (r >= 0 && r < rows && j - 1 >= 0 && j - 1 < cols)
			{
				if (inputImage->getAtPosition(r, j - 1) != 0)
				{
					iOut = r;
					jOut = j - 1;
					return;
				}
			}
		}
	}

	if (pos == WEST)
	{
		for (int k = j - 1; k <= j + 1; k++)
		{
			if (k >= 0 && k < cols && i + 1 < rows && i + 1 >= 0)
			{
				if (inputImage->getAtPosition(i + 1, k) != 0)
				{
					iOut = i + 1;
					jOut = k;
					return;
				}
			}
		}
		if (j + 1 < cols && j + 1 >= 0 && i < rows)
		{
			if (inputImage->getAtPosition(i, j + 1) != 0)
			{
				iOut = i;
				jOut = j + 1;
				return;
			}
			else
			{
				if (inputImage->getAtPosition(i, j + 1) == 0)
				{
					flag = true;
				}
			}
		}
		for (int k = j + 1; k >= j - 1; k--)
		{
			if (k >= 0 && k < cols && i - 1 >= 0 && i - 1 < rows)
			{
				if (inputImage->getAtPosition(i - 1, k) != 0)
				{
					iOut = i - 1;
					jOut = k;
					return;
				}
			}
		}
		if (j - 1 >= 0 && j - 1 < cols && i < rows)
		{
			if (inputImage->getAtPosition(i, j - 1) != 0)
			{
				iOut = i;
				jOut = j - 1;
				return;
			}
		}
	}

	if (pos == NORTHWEST)
	{
		if (j - 1 >= 0 && j - 1 < cols && i < rows)
		{
			if (inputImage->getAtPosition(i, j - 1) != 0)
			{
				iOut = i;
				jOut = j - 1;
				return;
			}
		}

		for (int k = j - 1; k <= j + 1; k++)
		{
			if (k >= 0 && k < cols && i + 1 < rows && i + 1 >= 0)
			{
				if (inputImage->getAtPosition(i + 1, k) != 0)
				{
					iOut = i + 1;
					jOut = k;
					return;
				}
			}
		}
		if (j + 1 < cols && j + 1 >= 0 && i < rows)
		{
			if (inputImage->getAtPosition(i, j + 1) != 0)
			{
				iOut = i;
				jOut = j + 1;
				return;
			}
			else
			{
				if (inputImage->getAtPosition(i, j + 1) == 0)
				{
					flag = true;
				}
			}
		}
		for (int k = j + 1; k >= j - 1; k--)
		{
			if (k >= 0 && k < cols && i - 1 >= 0 && i - 1 < rows)
			{
				if (inputImage->getAtPosition(i - 1, k) != 0)
				{
					iOut = i - 1;
					jOut = k;
					return;
				}
			}
		}
	}

	if (pos == NORTH)
	{
		for (int r = i - 1; r <= i + 1; r++)
		{
			if (r >= 0 && r < rows && j - 1 >= 0 && j - 1 < cols)
			{
				if (inputImage->getAtPosition(r, j - 1) != 0)
				{
					iOut = r;
					jOut = j - 1;
					return;
				}
			}
		}
		if (i + 1 < rows && i + 1 >= 0 && j < cols)
		{
			if (inputImage->getAtPosition(i + 1, j) != 0)
			{
				iOut = i + 1;
				jOut = j;
				return;
			}
		}
		for (int r = i + 1; r >= i - 1; r--)
		{
			if (r >= 0 && r < rows && j + 1 < cols && j + 1 >= 0)
			{
				if (r == i && inputImage->getAtPosition(r, j + 1) == 0)
				{
					flag = true;
				}
				if (inputImage->getAtPosition(r, j + 1) != 0)
				{
					iOut = r;
					jOut = j + 1;

					return;
				}
			}
		}
		if (i - 1 >= 0 && i - 1 < rows && j < cols)
		{
			if (inputImage->getAtPosition(i - 1, j) != 0)
			{
				iOut = i - 1;
				jOut = j;
				return;
			}
		}

	}

	if (pos == NORTHEAST)
	{
		if (i - 1 >= 0 && i - 1 < rows && j < cols)
		{
			if (inputImage->getAtPosition(i - 1, j) != 0)
			{
				iOut = i - 1;
				jOut = j;
				return;
			}
		}
		for (int r = i - 1; r <= i + 1; r++)
		{
			if (r >= 0 && r < rows && j - 1 >= 0 && j - 1 < cols)
			{
				if (inputImage->getAtPosition(r, j - 1) != 0)
				{
					iOut = r;
					jOut = j - 1;
					return;
				}
			}
		}
		if (i + 1 < rows && i + 1 >= 0 && j < cols)
		{
			if (inputImage->getAtPosition(i + 1, j) != 0)
			{
				iOut = i + 1;
				jOut = j;
				return;
			}
		}
		for (int r = i + 1; r >= i - 1; r--)
		{
			if (r >= 0 && r < rows && j + 1 < cols && j + 1 >= 0)
			{
				if (r == i && inputImage->getAtPosition(r, j + 1) == 0)
				{
					flag = true;
				}
				if (inputImage->getAtPosition(r, j + 1) != 0)
				{
					iOut = r;
					jOut = j + 1;
					return;
				}
			}
		}

	}

}

vector<Edge> suzuki(ptr_IntMatrix inputImage)
{
	vector<vector<Point> > edges;
	vector<Point> edge;
	int rows = inputImage->getRows();
	int cols = inputImage->getCols();
	int nBD = 1;
	int lNBD = 1;
	int i1 = -1, j1 = -1, i2 = -1, j2 = -1, i3 = -1, j3 = -1, i4 = -1, j4 = -1;
	bool flag = false;

	// threshold image into 0-1 image
	ptr_IntMatrix sbinMatrix = binaryThreshold(inputImage, 1, 1);
	for (int i = 0; i < rows - 1; i++)
	{
		lNBD = 1;
		for (int j = 0; j < cols - 1; j++)
		{
			if (((j - 1) >= 0) && sbinMatrix->getAtPosition(i, j) == 1
				&& sbinMatrix->getAtPosition(i, j - 1) == 0)
			{
				nBD += 1;
				i2 = i;
				j2 = j - 1;
			}
			else
			{
				if (j + 1 < cols && sbinMatrix->getAtPosition(i, j) >= 1
					&& sbinMatrix->getAtPosition(i, j + 1) == 0)
				{
					nBD += 1;
					i2 = i;
					j2 = j + 1;
					if (sbinMatrix->getAtPosition(i, j) > 1)
					{
						lNBD = sbinMatrix->getAtPosition(i, j);
					}
				}
				else
				{
					goto step4;
				}
			}

			// step 3.1
			clockWiseCheck(sbinMatrix, i, j, i2, j2, i1, j1);
			if (i1 == -1 && j1 == -1)
			{
				sbinMatrix->setAtPosition(i, j, -nBD);
				edge.push_back(Point(j, i));
				goto step4;
			}
			else
			{

				i2 = i1;
				j2 = j1;
				i3 = i;
				j3 = j;

				step33: edge.push_back(Point(j3, i3));
				flag = false;
				counterClockWiseCheck(sbinMatrix, i3, j3, i2, j2, i4, j4, flag);

				if (j3 + 1 < cols && sbinMatrix->getAtPosition(i3, j3 + 1) == 0)
				{
					if (flag)
						sbinMatrix->setAtPosition(i3, j3, -nBD);
					else
					{
						if (sbinMatrix->getAtPosition(i3, j3) == 1)
						{
							sbinMatrix->setAtPosition(i3, j3, nBD);
						}
					}
				}
				else
				{
					if (j3 + 1 < cols && sbinMatrix->getAtPosition(i3, j3 + 1) != 0
						&& sbinMatrix->getAtPosition(i3, j3) == 1)
					{
						sbinMatrix->setAtPosition(i3, j3, nBD);

					}
				}
				if (i4 == i && j4 == j && i3 == i1 && j3 == j1)
				{
					goto step4;
				}
				else
				{

					i2 = i3;
					j2 = j3;
					i3 = i4;
					j3 = j4;
					goto step33;
				}

			}
			step4: if (sbinMatrix->getAtPosition(i, j) != 1)
			{
				if (sbinMatrix->getAtPosition(i, j) != 0)
					lNBD = abs(sbinMatrix->getAtPosition(i, j));
				if (edge.size() > 0)
				{
					edges.push_back(edge);
				}
				edge.clear();

			}

		}

	}
	cout << "\n Size of edges: " << edges.size();
	vector<Edge> result;

	for (int i = edges.size() - 1; i >= 0; i--)
	{
		vector<Point> edgei = edges.at(i);
		if (edgei.size() >= 1)
			result.push_back(Edge(edgei));
	}
	//edgei.clear();
	edge.clear();
	edges.clear();
	delete sbinMatrix;

	return result;
}

// the region is represent by 2 points top-left and right-bottom (one a line)
double areaRegion(Line area)
{
	double result = 0;
	Point tl = area.getBegin();
	Point br = area.getEnd();
	int w = abs(tl.getX() - br.getX());
	int h = abs(tl.getY() - br.getY());
	result = (double) w * h;
	return result;
}
vector<Edge> verifyProcess(vector<Edge> listOfEdges)
{
	vector<Edge> result;
	vector<Line> boxes;
	Edge edgei;
	int xl = 0, yl = 0, xr = 0, yr = 0;
	for (size_t i = 0; i < listOfEdges.size(); i++)
	{
		edgei = listOfEdges.at(i);
		edgei.sortByX();
		xl = edgei.getPoints().at(0).getX();
		xr = edgei.getPoints().at(edgei.getPoints().size() - 1).getX();
		edgei.sortByY();
		yl = edgei.getPoints().at(0).getY();
		yr = edgei.getPoints().at(edgei.getPoints().size() - 1).getY();
		boxes.push_back(Line(Point(xl, yl), Point(xr, yr)));
	}

	// remove small edge
	for (size_t i = 0; i < boxes.size(); i++)
	{
		if (areaRegion(boxes.at(i)) < 1000)
		{
			listOfEdges.erase(listOfEdges.begin() + i);
			boxes.erase(boxes.begin() + i);
			i--;
		}
	}


	result = listOfEdges;
	return result;
}
