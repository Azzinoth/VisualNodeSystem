#pragma once

#include "../BaseExecutionFlowNode.h"

class Vec4VariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::vec4 Data = glm::vec4(0.0f);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	Vec4VariableNode();
	Vec4VariableNode(const Vec4VariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::vec4 GetData() const;
	void SetData(glm::vec4 NewValue);

	void Draw();
};