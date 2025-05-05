#pragma once

#include "../BaseExecutionFlowNode.h"

class BoolVariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	bool bData = false;

	std::function<void* ()> BoolDataGetter = [this]() -> void* {
		return &bData;
	};

public:
	BoolVariableNode();
	BoolVariableNode(const BoolVariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	bool GetData() const;
	void SetData(bool NewValue);

	void Draw();
};