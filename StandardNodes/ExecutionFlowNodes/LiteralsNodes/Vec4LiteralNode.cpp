#include "Vec4LiteralNode.h"
using namespace VisNodeSys;

Vec4LiteralNode::Vec4LiteralNode() : VisNodeSys::Node()
{
	Type = "Vec4LiteralNode";

	SetStyle(DEFAULT);
	SetName("Vec4 Literal");

	TitleBackgroundColor = ImColor(244, 193, 34);
	TitleBackgroundColorHovered = ImColor(255, 217, 140);

	AddSocket(new NodeSocket(this, "VEC4", "Out", true));

	SetSize(ImVec2(210, NODE_WITH_PER_SOCKET * 2));
	Output[0]->SetFunctionToOutputData(Vec4DataGetter);
}

Vec4LiteralNode::Vec4LiteralNode(const Vec4LiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(Vec4DataGetter);
}

Json::Value Vec4LiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;
	Result["ValueZ"] = Data.z;
	Result["ValueW"] = Data.w;

	return Result;
}

bool Vec4LiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ValueX") || !Json.isMember("ValueY") || !Json.isMember("ValueZ") || !Json.isMember("ValueW"))
		return false;

	if (!Json["ValueX"].isNumeric() || !Json["ValueY"].isNumeric() || !Json["ValueZ"].isNumeric() || !Json["ValueW"].isNumeric())
		return false;

	Data.x = Json["ValueX"].asFloat();
	Data.y = Json["ValueY"].asFloat();
	Data.z = Json["ValueZ"].asFloat();
	Data.w = Json["ValueW"].asFloat();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(Vec4DataGetter);

	return true;
}

void Vec4LiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	ImGui::SetNextItemWidth(140 * Zoom);

	static float VectorData[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	VectorData[0] = Data.x;
	VectorData[1] = Data.y;
	VectorData[2] = Data.z;
	VectorData[3] = Data.w;

	if (ImGui::DragFloat4("##Vector", VectorData, 0.1f, -FLT_MAX, FLT_MAX, "%.2f"))
	{
		Data = glm::vec4(VectorData[0], VectorData[1], VectorData[2], VectorData[3]);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

glm::vec4 Vec4LiteralNode::GetData() const
{
	return Data;
}

void Vec4LiteralNode::SetData(glm::vec4 NewValue)
{
	Data = NewValue;
}