#pragma once

#include "../../VisualNode.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API VisualLinkNode : public VisNodeSys::Node
	{
		friend class NodeArea;
		friend class NodeSystem;
		static bool bIsRegistered;

		std::string PartnerNodeID;
		bool bIsInputNode = true;
		std::string LinkedAreaID;

		VisualLinkNode();
		VisualLinkNode(const VisualLinkNode& Other);
		~VisualLinkNode();

		//bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
		//void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
	public:
		bool IsInputNode() const;
		Node* GetPartnerNode() const;
		NodeArea* GetLinkedArea() const;

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		void Draw();
	};
}