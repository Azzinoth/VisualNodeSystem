#pragma once

#include "../../../VisualNodeSystem.h"

class IntegerLiteralNode : public VisNodeSys::Node
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	int Data = 0;

	std::function<void* ()> IntDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	IntegerLiteralNode();
	IntegerLiteralNode(const IntegerLiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	int GetData() const;
	void SetData(int NewValue);

	void Draw();
};