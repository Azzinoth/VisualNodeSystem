#include "Vec3LiteralNode.h"
using namespace VisNodeSys;

Vec3LiteralNode::Vec3LiteralNode() : VisNodeSys::Node()
{
	Type = "Vec3LiteralNode";

	SetStyle(DEFAULT);
	SetName("Vec3 Literal");

	TitleBackgroundColor = ImColor(244, 193, 34);
	TitleBackgroundColorHovered = ImColor(255, 217, 140);

	AddSocket(new NodeSocket(this, "VEC3", "Out", true));

	SetSize(ImVec2(210, NODE_WITH_PER_SOCKET * 2));
	Output[0]->SetFunctionToOutputData(Vec3DataGetter);
}

Vec3LiteralNode::Vec3LiteralNode(const Vec3LiteralNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[0]->SetFunctionToOutputData(Vec3DataGetter);
}

Json::Value Vec3LiteralNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;
	Result["ValueZ"] = Data.z;

	return Result;
}

bool Vec3LiteralNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ValueX") || !Json.isMember("ValueY")|| !Json.isMember("ValueZ"))
		return false;

	if (!Json["ValueX"].isNumeric() || !Json["ValueY"].isNumeric() || !Json["ValueZ"].isNumeric())
		return false;

	Data.x = Json["ValueX"].asFloat();
	Data.y = Json["ValueY"].asFloat();
	Data.z = Json["ValueZ"].asFloat();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1 || Output[0] == nullptr)
		return false;

	Output[0]->SetFunctionToOutputData(Vec3DataGetter);

	return true;
}

void Vec3LiteralNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));

	ImGui::SetNextItemWidth(140 * Zoom);
	static float VectorData[] = { 0.0f, 0.0f, 0.0f };
	VectorData[0] = Data.x;
	VectorData[1] = Data.y;
	VectorData[2] = Data.z;

	if (ImGui::DragFloat3("##Vector", VectorData, 0.1f, -FLT_MAX, FLT_MAX, "%.2f"))
	{
		Data = glm::vec3(VectorData[0], VectorData[1], VectorData[2]);

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

glm::vec3 Vec3LiteralNode::GetData() const
{
	return Data;
}

void Vec3LiteralNode::SetData(glm::vec3 NewValue)
{
	Data = NewValue;
}