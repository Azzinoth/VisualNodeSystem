#include "FloatVariableNode.h"
#include "../OperatorNodes/Arithmetic/BaseArithmeticOperatorNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

FloatVariableNode::FloatVariableNode() : BaseExecutionFlowNode()
{
	Type = "FloatVariableNode";

	SetStyle(DEFAULT);
	SetName("Float Variable");

	TitleBackgroundColor = ImColor(56, 210, 0);
	TitleBackgroundColorHovered = ImColor(144, 225, 137);

	AddSocket(new NodeSocket(this, "FLOAT", "Set", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, "FLOAT", "Get", true));

	SetSize(ImVec2(220.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(FloatDataGetter);
}

FloatVariableNode::FloatVariableNode(const FloatVariableNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	Data = Other.Data;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(FloatDataGetter);
}

Json::Value FloatVariableNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["Value"] = Data;
	return Result;
}

bool FloatVariableNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("Value"))
		return false;

	if (!Json["Value"].isNumeric())
		return false;

	Data = Json["Value"].asFloat();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 1)
		return false;

	if (Output[0] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(FloatDataGetter);

	return true;
}

void FloatVariableNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	float XPosition = ImGui::GetCursorScreenPos().x + 55.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 45.0f * Zoom;
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(100 * Zoom);
	if (ImGui::InputFloat("##value", &Data))
	{
		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
	}
}

void FloatVariableNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TemporaryData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TemporaryData != nullptr)
				Data = *reinterpret_cast<float*>(TemporaryData);
		}

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool FloatVariableNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
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

float FloatVariableNode::GetData() const
{
	return Data;
}

void FloatVariableNode::SetData(float NewValue)
{
	Data = NewValue;

	if (Output[0]->GetConnectedSockets().size() > 0)
		ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], UPDATE);
}