#include "CustomNode4.h"
using namespace VisNodeSys;

bool CustomNode4::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("CustomNode4",
		[]() -> Node* {
			return new CustomNode4();
		},

		[](const Node& CurrentNode) -> Node* {
			const CustomNode4& NodeToCopy = static_cast<const CustomNode4&>(CurrentNode);
			return new CustomNode4(NodeToCopy);
		}
	);

	return true;
}();

CustomNode4::CustomNode4() : Node()
{
	Type = "CustomNode4";

	SetStyle(DEFAULT);

	SetSize(ImVec2(370, 130));
	SetName("CustomNode4");

	TitleBackgroundColor = ImColor(128, 117, 208);
	TitleBackgroundColorHovered = ImColor(135, 145, 255);
	
	AddSocket(new NodeSocket(this, "EXEC", "in", false));
}

CustomNode4::CustomNode4(const CustomNode4& Src) : Node(Src)
{
	Data = Src.Data;

	SetStyle(DEFAULT);
}

void CustomNode4::SetStyle(NODE_STYLE NewValue)
{
	// Do nothing. We don't want to change style
}

void CustomNode4::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 NodePosition = ImGui::GetCursorScreenPos();
	ImVec2 NodeSize = GetSize();

	ImGui::SetCursorScreenPos(NodePosition + ImVec2(55.0f, 50.0f) * Zoom);
	ImGui::Text("Node demonstrate socket events");

	ImGui::SetCursorScreenPos(NodePosition + ImVec2(55.0f, 70.0f) * Zoom);
	std::string OutputText = "Last socket event: \n" + LastSocketEventDiscription;
	ImGui::Text(OutputText.c_str());
}

void CustomNode4::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	std::string EventTypeStr = "Unknown";
	switch (EventType)
	{
		case CONNECTED:
			EventTypeStr = "Connected";
		break;

		case DISCONNECTED:
			EventTypeStr = "Disconnected";
		break;

		case DESTRUCTION:
			EventTypeStr = "Destruction";
		break;

		case UPDATE:
			EventTypeStr = "Update";
		break;
	}

	LastSocketEventDiscription = "Node ID: " + ConnectedSocket->GetParent()->GetID() + "\nEvent type: " + EventTypeStr;
}

float CustomNode4::GetData()
{
	return Data;
}

bool CustomNode4::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}