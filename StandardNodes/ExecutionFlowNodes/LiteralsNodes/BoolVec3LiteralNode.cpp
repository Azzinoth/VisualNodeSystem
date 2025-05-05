#include "BoolVec3LiteralNode.h"
using namespace VisNodeSys;

BoolVec3LiteralNode::BoolVec3LiteralNode() : VisNodeSys::Node()
{
	Type = "BoolVec3LiteralNode";

	SetStyle(DEFAULT);
	SetName("Bool Vec3 Literal");

	TitleBackgroundColor = ImColor(125, 62, 11);
	TitleBackgroundColorHovered = ImColor(196, 97, 17);

	AddSocket(new NodeSocket(this, "BVEC3", "Out", true));

	SetSize(ImVec2(210, NODE_WITH_PER_SOCKET * 2));
	Output[0]->SetFunctionToOutputData(DataGetter);
}

BoolVec3LiteralNode::BoolVec3LiteralNode(const BoolVec3LiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(DataGetter);
}

Json::Value BoolVec3LiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;
	Result["ValueZ"] = Data.z;

	return Result;
}

bool BoolVec3LiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ValueX") || !Json.isMember("ValueY") || !Json.isMember("ValueZ"))
		return false;

	if (!Json["ValueX"].isBool() || !Json["ValueY"].isBool() || !Json["ValueZ"].isBool())
		return false;

	Data.x = Json["ValueX"].asBool();
	Data.y = Json["ValueY"].asBool();
	Data.z = Json["ValueZ"].asBool();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(DataGetter);

	return true;
}

void BoolVec3LiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	bool VectorData[3] = { Data.x, Data.y, Data.z };
	int VectorDimensions = 3;
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

	XPosition += AvailableWidth * SectorPercent;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::Checkbox("##Z", &VectorData[2]);

	if (VectorData[0] != Data.x || VectorData[1] != Data.y || VectorData[2] != Data.z)
		SetData(glm::bvec3(VectorData[0], VectorData[1], VectorData[2]));
}

glm::bvec3 BoolVec3LiteralNode::GetData() const
{
	return Data;
}

void BoolVec3LiteralNode::SetData(glm::bvec3 NewValue)
{
	Data = NewValue;
}