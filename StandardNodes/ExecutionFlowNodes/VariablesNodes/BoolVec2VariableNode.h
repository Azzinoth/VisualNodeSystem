#pragma once

#include "../BaseExecutionFlowNode.h"

class BoolVec2VariableNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	glm::bvec2 Data = glm::bvec2(false);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	BoolVec2VariableNode();
	BoolVec2VariableNode(const BoolVec2VariableNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::bvec2 GetData() const;
	void SetData(glm::bvec2 NewValue);

	void Draw();
};