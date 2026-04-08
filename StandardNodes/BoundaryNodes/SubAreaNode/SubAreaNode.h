#pragma once

#include "SubAreaInputNode.h"
#include "SubAreaOutputNode.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API SubAreaNode : public SocketMirrorNode
	{
		friend class NodeCore;
		friend class NodeArea;
		friend class NodeSystem;

		static bool bIsRegistered;

		std::string SubAreaInputNodeID;
		std::string SubAreaOutputNodeID;
		std::string OwnedAreaID;
		// Prevents mutual destruction cycle, deleting one LinkNode triggers
		// deletion of its partner, which would try to delete this node again.
		bool bIsInProcessOfBeingDestroyed = false;

		std::function<void* ()> CreateCrossAreaDataGetter(int SocketIndex);

		SubAreaNode();
		SubAreaNode(NodeArea* OwnedArea);
		SubAreaNode(const SubAreaNode& Other);
		// Move constructor.
		//SubAreaNode(SubAreaNode&& Other) noexcept;
		~SubAreaNode();

		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);

		std::vector<Node*> GetMirrorPartners() const;
	public:
		SubAreaInputNode* GetSubAreaInputNode() const;
		SubAreaOutputNode* GetSubAreaOutputNode() const;
		NodeArea* GetOwnedArea() const;

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		bool AddSocket(NodeSocket* Socket);
		bool AddSocket(std::vector<std::string> AllowedTypes, std::string Name, NodeSocket::Direction SocketDirection);

		void Draw();
	};
}