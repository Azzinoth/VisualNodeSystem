#include "BoolVec2LiteralNode.h"
using namespace VisNodeSys;

BoolVec2LiteralNode::BoolVec2LiteralNode() : VisNodeSys::Node()
{
	Type = "BoolVec2LiteralNode";

	SetStyle(DEFAULT);
	SetName("Bool Vec2 Literal");

	TitleBackgroundColor = ImColor(125, 62, 11);
	TitleBackgroundColorHovered = ImColor(196, 97, 17);

	AddSocket(new NodeSocket(this, "BVEC2", "Out", true));

	SetSize(ImVec2(210, NODE_WITH_PER_SOCKET * 2));
	Output[0]->SetFunctionToOutputData(DataGetter);
}

BoolVec2LiteralNode::BoolVec2LiteralNode(const BoolVec2LiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(DataGetter);
}

Json::Value BoolVec2LiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;

	return Result;
}

bool BoolVec2LiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ValueX") || !Json.isMember("ValueY"))
		return false;

	if (!Json["ValueX"].isBool() || !Json["ValueY"].isBool())
		return false;

	Data.x = Json["ValueX"].asBool();
	Data.y = Json["ValueY"].asBool();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(DataGetter);

	return true;
}

void BoolVec2LiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	bool VectorData[2] = { Data.x, Data.y };
	int VectorDimensions = 2;
	float ChekboxWidth = 18.0f * Zoom;
	float Padding = 45.0f * Zoom;
	float AvailableWidth = 210.0f * Zoom - Padding;
	float SectorPercent = 1.0f / float(VectorDimensions);

	float XPosition = ImGui::GetCursorScreenPos().x;
	float YPosition = ImGui::GetCursorScreenPos().y + 45.0f * Zoom;

	XPosition += AvailableWidth * SectorPercent / 2.0f - ChekboxWidth / 2.0f;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::Checkbox("##X", &VectorData[0]);

	XPosition += AvailableWidth * SectorPercent;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::Checkbox("##Y", &VectorData[1]);

	if (VectorData[0] != Data.x || VectorData[1] != Data.y)
		SetData(glm::bvec2(VectorData[0], VectorData[1]));
}

glm::bvec2 BoolVec2LiteralNode::GetData() const
{
	return Data;
}

void BoolVec2LiteralNode::SetData(glm::bvec2 NewValue)
{
	Data = NewValue;
}