#include "CustomNode3.h"
using namespace VisNodeSys;

bool CustomNode3::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("CustomNode3",
		[]() -> Node* {
			return new CustomNode3();
		},

		[](const Node& CurrentNode) -> Node* {
			const CustomNode3& NodeToCopy = static_cast<const CustomNode3&>(CurrentNode);
			return new CustomNode3(NodeToCopy);
		}
	);

	return true;
}();

CustomNode3::CustomNode3() : Node()
{
	Type = "CustomNode3";

	SetStyle(DEFAULT);

	SetSize(ImVec2(350, 120));
	SetName("CustomNode3");

	TitleBackgroundColor = ImColor(128, 117, 208);
	TitleBackgroundColorHovered = ImColor(135, 145, 255);
	
	AddSocket(new NodeSocket(this, "EXEC", "in", false));
}

CustomNode3::CustomNode3(const CustomNode3& Src) : Node(Src)
{
	Data = Src.Data;

	SetStyle(DEFAULT);
}

void CustomNode3::SetStyle(NODE_STYLE NewValue)
{
	// Do nothing. We don't want to change style
}

void CustomNode3::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 NodePosition = ImGui::GetCursorScreenPos();
	ImVec2 NodeSize = GetSize();

	ImGui::SetCursorScreenPos(NodePosition + ImVec2(5.0f, 50.0f) * Zoom);
	ImGui::Text("This is custom node");

	ImGui::SetCursorScreenPos(NodePosition + ImVec2(5.0f, 90.0f) * Zoom);
	ImGui::Text("This node can be connected only with CustomNode2.");
}

void CustomNode3::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

float CustomNode3::GetData()
{
	return Data;
}

bool CustomNode3::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	if (CandidateSocket->GetParent()->GetType() == "CustomNode2")
		return true;

	return false;
}