#include "BoolVec2VariableNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BoolVec2VariableNode::BoolVec2VariableNode() : BaseExecutionFlowNode()
{
	Type = "BoolVec2VariableNode";

	SetStyle(DEFAULT);
	SetName("Bool Vec2 Variable");

	TitleBackgroundColor = ImColor(125, 62, 11);
	TitleBackgroundColorHovered = ImColor(196, 97, 17);

	AddSocket(new NodeSocket(this, "BVEC2", "Set", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, "BVEC2", "Get", true));

	SetSize(ImVec2(220.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(DataGetter);
}

BoolVec2VariableNode::BoolVec2VariableNode(const BoolVec2VariableNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(DataGetter);
}

Json::Value BoolVec2VariableNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["ValueX"] = Data.x;
	Result["ValueY"] = Data.y;

	return Result;
}

bool BoolVec2VariableNode::FromJson(Json::Value Json)
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
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(DataGetter);

	return true;
}

void BoolVec2VariableNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	bool VectorData[2] = { Data.x, Data.y };
	int VectorDimensions = 2;
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

	if (VectorData[0] != Data.x || VectorData[1] != Data.y)
		SetData(glm::bvec2(VectorData[0], VectorData[1]));
}

void BoolVec2VariableNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Data = *reinterpret_cast<glm::bvec2*>(TemporaryData);
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BoolVec2VariableNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
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

glm::bvec2 BoolVec2VariableNode::GetData() const
{
	return Data;
}

void BoolVec2VariableNode::SetData(glm::bvec2 NewValue)
{
	Data = NewValue;

	if (Output[0]->GetConnectedSockets().size() > 0)
		ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
}