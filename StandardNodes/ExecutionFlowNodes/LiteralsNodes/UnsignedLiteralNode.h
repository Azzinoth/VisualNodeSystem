#pragma once

#include "../../../VisualNodeSystem.h"

class UnsignedLiteralNode : public VisNodeSys::Node
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	uint64_t Data = 0;

	std::function<void* ()> UIntDataGetter = [this]() -> void* {
		return &Data;
	};

public:
	UnsignedLiteralNode();
	UnsignedLiteralNode(const UnsignedLiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	unsigned int GetData() const;
	void SetData(unsigned int NewValue);

	void Draw();
};
