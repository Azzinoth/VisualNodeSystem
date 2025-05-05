#pragma once

#include "../BaseExecutionFlowNode.h"

class Vec3VariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::vec3 Data = glm::vec3(0.0f);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	Vec3VariableNode();
	Vec3VariableNode(const Vec3VariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::vec3 GetData() const;
	void SetData(glm::vec3 NewValue);

	void Draw();
};