#include "BoolVariableNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BoolVariableNode::BoolVariableNode() : BaseExecutionFlowNode()
{
	Type = "BoolVariableNode";

	SetStyle(DEFAULT);
	SetName("Bool Variable");

	TitleBackgroundColor = ImColor(148, 0, 0);
	TitleBackgroundColorHovered = ImColor(183, 137, 137);

	AddSocket(new NodeSocket(this, "BOOL", "Set", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, "BOOL", "Get", true));

	SetSize(ImVec2(180.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
}

BoolVariableNode::BoolVariableNode(const BoolVariableNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	bData = Other.bData;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(BoolDataGetter);
}

Json::Value BoolVariableNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Value"] = bData;
	return Result;
}

bool BoolVariableNode::FromJson(Json::Value Json)
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

	Output[1]->SetFunctionToOutputData(BoolDataGetter);

	return true;
}

void BoolVariableNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	float XPosition = ImGui::GetCursorScreenPos().x + 55.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 45.0f * Zoom;

	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::Checkbox("Value", &bData);
}

void BoolVariableNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				bData = *reinterpret_cast<bool*>(TemporaryData);
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BoolVariableNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
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

bool BoolVariableNode::GetData() const
{
	return bData;
}

void BoolVariableNode::SetData(bool NewValue)
{
	bData = NewValue;

	if (Output[0]->GetConnectedSockets().size() > 0)
		ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
}