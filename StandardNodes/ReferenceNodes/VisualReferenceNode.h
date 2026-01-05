#pragma once

#include "../../VisualNode.h"

//#define NODE_HEIGHT_PER_SOCKET 40

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API VisualReferenceNode : public VisNodeSys::Node
	{
		friend class NodeArea;
		static bool bIsRegistered;

		//bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
		//void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

		std::string ReferencedAreaID;
	public:
		VisualReferenceNode(bool bCreateEmptyNodeArea = false);
		VisualReferenceNode(NodeArea* ReferencedArea);
		VisualReferenceNode(const VisualReferenceNode& Other);
		~VisualReferenceNode();

		NodeArea* GetReferencedArea() const;
		bool SetReferencedArea(NodeArea* NewReferencedArea);
		bool SetReferencedArea(std::string NewReferencedAreaID);

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		void Draw();
	};
}