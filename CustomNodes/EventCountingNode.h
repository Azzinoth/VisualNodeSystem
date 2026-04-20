#pragma once

#include "../../Core.h"

class EventCountingNode : public VisNodeSys::Node
{
	friend class VisNodeSys::NodeFactory;
	static bool bIsRegistered;

	int ConnectedCount = 0;
	int DisconnectedCount = 0;
	int DestructionCount = 0;
	int UpdateCount = 0;
	int ExecuteCount = 0;

	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType) override;

public:
	EventCountingNode();
	EventCountingNode(const EventCountingNode& Src);

	void Draw() override;

	int GetConnectedCount() const;
	int GetDisconnectedCount() const;
	int GetDestructionCount() const;
	int GetUpdateCount() const;
	int GetExecuteCount() const;

	// Zeroes all counters.
	void ResetCounters();
};