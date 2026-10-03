#pragma once

#include "../BaseExecutionFlowNode.h"

class UnsignedVariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	unsigned int Data = 0;

	std::function<void* ()> UnsignedDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	UnsignedVariableNode();
	UnsignedVariableNode(const UnsignedVariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	unsigned int GetData() const;
	void SetData(unsigned int NewValue);

	void Draw();
};
