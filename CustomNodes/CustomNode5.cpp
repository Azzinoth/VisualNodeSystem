#include "CustomNode5.h"
using namespace VisNodeSys;

bool CustomNode5::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("CustomNode5",
		[]() -> Node* {
			return new CustomNode5();
		},

		[](const Node& CurrentNode) -> Node* {
			const CustomNode5& NodeToCopy = static_cast<const CustomNode5&>(CurrentNode);
			return new CustomNode5(NodeToCopy);
		}
	);

	return true;
}();

CustomNode5::CustomNode5() : Node()
{
	Type = "CustomNode5";

	SetStyle(DEFAULT);

	SetSize(ImVec2(350, 120));
	SetName("CustomNode5");

	TitleBackgroundColor = ImColor(128, 117, 208);
	TitleBackgroundColorHovered = ImColor(135, 145, 255);
	
	AddSocket(new NodeSocket(this, "FLOAT", "int", false));
	AddSocket(new NodeSocket(this, "FLOAT", "out", true));
}

CustomNode5::CustomNode5(const CustomNode5& Src) : Node(Src)
{
	Data = Src.Data;

	SetStyle(DEFAULT);
}

void CustomNode5::SetStyle(NODE_STYLE NewValue)
{
	// Do nothing. We don't want to change style
}

void CustomNode5::Draw()
{	
	Node::Draw();
}

void CustomNode5::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

float CustomNode5::GetData() const
{
	return Data;
}

void CustomNode5::SetData(float NewValue)
{
	Data = NewValue;
}

bool CustomNode5::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}