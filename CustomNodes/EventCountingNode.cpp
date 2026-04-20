#include "EventCountingNode.h"
using namespace VisNodeSys;

bool EventCountingNode::bIsRegistered = []() {
	NODE_FACTORY.RegisterNodeType("EventCountingNode",
		[]() -> Node* {
			return new EventCountingNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const EventCountingNode& NodeToCopy = static_cast<const EventCountingNode&>(CurrentNode);
			return new EventCountingNode(NodeToCopy);
		}
	);

	return true;
}();

EventCountingNode::EventCountingNode() : Node()
{
	Type = "EventCountingNode";

	SetSize(ImVec2(220, 80));
	SetName("EventCountingNode");

	TitleBackgroundColor = ImColor(90, 130, 90);
	TitleBackgroundColorHovered = ImColor(110, 160, 110);
}

EventCountingNode::EventCountingNode(const EventCountingNode& Src) : Node(Src)
{

}

void EventCountingNode::Draw()
{
	Node::Draw();
}

void EventCountingNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	switch (EventType)
	{
		case CONNECTED:     ConnectedCount++; break;
		case DISCONNECTED:  DisconnectedCount++; break;
		case DESTRUCTION:   DestructionCount++; break;
		case UPDATE:        UpdateCount++; break;
		case EXECUTE:       ExecuteCount++; break;
		default: break;
	}
}

int EventCountingNode::GetConnectedCount() const
{ 
	return ConnectedCount;
}

int EventCountingNode::GetDisconnectedCount() const
{ 
	return DisconnectedCount;
}

int EventCountingNode::GetDestructionCount() const
{ 
	return DestructionCount;
}

int EventCountingNode::GetUpdateCount() const
{ 
	return UpdateCount;
}

int EventCountingNode::GetExecuteCount() const 
{ 
	return ExecuteCount;
}

void EventCountingNode::ResetCounters()
{
	ConnectedCount = 0;
	DisconnectedCount = 0;
	DestructionCount = 0;
	UpdateCount = 0;
	ExecuteCount = 0;
}