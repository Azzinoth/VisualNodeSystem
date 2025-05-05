#pragma once

#include "../BaseExecutionFlowNode.h"

class WhileLoopNode : public BaseExecutionFlowNode
{
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);
public:
	WhileLoopNode();
};