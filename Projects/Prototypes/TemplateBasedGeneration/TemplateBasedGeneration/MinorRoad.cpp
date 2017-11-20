#include <algorithm>
#include <iostream>
#include <vector>

#include "MinorRoad.h"
#include "Vec2.h"
#include "Utility.h"
#include "Road.h"

extern std::vector<Vec2*> intersectionPoints;

std::vector<MinorRoad*> MinorRoad::minorRoads;

MinorRoad::MinorRoad(Vec2* newStart, float angle, Road* newParent)
{
	start = newStart;
	parent = newParent;

	dirNormalized = Vec2::AngleToVector(angle);

	minorRoads.push_back(this);

	ExtendUntilHit();
}

MinorRoad::MinorRoad(float startX, float startY, float angle, Road* newParent)
{
	start = new Vec2(startX, startY);
	parent = newParent;

	dirNormalized = Vec2::AngleToVector(angle);

	minorRoads.push_back(this);

	ExtendUntilHit();
}

MinorRoad::~MinorRoad() {}

void MinorRoad::ExtendUntilHit()
{
	int extendAmount = 17;

	end = new Vec2(start->getX() + (dirNormalized->getX() * extendAmount), start->getY() + (dirNormalized->getY() * extendAmount));

	bool keepExtending = true;
	while (keepExtending)
	{
		end = new Vec2(end->getX() + (dirNormalized->getX() * extendAmount), end->getY() + (dirNormalized->getY() * extendAmount));

		if (Collision())
		{
			keepExtending = false;
		}
	}

	end = getIntersectionPoint();
}

bool MinorRoad::Collision()
{
	// Find any intersection
	for (auto road : Road::getRoads())
	{
		if (*road == *parent) continue;

		Vec2* ip = Utility::GetIntersectionPoint(start, end, road->start, road->end);

		if (ip != nullptr)
		{
			return true;
		}
	}
	return false;
}

Vec2* MinorRoad::getIntersectionPoint()
{
	// To hold all of the points of intersection
	std::vector<Vec2*> iPoints;

	// Find all of the intersections
	for (auto road : Road::getRoads())
	{
		if (*road == *parent) continue;

		Vec2* ip;

		ip = Utility::GetIntersectionPoint(start, end, road->start, road->end);

		if (ip != nullptr)
		{
			iPoints.push_back(ip);
		}
	}

	// Return null if we don't find any points
	if (iPoints.size() == 0) { return nullptr; }

	// Otherwise, find the closest pointer...

	// Local function for use within the lambda
	Vec2* beginning = start;

	// Lambda
	auto closestTo = [beginning](Vec2* a, Vec2* b) -> bool
	{
		return
			(
				Utility::DistanceBetween(a, beginning)
				<
				Utility::DistanceBetween(b, beginning)
				);
	};

	// Sort
	std::sort(iPoints.begin(), iPoints.end(), closestTo);

	if (*iPoints[0] == *beginning)
	{
		return iPoints[1];
		return nullptr;
	}
	else
	{
		return iPoints[0];
	}

	return nullptr;

	//// Return false now if no intersections were found
	//if (intersects.size() <= 0) return false;

	//// Local variable for use within the lambda function
	//Vec2* beginning = start;

	//// Sort the intersections by distance
	//std::sort(intersects.begin(), intersects.end(), [beginning](Vec2* a, Vec2* b)
	//{
	//	return(
	//		Utility::DistanceBetween(a, beginning)
	//		<
	//		Utility::DistanceBetween(b, beginning)
	//		);
	//});


	//return intersects[0];
}