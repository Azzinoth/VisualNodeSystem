#pragma once

#include "SubSystems/VisualNodeArea/VisualNodeArea.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API NodeSystem
	{
		friend class VisualReferenceNode;
		friend class NodeArea;

		struct ReferenceNodeData
		{
			std::string NodeID;
			std::string ParentNodeAreaID;
			std::string ReferencedAreaID;
		};

		SINGLETON_PRIVATE_PART(NodeSystem)

		std::vector<NodeArea*> CreatedAreas;
		std::unordered_map<std::string, ReferenceNodeData> ReferenceNodeRecords;

		ReferenceNodeData GetReferenceNodeDataByNodeID(const std::string& NodeID) const;
		std::vector<ReferenceNodeData> GetReferenceNodeDataByReferencedAreaID(const std::string& ReferencedAreaID) const;
		std::vector<ReferenceNodeData> GetReferenceNodeDataByParentAreaID(const std::string& ParentAreaID) const;

		bool CreateReferenceNodeRecord(const std::string& NodeID, const std::string& ParentNodeAreaID, const std::string& ReferencedAreaID);
		bool UpdateReferenceNodeRecord(const std::string& NodeID, const std::string& ParentNodeAreaID, const std::string& ReferencedAreaID);
		bool DeleteReferenceNodeRecord(const std::string& NodeID);

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
		void RegisterStandardNodes();
#endif
		void OnNodeAreaFocusChanging(NodeArea* CurrentNodeArea, bool NewFocusValue);
	public:
		SINGLETON_PUBLIC_PART(NodeSystem)

		void Initialize(bool bTestMode = false);

		std::string ToJson() const;
		bool SaveToFile(const std::string& FilePath) const;

		bool LoadFromJson(const std::string& JsonText);
		bool LoadFromFile(const std::string& FilePath);

		std::vector<std::string> GetNodeAreaIDList() const;
		
		std::string GetVersion();
		std::string GetFullVersion();

		NodeArea* CreateNodeArea();
		NodeArea* GetNodeAreaByID(const std::string& NodeAreaID) const;

		void DeleteNodeArea(const NodeArea* NodeArea);

		void MoveNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea, bool SelectMovedNodes = false);

		std::vector<std::pair<std::string, ImColor>> GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color);
		void AssociateSocketTypeToColor(std::string SocketType, ImColor Color);

		std::vector<NodeArea*> GetDirectlyReferencedAreas(const NodeArea* CurrentNodeArea) const;
		std::vector<NodeArea*> GetAllReferencedAreasRecursive(const NodeArea* CurrentNodeArea) const;

		std::vector<NodeArea*> GetReferencingAreas(const NodeArea* CurrentNodeArea) const;
		std::vector<NodeArea*> GetAllReferencingAreasRecursive(const NodeArea* CurrentNodeArea) const;
	};

#ifdef VISUAL_NODE_SYSTEM_SHARED
	extern "C" __declspec(dllexport) void* GetNodeSystem();
	#define NODE_SYSTEM (*static_cast<VisNodeSys::NodeSystem*>(VisNodeSys::GetNodeSystem()))
#else
	#define NODE_SYSTEM VisNodeSys::NodeSystem::GetInstance()
#endif
}