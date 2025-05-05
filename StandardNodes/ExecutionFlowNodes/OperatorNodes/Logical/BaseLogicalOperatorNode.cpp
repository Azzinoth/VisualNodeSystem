#include "BaseLogicalOperatorNode.h"
#include "../../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BaseLogicalOperatorNode::BaseLogicalOperatorNode(bool bNeedBInput) : BaseExecutionFlowNode()
{
	Type = "BaseLogicalOperatorNode";

	SetStyle(DEFAULT);
	SetName("Base Logical Operator");

	TitleBackgroundColor = ImColor(44, 29, 51);
	TitleBackgroundColorHovered = ImColor(90, 59, 104);

	AddSocket(new NodeSocket(this, "BOOL", "A", false));
	if (bNeedBInput)
		AddSocket(new NodeSocket(this, "BOOL", "B", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, "BOOL", "Result", true));

	SetSize(ImVec2(130.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(DataGetter);
}

BaseLogicalOperatorNode::BaseLogicalOperatorNode(const BaseLogicalOperatorNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);

	OperatorType = Other.OperatorType;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(DataGetter);
}

Json::Value BaseLogicalOperatorNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	return Result;
}

bool BaseLogicalOperatorNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(DataGetter);

	return true;
}

void BaseLogicalOperatorNode::Draw()
{	
	Node::Draw();
}

void BaseLogicalOperatorNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		Execute();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BaseLogicalOperatorNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}