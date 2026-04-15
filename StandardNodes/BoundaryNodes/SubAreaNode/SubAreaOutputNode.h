#pragma once

#include "../SocketMirrorNode.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API SubAreaOutputNode : public SocketMirrorNode
	{
		friend class SubAreaNode;
		friend class NodeCore;
		friend class NodeArea;
		friend class NodeSystem;

		static bool bIsRegistered;

		SubAreaOutputNode();
		SubAreaOutputNode(const SubAreaOutputNode& Other);
		~SubAreaOutputNode();

		std::string OwnerSubAreaNodeID = "";

		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);

		std::vector<Node*> GetMirrorPartners() const;

	public:
		Node* GetOwnerSubAreaNode() const;
		NodeArea* GetParentArea() const;

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		void Draw();
	};
}