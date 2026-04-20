#pragma once

#include "../SocketMirrorNode.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API SubAreaInputNode : public SocketMirrorNode
	{
		friend class SubAreaNode;
		friend class NodeCore;
		friend class NodeArea;
		friend class NodeSystem;

		static bool bIsRegistered;

		void Init();
		SubAreaInputNode();
		SubAreaInputNode(const SubAreaInputNode& Other);
		~SubAreaInputNode();

		std::string OwnerSubAreaNodeID = "";

		void SetCorrectSize();

		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);

		std::vector<Node*> GetMirrorPartners() const;
	public:
		Node* GetOwnerSubAreaNode() const;
		NodeArea* GetOwningParentArea() const;

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		void Draw();

		bool IsDangling() const;
	};
}