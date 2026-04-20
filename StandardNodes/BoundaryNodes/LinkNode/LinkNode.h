#pragma once

#include "../SocketMirrorNode.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API LinkNode : public SocketMirrorNode
	{
		friend class NodeCore;
		friend class NodeArea;
		friend class NodeSystem;

		static bool bIsRegistered;

		std::string PartnerNodeID;
		std::string LinkedAreaID;
		// Prevents mutual destruction cycle, deleting one LinkNode triggers deletion of its partner, which would try to delete this node again.
		bool bIsInProcessOfBeingDestroyed = false;

		LinkNode();
		LinkNode(const LinkNode& Other);
		~LinkNode();

		void SetCorrectSize();

		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);

		std::vector<Node*> GetMirrorPartners() const;
	public:
		bool IsInputNode() const;
		Node* GetPartnerNode() const;
		NodeArea* GetLinkedArea() const;
		bool IsDangling() const;

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		using SocketMirrorNode::AddSocket;

		void Draw();
	};
}