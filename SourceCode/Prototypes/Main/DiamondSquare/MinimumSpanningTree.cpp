#include <algorithm>
#include <queue>
#include <map>
#include <list>
#include <stdlib.h>

#include "MinimumSpanningTree.h"
#include "MstNode.h"
#include "Edge.h"
#include "V2.h"
#include "WaterData.h"
#include "UtilRandom.h"

MinimumSpanningTree::MinimumSpanningTree() {}

MinimumSpanningTree::~MinimumSpanningTree() {}

void MinimumSpanningTree::SpawnPoints(int numberOfPoints, int minX, int minY, int maxX, int maxY)
{
	for (int i = 0; i < numberOfPoints; i++)
	{
		int nodeX = rand() % maxX - minX;
		int nodeY = rand() % maxY - minY;
		MstNode* node = new MstNode(nodeX, nodeY);
		node->id = nodeCount++;
		AddNode(node);
	}
}

void MinimumSpanningTree::SpawnPoint(WaterData &wd, int offset, int minX, int minY, int maxX, int maxY)
{
	// Find all valid land positions in this quad
	std::vector<V2> landPositions;
	for (int y = minY; y < maxY; y++)
	{
		for (int x = minX; x < maxX; x++)
		{
			if (!wd.IsWater(x, y))
			{
				landPositions.push_back(V2(x, y));
			}
		}
	}

	// If none exist, return without placing a point
	if (landPositions.size() == 0) { return; }

	int index = UtilRandom::Instance()->RandomInt(0, landPositions.size()-1);

	int x = landPositions[index].x;
	int y = landPositions[index].y;

	int xdifference = (x % offset);
	x = x - xdifference;

	int ydifference = (y % offset);
	y = y - ydifference;

	//do
	//{
	//	int nodeX = rand() % (maxX - minX + 1) + minX;
	//	int nodeY = rand() % (maxY - minY + 1) + minY;

	//	int nodeXDifference = (nodeX % 7);
	//	int nodeXToFive = nodeX - nodeXDifference;

	//	int nodeYDifference = (nodeY % 7);
	//	int nodeYToFive = (nodeY - nodeYDifference);

	//	nx = nodeXToFive;
	//	ny = nodeYToFive;

	//} while (/*wd.IsWater(x, y)*/ 1 == 2);

	MstNode* node = new MstNode(x, y);
	node->id = nodeCount++;
	AddNode(node);
}

void MinimumSpanningTree::AssignNighbours(float maxDist)
{
	// For each node
	for (MstNode* node : nodes)
	{
		bool neighbourFound = false;
		MstNode* closestNeighbour = nullptr;

		// For every other ndoe
		for (MstNode* possibleNeighbour : nodes)
		{
			// Skip itself
			if (*node == *possibleNeighbour) { continue; }

			// The first neighbour is the closest if closestNeighbour is nullptr
			if (closestNeighbour == nullptr)
			{
				closestNeighbour = possibleNeighbour;
				continue;
			}

			// If possible neighbour is closer than closest neighbour, replace it
			if (V2::DistanceBetween(*node->position, *possibleNeighbour->position) < V2::DistanceBetween(*node->position, *closestNeighbour->position))
			{
				closestNeighbour = possibleNeighbour;
			}

			// If possibleNeighbour is close enough, add it as a possible neighbour
			if (V2::DistanceBetween(*node->position, *possibleNeighbour->position) < maxDist)
			{
				neighbourFound = true;
				node->AddPossibleNeighbour(possibleNeighbour);
			}
		}

		// After checking each neighbour, if a close enough neighbour wasnt found, add the closest neighbour
		if (!neighbourFound)
		{
			node->AddPossibleNeighbour(closestNeighbour);
		}
	}
}

void MinimumSpanningTree::CreateAllEdges()
{
	// For every node
	for (MstNode* node : nodes)
	{
		// For each neighbour
		for (MstNode* neighbour : node->getPossibleNeighbours())
		{
			if (neighbour->checkedForNeighbours) { continue; }
			AddPossibleEdge(new Edge(node, neighbour));
		}
		node->checkedForNeighbours = true;
	}
}

void MinimumSpanningTree::Sort()
{
	// Completely reset the tree
	treeEdges.clear();
	for (MstNode* node : nodes)
	{
		node->partOfTree = false;
	}
	for (Edge* edge : allEdges)
	{
		edge->partOfTree = false;
	}

	// Start

	// Sortest list of all edges
	std::list<Edge> sortedEdgeList;
	for (auto const& edge : allEdges)
	{
		sortedEdgeList.push_back(*edge);
	}
	sortedEdgeList.sort();
	sortedEdgeList.unique();

	// Find the edge of lowest cost
	Edge shortest = sortedEdgeList.front();
	sortedEdgeList.pop_front();

	treeEdges.push_back(shortest);
	shortest.partOfTree = true;
	shortest.start->partOfTree = true;
	shortest.end->partOfTree = true;

	// Dictionary for debugging to keep count of how many times each node was added.
	std::map<int, int> myMap;
	for (MstNode* node : nodes)
	{
		myMap.insert(std::pair<int, int>(node->id, 0));
	}

	// Loop...
	bool completed = false;
	int loopCount = 0;
	while (!completed)
	{
		loopCount++;

		// Find the node of lowest cost that is connected to the MST...
		bool nodeFound = false;
		Edge shortestEdge;
		for (auto const& edge : sortedEdgeList)
		{
			// If start and not end
			if (edge.start->partOfTree && !edge.end->partOfTree)
			{
				shortestEdge = edge;
				sortedEdgeList.remove(edge);
				break;
			}
			// Or end and not start
			else if (edge.end->partOfTree && !edge.start->partOfTree)
			{
				shortestEdge = edge;
				sortedEdgeList.remove(edge);
				break;
			}
		}


		// Add the shortest to the MST...
		treeEdges.push_back(shortestEdge);

		//Mark them
		shortestEdge.partOfTree = true;
		shortestEdge.start->partOfTree = true;
		shortestEdge.end->partOfTree = true;

		// Check to see if all nodes are now a part of the tree..
		bool allInTree = true;
		for (MstNode* node : nodes)
		{
			if (!node->partOfTree)
			{
				allInTree = false;
				break;
			}
		}

		// If they all are in MST...
		if (allInTree)
		{
			// stop looping
			completed = true;
		}
	}

	// Debugging - output how many times each was added
	for (MstNode* node : nodes)
	{
	}
}

std::vector<Edge*> MinimumSpanningTree::GetEdgesForNode(MstNode node)
{
	std::vector<Edge*> edges;

	for (Edge* edge : allEdges)
	{
		if (edge->EdgeContainsNode(node))
		{
			edges.push_back(edge);
		}
	}

	return edges;
}

bool MinimumSpanningTree::TreeEdgesContainsNode(MstNode node)
{
	for (Edge edge : treeEdges)
	{
		if (*edge.start == node || *edge.end == node)
		{
			return true;
		}
	}
	return false;
}

void MinimumSpanningTree::AddNode(MstNode* newNode)
{
	nodes.push_back(newNode);
}

void MinimumSpanningTree::AddPossibleEdge(Edge* newEdge)
{
	allEdges.insert(newEdge);
}