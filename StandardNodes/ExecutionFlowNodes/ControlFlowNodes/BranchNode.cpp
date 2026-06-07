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

	AddSocket(new NodeSocket(this, "BOOL", "Condition", NodeSocket::SocketFlow::Input));
	
	AddSocket(new NodeSocket(this, "EXECUTE", "True", NodeSocket::SocketFlow::Output));
	AddSocket(new NodeSocket(this, "EXECUTE", "False", NodeSocket::SocketFlow::Output));

	SetSize(ImVec2(190.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * std::max(Input.size(), Output.size()))));

	// All sockets are structural and must not be user-deletable.
	for (NodeSocket* Socket : Input)
		Socket->SetCanBeDeletedByUser(false);
	for (NodeSocket* Socket : Output)
		Socket->SetCanBeDeletedByUser(false);
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

		if (Input.size() > 1 && Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				bConditionState = *reinterpret_cast<bool*>(TempData);
		}

		if (bConditionState)
		{
			if (Output.size() > 0 && Output[0]->GetConnectedSockets().size() > 0)
				ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[0], EXECUTE);
		}
		else
		{
			if (Output.size() > 1 && Output[1]->GetConnectedSockets().size() > 0)
				ParentArea->TriggerSocketEvent(Output[1], Output[1]->GetConnectedSockets()[0], EXECUTE);
		}
	}
}

bool BranchNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
		return false;

	std::vector<std::string> CandidateAllowedTypes = CandidateSocket->GetAllowedTypes();
	if (CandidateAllowedTypes.size() == 1)
		return true;

	if (CandidateSocket->GetParent() == nullptr)
		return false;

	// A multi-type comparison Result is only valid on the bool Condition input while it is in BOOL mode.
	BaseComparisonOperatorNode* CandidateNode = dynamic_cast<BaseComparisonOperatorNode*>(CandidateSocket->GetParent());
	if (CandidateNode == nullptr)
		return false;

	if (CandidateNode->GetActiveOUTDataType() != OwnSocket->GetAllowedTypes()[0])
		return false;

	return true;
}