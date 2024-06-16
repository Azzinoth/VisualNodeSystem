#include "CustomNode.h"
using namespace VisNodeSys;

bool CustomNode::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("CustomNode",
		[]() -> Node* {
			return new CustomNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const CustomNode& NodeToCopy = static_cast<const CustomNode&>(CurrentNode);
			return new CustomNode(NodeToCopy);
		}
	);

	return true;
}();

CustomNode::CustomNode() : Node()
{
	Type = "CustomNode";
	bCouldBeDestroyed = false;

	SetStyle(DEFAULT);

	SetSize(ImVec2(220, 130));
	SetName("CustomNode");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);
	
	AddSocket(new NodeSocket(this, "EXEC", "out", true));
}

CustomNode::CustomNode(const CustomNode& Src) : Node(Src)
{
	Data = Src.Data;
	bCouldBeDestroyed = false;

	SetStyle(DEFAULT);
}

void CustomNode::SetStyle(NODE_STYLE NewValue)
{
	// Do nothing. We don't want to change style
}

void CustomNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 NodePosition = ImGui::GetCursorScreenPos();

	ImGui::SetCursorScreenPos(NodePosition + ImVec2(5.0f, 34.0f) * Zoom);
	ImGui::Text("This is custom node");

	ImGui::SetCursorScreenPos(NodePosition + ImVec2(5.0f, 55.0f) * Zoom);
	ImGui::Text("User non-deletable node.");
}

void CustomNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

float CustomNode::GetData()
{
	return Data;
}

bool CustomNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return false;
}