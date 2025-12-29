#pragma once

#include "SubSystems/VisualNodeArea/VisualNodeArea.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API NodeSystem
	{
		friend class VisualSubAreaNode;
		friend class NodeArea;

		struct SubAreaNodeData
		{
			std::string NodeID;
			std::string ParentNodeAreaID;
			std::string SubAreaID;
		};

		SINGLETON_PRIVATE_PART(NodeSystem)

		std::vector<NodeArea*> CreatedAreas;
		std::unordered_map<std::string, SubAreaNodeData> SubAreaNodeRecords;

		SubAreaNodeData GetSubAreaNodeDataByNodeID(const std::string& NodeID) const;
		std::vector<SubAreaNodeData> GetSubAreaNodeDataBySubAreaID(const std::string& SubAreaID) const;
		std::vector<SubAreaNodeData> GetSubAreaNodeDataByParentAreaID(const std::string& ParentAreaID) const;

		bool CreateSubAreaNodeRecord(const std::string& NodeID, const std::string& ParentNodeAreaID, const std::string& SubAreaID);
		bool UpdateSubAreaNodeRecord(const std::string& NodeID, const std::string& ParentNodeAreaID, const std::string& SubAreaID);
		bool DeleteSubAreaNodeRecord(const std::string& NodeID);

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
		void RegisterStandardNodes();
#endif
		void OnNodeAreaFocusChanging(NodeArea* CurrentNodeArea, bool NewFocusValue);
	public:
		SINGLETON_PUBLIC_PART(NodeSystem)

		void Initialize(bool bTestMode = false);

		NodeArea* CreateNodeArea();
		NodeArea* GetNodeAreaByID(const std::string& NodeAreaID) const;

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