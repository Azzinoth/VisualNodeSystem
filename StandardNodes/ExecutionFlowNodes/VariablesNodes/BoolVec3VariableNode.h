#pragma once

#include "../BaseExecutionFlowNode.h"

class BoolVec3VariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::bvec3 Data = glm::bvec3(false);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	BoolVec3VariableNode();
	BoolVec3VariableNode(const BoolVec3VariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::bvec3 GetData() const;
	void SetData(glm::bvec3 NewValue);

	void Draw();
};