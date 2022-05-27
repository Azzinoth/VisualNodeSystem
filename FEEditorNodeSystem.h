#pragma once

#include "FEEditorNodeArea.h"

class FEEditorNodeSystem
{
	SINGLETON_PRIVATE_PART(FEEditorNodeSystem)

	std::vector<FEEditorNodeArea*> createdAreas;
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
	SINGLETON_PUBLIC_PART(FEEditorNodeSystem)

	FEEditorNodeArea* createNodeArea();
	//FEEditorNodeArea* createNodeArea(std::vector<FEEditorNode*> nodes);
	void deleteNodeArea(FEEditorNodeArea* nodeArea);

	void moveNodesTo(FEEditorNodeArea* sourceNodeArea, FEEditorNodeArea* targetNodeArea, bool selectMovedNodes = false);
};

#define NODE_SYSTEM FEEditorNodeSystem::getInstance()