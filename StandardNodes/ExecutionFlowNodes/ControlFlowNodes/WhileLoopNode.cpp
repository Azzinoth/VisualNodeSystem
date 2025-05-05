#include "WhileLoopNode.h"
#include "../../../SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

WhileLoopNode::WhileLoopNode() : BaseExecutionFlowNode()
{
	Type = "WhileLoopNode";

	SetStyle(DEFAULT);
	SetName("While Loop");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	AddSocket(new NodeSocket(this, "BOOL", "Condition", false));

	AddSocket(new NodeSocket(this, "EXECUTE", "Loop Body", true));
	AddSocket(new NodeSocket(this, "EXECUTE", "Completed", true));
	
	SetSize(ImVec2(220.0f, static_cast<float>(NODE_WITH_PER_SOCKET * std::max(Input.size(), Output.size()))));
}

void WhileLoopNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket,  ConnectedSocket, EventType);

	if (EventType == EXECUTE)
	{
		bool bCurrentConditionState = false;
		if (Input[1]->GetConnectedSockets().size() > 0)
		{
			void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
			if (TempData != nullptr)
				bCurrentConditionState = *reinterpret_cast<bool*>(TempData);
		}

		while (bCurrentConditionState)
		{
			if (Output[0]->GetConnectedSockets().size() > 0)
			{
				for (size_t j = 0; j < Output[0]->GetConnectedSockets().size(); j++)
				{
					ParentArea->TriggerSocketEvent(Output[0], Output[0]->GetConnectedSockets()[j], EXECUTE);
				}
			}

			if (Input[1]->GetConnectedSockets().size() > 0)
			{
				void* TempData = Input[1]->GetConnectedSockets()[0]->GetData();
				if (TempData != nullptr)
					bCurrentConditionState = *reinterpret_cast<bool*>(TempData);
			}
			else
			{
				bCurrentConditionState = false;
			}
		}

		// After the while loop is done, we will trigger completed socket.
		if (Output[1]->GetConnectedSockets().size() > 0)
		{
			if (Output[1]->GetConnectedSockets().size() > 0)
			{
				for (size_t j = 0; j < Output[1]->GetConnectedSockets().size(); j++)
				{
					ParentArea->TriggerSocketEvent(Output[1], Output[1]->GetConnectedSockets()[j], EXECUTE);
				}
			}
		}
	}
}