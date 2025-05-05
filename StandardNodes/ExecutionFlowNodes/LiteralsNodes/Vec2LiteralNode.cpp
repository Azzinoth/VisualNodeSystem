#include "Vec2LiteralNode.h"
using namespace VisNodeSys;

Vec2LiteralNode::Vec2LiteralNode() : VisNodeSys::Node()
{
	Type = "Vec2LiteralNode";

	SetStyle(DEFAULT);
	SetName("Vec2 Literal");

	TitleBackgroundColor = ImColor(244, 193, 34);
	TitleBackgroundColorHovered = ImColor(255, 217, 140);

	AddSocket(new NodeSocket(this, "VEC2", "Out", true));

	SetSize(ImVec2(210, NODE_WITH_PER_SOCKET * 2));
	Output[0]->SetFunctionToOutputData(Vec2DataGetter);
}

Vec2LiteralNode::Vec2LiteralNode(const Vec2LiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(Vec2DataGetter);
}

Json::Value Vec2LiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;

	return Result;
}

bool Vec2LiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ValueX") || !Json.isMember("ValueY"))
		return false;

	if (!Json["ValueX"].isNumeric() || !Json["ValueY"].isNumeric())
		return false;

	Data.x = Json["ValueX"].asFloat();
	Data.y = Json["ValueY"].asFloat();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(Vec2DataGetter);

	return true;
}

void Vec2LiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	ImGui::SetNextItemWidth(140 * Zoom);
	static float VectorData[] = { 0.0f, 0.0f };
	VectorData[0] = Data.x;
	VectorData[1] = Data.y;

	if (ImGui::DragFloat2("##Vector", VectorData, 0.1f, -FLT_MAX, FLT_MAX, "%.2f"))
	{
		Data = glm::vec2(VectorData[0], VectorData[1]);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

glm::vec2 Vec2LiteralNode::GetData() const
{
	return Data;
}

void Vec2LiteralNode::SetData(glm::vec2 NewValue)
{
	Data = NewValue;
}