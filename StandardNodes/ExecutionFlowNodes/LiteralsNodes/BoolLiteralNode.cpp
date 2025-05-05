#include "BoolLiteralNode.h"
using namespace VisNodeSys;

BoolLiteralNode::BoolLiteralNode() : VisNodeSys::Node()
{
	Type = "BoolLiteralNode";

	SetStyle(DEFAULT);
	SetName("Bool Literal");

	TitleBackgroundColor = ImColor(148, 0, 0);
	TitleBackgroundColorHovered = ImColor(183, 137, 137);

	AddSocket(new NodeSocket(this, "BOOL", "Out", true));

	SetSize(ImVec2(150, NODE_WITH_PER_SOCKET * 2));
	Output[0]->SetFunctionToOutputData(BoolDataGetter);
}

BoolLiteralNode::BoolLiteralNode(const BoolLiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	bData = Other.bData;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(BoolDataGetter);
}

Json::Value BoolLiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Value"] = bData;
	return Result;
}

bool BoolLiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Value"))
		return false;

	if (!Json["Value"].isBool())
		return false;

	bData = Json["Value"].asBool();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(BoolDataGetter);

	return true;
}

void BoolLiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));
	ImGui::Checkbox("Value", &bData);
}

void BoolLiteralNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);
}

bool BoolLiteralNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}

bool BoolLiteralNode::GetData() const
{
	return bData;
}

void BoolLiteralNode::SetData(bool NewValue)
{
	bData = NewValue;
}