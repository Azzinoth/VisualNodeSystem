#include "CustomNode2.h"
using namespace VisNodeSys;

bool CustomNode2::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("CustomNode2",
		[]() -> Node* {
			return new CustomNode2();
		},

		[](const Node& CurrentNode) -> Node* {
			const CustomNode2& NodeToCopy = static_cast<const CustomNode2&>(CurrentNode);
			return new CustomNode2(NodeToCopy);
		}
	);

	return true;
}();

CustomNode2::CustomNode2() : Node()
{
	Type = "CustomNode2";

	SetStyle(DEFAULT);

	SetSize(ImVec2(350, 120));
	SetName("CustomNode2");

	TitleBackgroundColor = ImColor(128, 117, 208);
	TitleBackgroundColorHovered = ImColor(135, 145, 255);
	
	AddSocket(new NodeSocket(this, "EXEC", "out", true));
}

CustomNode2::CustomNode2(const CustomNode2& Src) : Node(Src)
{
	Data = Src.Data;

	SetStyle(DEFAULT);
}

void CustomNode2::SetStyle(NODE_STYLE NewValue)
{
	// Do nothing. We don't want to change style
}

void CustomNode2::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 NodePosition = ImGui::GetCursorScreenPos();
	ImVec2 NodeSize = GetSize();

	ImGui::SetCursorScreenPos(NodePosition + ImVec2(5.0f, 50.0f) * Zoom);
	ImGui::Text("This is custom node");

	ImGui::SetCursorScreenPos(NodePosition + ImVec2(5.0f, 90.0f) * Zoom);
	ImGui::Text("This node can be connected to CustomNode3.");
}

void CustomNode2::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

float CustomNode2::GetData()
{
	return Data;
}

bool CustomNode2::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return false;
}