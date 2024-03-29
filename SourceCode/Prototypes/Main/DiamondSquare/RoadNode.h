#pragma once
#include "Node.h"
class V2;
class RoadNode : public Node
{
public:
	RoadNode();
	RoadNode(int _xIndex, int _yIndex, V2* vec);
	RoadNode(int _xIndex, int _yIndex, int x, int y);
	RoadNode(int _xIndex, int _yIndex, int x, int y, float cost);
	~RoadNode();

	bool checked;
	float distance;
	RoadNode* parent;
	std::vector<RoadNode*> GetNeighbors(std::vector<std::vector<RoadNode*>> grid);	
	
	void SetCost(int newCost);
	
	int getXIndex() const { return xIndex; }
	int getYIndex() const { return yIndex; }
	float GetCost() { return cost; }

private:
	void FillNeighbours(std::vector<std::vector<RoadNode*>> grid, int gridWidth, int gridHeight);
	int xIndex, yIndex;
	float cost;
	std::vector<RoadNode*> neighbours;
};