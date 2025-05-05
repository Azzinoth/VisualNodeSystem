#include "BranchNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

BranchNode::BranchNode() : BaseExecutionFlowNode()
{
	Type = "BranchNode";

	SetStyle(DEFAULT);
	SetName("Branch");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "BOOL", "Condition", false));
	
	AddSocket(new NodeSocket(this, "EXECUTE", "True", true));
	AddSocket(new NodeSocket(this, "EXECUTE", "False", true));

	SetSize(ImVec2(190.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
}

BranchNode::BranchNode(const BranchNode& Other) : BaseExecutionFlowNode(Other)
{
	SetStyle(DEFAULT);
}

void BranchNode::Draw()
{	
	Node::Draw();
}

void BranchNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		bool bConditionState = false;

		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				bConditionState = *reinterpret_cast<bool*>(TempData);
		}

		if (bConditionState)
		{
			if (Output[0]->GetConnectedSockets().size() > 0)
				ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
		}
		else
		{
			if (Output[1]->GetConnectedSockets().size() > 0)
				ParentArea->TriggerSocketEvent(Output[1], Output[1]->GetConnectedSockets()[0], EXECUTE);
		}
	}
}

bool BranchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	return true;
}