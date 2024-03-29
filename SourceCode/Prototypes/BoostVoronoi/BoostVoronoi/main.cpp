//#include <vector>
//
//#include <SFML\Graphics.hpp>
//
//#include "Road.h"
//#include "Vec2.h"
//#include "Utility.h"
//
//int main()
//{
//	std::vector<Road> majorRoads;
//
//	majorRoads.push_back(Road(100, 100, 400, 400));
//	majorRoads.push_back(Road(100, 400, 400, 100));
//
//	Vec2* ip = Utility::New_GetLineIntersectionPoint(majorRoads[0].start, majorRoads[0].end, majorRoads[1].start, majorRoads[1].end);
//
//	sf::RenderWindow window(sf::VideoMode(500, 500), "Test");
//
//	while (window.isOpen())
//	{
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed) { window.close(); }
//		}
//	
//		// Clear
//		window.clear();
//	
//		// Draw
//		
//
//	
//		// Major Roads
//		for (Road road : majorRoads)
//		{
//			const int sz = 2;
//			sf::Vertex va[2] =
//			{
//				sf::Vertex(sf::Vector2f(road.start->getX(), road.start->getY()), sf::Color::Red),
//				sf::Vertex(sf::Vector2f(road.end->getX(), road.end->getY()), sf::Color::Red)
//			};
//			window.draw(va, sz, sf::LineStrip);
//		}
//
//		if (ip != nullptr)
//		{
//			sf::CircleShape shape;
//			shape.setPosition(ip->getX(), ip->getY());
//			shape.setRadius(2);
//			shape.setFillColor(sf::Color::Yellow);
//
//			window.draw(shape);
//		}
//	
//	
//		// Display
//		window.display();
//	}
//}

// Test ^
// --------------------------------------------------------------------------------------------
// Work v

#include <iostream>
#include <vector>
#include <random>
#include <ctime>

/*
	Requires C++ Boost Library for 1.65s for windows
	https://dl.bintray.com/boostorg/release/1.65.1/binaries/
	(boost_1_65_1-msvc-10.0-32.exe)

	include directory: C:\local\boost_1_65_1
*/
#include<boost\polygon\voronoi.hpp>
#include <SFML\Graphics.hpp>

#include "UtilRandom.h"
#include "Road.h"
#include "MinorRoad.h"
#include "Vec2.h"
#include "QuadTree.h"

using namespace std;
using namespace boost::polygon;

typedef int coordinate_type;
typedef point_data<coordinate_type> point_type;
typedef voronoi_diagram<double> VD;

int winX, winY;

int main(int argc, char* argv[])	
{
	// Read in population data
	std::string populationMapFileName = "PopulationMap.bmp";
	sf::Image populationMap;
	if (!populationMap.loadFromFile(populationMapFileName))
	{
		std::cerr << "Could not find/open populationMap" << std::endl;
		return -1;
	}

	// Window size
	winX = populationMap.getSize().x;
	winY = populationMap.getSize().y;

	// Quad tree
	QuadTree* qt = new QuadTree(0, 0, winX, winY, populationMap);
	
	// Points
	vector<point_type> points;
	int bufferSpace = 5;
	for (auto quad : QuadTree::Children)
	{
		int minX = quad->xOrigin;
		int maxX = quad->xOrigin + quad->width;
		int minY = quad->yOrigin;
		int maxY = quad->yOrigin + quad->height;

		double px = UtilRandom::Instance()->Random(minX + bufferSpace, maxX - bufferSpace);
		double py = UtilRandom::Instance()->Random(minY + bufferSpace, maxY - bufferSpace);

		points.push_back(point_type(px, py));
	}

	// Construct the voronoi diagram
	VD vd;
	construct_voronoi(points.begin(), points.end(), &vd);

	// Create major roads from voronoi edges
	std::vector<Road> majorRoads;
	for (auto const &edge : vd.edges())
	{
		if (edge.vertex0() != NULL && edge.vertex1() != NULL)
		{
			int sX = edge.vertex0()->x();
			int sY = edge.vertex0()->y();

			int eX = edge.vertex1()->x();
			int eY = edge.vertex1()->y();

			Road road = Road(sX, sY, eX, eY);

			bool exists = false;
			for (auto r : majorRoads)
			{
				if (*road.start == *r.end && *road.end == *r.start)
				{
					exists = true;
					break;
				}
			}
			if (!exists)
			{
				majorRoads.push_back(road);
			}
		}
	}

	// Create minor roads
	std::vector<MinorRoad> minorRoads;
	float distBetweenRoads; // = 15.f;
	float angle; // = 45;
	for (auto const &point : points)
	{
		distBetweenRoads = UtilRandom::Instance()->Random(13, 20);
		angle = UtilRandom::Instance()->Random(20, 160);

		// Extend a minor road from the center until it hits a major road
		MinorRoad mr = MinorRoad(point.x(), point.y(), angle, nullptr);
		mr.Extend(majorRoads);

		// Do this again, in the opposite direction
		MinorRoad mrOpposite = MinorRoad(point.x(), point.y(), angle+180, nullptr);
		mrOpposite.Extend(majorRoads);

		// We can use the end points of these two roads to construct one road going across
		Road road = Road(mr.end, mrOpposite.end);

		// Branch left outwards from this road
		road.CreateMinorRoads(distBetweenRoads, 0, minorRoads, majorRoads);
		// Branch right outwards from this road
		road.CreateMinorRoads(distBetweenRoads, 1, minorRoads, majorRoads);

		if (*mr.start == *mr.end || *mrOpposite.start == *mrOpposite.end)
		{

		}
		else
		{

		}

		// -------------------------------------------------------------------------------------------------

		// Do the same thing again, but rotated 90 degrees
		angle += 90;

		// Extend a minor road from the center until it hits a major road
		MinorRoad mrRotated = MinorRoad(point.x(), point.y(), angle, nullptr);
		mrRotated.Extend(majorRoads);

		// Do this again, in the opposite direction
		MinorRoad mrOppositeRotated = MinorRoad(point.x(), point.y(), angle + 180, nullptr);
		mrOppositeRotated.Extend(majorRoads);

		// We can use the end points of these two roads to construct one road going across
		Road roadRotated = Road(mrRotated.end, mrOppositeRotated.end);

		// Branch left outwards from this road
		roadRotated.CreateMinorRoads(distBetweenRoads, 0, minorRoads, majorRoads);
		// Branch right outwards from this road
		roadRotated.CreateMinorRoads(distBetweenRoads, 1, minorRoads, majorRoads);

		if (*mrRotated.start == *mrRotated.end || *mrOppositeRotated.start == *mrOppositeRotated.end)
		{

		}
		else
		{

		}
	}

	// Create window
	sf::RenderWindow window(sf::VideoMode(winX, winY), "Template-based generation - Voronoi");

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) { window.close(); }
		}

		// Clear
		window.clear();

		// Draw

		// Minor roads
		for (MinorRoad road : minorRoads)
		{
			const int sz = 2;
			sf::Vertex va[2] =
			{
				sf::Vertex(sf::Vector2f(road.start->getX(), road.start->getY()), sf::Color(0, 100, 255, 80)),
				sf::Vertex(sf::Vector2f(road.end->getX(), road.end->getY()), sf::Color(0, 100, 255, 80))
			};
			window.draw(va, sz, sf::LineStrip);
		}

		// Major Roads
		for (Road road : majorRoads)
		{
			const int sz = 2;
			sf::Vertex va[2] =
			{
				sf::Vertex(sf::Vector2f(road.start->getX(), road.start->getY()), sf::Color::Red),
				sf::Vertex(sf::Vector2f(road.end->getX(), road.end->getY()), sf::Color::Red)
			};

			window.draw(va, sz, sf::LineStrip);
		}

		// vertices
		for (auto const &point : points)
		{
			sf::CircleShape shape;
			shape.setPosition(point.x(), point.y());
			shape.setRadius(1);
			shape.setFillColor(sf::Color::Green);

			window.draw(shape);
		}


		// Display
		window.display();
	}

	return 0;
}