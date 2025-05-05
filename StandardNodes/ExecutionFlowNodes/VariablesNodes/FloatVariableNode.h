#pragma once

#include "../BaseExecutionFlowNode.h"

class FloatVariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	float Data = 0.0f;

	std::function<void* ()> FloatDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	FloatVariableNode();
	FloatVariableNode(const FloatVariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	float GetData() const;
	void SetData(float NewValue);

	void Draw();
};