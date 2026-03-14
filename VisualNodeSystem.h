#pragma once

#include "SubSystems/VisualNodeArea/VisualNodeArea.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API NodeSystem
	{
		friend class VisualLinkNode;
		friend class NodeArea;

		SINGLETON_PRIVATE_PART(NodeSystem)

		struct NodeAreaLinkRecord
		{
			std::string ID;

			std::string InNodeID;
			std::string OutNodeID;

			std::string InAreaID;
			std::string OutAreaID;

			bool IsNull() const
			{
				return ID.empty() || InNodeID.empty() || OutNodeID.empty() || InAreaID.empty() || OutAreaID.empty();
			}
		};

		std::vector<NodeArea*> Areas;
		std::unordered_map<std::string, NodeAreaLinkRecord> NodeAreaLinkRecords;

		NodeAreaLinkRecord GetLinkDataByNodeID(const std::string& NodeID) const;
		std::vector<NodeAreaLinkRecord> GetLinkDataByAreaID(const std::string& AreaID) const;
		
		void OnNodeDeletion(Node* DeletedNode);
		bool DeleteLinkRecord(const std::string& LinkID);

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

		bool LinkNodeAreas(const std::string& UpstreamAreaID,
						   const std::string& DownstreamAreaID,
						   std::pair<std::string, std::string>* CreatedLinkNodeIDs = nullptr);
		bool IsLinked(const std::string& FirstAreaID, const std::string& SecondAreaID) const;
		bool UnlinkNodeAreas(const std::string& FirstAreaID, const std::string& SecondAreaID);
		std::vector<std::pair<std::string, std::string>> GetLinkingNodesForAreas(const std::string& FirstAreaID, const std::string& SecondAreaID) const;

		std::vector<NodeArea*> GetImmediateDownstreamAreas(const std::string& AreaID) const;
		std::vector<NodeArea*> GetAllDownstreamAreas(const std::string& AreaID) const;

		std::vector<NodeArea*> GetImmediateUpstreamAreas(const std::string& AreaID) const;
		std::vector<NodeArea*> GetAllUpstreamAreas(const std::string& AreaID) const;
	};

#ifdef VISUAL_NODE_SYSTEM_SHARED
	extern "C" __declspec(dllexport) void* GetNodeSystem();
	#define NODE_SYSTEM (*static_cast<VisNodeSys::NodeSystem*>(VisNodeSys::GetNodeSystem()))
#else
	#define NODE_SYSTEM VisNodeSys::NodeSystem::GetInstance()
#endif
}