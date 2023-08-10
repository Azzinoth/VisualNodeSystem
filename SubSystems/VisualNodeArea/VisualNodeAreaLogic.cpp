#include "VisualNodeArea.h"

void VisualNodeArea::AddNode(VisualNode* NewNode)
{
	if (NewNode == nullptr)
		return;

	NewNode->ParentArea = this;
	Nodes.push_back(NewNode);
}

std::vector<VisualNodeConnection*> VisualNodeArea::GetAllConnections(const NodeSocket* Socket) const
{
	std::vector<VisualNodeConnection*> result;

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i]->In == Socket || Connections[i]->Out == Socket)
			result.push_back(Connections[i]);
	}

	return result;
}

void VisualNodeArea::Disconnect(VisualNodeConnection*& Connection)
{
	if (HoveredConnection == Connection)
		HoveredConnection = nullptr;

	for (size_t i = 0; i < SelectedConnections.size(); i++)
	{
		if (SelectedConnections[i] == Connection)
		{
			SelectedConnections.erase(SelectedConnections.begin() + i, SelectedConnections.begin() + i + 1);
			break;
		}
	}

	for (size_t i = 0; i < Connection->RerouteConnections.size(); i++)
		Delete(Connection->RerouteConnections[i]);

	for (int i = 0; i < static_cast<int>(Connection->In->SocketConnected.size()); i++)
	{
		if (Connection->In->SocketConnected[i] == Connection->Out)
		{
			VisualNode* parent = Connection->In->SocketConnected[i]->Parent;
			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, VISUAL_NODE_BEFORE_DISCONNECTED);

			Connection->In->SocketConnected.erase(Connection->In->SocketConnected.begin() + i, Connection->In->SocketConnected.begin() + i + 1);
			Connection->In->Parent->SocketEvent(Connection->In, Connection->Out, bClearing ? VISUAL_NODE_SOCKET_DESTRUCTION : VISUAL_NODE_SOCKET_DISCONNECTED);
			i--;

			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, VISUAL_NODE_AFTER_DISCONNECTED);
		}
	}

	for (int i = 0; i < static_cast<int>(Connection->Out->SocketConnected.size()); i++)
	{
		if (Connection->Out->SocketConnected[i] == Connection->In)
		{
			VisualNode* parent = Connection->Out->SocketConnected[i]->Parent;
			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, VISUAL_NODE_BEFORE_DISCONNECTED);

			Connection->Out->SocketConnected.erase(Connection->Out->SocketConnected.begin() + i, Connection->Out->SocketConnected.begin() + i + 1);
			i--;

			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, VISUAL_NODE_AFTER_DISCONNECTED);
		}
	}

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i] == Connection)
		{
			Connections.erase(Connections.begin() + i, Connections.begin() + i + 1);
			delete Connection;
			Connection = nullptr;
			return;
		}
	}
}

void VisualNodeArea::Delete(VisualNodeRerouteNode*& RerouteNode)
{
	if (RerouteNodeHovered == RerouteNode)
		RerouteNodeHovered = nullptr;

	for (size_t i = 0; i < SelectedRerouteNodes.size(); i++)
	{
		if (SelectedRerouteNodes[i] == RerouteNode)
		{
			SelectedRerouteNodes.erase(SelectedRerouteNodes.begin() + i, SelectedRerouteNodes.begin() + i + 1);
			break;
		}
	}

	size_t IndexOfRerouteNode = 0;
	for (size_t i = 0; i < RerouteNode->Parent->RerouteConnections.size(); i++)
	{
		if (RerouteNode->Parent->RerouteConnections[i] == RerouteNode)
		{
			IndexOfRerouteNode = i;
			break;
		}
	}

	if (RerouteNode->Parent->RerouteConnections.size() > 1)
	{
		// If the reroute node is the first one, we need to update the begin socket of the next one
		if (RerouteNode->Parent->RerouteConnections[0] == RerouteNode)
		{
			RerouteNode->Parent->RerouteConnections[1]->BeginSocket = RerouteNode->Parent->RerouteConnections[0]->BeginSocket;
			RerouteNode->Parent->RerouteConnections[1]->BeginReroute = nullptr;
		}
		// If the reroute node is the last one, we need to update the end socket of the previous one
		else if (RerouteNode->Parent->RerouteConnections.back() == RerouteNode)
		{
			RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode - 1]->EndSocket = RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode]->EndSocket;
			RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode - 1]->EndReroute = nullptr;
		}
		// If the reroute node is in the middle, we need to update the previous and next one
		else
		{
			RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode - 1]->EndReroute = RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode + 1];
			RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode - 1]->EndSocket = nullptr;

			RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode + 1]->BeginReroute = RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode - 1];
			RerouteNode->Parent->RerouteConnections[IndexOfRerouteNode + 1]->BeginSocket = nullptr;
		}
	}

	if (!RerouteNode->Parent->RerouteConnections.empty())
	{
		RerouteNode->Parent->RerouteConnections.erase(RerouteNode->Parent->RerouteConnections.begin() + IndexOfRerouteNode, RerouteNode->Parent->RerouteConnections.begin() + IndexOfRerouteNode + 1);
		delete RerouteNode;
		RerouteNode = nullptr;
	}
}

void VisualNodeArea::DeleteNode(const VisualNode* Node)
{
	if (!Node->bCouldBeDestroyed)
		return;

	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i] == Node)
		{
			PropagateNodeEventsCallbacks(Nodes[i], VISUAL_NODE_REMOVED);

			for (size_t j = 0; j < Nodes[i]->Input.size(); j++)
			{
				auto connections = GetAllConnections(Nodes[i]->Input[j]);
				for (size_t p = 0; p < connections.size(); p++)
				{
					Disconnect(connections[p]);
				}
			}

			for (size_t j = 0; j < Nodes[i]->Output.size(); j++)
			{
				auto connections = GetAllConnections(Nodes[i]->Output[j]);
				for (size_t p = 0; p < connections.size(); p++)
				{
					Disconnect(connections[p]);
				}
			}

			delete Nodes[i];
			Nodes.erase(Nodes.begin() + i, Nodes.begin() + i + 1);

			break;
		}
	}
}

void VisualNodeArea::PropagateUpdateToConnectedNodes(const VisualNode* CallerNode) const
{
	if (CallerNode == nullptr)
		return;

	for (size_t i = 0; i < CallerNode->Input.size(); i++)
	{
		auto connections = GetAllConnections(CallerNode->Input[i]);
		for (size_t j = 0; j < connections.size(); j++)
		{
			connections[j]->In->GetParent()->SocketEvent(connections[j]->In, connections[j]->Out, VISUAL_NODE_SOCKET_UPDATE);
		}
	}

	for (size_t i = 0; i < CallerNode->Output.size(); i++)
	{
		auto connections = GetAllConnections(CallerNode->Output[i]);
		for (size_t j = 0; j < connections.size(); j++)
		{
			connections[j]->In->GetParent()->SocketEvent(connections[j]->In, connections[j]->Out, VISUAL_NODE_SOCKET_UPDATE);
		}
	}
}

bool VisualNodeArea::TryToConnect(const VisualNode* OutNode, const size_t OutNodeSocketIndex, const VisualNode* InNode, const size_t InNodeSocketIndex)
{
	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return false;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return false;

	NodeSocket* OutSocket = OutNode->Output[OutNodeSocketIndex];
	NodeSocket* InSocket = InNode->Input[InNodeSocketIndex];

	char* msg = nullptr;
	const bool result = InSocket->GetParent()->CanConnect(InSocket, OutSocket, &msg);

	if (result)
	{
		PropagateNodeEventsCallbacks(OutSocket->GetParent(), VISUAL_NODE_BEFORE_CONNECTED);
		PropagateNodeEventsCallbacks(InSocket->GetParent(), VISUAL_NODE_BEFORE_CONNECTED);

		OutSocket->SocketConnected.push_back(InSocket);
		InSocket->SocketConnected.push_back(OutSocket);

		Connections.push_back(new VisualNodeConnection(OutSocket, InSocket));

		OutSocket->GetParent()->SocketEvent(OutSocket, InSocket, VISUAL_NODE_SOCKET_CONNECTED);
		InSocket->GetParent()->SocketEvent(InSocket, OutSocket, VISUAL_NODE_SOCKET_CONNECTED);

		PropagateNodeEventsCallbacks(OutSocket->GetParent(), VISUAL_NODE_AFTER_CONNECTED);
		PropagateNodeEventsCallbacks(InSocket->GetParent(), VISUAL_NODE_AFTER_CONNECTED);
	}

	return result;
}

void VisualNodeArea::RunOnEachNode(void(*Func)(VisualNode*))
{
	if (Func != nullptr)
		std::for_each(Nodes.begin(), Nodes.end(), Func);
}

void VisualNodeArea::RunOnEachConnectedNode(VisualNode* StartNode, void(*Func)(VisualNode*))
{
	if (Func == nullptr)
		return;

	static std::unordered_map<VisualNode*, bool> SeenNodes;
	SeenNodes.clear();
	auto bWasNodeSeen = [](VisualNode* Node) {
		if (SeenNodes.find(Node) == SeenNodes.end())
		{
			SeenNodes[Node] = true;
			return false;
		}

		return true;
	};
	
	std::vector<VisualNode*> CurrentNodes;
	CurrentNodes.push_back(StartNode);
	if (bWasNodeSeen(StartNode))
		return;
	while (!EmptyOrFilledByNulls(CurrentNodes))
	{
		for (int i = 0; i < static_cast<int>(CurrentNodes.size()); i++)
		{
			if (CurrentNodes[i] == nullptr)
			{
				CurrentNodes.erase(CurrentNodes.begin() + i);
				i--;
				continue;
			}

			Func(CurrentNodes[i]);

			std::vector<VisualNode*> NewNodes = CurrentNodes[i]->GetNodesConnectedToOutput();
			for (size_t j = 0; j < NewNodes.size(); j++)
			{
				CurrentNodes.push_back(NewNodes[j]);
				if (bWasNodeSeen(NewNodes[j]))
					return;
			}

			CurrentNodes.erase(CurrentNodes.begin() + i);
			i--;
		}
	}
}

bool VisualNodeArea::TryToConnect(const VisualNode* OutNode, const std::string OutSocketID, const VisualNode* InNode, const std::string InSocketID)
{
	size_t OutSocketIndex = 0;
	for (size_t i = 0; i < OutNode->Output.size(); i++)
	{
		if (OutNode->Output[i]->GetID() == OutSocketID)
		{
			OutSocketIndex = i;
			break;
		}
	}

	size_t InSocketIndex = 0;
	for (size_t i = 0; i < InNode->Input.size(); i++)
	{
		if (InNode->Input[i]->GetID() == InSocketID)
		{
			InSocketIndex = i;
			break;
		}
	}

	return TryToConnect(OutNode, OutSocketIndex, InNode, InSocketIndex);
}

bool VisualNodeArea::TriggerSocketEvent(NodeSocket* CallerNodeSocket, NodeSocket* TriggeredNodeSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	if (CallerNodeSocket == nullptr || TriggeredNodeSocket == nullptr)
		return false;

	if (EventType != VISUAL_NODE_SOCKET_EXECUTE && EventType != VISUAL_NODE_SOCKET_UPDATE)
		return false;

	if (TriggeredNodeSocket->GetParent() == nullptr)
		return false;

	SocketEventQueue.push({ TriggeredNodeSocket, CallerNodeSocket, EventType });

	return true;
}

bool VisualNodeArea::TriggerOrphanSocketEvent(VisualNode* Node, VISUAL_NODE_SOCKET_EVENT EventType)
{
	if (Node == nullptr)
		return false;

	if (EventType != VISUAL_NODE_SOCKET_EXECUTE)
		return false;

	Node->SocketEvent(nullptr, nullptr, EventType);

	return true;
}

void VisualNodeArea::ProcessSocketEventQueue()
{
	while (!SocketEventQueue.empty())
	{
		SocketEvent Event = SocketEventQueue.front();
		SocketEventQueue.pop();

		Event.TriggeredNodeSocket->GetParent()->SocketEvent(Event.TriggeredNodeSocket, Event.CallerNodeSocket, Event.EventType);
	}
}