#include "FloatLiteralNode.h"
using namespace VisNodeSys;

FloatLiteralNode::FloatLiteralNode() : VisNodeSys::Node()
{
	Type = "FloatLiteralNode";

	SetStyle(DEFAULT);
	SetName("Float Literal");

	TitleBackgroundColor = ImColor(56, 210, 0);
	TitleBackgroundColorHovered = ImColor(144, 225, 137);

	AddSocket(new NodeSocket(this, "FLOAT", "Out", true));

	SetSize(ImVec2(170, NODE_WITH_PER_SOCKET * 2));
	Output[0]->SetFunctionToOutputData(FloatDataGetter);
}

FloatLiteralNode::FloatLiteralNode(const FloatLiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(FloatDataGetter);
}

Json::Value FloatLiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Value"] = Data;
	return Result;
}

bool FloatLiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Value"))
		return false;

	Data = Json["Value"].asFloat();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(FloatDataGetter);

	return true;
}

void FloatLiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	float XPosition = ImGui::GetCursorScreenPos().x - 17.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 0.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(100.0f * Zoom);
	if (ImGui::InputFloat("##value", &Data))
	{
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

void FloatLiteralNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool FloatLiteralNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

float FloatLiteralNode::GetData() const
{
	return Data;
}

void FloatLiteralNode::SetData(float NewValue)
{
	Data = NewValue;
}