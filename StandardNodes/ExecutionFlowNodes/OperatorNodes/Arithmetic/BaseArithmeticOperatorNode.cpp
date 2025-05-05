#include "BaseArithmeticOperatorNode.h"
#include "../../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BaseArithmeticOperatorNode::BaseArithmeticOperatorNode(std::vector<std::string> AllowedTypes) : BaseExecutionFlowNode()
{
	Type = "BaseArithmeticOperatorNode";

	SetStyle(DEFAULT);
	SetName("Base Arithmetic Operator");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, AllowedTypes, "A", false));
	AddSocket(new NodeSocket(this, AllowedTypes, "B", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, AllowedTypes, "Result", true));

	Output[1]->SetFunctionToOutputData(ResultDataGetter);
	
	SetSize(ImVec2(130.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
}

BaseArithmeticOperatorNode::BaseArithmeticOperatorNode(const BaseArithmeticOperatorNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);

	OperatorType = Other.OperatorType;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(ResultDataGetter);
}

Json::Value BaseArithmeticOperatorNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	return Result;
}

bool BaseArithmeticOperatorNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ResultDataGetter);

	return true;
}

void BaseArithmeticOperatorNode::Draw()
{	
	Node::Draw();
}

std::string BaseArithmeticOperatorNode::GetActiveINDataType()
{
	if (Input[1]->GetConnectedSockets().empty() && Input[2]->GetConnectedSockets().empty())
		return "";

	if (Input[1]->GetConnectedSockets().empty() && !Input[2]->GetConnectedSockets().empty())
		return Input[2]->GetConnectedSockets()[0]->GetAllowedTypes()[0];

	if (!Input[1]->GetConnectedSockets().empty() && Input[2]->GetConnectedSockets().empty())
		return Input[1]->GetConnectedSockets()[0]->GetAllowedTypes()[0];

	// If we have both A and B inputs connected, we can check their types.
	std::string AInputType = Input[1]->GetConnectedSockets()[0]->GetAllowedTypes()[0];
	std::string BInputType = Input[2]->GetConnectedSockets()[0]->GetAllowedTypes()[0];

	if (AInputType == BInputType)
		return AInputType;

	return "";
}

void BaseArithmeticOperatorNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		Execute();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BaseArithmeticOperatorNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	std::vector<std::string> CandidateAllowedTypes = CandidateSocket->GetAllowedTypes();
	if (CandidateAllowedTypes.size() == 1)
	{
		if (CandidateAllowedTypes[0] != "EXECUTE")
		{
			if (!Input[1]->GetConnectedSockets().empty())
			{
				std::string AInputType = Input[1]->GetConnectedSockets()[0]->GetAllowedTypes()[0];
				if (AInputType != CandidateAllowedTypes[0])
					return false;
			}

			if (!Input[2]->GetConnectedSockets().empty())
			{
				std::string BInputType = Input[2]->GetConnectedSockets()[0]->GetAllowedTypes()[0];
				if (BInputType != CandidateAllowedTypes[0])
					return false;
			}
		}

		return true;
	}

	if (CandidateSocket->GetParent() == nullptr)
		return false;

	BaseArithmeticOperatorNode* CandidateNode = dynamic_cast<BaseArithmeticOperatorNode*>(CandidateSocket->GetParent());
	if (CandidateNode == nullptr)
		return false;
	
	std::string CandidateNodeActiveDataType = CandidateNode->GetActiveINDataType();
	// If node already have data type, check if it is the same as candidate node.
	if (!GetActiveINDataType().empty())
	{
		if (CandidateNodeActiveDataType != GetActiveINDataType())
			return false;
	}
	
	return true;
}