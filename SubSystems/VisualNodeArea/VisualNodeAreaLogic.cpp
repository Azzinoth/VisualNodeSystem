#include "VisualNodeArea.h"
#include "../VisualNodeSystem.h"
using namespace VisNodeSys;

bool NodeArea::AddNode(Node* NewNode)
{
	if (NewNode == nullptr)
		return false;

	if (NewNode->GetParentArea() != nullptr)
		return false;

	if (GetNodeByID(NewNode->GetID()) != nullptr)
		return false;

	if (NewNode->GetType() == "LinkNode")
	{
		LinkNode* ReferenceNode = static_cast<LinkNode*>(NewNode);
		if (ReferenceNode->GetLinkedArea() == this)
			return false;
	}

	// A SubAreaNode cannot own the area it lives in.
	if (NewNode->GetType() == "SubAreaNode")
	{
		SubAreaNode* AsSubArea = static_cast<SubAreaNode*>(NewNode);
		if (AsSubArea->OwnedAreaID == GetID())
		{
			// Clear the OwnedAreaID before rejecting.
			AsSubArea->OwnedAreaID = "";
			return false;
		}

		// A SubAreaNode without an owned area considered broken, and we should not allow adding it to the area.
		if (AsSubArea->OwnedAreaID.empty())
			return false;
	}

	// if OwnerSubAreaNodeID of SubAreaInputNode resolves to a SubAreaNode whose GetOwnedArea() is not this, the field must be cleared.
	if (NewNode->GetType() == "SubAreaInputNode")
	{
		SubAreaInputNode* AsSubAreaInput = static_cast<SubAreaInputNode*>(NewNode);
		if (!AsSubAreaInput->OwnerSubAreaNodeID.empty())
		{
			Node* OwnerNode = NODE_SYSTEM.GetNodeByID(AsSubAreaInput->OwnerSubAreaNodeID);
			if (OwnerNode != nullptr && OwnerNode->GetType() == "SubAreaNode")
			{
				SubAreaNode* OwnerSubArea = static_cast<SubAreaNode*>(OwnerNode);
				if (OwnerSubArea->GetOwnedArea() != this)
					AsSubAreaInput->OwnerSubAreaNodeID = "";
			}
		}
	}

	// if OwnerSubAreaNodeID of SubAreaOutputNode resolves to a SubAreaNode whose GetOwnedArea() is not this, the field must be cleared.
	if (NewNode->GetType() == "SubAreaOutputNode")
	{
		SubAreaOutputNode* AsSubAreaOutput = static_cast<SubAreaOutputNode*>(NewNode);
		if (!AsSubAreaOutput->OwnerSubAreaNodeID.empty())
		{
			Node* OwnerNode = NODE_SYSTEM.GetNodeByID(AsSubAreaOutput->OwnerSubAreaNodeID);
			if (OwnerNode != nullptr && OwnerNode->GetType() == "SubAreaNode")
			{
				SubAreaNode* OwnerSubArea = static_cast<SubAreaNode*>(OwnerNode);
				if (OwnerSubArea->GetOwnedArea() != this)
					AsSubAreaOutput->OwnerSubAreaNodeID = "";
			}
		}
	}

	NewNode->ParentArea = this;
	Nodes.push_back(NewNode);

	return true;
}

std::vector<Connection*> NodeArea::GetAllConnections(const NodeSocket* Socket) const
{
	std::vector<Connection*> Result;

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i]->In == Socket || Connections[i]->Out == Socket)
			Result.push_back(Connections[i]);
	}

	return Result;
}

std::vector<Connection*> NodeArea::GetAllConnections(const Node* Node) const
{
	std::vector<Connection*> Result;
	if (Node == nullptr)
		return Result;

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i] == nullptr)
			continue;

		bool bTouchesNode = false;
		if (Connections[i]->In != nullptr && Connections[i]->In->GetParent() == Node)
			bTouchesNode = true;

		else if (Connections[i]->Out != nullptr && Connections[i]->Out->GetParent() == Node)
			bTouchesNode = true;

		if (bTouchesNode)
			Result.push_back(Connections[i]);
	}

	return Result;
}

Connection* NodeArea::GetConnection(const NodeSocket* FirstSocket, const NodeSocket* SecondSocket) const
{
	for (size_t i = 0; i < Connections.size(); i++)
	{
		if ((Connections[i]->In == FirstSocket && Connections[i]->Out == SecondSocket) || (Connections[i]->In == SecondSocket && Connections[i]->Out == FirstSocket))
			return Connections[i];
	}

	return nullptr;
}

void NodeArea::Delete(Connection* Connection)
{
	if (Connection == nullptr)
		return;

	if (HoveredConnection == Connection)
		HoveredConnection = nullptr;

	UnSelect(Connection);

	for (size_t i = 0; i < Connection->RerouteNodes.size(); i++)
	{
		if (Delete(Connection->RerouteNodes[i]))
			i--;
	}

	std::vector<Node*> NodesToNotify;
	for (int i = 0; i < static_cast<int>(Connection->In->ConnectedSockets.size()); i++)
	{
		if (Connection->In->ConnectedSockets[i] == Connection->Out)
		{
			Node* Parent = Connection->In->ConnectedSockets[i]->Parent;
			NodesToNotify.push_back(Parent);
		}
	}

	for (int i = 0; i < static_cast<int>(Connection->Out->ConnectedSockets.size()); i++)
	{
		if (Connection->Out->ConnectedSockets[i] == Connection->In)
		{
			Node* Parent = Connection->Out->ConnectedSockets[i]->Parent;
			NodesToNotify.push_back(Parent);
		}
	}

	if (!bClearing)
	{
		for (size_t i = 0; i < NodesToNotify.size(); i++)
		{
			PropagateNodeEventsCallbacks(NodesToNotify[i], BEFORE_DISCONNECTED);
		}
	}

	for (int i = 0; i < static_cast<int>(Connection->In->ConnectedSockets.size()); i++)
	{
		if (Connection->In->ConnectedSockets[i] == Connection->Out)
		{
			Connection->In->ConnectedSockets.erase(Connection->In->ConnectedSockets.begin() + i, Connection->In->ConnectedSockets.begin() + i + 1);
			// FE_TO_DO : Add some variation of disconnected event, like DISCONNECTED_INCOMING
			Connection->In->Parent->SocketEvent(Connection->In, Connection->Out, bClearing ? DESTRUCTION : DISCONNECTED);
			i--;
		}
	}

	for (int i = 0; i < static_cast<int>(Connection->Out->ConnectedSockets.size()); i++)
	{
		if (Connection->Out->ConnectedSockets[i] == Connection->In)
		{
			Connection->Out->ConnectedSockets.erase(Connection->Out->ConnectedSockets.begin() + i, Connection->Out->ConnectedSockets.begin() + i + 1);
			// FE_TO_DO : Add some variation of disconnected event, like DISCONNECTED_OUTGOING
			Connection->Out->Parent->SocketEvent(Connection->Out, Connection->In, bClearing ? DESTRUCTION : DISCONNECTED);
			i--;
		}
	}

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i] == Connection)
		{
			delete Connection;
			Connections.erase(Connections.begin() + i, Connections.begin() + i + 1);
			break;
		}
	}

	if (!bClearing)
	{
		for (size_t i = 0; i < NodesToNotify.size(); i++)
		{
			PropagateNodeEventsCallbacks(NodesToNotify[i], AFTER_DISCONNECTED);
		}
	}
}

bool NodeArea::Delete(RerouteNode* RerouteNode)
{
	if (RerouteNode == nullptr)
		return false;

	// Reject reroutes whose parent Connection does not live in this area.
	bool bOwnedHere = false;
	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i] == RerouteNode->Parent)
		{
			bOwnedHere = true;
			break;
		}
	}
	if (!bOwnedHere)
		return false;

	if (RerouteNodeHovered == RerouteNode)
		RerouteNodeHovered = nullptr;

	UnSelect(RerouteNode);

	Connection* Connection = RerouteNode->Parent;
	size_t IndexOfRerouteNode = 0;
	for (size_t i = 0; i < Connection->RerouteNodes.size(); i++)
	{
		if (Connection->RerouteNodes[i] == RerouteNode)
		{
			IndexOfRerouteNode = i;
			break;
		}
	}

	if (Connection->RerouteNodes.size() > 1)
	{
		// If the reroute node is the first one, we need to update the begin socket of the next one
		if (Connection->RerouteNodes[0] == RerouteNode)
		{
			Connection->RerouteNodes[1]->BeginSocket = Connection->RerouteNodes[0]->BeginSocket;
			Connection->RerouteNodes[1]->BeginReroute = nullptr;
		}
		// If the reroute node is the last one, we need to update the end socket of the previous one
		else if (Connection->RerouteNodes.back() == RerouteNode)
		{
			Connection->RerouteNodes[IndexOfRerouteNode - 1]->EndSocket = Connection->RerouteNodes[IndexOfRerouteNode]->EndSocket;
			Connection->RerouteNodes[IndexOfRerouteNode - 1]->EndReroute = nullptr;
		}
		// If the reroute node is in the middle, we need to update the previous and next one
		else
		{
			Connection->RerouteNodes[IndexOfRerouteNode - 1]->EndReroute = Connection->RerouteNodes[IndexOfRerouteNode + 1];
			Connection->RerouteNodes[IndexOfRerouteNode - 1]->EndSocket = nullptr;

			Connection->RerouteNodes[IndexOfRerouteNode + 1]->BeginReroute = Connection->RerouteNodes[IndexOfRerouteNode - 1];
			Connection->RerouteNodes[IndexOfRerouteNode + 1]->BeginSocket = nullptr;
		}
	}

	if (!Connection->RerouteNodes.empty())
	{
		delete RerouteNode;
		Connection->RerouteNodes.erase(Connection->RerouteNodes.begin() + IndexOfRerouteNode, Connection->RerouteNodes.begin() + IndexOfRerouteNode + 1);
		return true;
	}

	return false;
}

int NodeArea::GetNodeIndex(const Node* Node) const
{
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i] == Node)
			return static_cast<int>(i);
	}

	return -1;
}

bool NodeArea::Delete(const Node* NodeToDelete)
{
	if (NodeToDelete == nullptr)
		return false;

	if (!NodeToDelete->bCouldBeDestroyedByUser)
		return false;

	int Index = GetNodeIndex(NodeToDelete);
	if (Index == -1)
		return false;

	UnSelect(NodeToDelete);
	PropagateNodeEventsCallbacks(Nodes[Index], DESTROYED);
	// After propagating the DESTROYED event, the node might be already deleted.
	Node* CheckNode = GetNodeByID(NodeToDelete->GetID());
	if (CheckNode == nullptr)
		return false;

	for (size_t i = 0; i < Nodes[Index]->Input.size(); i++)
	{
		auto Connections = GetAllConnections(Nodes[Index]->Input[i]);
		for (size_t j = 0; j < Connections.size(); j++)
			Delete(Connections[j]);
	}

	for (size_t i = 0; i < Nodes[Index]->Output.size(); i++)
	{
		auto Connections = GetAllConnections(Nodes[Index]->Output[i]);
		for (size_t j = 0; j < Connections.size(); j++)
			Delete(Connections[j]);
	}

	std::string NodeToDeleteID = Nodes[Index]->GetID();
	NODE_SYSTEM.OnNodeDeletion(Nodes[Index]);
	Node* NodeAfterOnNodeDeletion = GetNodeByID(NodeToDeleteID);
	// NODE_SYSTEM.OnNodeDeletion can potentially delete the node, so we need to check if it still exists before trying to delete it.
	if (NodeAfterOnNodeDeletion != nullptr)
	{
		int NewIndex = GetNodeIndex(NodeToDelete);
		DeleteNodeInternal(NodeAfterOnNodeDeletion, NewIndex);
	}

	return true;
}

void NodeArea::DeleteNodeInternal(const Node* Node, int Index)
{
	if (Node == nullptr)
		return;

	if (!Node->bCouldBeDestroyedByUser)
		return;

	if (Index == -1)
	{
		for (size_t i = 0; i < Nodes.size(); i++)
		{
			if (Nodes[i] == Node)
				Index = static_cast<int>(i);
		}
	}

	if (Index < 0 || Index >= Nodes.size())
		return;

	delete Nodes[Index];
	Nodes.erase(Nodes.begin() + Index, Nodes.begin() + Index + 1);
}

void NodeArea::PropagateUpdateToConnectedNodes(const Node* CallerNode) const
{
	if (CallerNode == nullptr)
		return;

	for (size_t i = 0; i < CallerNode->Input.size(); i++)
	{
		auto Connections = GetAllConnections(CallerNode->Input[i]);
		for (size_t j = 0; j < Connections.size(); j++)
		{
			Connections[j]->In->GetParent()->SocketEvent(Connections[j]->In, Connections[j]->Out, UPDATE);
		}
	}

	for (size_t i = 0; i < CallerNode->Output.size(); i++)
	{
		auto Connections = GetAllConnections(CallerNode->Output[i]);
		for (size_t j = 0; j < Connections.size(); j++)
		{
			Connections[j]->In->GetParent()->SocketEvent(Connections[j]->In, Connections[j]->Out, UPDATE);
		}
	}
}

size_t NodeArea::GetConnectionCount() const
{
	return Connections.size();
}

bool NodeArea::IsThisAreaResponsibleFor(const Node* NodeToCheck) const
{
	if (NodeToCheck == nullptr)
		return false;

	if (NodeToCheck->GetParentArea() != this)
		return false;

	Node* NodeInArea = GetNodeByID(NodeToCheck->GetID());
	if (NodeInArea == nullptr)
		return false;

	return true;
}

bool NodeArea::IsThisAreaResponsibleFor(const Node* OutNode, const Node* InNode) const
{
	if (!IsThisAreaResponsibleFor(OutNode) || !IsThisAreaResponsibleFor(InNode))
		return false;

	return true;
}

bool NodeArea::ValidateSocketPair(const Node* OutNode, const std::string& OutSocketID, const Node* InNode, const std::string& InSocketID) const
{
	if (OutNode == nullptr || InNode == nullptr)
		return false;

	NodeSocket* OutSocket = OutNode->GetSocketByID(OutSocketID);
	NodeSocket* InSocket = InNode->GetSocketByID(InSocketID);
	if (OutSocket == nullptr || InSocket == nullptr)
		return false;

	if (OutSocket->GetFlowDirection() != NodeSocket::SocketFlow::Output || InSocket->GetFlowDirection() != NodeSocket::SocketFlow::Input)
		return false;

	return true;
}

bool NodeArea::TryToConnect(const Node* OutNode, const size_t OutNodeSocketIndex, const Node* InNode, const size_t InNodeSocketIndex)
{
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return false;

	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return false;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return false;

	NodeSocket* OutSocket = OutNode->Output[OutNodeSocketIndex];
	NodeSocket* InSocket = InNode->Input[InNodeSocketIndex];

	char* Message = nullptr;
	const bool bResult = InSocket->GetParent()->CanConnect(InSocket, OutSocket, &Message);
	if (bResult)
	{
		PropagateNodeEventsCallbacks(OutSocket->GetParent(), BEFORE_CONNECTED);
		PropagateNodeEventsCallbacks(InSocket->GetParent(), BEFORE_CONNECTED);

		OutSocket->ConnectedSockets.push_back(InSocket);
		InSocket->ConnectedSockets.push_back(OutSocket);

		Connections.push_back(new Connection(OutSocket, InSocket));

		OutSocket->GetParent()->SocketEvent(OutSocket, InSocket, CONNECTED);
		InSocket->GetParent()->SocketEvent(InSocket, OutSocket, CONNECTED);

		PropagateNodeEventsCallbacks(OutSocket->GetParent(), AFTER_CONNECTED);
		PropagateNodeEventsCallbacks(InSocket->GetParent(), AFTER_CONNECTED);
	}

	return bResult;
}

bool NodeArea::TryToDisconnect(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex)
{
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return false;

	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return false;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return false;

	NodeSocket* OutSocket = OutNode->Output[OutNodeSocketIndex];
	NodeSocket* InSocket = InNode->Input[InNodeSocketIndex];

	Connection* Connection = GetConnection(OutSocket, InSocket);
	if (Connection == nullptr)
		return false;

	Delete(Connection);
	return true;
}

bool NodeArea::TryToDisconnect(const Node* OutNode, std::string OutSocketID, const Node* InNode, std::string InSocketID)
{
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return false;

	if (!ValidateSocketPair(OutNode, OutSocketID, InNode, InSocketID))
		return false;
	
	return TryToDisconnect(OutNode, OutNode->GetSocketIndexByID(OutSocketID), InNode, InNode->GetSocketIndexByID(InSocketID));
}

bool NodeArea::TryToDisconnect(const Node* Node, std::string SocketID)
{
	if (Node == nullptr || Node->GetParentArea() != this)
		return false;

	const NodeSocket* Socket = Node->GetSocketByID(SocketID);
	if (Socket == nullptr)
		return false;

	std::vector<Connection*> Connections = GetAllConnections(Socket);
	bool bResult = !Connections.empty();
	for (size_t i = 0; i < Connections.size(); i++)
		Delete(Connections[i]);
	
	return bResult;
}

bool NodeArea::IsConnected(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex)
{
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return false;

	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return false;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return false;

	NodeSocket* OutSocket = OutNode->Output[OutNodeSocketIndex];
	NodeSocket* InSocket = InNode->Input[InNodeSocketIndex];

	Connection* Connection = GetConnection(OutSocket, InSocket);
	if (Connection == nullptr)
		return false;

	for (size_t i = 0; i < OutSocket->ConnectedSockets.size(); i++)
	{
		if (OutSocket->ConnectedSockets[i] == InSocket)
			return true;
	}

	return false;
}

bool NodeArea::IsConnected(const Node* OutNode, std::string OutSocketID, const Node* InNode, std::string InSocketID)
{
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return false;

	if (!ValidateSocketPair(OutNode, OutSocketID, InNode, InSocketID))
		return false;

	return IsConnected(OutNode, OutNode->GetSocketIndexByID(OutSocketID), InNode, InNode->GetSocketIndexByID(InSocketID));
}

bool NodeArea::IsConnected(const Node* FirstNode, const Node* SecondNode)
{
	if (FirstNode == nullptr || SecondNode == nullptr)
		return false;

	for (size_t i = 0; i < FirstNode->Input.size(); i++)
	{
		auto Connections = GetAllConnections(FirstNode->Input[i]);
		for (size_t j = 0; j < Connections.size(); j++)
		{
			if (Connections[j]->Out->GetParent() == SecondNode)
				return true;
		}
	}

	for (size_t i = 0; i < FirstNode->Output.size(); i++)
	{
		auto Connections = GetAllConnections(FirstNode->Output[i]);
		for (size_t j = 0; j < Connections.size(); j++)
		{
			if (Connections[j]->In->GetParent() == SecondNode)
				return true;
		}
	}

	return false;
}

void NodeArea::RunOnEachNode(const std::function<void(Node*)>& Function)
{
	if (Function != nullptr)
		std::for_each(Nodes.begin(), Nodes.end(), Function);
}

void NodeArea::RunOnEachConnectedNode(Node* StartNode, const std::function<void(Node*)>& Function)
{
	if (StartNode == nullptr || Function == nullptr)
		return;

	std::unordered_map<Node*, bool> SeenNodes;
	SeenNodes.clear();
	auto bWasNodeSeen = [&SeenNodes](Node* Node) {
		if (SeenNodes.find(Node) == SeenNodes.end())
		{
			SeenNodes[Node] = true;
			return false;
		}

		return true;
	};
	
	std::vector<Node*> CurrentNodes;
	std::vector<Node*> NewNodes = StartNode->GetNodesConnectedToOutput();
	for (size_t j = 0; j < NewNodes.size(); j++)
	{
		if (bWasNodeSeen(NewNodes[j]))
			continue;

		CurrentNodes.push_back(NewNodes[j]);
	}

	while (!IsEmptyOrFilledByNulls(CurrentNodes))
	{
		for (int i = 0; i < static_cast<int>(CurrentNodes.size()); i++)
		{
			if (CurrentNodes[i] == nullptr)
			{
				CurrentNodes.erase(CurrentNodes.begin() + i);
				i--;
				continue;
			}

			Function(CurrentNodes[i]);

			std::vector<Node*> NewNodes = CurrentNodes[i]->GetNodesConnectedToOutput();
			for (size_t j = 0; j < NewNodes.size(); j++)
			{
				if (bWasNodeSeen(NewNodes[j]))
					continue;

				CurrentNodes.push_back(NewNodes[j]);
			}

			CurrentNodes.erase(CurrentNodes.begin() + i);
			i--;
		}
	}
}

bool NodeArea::TryToConnect(const Node* OutNode, const std::string OutSocketID, const Node* InNode, const std::string InSocketID)
{
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return false;

	if (!ValidateSocketPair(OutNode, OutSocketID, InNode, InSocketID))
		return false;

	return TryToConnect(OutNode, OutNode->GetSocketIndexByID(OutSocketID), InNode, InNode->GetSocketIndexByID(InSocketID));
}

bool NodeArea::TriggerSocketEvent(NodeSocket* CallerNodeSocket, NodeSocket* TriggeredNodeSocket, NODE_SOCKET_EVENT EventType)
{
	if (CallerNodeSocket == nullptr || TriggeredNodeSocket == nullptr)
		return false;

	if (EventType != EXECUTE && EventType != UPDATE)
		return false;

	if (TriggeredNodeSocket->GetParent() == nullptr)
		return false;

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
	if (EventType == EXECUTE && Settings.bSaveExecutedNodes)
		LastExecutedNodes.push_back(TriggeredNodeSocket->GetParent());
#endif
	TriggeredNodeSocket->GetParent()->SocketEvent(TriggeredNodeSocket, CallerNodeSocket, EventType);
	
	return true;
}

bool NodeArea::TriggerOrphanSocketEvent(Node* Node, NODE_SOCKET_EVENT EventType)
{
	if (Node == nullptr)
		return false;

	if (EventType != EXECUTE)
		return false;

	if (Node->GetParentArea() != this)
		return false;

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
	if (EventType == EXECUTE && Settings.bSaveExecutedNodes)
		LastExecutedNodes.push_back(Node);
#endif

	// Synthesise an OwnSocket for SocketEvent dispatch.
	// Prevents crash if SocketEvent implementation accesses the connected socket.
	NodeSocket* SimulatedSourceSocket = nullptr;

	// First try to find an EXECUTE input socket.
	size_t InputCount = Node->GetInputSocketCount();
	for (size_t i = 0; i < InputCount; i++)
	{
		NodeSocket* CurrentSocket = Node->GetSocketByIndex(static_cast<int>(i), NodeSocket::SocketFlow::Input);
		if (CurrentSocket == nullptr)
			continue;

		const std::vector<std::string>& AllowedTypes = CurrentSocket->GetAllowedTypes();
		bool bIsExecute = false;
		for (size_t j = 0; j < AllowedTypes.size(); j++)
		{
			if (AllowedTypes[j] == "EXECUTE")
			{
				bIsExecute = true;
				break;
			}
		}

		if (bIsExecute)
		{
			SimulatedSourceSocket = CurrentSocket;
			break;
		}
	}

	// If no EXECUTE input socket was found, try to find an EXECUTE output socket.
	if (SimulatedSourceSocket == nullptr)
	{
		size_t OutputCount = Node->GetOutputSocketCount();
		for (size_t i = 0; i < OutputCount; i++)
		{
			NodeSocket* CurrentSocket = Node->GetSocketByIndex(static_cast<int>(i), NodeSocket::SocketFlow::Output);
			if (CurrentSocket == nullptr)
				continue;

			const std::vector<std::string>& AllowedTypes = CurrentSocket->GetAllowedTypes();
			bool bIsExecute = false;
			for (size_t j = 0; j < AllowedTypes.size(); j++)
			{
				if (AllowedTypes[j] == "EXECUTE")
				{
					bIsExecute = true;
					break;
				}
			}

			if (bIsExecute)
			{
				SimulatedSourceSocket = CurrentSocket;
				break;
			}
		}
	}

	Node->SocketEvent(SimulatedSourceSocket, nullptr, EventType);
	return true;
}

RerouteNode* NodeArea::AddRerouteNode(Connection* Connection, size_t SegmentToDivide, ImVec2 Position)
{
	if (Connection == nullptr)
		return nullptr;

	RerouteNode* NewReroute = new RerouteNode();
	NewReroute->ID = NODE_CORE.GetUniqueHexID();
	NewReroute->Parent = Connection;
	NewReroute->Position = Position;

	if (Connection->RerouteNodes.empty())
	{
		NewReroute->BeginSocket = Connection->Out;
		NewReroute->EndSocket = Connection->In;

		Connection->RerouteNodes.push_back(NewReroute);
		return NewReroute;
	}
	else
	{
		std::vector<ConnectionSegment> ConnectionSegments = GetConnectionSegments(Connection);
		if (SegmentToDivide < 0 || SegmentToDivide >= ConnectionSegments.size())
		{
			delete NewReroute;
			return nullptr;
		}

		for (size_t i = 0; i < ConnectionSegments.size(); i++)
		{
			if (i == SegmentToDivide)
			{
				if (i == 0)
				{
					NewReroute->BeginSocket = ConnectionSegments[i].BeginSocket;
				}
				else
				{
					NewReroute->BeginReroute = Connection->RerouteNodes[i - 1];
					Connection->RerouteNodes[i - 1]->EndSocket = nullptr;
					Connection->RerouteNodes[i - 1]->EndReroute = NewReroute;
				}

				if (i < Connection->RerouteNodes.size())
				{
					Connection->RerouteNodes[i]->BeginSocket = nullptr;
					Connection->RerouteNodes[i]->BeginReroute = NewReroute;
				}

				NewReroute->EndSocket = ConnectionSegments[i].EndSocket;
				NewReroute->EndReroute = ConnectionSegments[i].EndReroute;

				Connection->RerouteNodes.insert(Connection->RerouteNodes.begin() + i, NewReroute);
				return NewReroute;
			}
		}
	}

	delete NewReroute;
	return nullptr;
}


std::vector<std::pair<ImVec2, ImVec2>> NodeArea::GetConnectionSegments(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex) const
{
	std::vector<std::pair<ImVec2, ImVec2>> Result;
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return Result;

	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return Result;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return Result;

	Connection* Connection = GetConnection(OutNode->Output[OutNodeSocketIndex], InNode->Input[InNodeSocketIndex]);
	if (Connection == nullptr)
		return Result;

	std::vector<ConnectionSegment> ConnectionSegments = GetConnectionSegments(Connection);
	for (size_t i = 0; i < ConnectionSegments.size(); i++)
	{
		Result.push_back(std::make_pair(ConnectionSegments[i].Begin, ConnectionSegments[i].End));
	}

	return Result;
}

std::vector<std::pair<ImVec2, ImVec2>> NodeArea::GetConnectionSegments(const Node* OutNode, std::string OutSocketID, const Node* InNode, std::string InSocketID) const
{
	std::vector<std::pair<ImVec2, ImVec2>> Result;
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return Result;

	if (!ValidateSocketPair(OutNode, OutSocketID, InNode, InSocketID))
		return Result;

	return GetConnectionSegments(OutNode, OutNode->GetSocketIndexByID(OutSocketID), InNode, InNode->GetSocketIndexByID(InSocketID));
}

RerouteNode* NodeArea::GetRerouteNodeByID(std::string ID) const
{
	for (size_t i = 0; i < Connections.size(); i++)
	{
		for (size_t j = 0; j < Connections[i]->RerouteNodes.size(); j++)
		{
			if (Connections[i]->RerouteNodes[j]->ID == ID)
				return Connections[i]->RerouteNodes[j];
		}
	}

	return nullptr;
}

RerouteNode* NodeArea::AddRerouteNodeToConnection(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex, size_t SegmentToDivide, ImVec2 Position)
{
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return nullptr;

	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return nullptr;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return nullptr;

	Connection* Connection = GetConnection(OutNode->Output[OutNodeSocketIndex], InNode->Input[InNodeSocketIndex]);
	if (Connection == nullptr)
		return nullptr;

	return AddRerouteNode(Connection, SegmentToDivide, Position);
}

RerouteNode* NodeArea::AddRerouteNodeToConnection(const Node* OutNode, std::string OutSocketID, const Node* InNode, std::string InSocketID, size_t SegmentToDivide, ImVec2 Position)
{
	if (!IsThisAreaResponsibleFor(OutNode, InNode))
		return nullptr;

	if (!ValidateSocketPair(OutNode, OutSocketID, InNode, InSocketID))
		return nullptr;

	return AddRerouteNodeToConnection(OutNode, OutNode->GetSocketIndexByID(OutSocketID), InNode, InNode->GetSocketIndexByID(InSocketID), SegmentToDivide, Position);
}

GroupComment* NodeArea::GetGroupCommentByID(std::string GroupCommentID) const
{
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i]->GetID() == GroupCommentID)
			return GroupComments[i];
	}

	return nullptr;
}

std::vector<GroupComment*> NodeArea::GetGroupCommentsByName(std::string GroupCommentName) const
{
	std::vector<GroupComment*> Result;
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i]->GetCaption() == GroupCommentName)
			Result.push_back(GroupComments[i]);
	}

	return Result;
}

bool NodeArea::AddGroupComment(GroupComment* NewGroupComment)
{
	if (NewGroupComment == nullptr)
		return false;

	if (NewGroupComment->GetParentArea() != nullptr)
		return false;

	if (GetGroupCommentByID(NewGroupComment->GetID()) != nullptr)
		return false;

	NewGroupComment->ParentArea = this;
	GroupComments.push_back(NewGroupComment);
	return true;
}

bool NodeArea::Delete(GroupComment* GroupComment)
{
	if (GroupComment == nullptr)
		return false;

	if (GroupCommentHovered == GroupComment)
		GroupCommentHovered = nullptr;

	UnSelect(GroupComment);

	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i] == GroupComment)
		{
			GroupComments.erase(GroupComments.begin() + i);
			delete GroupComment;
			return true;
		}
	}

	return false;
}

size_t NodeArea::GetGroupCommentCount() const
{
	return GroupComments.size();
}

std::vector<Node*> NodeArea::GetNodesInGroupComment(GroupComment* GroupCommentToCheck) const
{
	std::vector<Node*> Result;
	if (GroupCommentToCheck == nullptr)
		return Result;

	if (GroupCommentToCheck->GetParentArea() != this)
		return Result;

	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetStyle() == DEFAULT)
		{
			if (IsSecondRectInsideFirstOne(GroupCommentToCheck->GetPosition(), GroupCommentToCheck->GetSize(), Nodes[i]->GetPosition(), Nodes[i]->GetSize()))
				Result.push_back(Nodes[i]);
		}
		else if (Nodes[i]->GetStyle() == CIRCLE)
		{
			if (IsSecondRectInsideFirstOne(GroupCommentToCheck->GetPosition(), GroupCommentToCheck->GetSize(), Nodes[i]->GetPosition(), ImVec2(NODE_DIAMETER, NODE_DIAMETER)))
				Result.push_back(Nodes[i]);
		}
	}

	return Result;
}

std::vector<RerouteNode*> NodeArea::GetRerouteNodesInGroupComment(GroupComment* GroupCommentToCheck) const
{
	std::vector<RerouteNode*> Result;
	if (GroupCommentToCheck == nullptr)
		return Result;

	if (GroupCommentToCheck->GetParentArea() != this)
		return Result;

	for (size_t i = 0; i < Connections.size(); i++)
	{
		for (size_t j = 0; j < Connections[i]->RerouteNodes.size(); j++)
		{
			const ImVec2 ReroutePosition = Connections[i]->RerouteNodes[j]->Position;
			if (IsSecondRectInsideFirstOne(GroupCommentToCheck->GetPosition(), GroupCommentToCheck->GetSize(), ReroutePosition, ImVec2(GetRerouteNodeSize() / Zoom, GetRerouteNodeSize() / Zoom)))
				Result.push_back(Connections[i]->RerouteNodes[j]);
		}
	}

	return Result;
}

std::vector<GroupComment*> NodeArea::GetGroupCommentsInGroupComment(GroupComment* GroupCommentToCheck) const
{
	std::vector<GroupComment*> Result;
	if (GroupCommentToCheck == nullptr)
		return Result;

	if (GroupCommentToCheck->GetParentArea() != this)
		return Result;

	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i] == GroupCommentToCheck)
			continue;

		if (IsSecondRectInsideFirstOne(GroupCommentToCheck->GetPosition(), GroupCommentToCheck->GetSize(), GroupComments[i]->GetPosition(), GroupComments[i]->GetSize()))
			Result.push_back(GroupComments[i]);
	}

	return Result;
}

void NodeArea::AttachElementsToGroupComment(GroupComment* GroupComment)
{
	GroupComment->AttachedNodes.clear();
	GroupComment->AttachedRerouteNodes.clear();
	GroupComment->AttachedGroupComments.clear();

	if (!GroupComment->bMoveElementsWithComment)
		return;

	GroupComment->AttachedNodes = GetNodesInGroupComment(GroupComment);
	GroupComment->AttachedRerouteNodes = GetRerouteNodesInGroupComment(GroupComment);
	GroupComment->AttachedGroupComments = GetGroupCommentsInGroupComment(GroupComment);
}

size_t NodeArea::GetRerouteConnectionCount() const
{
	size_t Count = 0;
	for (size_t i = 0; i < Connections.size(); i++)
		Count += Connections[i]->RerouteNodes.size();
	
	return Count;
}

bool NodeArea::IsRerouteNodeValid(const RerouteNode* RerouteNode)
{
	if (RerouteNode == nullptr)
		return false;

	if (RerouteNode->Parent == nullptr)
		return false;

	if (RerouteNode->Parent->In == nullptr || RerouteNode->Parent->Out == nullptr)
		return false;

	if (RerouteNode->BeginSocket == nullptr && RerouteNode->BeginReroute == nullptr)
		return false;

	if (RerouteNode->BeginSocket != nullptr && RerouteNode->BeginReroute != nullptr)
		return false;

	if (RerouteNode->EndSocket == nullptr && RerouteNode->EndReroute == nullptr)
		return false;

	if (RerouteNode->EndSocket != nullptr && RerouteNode->EndReroute != nullptr)
		return false;

	return true;
}

bool NodeArea::DeleteByID(std::string ID)
{
	Node* FoundNode = GetNodeByID(ID);
	if (FoundNode != nullptr)
		return Delete(FoundNode);

	GroupComment* FoundGroupComment = GetGroupCommentByID(ID);
	if (FoundGroupComment != nullptr)
	{
		Delete(FoundGroupComment);
		return true;
	}

	RerouteNode* FoundRerouteNode = GetRerouteNodeByID(ID);
	if (FoundRerouteNode != nullptr)
	{
		Delete(FoundRerouteNode);
		return true;
	}

	return false;
}

NodeArea* NodeArea::GetParent() const
{
	NodeArea* Result = nullptr;

	std::vector<SubAreaInputNode*> SubAreaInputNodes = GetNodesByType<SubAreaInputNode>();
	if (!SubAreaInputNodes.empty())
		Result = SubAreaInputNodes[0]->GetOwningParentArea();

	return Result;
}

bool NodeArea::IsChildOf(const NodeArea* PotentialParent) const
{
	if (PotentialParent == nullptr)
		return false;

	NodeArea* Parent = GetParent();
	while (Parent != nullptr)
	{
		if (Parent == PotentialParent)
			return true;

		Parent = Parent->GetParent();
	}

	return false;
}

bool NodeArea::IsParentOf(const NodeArea* PotentialChild) const
{
	if (PotentialChild == nullptr)
		return false;

	return PotentialChild->IsChildOf(this);
}

size_t NodeArea::GetImediateChildrenCount() const
{
	std::vector<SubAreaNode*> SubAreaNodes = GetNodesByType<SubAreaNode>();
	return SubAreaNodes.size();
}

size_t NodeArea::GetRecursiveChildCount() const
{
	std::vector<SubAreaNode*> SubAreaNodes = GetNodesByType<SubAreaNode>();
	size_t Count = SubAreaNodes.size();
	for (size_t i = 0; i < SubAreaNodes.size(); i++)
	{
		NodeArea* ChildArea = SubAreaNodes[i]->GetOwnedArea();
		if (ChildArea == nullptr)
			continue;

		Count += ChildArea->GetRecursiveChildCount();
	}

	return Count;
}

std::vector<NodeArea*> NodeArea::GetImediateChildren() const
{
	std::vector<NodeArea*> Result;
	std::vector<SubAreaNode*> SubAreaNodes = GetNodesByType<SubAreaNode>();
	for (size_t i = 0; i < SubAreaNodes.size(); i++)
	{
		NodeArea* ChildArea = SubAreaNodes[i]->GetOwnedArea();
		if (ChildArea == nullptr)
			continue;

		Result.push_back(ChildArea);
	}

	return Result;
}

std::vector<NodeArea*> NodeArea::GetRecursiveChildren() const
{
	std::vector<NodeArea*> Result;
	std::vector<SubAreaNode*> SubAreaNodes = GetNodesByType<SubAreaNode>();
	for (size_t i = 0; i < SubAreaNodes.size(); i++)
	{
		NodeArea* ChildArea = SubAreaNodes[i]->GetOwnedArea();
		if (ChildArea == nullptr)
			continue;

		Result.push_back(ChildArea);
		std::vector<NodeArea*> ChildAreas = ChildArea->GetRecursiveChildren();
		for (size_t j = 0; j < ChildAreas.size(); j++)
		{
			// Check if we have already added this area to result to avoid duplicates in case of multiple links between same areas.
			if (!NODE_SYSTEM.IsInAListOfAreas(ChildAreas[j], Result))
				Result.push_back(ChildAreas[j]);
		}
	}

	return Result;
}