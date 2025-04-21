#include "VisualNodeArea.h"
using namespace VisNodeSys;

void NodeArea::AddNode(Node* NewNode)
{
	if (NewNode == nullptr)
		return;

	NewNode->ParentArea = this;
	Nodes.push_back(NewNode);
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
	if (HoveredConnection == Connection)
		HoveredConnection = nullptr;

	UnSelect(Connection);

	for (size_t i = 0; i < Connection->RerouteNodes.size(); i++)
	{
		Delete(Connection->RerouteNodes[i]);
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
			// TO-DO : Add some variation of disconnected event, like DISCONNECTED_INCOMING
			Connection->In->Parent->SocketEvent(Connection->In, Connection->Out, bClearing ? DESTRUCTION : DISCONNECTED);
			i--;
		}
	}

	for (int i = 0; i < static_cast<int>(Connection->Out->ConnectedSockets.size()); i++)
	{
		if (Connection->Out->ConnectedSockets[i] == Connection->In)
		{
			Connection->Out->ConnectedSockets.erase(Connection->Out->ConnectedSockets.begin() + i, Connection->Out->ConnectedSockets.begin() + i + 1);
			// TO-DO : Add some variation of disconnected event, like DISCONNECTED_OUTGOING
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

void NodeArea::Delete(RerouteNode* RerouteNode)
{
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
	}
}

void NodeArea::Delete(GroupComment* GroupComment)
{
	if (GroupCommentHovered == GroupComment)
		GroupCommentHovered = nullptr;

	UnSelect(GroupComment);

	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i] == GroupComment)
		{
			delete GroupComments[i];
			GroupComments.erase(GroupComments.begin() + i, GroupComments.begin() + i + 1);

			break;
		}
	}
}

void NodeArea::DeleteNode(const Node* Node)
{
	if (!Node->bCouldBeDestroyed)
		return;

	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i] == Node)
		{
			PropagateNodeEventsCallbacks(Nodes[i], REMOVED);

			for (size_t j = 0; j < Nodes[i]->Input.size(); j++)
			{
				auto Connections = GetAllConnections(Nodes[i]->Input[j]);
				for (size_t p = 0; p < Connections.size(); p++)
				{
					Delete(Connections[p]);
				}
			}

			for (size_t j = 0; j < Nodes[i]->Output.size(); j++)
			{
				auto Connections = GetAllConnections(Nodes[i]->Output[j]);
				for (size_t p = 0; p < Connections.size(); p++)
				{
					Delete(Connections[p]);
				}
			}

			delete Nodes[i];
			Nodes.erase(Nodes.begin() + i, Nodes.begin() + i + 1);

			break;
		}
	}
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

bool NodeArea::TryToConnect(const Node* OutNode, const size_t OutNodeSocketIndex, const Node* InNode, const size_t InNodeSocketIndex)
{
	if (OutNode == nullptr || InNode == nullptr)
		return false;

	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return false;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return false;

	NodeSocket* OutSocket = OutNode->Output[OutNodeSocketIndex];
	NodeSocket* InSocket = InNode->Input[InNodeSocketIndex];

	char* Message = nullptr;
	const bool Result = InSocket->GetParent()->CanConnect(InSocket, OutSocket, &Message);

	if (Result)
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

	return Result;
}

bool NodeArea::TryToDisconnect(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex)
{
	if (OutNode == nullptr || InNode == nullptr)
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
	if (OutNode == nullptr || InNode == nullptr)
		return false;

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

	return TryToDisconnect(OutNode, OutSocketIndex, InNode, InSocketIndex);
}

bool NodeArea::IsConnected(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex)
{
	if (OutNode == nullptr || InNode == nullptr)
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
	if (OutNode == nullptr || InNode == nullptr)
		return false;

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

	return IsConnected(OutNode, OutSocketIndex, InNode, InSocketIndex);
}

void NodeArea::RunOnEachNode(void(*Func)(Node*))
{
	if (Func != nullptr)
		std::for_each(Nodes.begin(), Nodes.end(), Func);
}

void NodeArea::RunOnEachConnectedNode(Node* StartNode, void(*Func)(Node*))
{
	if (Func == nullptr)
		return;

	static std::unordered_map<Node*, bool> SeenNodes;
	SeenNodes.clear();
	auto bWasNodeSeen = [](Node* Node) {
		if (SeenNodes.find(Node) == SeenNodes.end())
		{
			SeenNodes[Node] = true;
			return false;
		}

		return true;
	};
	
	std::vector<Node*> CurrentNodes;
	CurrentNodes.push_back(StartNode);
	if (bWasNodeSeen(StartNode))
		return;

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

			Func(CurrentNodes[i]);

			std::vector<Node*> NewNodes = CurrentNodes[i]->GetNodesConnectedToOutput();
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

bool NodeArea::TryToConnect(const Node* OutNode, const std::string OutSocketID, const Node* InNode, const std::string InSocketID)
{
	if (OutNode == nullptr || InNode == nullptr)
		return false;

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

bool NodeArea::TriggerSocketEvent(NodeSocket* CallerNodeSocket, NodeSocket* TriggeredNodeSocket, NODE_SOCKET_EVENT EventType)
{
	if (CallerNodeSocket == nullptr || TriggeredNodeSocket == nullptr)
		return false;

	if (EventType != EXECUTE && EventType != UPDATE)
		return false;

	if (TriggeredNodeSocket->GetParent() == nullptr)
		return false;

	SocketEventQueue.push({ TriggeredNodeSocket, CallerNodeSocket, EventType });

	return true;
}

bool NodeArea::TriggerOrphanSocketEvent(Node* Node, NODE_SOCKET_EVENT EventType)
{
	if (Node == nullptr)
		return false;

	if (EventType != EXECUTE)
		return false;

	Node->SocketEvent(nullptr, nullptr, EventType);

	return true;
}

void NodeArea::ProcessSocketEventQueue()
{
	while (!SocketEventQueue.empty())
	{
		SocketEvent Event = SocketEventQueue.front();
		SocketEventQueue.pop();

		Event.TriggeredNodeSocket->GetParent()->SocketEvent(Event.TriggeredNodeSocket, Event.CallerNodeSocket, Event.EventType);
	}
}

bool NodeArea::AddRerouteNode(Connection* Connection, size_t SegmentToDivide, ImVec2 Position)
{
	if (Connection == nullptr)
		return false;

	RerouteNode* NewReroute = new RerouteNode();
	NewReroute->ID = NODE_CORE.GetUniqueHexID();
	NewReroute->Parent = Connection;
	NewReroute->Position = Position;

	if (Connection->RerouteNodes.empty())
	{
		NewReroute->BeginSocket = Connection->Out;
		NewReroute->EndSocket = Connection->In;

		Connection->RerouteNodes.push_back(NewReroute);
		return true;
	}
	else
	{
		std::vector<ConnectionSegment> ConnectionSegments = GetConnectionSegments(Connection);
		if (SegmentToDivide < 0 || SegmentToDivide >= ConnectionSegments.size())
			return false;

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
				return true;
			}
		}
	}

	delete NewReroute;
	return false;
}


std::vector<std::pair<ImVec2, ImVec2>> NodeArea::GetConnectionSegments(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex) const
{
	std::vector<std::pair<ImVec2, ImVec2>> Result;
	if (OutNode == nullptr || InNode == nullptr)
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
	if (OutNode == nullptr || InNode == nullptr)
		return Result;

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

	return GetConnectionSegments(OutNode, OutSocketIndex, InNode, InSocketIndex);
}

bool NodeArea::AddRerouteNodeToConnection(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex, size_t SegmentToDivide, ImVec2 Position)
{
	if (OutNode == nullptr || InNode == nullptr)
		return false;

	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return false;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return false;

	Connection* Connection = GetConnection(OutNode->Output[OutNodeSocketIndex], InNode->Input[InNodeSocketIndex]);
	if (Connection == nullptr)
		return false;

	return AddRerouteNode(Connection, SegmentToDivide, Position);
}

bool NodeArea::AddRerouteNodeToConnection(const Node* OutNode, std::string OutSocketID, const Node* InNode, std::string InSocketID, size_t SegmentToDivide, ImVec2 Position)
{
	std::vector<int> Result;
	if (OutNode == nullptr || InNode == nullptr)
		return false;

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

	return AddRerouteNodeToConnection(OutNode, OutSocketIndex, InNode, InSocketIndex, SegmentToDivide, Position);
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

void NodeArea::AddGroupComment(GroupComment* NewGroupComment)
{
	if (NewGroupComment == nullptr)
		return;

	NewGroupComment->ParentArea = this;
	GroupComments.push_back(NewGroupComment);
}

void NodeArea::DeleteGroupComment(GroupComment* GroupComment)
{
	if (GroupComment == nullptr)
		return;

	UnSelect(GroupComment);

	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i] == GroupComment)
		{
			GroupComments.erase(GroupComments.begin() + i);
			delete GroupComment;
			return;
		}
	}
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