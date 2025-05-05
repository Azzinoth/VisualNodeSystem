#include "IntegerLiteralNode.h"
using namespace VisNodeSys;

IntegerLiteralNode::IntegerLiteralNode() : VisNodeSys::Node()
{
	Type = "IntegerLiteralNode";

	SetStyle(DEFAULT);
	SetName("Integer Literal");

	TitleBackgroundColor = ImColor(30, 221, 170);
	TitleBackgroundColorHovered = ImColor(139, 235, 199);

	AddSocket(new NodeSocket(this, "INT", "Out", true));

	SetSize(ImVec2(170, NODE_WITH_PER_SOCKET * 2));
	Output[0]->SetFunctionToOutputData(IntDataGetter);
}

IntegerLiteralNode::IntegerLiteralNode(const IntegerLiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(IntDataGetter);
}

Json::Value IntegerLiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Value"] = Data;
	return Result;
}

bool IntegerLiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Value"))
		return false;

	if (!Json["Value"].isInt())
		return false;

	Data = Json["Value"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(IntDataGetter);

	return true;
}

void IntegerLiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 30.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	float XPosition = ImGui::GetCursorScreenPos().x - 17.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 0.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(100 * Zoom);
	if (ImGui::InputInt("##value", &Data))
	{
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

void IntegerLiteralNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool IntegerLiteralNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

int IntegerLiteralNode::GetData() const
{
	return Data;
}

void IntegerLiteralNode::SetData(int NewData)
{
	Data = NewData;
}