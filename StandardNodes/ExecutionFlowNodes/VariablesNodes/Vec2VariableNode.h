#pragma once

#include "../BaseExecutionFlowNode.h"

class Vec2VariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::vec2 Data = glm::vec2(0.0f, 0.0f);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	Vec2VariableNode();
	Vec2VariableNode(const Vec2VariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::vec2 GetData() const;
	void SetData(glm::vec2 NewValue);

	void Draw();
};