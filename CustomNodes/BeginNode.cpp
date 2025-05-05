#include "BeginNode.h"
using namespace VisNodeSys;

bool BeginNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("BeginNode",
		[]() -> Node* {
			return new BeginNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const BeginNode& NodeToCopy = static_cast<const BeginNode&>(CurrentNode);
			return new BeginNode(NodeToCopy);
		}
	);

	return true;
}();

BeginNode::BeginNode() : BaseExecutionFlowNode(false)
{
	Type = "BeginNode";
	bCouldBeDestroyed = false;

	SetStyle(CIRCLE);

	SetSize(ImVec2(220, 78));
	SetName("BeginNode");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);
	
	AddSocket(new NodeSocket(this, "EXECUTE", "out", true));
}

BeginNode::BeginNode(const BeginNode& Other) : BaseExecutionFlowNode(Other)
{
	bCouldBeDestroyed = false;
	SetStyle(CIRCLE);
}

void BeginNode::Draw()
{	
	Node::Draw();
}

void BeginNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BeginNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return false;
}