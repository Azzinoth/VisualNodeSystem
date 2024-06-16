#include "CustomNodeStyleDemonstration.h"
using namespace VisNodeSys;

bool CustomNodeStyleDemonstration::isRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("CustomNodeStyleDemonstration",
		[]() -> Node* {
			return new CustomNodeStyleDemonstration();
		},

		[](const Node& CurrentNode) -> Node* {
			const CustomNodeStyleDemonstration& NodeToCopy = static_cast<const CustomNodeStyleDemonstration&>(CurrentNode);
			return new CustomNodeStyleDemonstration(NodeToCopy);
		}
	);

	return true;
}();

CustomNodeStyleDemonstration::CustomNodeStyleDemonstration() : Node()
{
	Type = "CustomNodeStyleDemonstration";

	SetStyle(DEFAULT);

	SetSize(ImVec2(160, 130));
	SetName("Style Demonstration");

	TitleBackgroundColor = ImColor(128, 117, 208);
	TitleBackgroundColorHovered = ImColor(135, 145, 255);

	// Socket type can be associated with color system wide.
	NODE_SYSTEM.AssociateSocketTypeToColor("BOOL", ImColor(0,0,225));
	NODE_SYSTEM.AssociateSocketTypeToColor("FLOAT", ImColor(0, 225, 0));
	NODE_SYSTEM.AssociateSocketTypeToColor("MARCHING_ANT", ImColor(0, 225, 225));
	
	AddSocket(new NodeSocket(this, "BOOL", "bool", false));
	AddSocket(new NodeSocket(this, "FLOAT", "float", false));
	AddSocket(new NodeSocket(this, "MARCHING_ANT", "ants", false));
	AddSocket(new NodeSocket(this, "FADE_OUT", "fade", false));

	AddSocket(new NodeSocket(this, "BOOL", "bool", true));
	AddSocket(new NodeSocket(this, "FLOAT", "float", true));
	AddSocket(new NodeSocket(this, "MARCHING_ANT", "ants", true));
	AddSocket(new NodeSocket(this, "FADE_OUT", "fade", true));
}

CustomNodeStyleDemonstration::CustomNodeStyleDemonstration(const CustomNodeStyleDemonstration& Src) : Node(Src)
{
	SetStyle(DEFAULT);

	// Also individual socket connections could have unique styles.
	ConnectionStyle CurrentStyle;
	GetParentArea()->GetConnectionStyle(this, true, 2, CurrentStyle);
	CurrentStyle.bMarchingAntsEffect = true;
	GetParentArea()->SetConnectionStyle(this, true, 2, CurrentStyle);

	GetParentArea()->GetConnectionStyle(this, true, 3, CurrentStyle);
	CurrentStyle.bPulseEffect = true;
	GetParentArea()->SetConnectionStyle(this, true, 3, CurrentStyle);
}

void CustomNodeStyleDemonstration::SetStyle(NODE_STYLE NewValue)
{
	// Do nothing. We don't want to change style
}

void CustomNodeStyleDemonstration::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	if (GetNodesConnectedToOutput().size() > 0)
	{
		ImVec2 NodePosition = ImGui::GetCursorScreenPos();
		ConnectionStyle CurrentStyle;
		GetParentArea()->GetConnectionStyle(this, true, 2, CurrentStyle);

		ImGui::SetCursorScreenPos(NodePosition + ImVec2(70.0f, 70.0f) * Zoom);
		if (ImGui::Checkbox("Reverse \ndirection", &bReverseDirection))
		{
			CurrentStyle.bMarchingAntsReverseDirection = bReverseDirection;
		}

		ImGui::SetCursorScreenPos(NodePosition + ImVec2(70.0f, 100.0f) * Zoom);
		ImGui::Text("Speed:");
		ImGui::SetCursorScreenPos(NodePosition + ImVec2(70.0f, 120.0f) * Zoom);
		ImGui::SetNextItemWidth(70.0f * Zoom);
		ImGui::SliderFloat("##Speed", &CurrentStyle.MarchingAntsSpeed, 0.01f, 10.0f);

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Speed of the marching ants effect");
			ImGui::EndTooltip();
		}

		GetParentArea()->SetConnectionStyle(this, true, 2, CurrentStyle);
	}
}

void CustomNodeStyleDemonstration::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == CONNECTED)
	{
		// Also individual socket connections could have unique styles.
		ConnectionStyle CurrentStyle;
		GetParentArea()->GetConnectionStyle(this, true, 2, CurrentStyle);
		CurrentStyle.bMarchingAntsEffect = true;
		GetParentArea()->SetConnectionStyle(this, true, 2, CurrentStyle);

		GetParentArea()->GetConnectionStyle(this, true, 3, CurrentStyle);
		CurrentStyle.bPulseEffect = true;
		GetParentArea()->SetConnectionStyle(this, true, 3, CurrentStyle);
	}
}

bool CustomNodeStyleDemonstration::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}