#include "BaseComparisonOperatorNode.h"
#include "../../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BaseComparisonOperatorNode::BaseComparisonOperatorNode() : BaseExecutionFlowNode()
{
	Type = "BaseComparisonOperatorNode";

	SetStyle(DEFAULT);

	SetSize(ImVec2(130, 78));
	SetName("Compare");

	TitleBackgroundColor = ImColor(44, 29, 51);
	TitleBackgroundColorHovered = ImColor(90, 59, 104);

	std::vector<std::string> AllowedInTypes;
	AllowedInTypes.push_back("BOOL");
	AllowedInTypes.push_back("INT");
	AllowedInTypes.push_back("FLOAT");
	AllowedInTypes.push_back("VEC2");
	AllowedInTypes.push_back("VEC3");
	AllowedInTypes.push_back("VEC4");

	std::vector<std::string> AllowedOutTypes;
	AllowedOutTypes.push_back("BOOL");
	AllowedOutTypes.push_back("BVEC2");
	AllowedOutTypes.push_back("BVEC3");
	AllowedOutTypes.push_back("BVEC4");

	AddSocket(new NodeSocket(this, AllowedInTypes, "A", false));
	AddSocket(new NodeSocket(this, AllowedInTypes, "B", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "", true));
	AddSocket(new NodeSocket(this, AllowedOutTypes, "Out", true));

	Output[1]->SetFunctionToOutputData(ComparisonGetter);
}

BaseComparisonOperatorNode::BaseComparisonOperatorNode(const BaseComparisonOperatorNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);

	OperatorType = Other.OperatorType;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(ComparisonGetter);
}

Json::Value BaseComparisonOperatorNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	return Result;
}

bool BaseComparisonOperatorNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 2 || Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(ComparisonGetter);

	return true;
}

void BaseComparisonOperatorNode::Draw()
{	
	Node::Draw();

	std::string Text = "";
	switch (OperatorType)
	{
		case ComparisonNodeOperatorType::EQUAL:
		{
			Text = "==";
			break;
		}
		case ComparisonNodeOperatorType::NOT_EQUAL:
		{
			Text = "!=";
			break;
		}
		case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
		{
			Text = ">=";
			break;
		}
		case ComparisonNodeOperatorType::GREATER_THAN:
		{
			Text = ">";
			break;
		}
		case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
		{
			Text = "<=";
			break;
		}
		case ComparisonNodeOperatorType::LESS_THAN:
		{
			Text = "<";
			break;
		}
		default:
		{
			Text = "==";
			break;
		}
	}

	float Zoom = ParentArea->GetZoomFactor();

	ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());
	float CaptionHeight = NODE_TITLE_HEIGHT * Zoom;
	ImVec2 TextPosition = ImVec2(ImGui::GetCursorScreenPos().x + (GetSize().x * Zoom - TextSize.x) / 2, ImGui::GetCursorScreenPos().y + CaptionHeight + (GetSize().y * Zoom - CaptionHeight - TextSize.y) / 2);
	ImGui::SetCursorScreenPos(TextPosition);
	ImGui::Text(Text.c_str());
}

std::string BaseComparisonOperatorNode::GetActiveOUTDataType()
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
	{
		if (AInputType == "VEC2")
			return "BVEC2";

		if (AInputType == "VEC3")
			return "BVEC3";

		if (AInputType == "VEC4")
			return "BVEC4";
		
		return "BOOL";
	}

	return "";
}

std::string BaseComparisonOperatorNode::GetActiveINDataType()
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

void BaseComparisonOperatorNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		Execute();

		if (Output[0]->GetConnectedSockets().size() > 0)
			ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
	}
}

bool BaseComparisonOperatorNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
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

	return true;
}