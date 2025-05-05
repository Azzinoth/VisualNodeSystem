#include "SequenceNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

SequenceNode::SequenceNode() : BaseExecutionFlowNode()
{
	Type = "SequenceNode";

	SetStyle(DEFAULT);
	SetName("Sequence");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSequenceOutput();
	AddSequenceOutput();

	SetSize(ImVec2(150.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
}

SequenceNode::SequenceNode(const SequenceNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	OutputCount = Other.OutputCount;
}

void SequenceNode::AddSequenceOutput()
{
	AddSocket(new NodeSocket(this, "EXECUTE", "Then " + std::to_string(Output.size()), true));
	OutputCount++;
}

Json::Value SequenceNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["OutputCount"] = OutputCount;
	return Result;
}

bool SequenceNode::FromJson(Json::Value Json)
{
	bool bPreviousCheck = Node::bInputCountCheck;
	Node::bOutputCountCheck = false;

	bool bResult = Node::FromJson(Json);
	Node::bInputCountCheck = bPreviousCheck;

	if (!bResult)
		return false;

	if (!Json.isMember("OutputCount") || !Json["OutputCount"].isNumeric())
		return false;

	int LocalOutputCount = Json["OutputCount"].asInt();
	OutputCount = LocalOutputCount;
	
	return true;
}

void SequenceNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	ImVec2 ButtonSize = ImVec2(25.0f * Zoom, 25.0f * Zoom);
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 35.0f * Zoom, ImGui::GetCursorScreenPos().y + (Size.y - 20.0f) * Zoom - ButtonSize.y / 2));
	if (ImGui::Button("+", ButtonSize))
		AddSequenceOutput();

	SetSize(ImVec2(Size.x, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
}

void SequenceNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		for (size_t i = 0; i < OutputCount; i++)
		{
			if (Output[i]->GetConnectedSockets().size() > 0)
				ParentArea->TriggerSocketEvent(Output[i], Output[i]->GetConnectedSockets()[0], EXECUTE);
		}
	}
}

bool SequenceNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}