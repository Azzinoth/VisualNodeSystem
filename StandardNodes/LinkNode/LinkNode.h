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
		// Prevents mutual destruction cycle, deleting one LinkNode triggers
		// deletion of its partner, which would try to delete this node again.
		bool bIsInProcessOfBeingDestroyed = false;

		std::function<void* ()> CreateCrossAreaDataGetter(int SocketIndex);

		LinkNode();
		LinkNode(const LinkNode& Other);
		~LinkNode();

		void AddSocket(NodeSocket* Socket);

		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);
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