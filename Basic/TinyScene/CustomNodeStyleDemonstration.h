#pragma once

#include "../../Core.h"

class CustomNodeStyleDemonstration : public VisNodeSys::Node
{
	friend class NodeFactory;
	static bool isRegistered;

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	// Make it private
	void SetStyle(VisNodeSys::NODE_STYLE NewValue);

	std::string LastSocketEventDiscription = "None";

	bool bReverseDirection = false;
public:
	CustomNodeStyleDemonstration();
	CustomNodeStyleDemonstration(const CustomNodeStyleDemonstration& Src);

	void Draw();
};