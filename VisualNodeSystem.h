#pragma once

#include "SubSystems/VisualNodeArea/VisualNodeArea.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API NodeSystem
	{
		friend class NodeSocket;
		friend class Node;
		friend class SocketMirrorNode;
		friend class LinkNode;
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

		bool IsAlreadyConnected(NodeSocket* FirstSocket, NodeSocket* SecondSocket, const std::vector<Connection*>& Connections);
		void ProcessConnections(const std::vector<NodeSocket*>& Sockets,
								std::unordered_map<NodeSocket*, NodeSocket*>& OldToNewSocket,
								NodeArea* TargetArea, size_t NodeShift, const std::vector<Node*>& SourceNodes);
		void CopyNodesInternal(const std::vector<Node*>& SourceNodes, NodeArea* TargetArea, const size_t NodeShift = 0);

		LinkNode* CreateLinkNodeInternal(bool bIsInputNode);
		NodeAreaLinkRecord* GetLinkDataByNodeID(const std::string& NodeID);
		std::vector<NodeAreaLinkRecord*> GetLinkDataByAreaID(const std::string& AreaID);
		
		void OnNodeDeletion(Node* DeletedNode);
		bool DeleteLinkRecord(const std::string& LinkID);

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
		void RegisterStandardNodes();
#endif
		void OnNodeAreaFocusChanging(NodeArea* CurrentNodeArea, bool bNewFocusValue);

		bool DeleteSocket(const std::string& NodeID, std::string SocketID);
		bool DeleteSocket(NodeSocket* Socket);
		bool RevalidateSocketConnections(NodeSocket* Socket);

		SocketMirrorNode* GetAppropriatePartner(SocketMirrorNode* MirrorNode, NodeSocket::SocketFlow CurrentDirection);
		std::pair<SocketMirrorNode*, NodeSocket*> GetAppropriatePartnerAndSocket(SocketMirrorNode* MirrorNode, NodeSocket* CurrentNodeSocket);

		bool AddSocketToMirrorNode(const std::string& NodeID, std::vector<std::string> AllowedTypes, std::string Name, NodeSocket::SocketFlow SocketDirection = NodeSocket::SocketFlow::Input);
		bool DeleteSocketFromMirrorNode(const std::string& NodeID, std::string SocketID);

		bool SyncMirrorNodeSocketAllowedTypes(const std::string& NodeID, std::string SocketID, std::vector<std::string> NewTypes);
		void SyncMirrorNodeSocketName(const std::string& NodeID, std::string SocketID, std::string NewName);

		std::vector<std::string> DeduplicateIDList(const std::vector<std::string>& ListOfIDs) const;

		// Walks the SubAreaNode ownership graph and get rid of cycles.
		void BreakSubAreaOwnershipCycles();
	public:
		SINGLETON_PUBLIC_PART(NodeSystem)

		void Initialize(bool bTestMode = false);

		std::string ToJson() const;
		bool SaveToFile(const std::string& FilePath) const;

		bool LoadFromJson(const std::string& JsonText);
		bool LoadFromFile(const std::string& FilePath);

		void Clear();

		std::vector<std::string> GetNodeAreaIDList() const;
		
		std::string GetVersion();
		std::string GetFullVersion();

		NodeArea* CreateNodeArea();
		NodeArea* CreateNodeArea(const std::vector<Node*> Nodes, const std::vector<GroupComment*> GroupComments);
		NodeArea* GetNodeAreaByID(const std::string& NodeAreaID) const;
		std::vector<NodeArea*> GetNodeAreasByName(const std::string& Name) const;
		void DeleteNodeArea(const NodeArea* NodeAreaToDelete);
		void DeleteNodeAreaByID(const std::string& NodeAreaID);
		void CopyElementsTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea);

		size_t GetNodeAreaCount() const;
		size_t GetTotalNodeCount(std::vector<std::string> AreaIDFilter = {}) const;
		size_t GetTotalConnectionCount(std::vector<std::string> AreaIDFilter = {}) const;
		size_t GetGroupCommentCount(std::vector<std::string> AreaIDFilter = {}) const;
		size_t GetRerouteConnectionCount(std::vector<std::string> AreaIDFilter = {}) const;

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
		// Will return a map where key is node area IDs and value is a list of of nodes that were executed last time in that area.
		std::unordered_map<std::string, std::vector<Node*>> GetLastExecutedNodes(std::string StartingAreaID = "") const;
#endif

		bool MoveNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea, bool bSelectNodesAfterMovement = false);

		Node* GetNodeByID(const std::string& NodeID) const;
		std::vector<Node*> GetNodesByName(const std::string& Name) const;
		std::vector<Node*> GetNodesByStringType(const std::string& Type) const;

		std::vector<std::pair<std::string, ImColor>> GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color);
		void AssociateSocketTypeToColor(std::string SocketType, ImColor Color);

		bool IsInAListOfAreas(const std::string& AreaID, const std::vector<std::string>& AreaIDList) const;
		bool IsInAListOfAreas(const NodeArea* Area, const std::vector<NodeArea*>& AreaIDList) const;

		bool LinkNodeAreas(const std::string& UpstreamAreaID,
						   const std::string& DownstreamAreaID,
						   std::pair<std::string, std::string>* CreatedLinkNodeIDs = nullptr);
		bool IsLinked(const std::string& FirstAreaID, const std::string& SecondAreaID) const;
		bool UnlinkNodeAreas(const std::string& FirstAreaID, const std::string& SecondAreaID);
		std::vector<std::pair<std::string, std::string>> GetLinkingNodesForAreas(const std::string& FirstAreaID, const std::string& SecondAreaID) const;
		std::vector<LinkNode*> GetDanglingLinkNodes() const;
		bool TryToFixDanglingLinkNode(LinkNode* LinkNodeToFix, bool bForceRestorePartner = false);
		std::vector<LinkNode*> TryToFixAllDanglingLinkNodes();

		std::vector<NodeArea*> GetImmediateDownstreamAreas(const std::string& AreaID);
		std::vector<NodeArea*> GetAllDownstreamAreas(const std::string& AreaID);

		std::vector<NodeArea*> GetImmediateUpstreamAreas(const std::string& AreaID);
		std::vector<NodeArea*> GetAllUpstreamAreas(const std::string& AreaID);

		SubAreaNode* CreateSubAreaNode(const std::string& ParentAreaID);
		SubAreaNode* FindOwnerSubAreaNode(const std::string& AreaID) const;
		SubAreaNode* ConvertNodesToSubArea(NodeArea* ParentArea, const std::vector<Node*>& NodesToConvert);
	};

#ifdef VISUAL_NODE_SYSTEM_SHARED
	extern "C" __declspec(dllexport) void* GetNodeSystem();
	#define NODE_SYSTEM (*static_cast<VisNodeSys::NodeSystem*>(VisNodeSys::GetNodeSystem()))
#else
	#define NODE_SYSTEM VisNodeSys::NodeSystem::GetInstance()
#endif
}