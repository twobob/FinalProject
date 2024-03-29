#include "shapeLine.h"
#include "V2.h"
#include "jbShape.h"
#include "Intersection.h"

shapeLine::shapeLine() {}

shapeLine::shapeLine(V2* _start, V2* _end, jbShape* _parent)
	:
	start(_start),
	end(_end),
	parent(_parent),
	markedForDeletion(false)
{}

shapeLine::shapeLine(float sX, float sY, float eX, float eY, jbShape* _parent)
	:
	start(new V2(sX, sY)),
	end(new V2(eX, eY)),
	parent(_parent),
	markedForDeletion(false)
{}

shapeLine::~shapeLine() {}

void shapeLine::AddIntersection(V2* point, jbShape* intersected)
{
	intersections.push_back(new Intersection(point->x, point->y, intersected));
}

void shapeLine::AddIntersection(int x, int y, jbShape* intersected)
{
	intersections.push_back(new Intersection(x, y, intersected));
}

std::vector<shapeLine*> shapeLine::returnLines()
{
	// 0 intersections
	if (intersections.size() == 0)
	{
		std::vector<shapeLine*> newLines;

		newLines.push_back(this);
		return newLines;
	}

	// 1 intersection
	else if (intersections.size() == 1)
	{
		std::vector<shapeLine*> newLines;

		// Check which side of the line is within the intersected shape...
		jbShape* intersectedShape = intersections[0]->shapeIntersected;

		// if END is inside
		if (intersectedShape->PointWithin(end))
		{
			// go from Start->Intersection
			newLines.push_back(new shapeLine(start, intersections[0]->point, parent));
			return newLines;
		}

		// if START is inside
		else
		{
			// go from Intersection->End
			newLines.push_back(new shapeLine(intersections[0]->point, end, parent));
			return newLines;
		}
	}

	// 2 intersections
	else if (intersections.size() == 2)
	{
		std::vector<shapeLine*> newLines;

		// Start->Intersection[0]
		newLines.push_back(new shapeLine(start, intersections[0]->point, parent));

		// Intersections[1]->End
		newLines.push_back(new shapeLine(intersections[1]->point, end, parent));

		return newLines;
	}

	// More...
	else
	{
		std::vector<shapeLine*> newLines;

		// Even amount of intersections
		if (intersections.size() % 2 == 0)
		{
			// Go from Start->Intersection[0]
			newLines.push_back(new shapeLine(start, intersections[0]->point, parent));

			// Loop to find the middle ones
			for (int i = 1; i < intersections.size(); i += 2)
			{
				newLines.push_back(new shapeLine(intersections[i]->point, intersections[i + 1]->point, parent));
			}

			// Go from Intersection[size-1]->End
			newLines.push_back(new shapeLine(intersections[intersections.size() - 1]->point, end, parent));
		}

		// Odd amount of intersections
		else
		{
			// Check which side of the line is within the intersected shape...
			jbShape* intersectedShape = intersections[0]->shapeIntersected;

			// If Start is within
			if (intersectedShape->PointWithin(start))
			{
				// Eg: Size = 5

				// We don't add start,
				// Add 0->1, 2->3, etc...
				for (int i = 0; i < intersections.size() - 1; i += 2)
				{
					newLines.push_back(new shapeLine(intersections[i]->point, intersections[i + 1]->point, parent));
				}

				// Now we add FinalIntersection->End
				newLines.push_back(new shapeLine(intersections[intersections.size() - 1]->point, end, parent));

			}
			// If End is within
			else
			{
				// Eg: Size = 5

				// We go from Start->Intersection[0]
				newLines.push_back(new shapeLine(start, intersections[0]->point, parent));

				// Now we go from
				// 1->2, 3->4, etc...
				for (int i = 1; i < intersections.size(); i += 2)
				{
					newLines.push_back(new shapeLine(intersections[i]->point, intersections[i + 1]->point, parent));
				}
			}

			return newLines;
		}
	}
}