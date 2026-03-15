#pragma once

#include "../../VisualNode.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API LinkNode : public Node
	{
		friend class NodeArea;
		friend class NodeSystem;
		static bool bIsRegistered;

		std::string PartnerNodeID;
		bool bIsInputNode = true;
		std::string LinkedAreaID;

		LinkNode();
		LinkNode(const LinkNode& Other);
		~LinkNode();

		void AddSocket(NodeSocket* Socket);

		//bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
		//void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
	public:
		bool IsInputNode() const;
		Node* GetPartnerNode() const;
		NodeArea* GetLinkedArea() const;
		bool IsDangling() const;

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		void Draw();
	};
}