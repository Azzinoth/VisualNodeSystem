#pragma once

#include "../../Core.h"

class CustomNode4 : public VisNodeSys::Node
{
	friend class VisNodeSys::NodeFactory;
	static bool isRegistered;

	float Data = 0.0f;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	// Make it private
	void SetStyle(VisNodeSys::NODE_STYLE NewValue);

	std::string LastSocketEventDiscription = "None";
public:
	CustomNode4();
	CustomNode4(const CustomNode4& Src);

	void Draw();
	float GetData();
};