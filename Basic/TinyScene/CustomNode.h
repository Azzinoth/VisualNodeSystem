#pragma once

#include "../../Core.h"

class CustomNode : public VisNodeSys::Node
{
	friend class VisNodeSys::NodeFactory;
	static bool isRegistered;

	float Data = 0.0f;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	// Make it private
	void SetStyle(VisNodeSys::NODE_STYLE NewValue);
public:
	CustomNode();
	CustomNode(const CustomNode& Src);

	void Draw();
	float GetData();
};