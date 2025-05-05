#include "LoopNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

LoopNode::LoopNode() : BaseExecutionFlowNode()
{
	Type = "LoopNode";

	SetStyle(DEFAULT);
	SetName("For Loop");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "INT", "Start Index", false));
	AddSocket(new NodeSocket(this, "INT", "Last Index", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "Loop Body", true));
	AddSocket(new NodeSocket(this, "INT", "Index", true));
	AddSocket(new NodeSocket(this, "EXECUTE", "Completed", true));

	SetSize(ImVec2(270.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
	Output[1]->SetFunctionToOutputData(CurrentIndexGetter);
}

LoopNode::LoopNode(const LoopNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
	
	LocalFirstIndex = Other.LocalFirstIndex;
	LocalLastIndex = Other.LocalLastIndex;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	Output[1]->SetFunctionToOutputData(CurrentIndexGetter);
}

Json::Value LoopNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["LocalFirstIndex"] = LocalFirstIndex;
	Result["LocalLastIndex"] = LocalLastIndex;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 3)
		return false;

	if (Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(CurrentIndexGetter);

	return Result;
}

bool LoopNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("LocalFirstIndex") || !Json["LocalFirstIndex"].isInt() ||
		!Json.isMember("LocalLastIndex") || !Json["LocalLastIndex"].isInt())
		return false;

	LocalFirstIndex = Json["LocalFirstIndex"].asInt();
	LocalLastIndex = Json["LocalLastIndex"].asInt();

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	if (Output.size() < 3)
		return false;

	if (Output[1] == nullptr)
		return false;

	Output[1]->SetFunctionToOutputData(CurrentIndexGetter);

	return true;
}

void LoopNode::Draw()
{	
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	float XPosition = ImGui::GetCursorScreenPos().x + 122.0f * Zoom;
	float YPosition = ImGui::GetCursorScreenPos().y + 65.0f * Zoom;

	ImGui::BeginDisabled(Input[1]->GetConnectedSockets().size() != 0);
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	ImGui::DragInt("##LocalFirstIndex", &LocalFirstIndex);
	ImGui::EndDisabled();

	YPosition += 30 * Zoom;
	ImGui::BeginDisabled(Input[2]->GetConnectedSockets().size() != 0);
	ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
	ImGui::SetNextItemWidth(35.0f * Zoom);
	ImGui::DragInt("##LocalLastIndex", &LocalLastIndex);
	ImGui::EndDisabled();
}

void LoopNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		CurrentIndex = 0;

		// We need to get first and last index.
		// First we will check if we can get it from connections.
		// If we have no connections, we will use values that was set by int input by user.
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				LocalFirstIndex = *reinterpret_cast<int*>(TempData);
		}

		if (Input[2]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[2]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				LocalLastIndex = *reinterpret_cast<int*>(TempData);
		}

		for (int i = LocalFirstIndex; i < LocalLastIndex; i++)
		{
			CurrentIndex = i;

			if (Output[0]->GetConnectedSockets().size() > 0)
			{
				for (size_t j = 0; j < Output[0]->GetConnectedSockets().size(); j++)
				{
					ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[j], EXECUTE);
				}
			}
		}

		// After the loop is done, we will trigger completed socket.
		if (Output[2]->GetConnectedSockets().size() > 0)
		{
			if (Output[2]->GetConnectedSockets().size() > 0)
			{
				for (size_t j = 0; j < Output[2]->GetConnectedSockets().size(); j++)
				{
					ParentArea->TriggerSocketEvent(Output[2], Output[2]->GetConnectedSockets()[j], EXECUTE);
				}
			}
		}
	}
}

bool LoopNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}