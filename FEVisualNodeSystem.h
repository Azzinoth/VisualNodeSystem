#pragma once

#include "FEVisualNodeArea.h"

class FEVisualNodeSystem
{
	SINGLETON_PRIVATE_PART(FEVisualNodeSystem)

	std::vector<FEVisualNodeArea*> createdAreas;
	/*bool isNodeIDInList(std::string ID, std::vector<FEEditorNode*> list)
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			if (list[i]->getID() == ID)
				return true;
		}

		return false;
	}*/
public:
	SINGLETON_PUBLIC_PART(FEVisualNodeSystem)

	FEVisualNodeArea* createNodeArea();
	//FEEditorNodeArea* createNodeArea(std::vector<FEEditorNode*> nodes);
	void deleteNodeArea(FEVisualNodeArea* nodeArea);

	void moveNodesTo(FEVisualNodeArea* sourceNodeArea, FEVisualNodeArea* targetNodeArea, bool selectMovedNodes = false);
};

#define NODE_SYSTEM FEVisualNodeSystem::getInstance()