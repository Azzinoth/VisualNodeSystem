#pragma once

#include "SubSystems/VisualNodeArea/VisualNodeArea.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API NodeSystem
	{
		SINGLETON_PRIVATE_PART(NodeSystem)

		std::vector<NodeArea*> CreatedAreas;
	public:
		SINGLETON_PUBLIC_PART(NodeSystem)

		void Initialize(bool bTestMode = false);

		NodeArea* CreateNodeArea();
		void DeleteNodeArea(const NodeArea* NodeArea);

		void MoveNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea, bool SelectMovedNodes = false);

		std::vector<std::pair<std::string, ImColor>> GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color);
		void AssociateSocketTypeToColor(std::string SocketType, ImColor Color);
	};

#ifdef VISUAL_NODE_SYSTEM_SHARED
	extern "C" __declspec(dllexport) void* GetNodeSystem();
	#define NODE_SYSTEM (*static_cast<VisNodeSys::NodeSystem*>(VisNodeSys::GetNodeSystem()))
#else
	#define NODE_SYSTEM VisNodeSys::NodeSystem::GetInstance()
#endif
}