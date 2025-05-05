#include "Vec4VariableNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
#include "../OperatorNodes/Arithmetic/BaseArithmeticOperatorNode.h"
using namespace VisNodeSys;

Vec4VariableNode::Vec4VariableNode() : BaseExecutionFlowNode()
{
	Type = "Vec4VariableNode";

	SetStyle(DEFAULT);
	SetName("Vec4 Variable");

	TitleBackgroundColor = ImColor(244, 193, 34);
	TitleBackgroundColorHovered = ImColor(255, 217, 140);

	AddSocket(new NodeSocket(this, "VEC4", "Set", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, "VEC4", "Get", true));

	SetSize(ImVec2(220.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(DataGetter);
}

Vec4VariableNode::Vec4VariableNode(const Vec4VariableNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(DataGetter);
}

Json::Value Vec4VariableNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;
	Result["ValueZ"] = Data.z;
	Result["ValueW"] = Data.w;

	return Result;
}

bool Vec4VariableNode::FromJson(Json::Value Json)
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
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(DataGetter);

	return true;
}

void Vec4VariableNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 27.0f * Zoom, ImGui::GetCursorScreenPos().y + 35.0f * Zoom));

	ImGui::SetNextItemWidth(165 * Zoom);
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

void Vec4VariableNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Data = *reinterpret_cast<glm::vec4*>(TemporaryData);
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool Vec4VariableNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	std::vector<std::string> CandidateAllowedTypes = CandidateSocket->GetAllowedTypes();
	if (CandidateAllowedTypes.size() == 1)
		return true;

	// Check if the candidate socket node is a child of BaseArithmeticOperatorNode.
	if (CandidateSocket->GetParent() == nullptr)
		return false;

	BaseArithmeticOperatorNode* CandidateNode = dynamic_cast<BaseArithmeticOperatorNode*>(CandidateSocket->GetParent());
	if (CandidateNode == nullptr)
		return false;

	std::string CandidateNodeActiveDataType = CandidateNode->GetActiveINDataType();
	if (CandidateNodeActiveDataType != OwnSocket->GetAllowedTypes()[0])
		return false;

	return true;
}

glm::vec4 Vec4VariableNode::GetData() const
{
	return Data;
}

void Vec4VariableNode::SetData(glm::vec4 NewValue)
{
	Data = NewValue;

	if (Output[0]->GetConnectedSockets().size() > 0)
		ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
}