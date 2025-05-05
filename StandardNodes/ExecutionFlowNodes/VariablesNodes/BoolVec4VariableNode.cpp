#include "BoolVec4VariableNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BoolVec4VariableNode::BoolVec4VariableNode() : BaseExecutionFlowNode()
{
	Type = "BoolVec4VariableNode";

	SetStyle(DEFAULT);
	SetName("Bool Vec4 Variable");

	TitleBackgroundColor = ImColor(125, 62, 11);
	TitleBackgroundColorHovered = ImColor(196, 97, 17);

	AddSocket(new NodeSocket(this, "BVEC4", "Set", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, "BVEC4", "Get", true));

	SetSize(ImVec2(220.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(DataGetter);
}

BoolVec4VariableNode::BoolVec4VariableNode(const BoolVec4VariableNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(DataGetter);
}

Json::Value BoolVec4VariableNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;
	Result["ValueZ"] = Data.z;
	Result["ValueW"] = Data.w;

	return Result;
}

bool BoolVec4VariableNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ValueX") || !Json.isMember("ValueY") || !Json.isMember("ValueZ") || !Json.isMember("ValueW"))
		return false;

	if (!Json["ValueX"].isBool() || !Json["ValueY"].isBool() || !Json["ValueZ"].isBool() || !Json["ValueW"].isBool())
		return false;

	Data.x = Json["ValueX"].asBool();
	Data.y = Json["ValueY"].asBool();
	Data.z = Json["ValueZ"].asBool();
	Data.w = Json["ValueW"].asBool();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(DataGetter);

	return true;
}

void BoolVec4VariableNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	bool VectorData[4] = { Data.x, Data.y, Data.z, Data.w };
	int VectorDimensions = 4;
	float ChekboxWidth = 18.0f * Zoom;
	float Padding = 30.0f * Zoom;
	float AvailableWidth = 220.0f * Zoom - Padding * 2;
	float SectorPercent = 1.0f / float(VectorDimensions);

	float XPosition = ImGui::GetCursorScreenPos().x + Padding;
	float YPosition = ImGui::GetCursorScreenPos().y + 35.0f * Zoom;

	XPosition += AvailableWidth * SectorPercent / 2.0f - ChekboxWidth / 2.0f;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::Checkbox("##X", &VectorData[0]);

	XPosition += AvailableWidth * SectorPercent;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::Checkbox("##Y", &VectorData[1]);

	XPosition += AvailableWidth * SectorPercent;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::Checkbox("##Z", &VectorData[2]);

	XPosition += AvailableWidth * SectorPercent;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::Checkbox("##W", &VectorData[3]);

	if (VectorData[0] != Data.x || VectorData[1] != Data.y || VectorData[2] != Data.z || VectorData[3] != Data.w)
		SetData(glm::bvec4(VectorData[0], VectorData[1], VectorData[2], VectorData[3]));
}

void BoolVec4VariableNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Data = *reinterpret_cast<glm::bvec4*>(TemporaryData);
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BoolVec4VariableNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	std::vector<std::string> CandidateAllowedTypes = CandidateSocket->GetAllowedTypes();
	if (CandidateAllowedTypes.size() == 1)
		return true;

	if (CandidateSocket->GetParent() == nullptr)
		return false;

	BaseComparisonOperatorNode* CandidateNode = dynamic_cast<BaseComparisonOperatorNode*>(CandidateSocket->GetParent());
	if (CandidateNode == nullptr)
		return false;

	std::string CandidateNodeActiveDataType = CandidateNode->GetActiveOUTDataType();
	if (CandidateNodeActiveDataType != OwnSocket->GetAllowedTypes()[0])
		return false;

	return true;
}

glm::bvec4 BoolVec4VariableNode::GetData() const
{
	return Data;
}

void BoolVec4VariableNode::SetData(glm::bvec4 NewValue)
{
	Data = NewValue;

	if (Output[0]->GetConnectedSockets().size() > 0)
		ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
}