#pragma once

#include "../BaseExecutionFlowNode.h"

class SequenceNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int OutputCount = 0;

	void AddSequenceOutput();
public:
	SequenceNode();
	SequenceNode(const SequenceNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();
};