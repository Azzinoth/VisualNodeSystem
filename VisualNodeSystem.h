#pragma once

#include "SubSystems/VisualNodeArea/VisualNodeArea.h"

namespace VisNodeSys
{
	class NodeSystem
	{
		SINGLETON_PRIVATE_PART(NodeSystem)

		std::vector<NodeArea*> CreatedAreas;
	public:
		SINGLETON_PUBLIC_PART(NodeSystem)

		void Initialize();

		NodeArea* CreateNodeArea();
		void DeleteNodeArea(const NodeArea* NodeArea);

		void MoveNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea, bool SelectMovedNodes = false);

		std::vector<std::pair<std::string, ImColor>> GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color);
		void AssociateSocketTypeToColor(std::string SocketType, ImColor Color);
	};

#define NODE_SYSTEM VisNodeSys::NodeSystem::getInstance()
}