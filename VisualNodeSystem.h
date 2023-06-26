#pragma once

#include "VisualNodeArea.h"

class VisualNodeSystem
{
	SINGLETON_PRIVATE_PART(VisualNodeSystem)

	std::vector<VisualNodeArea*> CreatedAreas;
	/*bool isNodeIDInList(std::string ID, std::vector<FEVisualNode*> list)
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			if (list[i]->getID() == ID)
				return true;
		}

		return false;
	}*/
public:
	SINGLETON_PUBLIC_PART(VisualNodeSystem)

	VisualNodeArea* CreateNodeArea();
	//VisualNodeArea* createNodeArea(std::vector<FEVisualNode*> nodes);
	void DeleteNodeArea(const VisualNodeArea* NodeArea);

	void MoveNodesTo(VisualNodeArea* SourceNodeArea, VisualNodeArea* TargetNodeArea, bool SelectMovedNodes = false);
};

#define NODE_SYSTEM VisualNodeSystem::getInstance()