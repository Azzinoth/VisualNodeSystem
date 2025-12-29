#pragma once

#include "../../VisualNode.h"

//#define NODE_HEIGHT_PER_SOCKET 40

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API VisualSubAreaNode : public VisNodeSys::Node
	{
		friend class NodeArea;
		static bool bIsRegistered;

		//bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
		//void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

		std::string SubAreaID;
	public:
		VisualSubAreaNode(bool bCreateEmptyNodeArea = false);
		VisualSubAreaNode(NodeArea* SubArea);
		VisualSubAreaNode(const VisualSubAreaNode& Other);
		~VisualSubAreaNode();

		NodeArea* GetSubArea() const;
		bool SetSubArea(NodeArea* NewSubArea);
		bool SetSubArea(std::string NewSubAreaID);

		Json::Value ToJson();
		bool FromJson(Json::Value Json);

		void Draw();
	};
}