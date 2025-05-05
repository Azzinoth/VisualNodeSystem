#pragma once

#include "../BaseExecutionFlowNode.h"

class BoolVec4VariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::bvec4 Data = glm::bvec4(false);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	BoolVec4VariableNode();
	BoolVec4VariableNode(const BoolVec4VariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::bvec4 GetData() const;
	void SetData(glm::bvec4 NewValue);

	void Draw();
};