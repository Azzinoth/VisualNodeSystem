#pragma once

#include "../../VisualNode.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API LinkNode : public Node
	{
		friend class NodeCore;
		friend class NodeArea;
		friend class NodeSystem;

		static bool bIsRegistered;

		std::string PartnerNodeID;
		bool bIsInputNode = true;
		std::string LinkedAreaID;
		// Prevents mutual destruction cycle, deleting one LinkNode triggers
		// deletion of its partner, which would try to delete this node again.
		bool bIsInProcessOfBeingDestroyed = false;
		// Prevents cycle when working with socket modification, when NODE_SYSTEM will trigger modification of partner node's socket, which would trigger modification of this node's socket again.
		std::string SocketIDBeingModified = "";

		bool bInEditMode = false;
		static ImTextureID LinkIconTextureID;
		static ImTextureID PlusIconTextureID;
		static ImTextureID EditIconTextureID;
		static ImTextureID TrashBinIconTextureID;

		std::function<void* ()> CreateCrossAreaDataGetter(int SocketIndex);

		LinkNode();
		LinkNode(const LinkNode& Other);
		~LinkNode();

		bool AddSocketInternal(std::vector<std::string> AllowedTypes, std::string Name = "");
		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);
	public:
		bool IsInputNode() const;
		Node* GetPartnerNode() const;
		NodeArea* GetLinkedArea() const;
		bool IsDangling() const;

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		bool AddSocket(NodeSocket* Socket);
		bool AddSocket(std::vector<std::string> AllowedTypes, std::string Name = "");
		bool DeleteSocket(NodeSocket* Socket);
		bool DeleteSocket(std::string SocketID);

		void Draw();
	};
}