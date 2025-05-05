#pragma once

#include "../BaseExecutionFlowNode.h"

class LoopNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int LocalFirstIndex = 0;
	int LocalLastIndex = 0;
	int CurrentIndex = 0;

	std::function<void* ()> CurrentIndexGetter = [this]() -> void* {
		return &CurrentIndex;
	};
public:
	LoopNode();
	LoopNode(const LoopNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};