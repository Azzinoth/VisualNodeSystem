#include "BoolVec3VariableNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BoolVec3VariableNode::BoolVec3VariableNode() : BaseExecutionFlowNode()
{
	Type = "BoolVec3VariableNode";

	SetStyle(DEFAULT);
	SetName("Bool Vec3 Variable");

	TitleBackgroundColor = ImColor(125, 62, 11);
	TitleBackgroundColorHovered = ImColor(196, 97, 17);

	AddSocket(new NodeSocket(this, "BVEC3", "Set", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, "BVEC3", "Get", true));

	SetSize(ImVec2(220.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(DataGetter);
}

BoolVec3VariableNode::BoolVec3VariableNode(const BoolVec3VariableNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(DataGetter);
}

Json::Value BoolVec3VariableNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;
	Result["ValueZ"] = Data.z;

	return Result;
}

bool BoolVec3VariableNode::FromJson(Json::Value Json)
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
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(DataGetter);

	return true;
}

void BoolVec3VariableNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	bool VectorData[3] = { Data.x, Data.y };
	int VectorDimensions = 3;
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

	if (VectorData[0] != Data.x || VectorData[1] != Data.y || VectorData[2] != Data.z)
		SetData(glm::bvec3(VectorData[0], VectorData[1], VectorData[2]));
}

void BoolVec3VariableNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Data = *reinterpret_cast<glm::bvec3*>(TemporaryData);
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BoolVec3VariableNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
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

glm::bvec3 BoolVec3VariableNode::GetData() const
{
	return Data;
}

void BoolVec3VariableNode::SetData(glm::bvec3 NewValue)
{
	Data = NewValue;

	if (Output[0]->GetConnectedSockets().size() > 0)
		ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
}