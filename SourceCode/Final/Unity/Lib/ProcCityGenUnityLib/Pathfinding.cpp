#include <algorithm>
#include <iostream>

#include "Pathfinding.h"
#include "RoadNode.h"
#include "MstNode.h"
#include "Vec2.h"

namespace Pathfinding
{
	std::vector<RoadNode*> PathFind(std::vector<std::vector<RoadNode*>> grid, int startX, int startY, int endX, int endY, int offsetPerRoadNode)
	{
		// Start, End
		RoadNode* start = grid[startY][startX];
		RoadNode* end = grid[endY][endX];

		// Null checks
		if (start == nullptr || end == nullptr)
		{
			return std::vector<RoadNode*>();
		}

		//// We need not pathfind if the road doesn't meet a threshold length
		//if (V2::DistanceBetween(*start->position, *end->position) < 500)
		//{
		//	std::vector<RoadNode*>  path;
		//	path.push_back(start);
		//	path.push_back(end);

		//	return path;
		//}

		// Q : the set of all nodes
		std::vector<RoadNode*> Q;
		for (auto vec : grid)
		{
			for (auto node : vec)
			{
				node->distance = FLT_MAX;
				node->parent = nullptr;
				Q.push_back(node);
			}
		}

		// The start has a distance of 0
		start->distance = 0;
		start->checked = true;

		while (!Q.empty())
		{
			//u is node in Q with the smallest distance...

			// Sort it
			std::sort(Q.begin(), Q.end(), [end](const RoadNode* lhs, const RoadNode *rhs) -> bool
			{
				return (lhs->distance + Vec2::Length(*lhs->position - *end->position)
					<
					(rhs->distance + Vec2::Length(*rhs->position - *end->position)));
			});

			// Pick the front
			RoadNode* u = Q.front();

			// Remove u from Q
			Q.erase(std::remove(Q.begin(), Q.end(), u), Q.end());

			// For each neighbour of u
			for (RoadNode* neighbour : u->GetNeighbours(grid))
			{
				//if (neighbour->checked) continue;
				// Skip non-walkables
				// if (!neighbour->isWalkable) continue;

				//neighbour->checked = true;

				auto alt = neighbour->GetCost() + Vec2::Length(*neighbour->position - *end->position);

				if (alt < neighbour->distance)
				{
					neighbour->distance = alt;
					neighbour->parent = u;
					if (neighbour->position->x == end->position->x
						&&
						neighbour->position->y == end->position->y)
					{
						return ReconstructPath(neighbour);
					}
				}
			}
		}
	}

	std::vector<RoadNode*> ReconstructPath(RoadNode* goal)
	{
		std::vector<RoadNode*> road = std::vector<RoadNode*>();
		int counter = 0;
		do
		{
			// std::cout << "Rebuilding path counter: " << counter++ << std::endl;
			road.push_back(goal);
			goal = goal->parent;
		} while (goal->parent != nullptr && goal != goal->parent->parent);
		road.push_back(goal);

		return road;
	}
}

