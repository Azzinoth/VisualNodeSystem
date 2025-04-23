#pragma once

#include "../../Core.h"

class CustomNode2 : public VisNodeSys::Node
{
	friend class VisNodeSys::NodeFactory;
	static bool isRegistered;

	float Data = 0.0f;
	std::string StringData = "";

	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

	// Make it private
	void SetStyle(VisNodeSys::NODE_STYLE NewValue);
public:
	CustomNode2();
	CustomNode2(const CustomNode2& Src);

	void Draw();

	float GetData() const;
	void SetData(float NewValue);

	std::string GetStringData() const;
	void SetStringData(std::string NewValue);
};