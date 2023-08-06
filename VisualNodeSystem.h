#pragma once

#include "SubSystems/VisualNodeArea/VisualNodeArea.h"

class VisualNodeSystem
{
	SINGLETON_PRIVATE_PART(VisualNodeSystem)

	std::vector<VisualNodeArea*> CreatedAreas;
public:
	SINGLETON_PUBLIC_PART(VisualNodeSystem)

	VisualNodeArea* CreateNodeArea();
	void DeleteNodeArea(const VisualNodeArea* NodeArea);

	void MoveNodesTo(VisualNodeArea* SourceNodeArea, VisualNodeArea* TargetNodeArea, bool SelectMovedNodes = false);

	std::vector<std::pair<std::string, ImColor>> GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color);
	void AssociateSocketTypeToColor(std::string SocketType, ImColor Color);
};

#define NODE_SYSTEM VisualNodeSystem::getInstance()