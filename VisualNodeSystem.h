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
			FEUUID ID;

			FEUUID InNodeID;
			FEUUID OutNodeID;

			FEUUID InAreaID;
			FEUUID OutAreaID;

			bool IsNull() const
			{
				return VisNodeSys::IsNull(ID) || VisNodeSys::IsNull(InNodeID) || VisNodeSys::IsNull(OutNodeID) || VisNodeSys::IsNull(InAreaID) || VisNodeSys::IsNull(OutAreaID);
			}
		};

		std::vector<NodeArea*> Areas;
		std::unordered_map<FEUUID, NodeAreaLinkRecord> NodeAreaLinkRecords;

		bool IsAlreadyConnected(NodeSocket* FirstSocket, NodeSocket* SecondSocket, const std::vector<Connection*>& Connections);
		void ProcessConnections(const std::vector<NodeSocket*>& Sockets,
								std::unordered_map<NodeSocket*, NodeSocket*>& OldToNewSocket,
								NodeArea* TargetArea, size_t NodeShift, const std::vector<Node*>& SourceNodes);
		void CopyNodesInternal(const std::vector<Node*>& SourceNodes, NodeArea* TargetArea, const size_t NodeShift = 0);

		LinkNode* CreateLinkNodeInternal(bool bIsInputNode);
		NodeAreaLinkRecord* GetLinkDataByNodeID(const FEUUID& NodeID);
		std::vector<NodeAreaLinkRecord*> GetLinkDataByAreaID(const FEUUID& AreaID);
		
		void OnNodeDeletion(Node* DeletedNode);
		bool DeleteLinkRecord(const FEUUID& LinkID);

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
		void RegisterStandardNodes();
#endif
		void OnNodeAreaFocusChanging(NodeArea* CurrentNodeArea, bool bNewFocusValue);

		bool DeleteSocket(const FEUUID& NodeID, const FEUUID& SocketID);
		bool DeleteSocket(NodeSocket* Socket);
		bool RevalidateSocketConnections(NodeSocket* Socket);

		SocketMirrorNode* GetAppropriatePartner(SocketMirrorNode* MirrorNode, NodeSocket::SocketFlow CurrentDirection);
		std::pair<SocketMirrorNode*, NodeSocket*> GetAppropriatePartnerAndSocket(SocketMirrorNode* MirrorNode, NodeSocket* CurrentNodeSocket);

		bool AddSocketToMirrorNode(const FEUUID& NodeID, std::vector<std::string> AllowedTypes, std::string Name, NodeSocket::SocketFlow SocketDirection = NodeSocket::SocketFlow::Input);
		bool DeleteSocketFromMirrorNode(const FEUUID& NodeID, const FEUUID& SocketID);

		bool SyncMirrorNodeSocketAllowedTypes(const FEUUID& NodeID, const FEUUID& SocketID, std::vector<std::string> NewTypes);
		void SyncMirrorNodeSocketName(const FEUUID& NodeID, const FEUUID& SocketID, std::string NewName);

		std::vector<FEUUID> DeduplicateIDList(const std::vector<FEUUID>& ListOfIDs) const;

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

		std::vector<FEUUID> GetNodeAreaIDList() const;
		
		std::string GetVersion();
		std::string GetFullVersion();

		NodeArea* CreateNodeArea();
		NodeArea* CreateNodeArea(const std::vector<Node*> Nodes, const std::vector<GroupComment*> GroupComments);
		NodeArea* GetNodeAreaByID(const FEUUID& NodeAreaID) const;
		std::vector<NodeArea*> GetNodeAreasByName(const std::string& Name) const;
		void DeleteNodeArea(const NodeArea* NodeAreaToDelete);
		void DeleteNodeAreaByID(const FEUUID& NodeAreaID);
		void CopyElementsTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea);

		size_t GetNodeAreaCount() const;
		size_t GetTotalNodeCount(std::vector<FEUUID> AreaIDFilter = {}) const;
		size_t GetTotalConnectionCount(std::vector<FEUUID> AreaIDFilter = {}) const;
		size_t GetGroupCommentCount(std::vector<FEUUID> AreaIDFilter = {}) const;
		size_t GetRerouteConnectionCount(std::vector<FEUUID> AreaIDFilter = {}) const;

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
		// Will return a map where key is node area IDs and value is a list of of nodes that were executed last time in that area.
		std::unordered_map<FEUUID, std::vector<Node*>> GetLastExecutedNodes(const FEUUID& StartingAreaID = FEUUID()) const;
#endif

		bool MoveNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea, bool bSelectNodesAfterMovement = false);

		Node* GetNodeByID(const FEUUID& NodeID) const;
		std::vector<Node*> GetNodesByName(const std::string& Name) const;
		std::vector<Node*> GetNodesByStringType(const std::string& Type) const;

		std::vector<std::pair<std::string, ImColor>> GetAssociationsOfSocketTypeToColor(std::string SocketType, ImColor Color);
		void AssociateSocketTypeToColor(std::string SocketType, ImColor Color);

		bool IsInAListOfAreas(const FEUUID& AreaID, const std::vector<FEUUID>& AreaIDList) const;
		bool IsInAListOfAreas(const NodeArea* Area, const std::vector<NodeArea*>& AreaIDList) const;

		bool LinkNodeAreas(const FEUUID& UpstreamAreaID,
						   const FEUUID& DownstreamAreaID,
						   std::pair<FEUUID, FEUUID>* CreatedLinkNodeIDs = nullptr);
		bool IsLinked(const FEUUID& FirstAreaID, const FEUUID& SecondAreaID) const;
		bool UnlinkNodeAreas(const FEUUID& FirstAreaID, const FEUUID& SecondAreaID);
		std::vector<std::pair<FEUUID, FEUUID>> GetLinkingNodesForAreas(const FEUUID& FirstAreaID, const FEUUID& SecondAreaID) const;
		std::vector<LinkNode*> GetDanglingLinkNodes() const;
		bool TryToFixDanglingLinkNode(LinkNode* LinkNodeToFix, bool bForceRestorePartner = false);
		std::vector<LinkNode*> TryToFixAllDanglingLinkNodes();

		std::vector<NodeArea*> GetImmediateDownstreamAreas(const FEUUID& AreaID);
		std::vector<NodeArea*> GetAllDownstreamAreas(const FEUUID& AreaID);

		std::vector<NodeArea*> GetImmediateUpstreamAreas(const FEUUID& AreaID);
		std::vector<NodeArea*> GetAllUpstreamAreas(const FEUUID& AreaID);

		SubAreaNode* CreateSubAreaNode(const FEUUID& ParentAreaID);
		SubAreaNode* FindOwnerSubAreaNode(const FEUUID& AreaID) const;
		SubAreaNode* ConvertNodesToSubArea(NodeArea* ParentArea, const std::vector<Node*>& NodesToConvert);
	};

#ifdef VISUAL_NODE_SYSTEM_SHARED
	extern "C" __declspec(dllexport) void* GetNodeSystem();
	#define NODE_SYSTEM (*static_cast<VisNodeSys::NodeSystem*>(VisNodeSys::GetNodeSystem()))
#else
	#define NODE_SYSTEM VisNodeSys::NodeSystem::GetInstance()
#endif
}