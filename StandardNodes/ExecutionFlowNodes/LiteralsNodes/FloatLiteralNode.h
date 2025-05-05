#pragma once
#include "../../../VisualNodeSystem.h"

class FloatLiteralNode : public VisNodeSys::Node
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	float Data = 0.0f;

	std::function<void* ()> FloatDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	FloatLiteralNode();
	FloatLiteralNode(const FloatLiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	float GetData() const;
	void SetData(float NewValue);

	void Draw();
};