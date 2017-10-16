#include <array>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <map>
#include <iostream>

#include <SFML\Graphics.hpp>

#include "AStar.h"
#include "MinimumSpanningTree.h"
#include "Edge.h"
#include "RoadNode.h"
#include "MstNode.h"
#include "Node.h"
#include "Road.h"
#include "V2.h"
#include "QuadTree.h"
#include "debug_mode.h"

int winX;
int winY;

MinimumSpanningTree* mst;

bool drawPopulationMap = true;
bool drawQuadTree = true;
<<<<<<< HEAD
bool drawMst = true;
bool drawNodes = true;
bool drawRoads = true;
bool drawElevationMap = false;

void SearchImageForPixelData(sf::Image image)
{
	int xSize = image.getSize().x;
	int ySize = image.getSize().y;

	int counter = 0;
	std::map<int, int> colorMap;

	for (int y = 0; y < ySize; y++)
	{
		for (int x = 0; x < xSize; x++)
		{
			// Grab the int32 representation of the color
			auto color = image.getPixel(x, y).toInteger();

			if (colorMap.find(color) == colorMap.end())
			{
				colorMap.insert(std::pair<int, int>(color, counter));
				counter++;
			}
			else
			{
				colorMap[color]++;
			}
		}
	}

	std::cout << "Number of different pixel colours:" << colorMap.size() << std::endl;
	std::map<int, int>::iterator iter;
	for (iter = colorMap.begin(); iter != colorMap.end(); iter++)
	{
		std::cout << "Colour key: " << iter->first << " entries: " << iter->second << std::endl;
	}
}
=======
bool drawMst = false;
bool drawNodes = true;
bool drawElevationMap = false;
bool drawRoads = true;
>>>>>>> fix

void CreateRoadNodes(sf::Image image)
{
	int xSize = image.getSize().x;
	int ySize = image.getSize().y;

	RoadNode::grid.resize(ySize);

	for (int y = 0; y < ySize; y++)
	{
		for (int x = 0; x < xSize; x++)
		{

			// Grab the int32 representation of the color
			auto color = image.getPixel(x, y).toInteger();

			// Int32 colours
			// -1061109505 = light grey
			// -2139062017 = dark grey
			// 255 = black

			RoadNode* node = new RoadNode(x, y);

			switch (color)
			{
			case -1061109505:
				node->SetCost(20);
				break;
			case -2139062017:
				node->SetCost(50);
				break;
			case 255:
				node->SetCost(100);
				break;
			default:
				node->SetCost(0);
				break;
			}

			RoadNode::grid[y].push_back(node);
		}
	}

	for (int y = 0; y < ySize - 1; y++)
	{
		for (int x = 0; x < xSize - 1; x++)
		{
			RoadNode::grid[y][x]->FillNeighbours(ySize - 1, xSize - 1);
		}
	}

	std::cout << "RoadNodes setup successfully" << std::endl;
}

void SearchImageForPixelData(sf::Image image)
{
	int xSize = image.getSize().x;
	int ySize = image.getSize().y;

	int counter = 0;
	std::map<int, int> colorMap;

	for (int y = 0; y < ySize; y++)
	{
		for (int x = 0; x < xSize; x++)
		{
			// Grab the int32 representation of the color
			auto color = image.getPixel(x, y).toInteger();

			if (colorMap.find(color) == colorMap.end())
			{
				colorMap.insert(std::pair<int, int>(color, counter));
				counter++;
			}
			else
			{
				colorMap[color]++;
			}
		}
	}

	std::cout << "Number of different pixel colours:" << colorMap.size() << std::endl;
	std::map<int, int>::iterator iter;
	for (iter = colorMap.begin(); iter != colorMap.end(); iter++)
	{
		std::cout << "Colour key: " << iter->first << " entries: " << iter->second << std::endl;
	}
}

int main()
{
	// Seed random
	srand(time(NULL));

	// Read in population data
	std::string populationMapFileName = "PopulationMap.bmp";
	sf::Image populationMap;
	if (!populationMap.loadFromFile(populationMapFileName))
	{
		std::cerr << "Could not find/open populationMap" << std::endl;
		return -1;
	}

	// Create a window
	winX = populationMap.getSize().x;
	winY = populationMap.getSize().y;
	sf::RenderWindow window(sf::VideoMode(winX, winY), "Population Data Quad Tree");

	// Create a sprite to display this image;
	sf::Texture populationTexture;
	if (!populationTexture.loadFromFile(populationMapFileName))
	{
		return -1;
	}
	populationTexture.setSmooth(true);

	sf::Sprite populationSprite;
	populationSprite.setTexture(populationTexture);
	populationSprite.setPosition(0, 0);

#if DEBUG_MODE == true
	SearchImageForPixelData(populationMap);
#endif

	// Create a quad tree
#if DEBUG_MODE == true
	std::cout << "Creating Quad tree" << std::endl;
#endif
	QuadTree* qt = new QuadTree(0, 0, winX, winY, populationMap);

	// Minimum Spanning tree
#if DEBUG_MODE == true
	std::cout << "Creating MST" << std::endl;
#endif
	mst = new MinimumSpanningTree();

	// Plot points
	for (QuadTree* qt : QuadTree::Children)
	{
#if DEBUG_MODE == true
		std::cout << "minX: " << qt->xOrigin << ", minY: " << qt->yOrigin << ", maxX: " << (qt->xOrigin + qt->width) << ", maxY: " << (qt->yOrigin + qt->height) << std::endl;
#endif
		mst->SpawnPoint(qt->xOrigin, qt->yOrigin, (qt->xOrigin + qt->width), (qt->yOrigin + qt->height));
	}

	// Assign neighbour
	float maxDist = winX / 4;
	mst->AssignNighbours(maxDist);

	// Edges
	mst->CreateAllEdges();

	// Sort the MST
	mst->Sort();
	std::cout << "The quad tree consts of " << QuadTree::Children.size() << " quads." << std::endl;
	std::cout << "The complete MST has " << mst->GetTreeEdges().size() << " edges." << std::endl;

	// Now that we've sorted the MST, we have to set up road nodes and use A* to pathfind the roads.

<<<<<<< HEAD
	// Read in elevation map
=======
	// Read elevation data
>>>>>>> fix
	std::string elevationMapFileName = "elevationMap.bmp";
	sf::Image evelationMap;
	if (!evelationMap.loadFromFile(elevationMapFileName))
	{
		std::cerr << "Could not find/open elevationMap" << std::endl;
		return -1;
	}

	// Create a sprite to display this image;
	sf::Texture elevationTexture;
	if (!elevationTexture.loadFromFile(elevationMapFileName))
	{
		return -1;
	}
	elevationTexture.setSmooth(true);

	sf::Sprite elevationSprite;
	elevationSprite.setTexture(elevationTexture);
	elevationSprite.setPosition(0, 0);

	// Create nodes for the roads
	CreateRoadNodes(evelationMap);

<<<<<<< HEAD
	// Create a sprite to display this image;
	sf::Texture elevationTexture;
	if (!elevationTexture.loadFromFile(elevationMapFileName))
	{
		return -1;
	}
	elevationTexture.setSmooth(true);

	sf::Sprite elevationSprite;
	elevationSprite.setTexture(elevationTexture);
	elevationSprite.setPosition(0, 0);

#if DEBUG_MODE == true
	int counter = 0;
#endif
=======
	int counter = 0;
>>>>>>> fix
	// Construct a road from each edge
	for (Edge edge : mst->GetTreeEdges())
	{
#if DEBUG_MODE = true
		std::cout << "Creating road for edge: " << ++counter << std::endl;
#endif
		// Make a road for this edge
		Road* road = new Road();

		// Pathfind it
		road->nodes = AStar::PathFind(edge.start->position->x, edge.start->position->y, edge.end->position->x, edge.end->position->y);
	}


	// Display
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) { window.close(); }
			else if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Num1:
					drawPopulationMap = !drawPopulationMap;
					break;
				case sf::Keyboard::Num2:
					drawQuadTree = !drawQuadTree;
					break;
				case sf::Keyboard::Num3:
					drawMst = !drawMst;
					break;
				case sf::Keyboard::Num4:
					drawNodes = !drawNodes;
					break;
				case sf::Keyboard::Num5:
					drawElevationMap = !drawElevationMap;
					break;
				case sf::Keyboard::Num6:
					drawRoads = !drawRoads;
					break;
<<<<<<< HEAD
					break;
=======
>>>>>>> fix
				}
			}
		}

		// Clear
		window.clear();

		// Draw...

		// Population map
		if (drawPopulationMap)
		{
			window.draw(populationSprite);
		}

		// Elevation map
		if (drawElevationMap)
		{
			window.draw(elevationSprite);
		}

		// Quad tree
		if (drawQuadTree)
		{
			for (QuadTree* qt : QuadTree::Children)
			{
				sf::Vertex qtVertices[5] =
				{
					sf::Vertex(sf::Vector2f(qt->xOrigin, qt->yOrigin), sf::Color::Red),
					sf::Vertex(sf::Vector2f(qt->xOrigin, qt->yOrigin + qt->height), sf::Color::Red),
					sf::Vertex(sf::Vector2f(qt->xOrigin + qt->width, qt->yOrigin + qt->height), sf::Color::Red),
					sf::Vertex(sf::Vector2f(qt->xOrigin + qt->width, qt->yOrigin), sf::Color::Red),
					sf::Vertex(sf::Vector2f(qt->xOrigin, qt->yOrigin), sf::Color::Red)
				};

				window.draw(qtVertices, 5, sf::LineStrip);
			}
		}


		// MST
		if (drawMst)
		{
			for (Edge edge : mst->GetTreeEdges())
			{
				sf::Vertex edgeVertices[2] =
				{
					sf::Vertex(sf::Vector2f(edge.start->position->x, edge.start->position->y), sf::Color::Blue),
					sf::Vertex(sf::Vector2f(edge.end->position->x, edge.end->position->y), sf::Color::Blue)
				};

				window.draw(edgeVertices, 2, sf::Lines);
			}
		}

		if (drawNodes)
		{
			// Nodes
			for (MstNode* node : mst->GetNodes())
			{

				sf::Vertex point[1] = { sf::Vertex(sf::Vector2f(node->position->x, node->position->y), sf::Color::Green) };

				//sf::CircleShape point(1);
				//point.setFillColor(sf::Color::Green);
				//point.setPosition(node->position->x, node->position->y);

				window.draw(point, 1, sf::Points);
			}
		}

<<<<<<< HEAD
		// Roads
=======
>>>>>>> fix
		if (drawRoads)
		{
			for (Road* road : Road::roads)
			{
<<<<<<< HEAD
				//int size = road->nodes.size();
				//sf::Vertex* nodeVertices = (sf::Vertex*)malloc(size);

				//for (int i = 0; i < size; i++)
				//{
				//	nodeVertices[i] = sf::Vertex(sf::Vector2f(road->nodes[i]->position->x, road->nodes[i]->position->y), sf::Color::Green);
				//}

				//window.draw(nodeVertices, size, sf::LineStrip);

				sf::VertexArray nodeVertices(sf::LineStrip, road->nodes.size());

				int nodeCounter = 0;
				for (RoadNode* node : road->nodes)
				{
					nodeVertices[nodeCounter].position = sf::Vector2f(node->position->x, node->position->y);
					nodeVertices[nodeCounter].color = sf::Color(255, 0, 255);
					nodeCounter++;
				}

				window.draw(nodeVertices);
			}
		}

		//for (Road* road : Road::roads)
		//{
		//	for (RoadNode* rn : road->nodes)
		//	{
		//		sf::CircleShape point(1);
		//		point.setFillColor(sf::Color::Cyan);
		//		point.setPosition(rn->position->x, rn->position->y);

		//		window.draw(point);
		//	}
		//}


=======
				sf::VertexArray roadVertices(sf::LineStrip, road->nodes.size());

				int nodeCounter = 0;
				for (RoadNode* node : road->nodes)
				{
					roadVertices[nodeCounter].position = sf::Vector2f(node->position->x, node->position->y);
					roadVertices[nodeCounter].color = sf::Color(255, 0, 255);
					nodeCounter++;
				}

				window.draw(roadVertices);
			}
		}

>>>>>>> fix
		// Display
		window.display();
	}


	return 0;
}