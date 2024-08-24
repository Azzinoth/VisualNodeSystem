#include "VisualNodeArea.h"
using namespace VisNodeSys;

ImVec2 NodeArea::NeededShift = ImVec2();

NodeArea::NodeArea()
{
	SetSize(ImVec2(256, 256));
};

NodeArea::~NodeArea() 
{
	Clear();
}

ImVec2 NodeArea::GetSize() const
{
	return Size;
}

void NodeArea::SetSize(const ImVec2 NewValue)
{
	if (NewValue.x < 1 || NewValue.y < 1)
		return;

	Size = NewValue;
}

ImVec2 NodeArea::GetPosition() const
{
	return Position;
}

void NodeArea::SetPosition(const ImVec2 NewValue)
{
	if (NewValue.x < 0 || NewValue.y < 0)
		return;

	Position = NewValue;
}

void NodeArea::Update()
{
	if (!NODE_CORE.bIsInTestMode)
		InputUpdate();

	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		if (Nodes[i]->bShouldBeDestroyed)
		{
			DeleteNode(Nodes[i]);
			i--;
			break;
		}
	}

	ProcessSocketEventQueue();
	if (!NODE_CORE.bIsInTestMode)
		Render();
}

void NodeArea::SetMainContextMenuFunc(void(*Func)())
{
	MainContextMenuFunc = Func;
}

void NodeArea::Clear()
{
	bClearing = true;

	for (int i = 0; i < static_cast<int>(GroupComments.size()); i++)
	{
		DeleteGroupComment(GroupComments[i]);
		i--;
	}
	GroupComments.clear();
	SelectedGroupComments.clear();

	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		PropagateNodeEventsCallbacks(Nodes[i], DESTROYED);
		Nodes[i]->bCouldBeDestroyed = true;
		DeleteNode(Nodes[i]);
		i--;
	}
	Nodes.clear();
	SelectedNodes.clear();
	SelectedRerouteNodes.clear();
	
	RenderOffset = ImVec2(0, 0);
	NodeAreaWindow = nullptr;
	
	SocketLookingForConnection = nullptr;
	SocketHovered = nullptr;

	MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);

	bClearing = false;
}

void NodeArea::Reset()
{
	Clear();

	MainContextMenuFunc = nullptr;
	NodeEventsCallbacks.clear();
}

void NodeArea::AddNodeEventCallback(std::function<void(Node*, NODE_EVENT)> Func)
{
	if (Func != nullptr)
		NodeEventsCallbacks.push_back(Func);
}

void NodeArea::PropagateNodeEventsCallbacks(Node* Node, const NODE_EVENT EventToPropagate) const
{
	for (size_t i = 0; i < NodeEventsCallbacks.size(); i++)
	{
		if (NodeEventsCallbacks[i] != nullptr)
			NodeEventsCallbacks[i](Node, EventToPropagate);
	}
}

void NodeArea::SaveToFile(const char* FileName) const
{
	const std::string json_file = ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << json_file;
	SaveFile.close();
}

void NodeArea::SaveNodesToFile(const char* FileName, std::vector<Node*> Nodes)
{
	if (Nodes.empty())
		return;

	const NodeArea* NewNodeArea = NodeArea::CreateNodeArea(Nodes, std::vector<GroupComment*>());
	const std::string json_file = NewNodeArea->ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << json_file;
	SaveFile.close();
	delete NewNodeArea;
}

bool NodeArea::IsAlreadyConnected(NodeSocket* FirstSocket, NodeSocket* SecondSocket, const std::vector<Connection*>& Connections)
{
	for (size_t i = 0; i < Connections.size(); i++)
	{
		// if node is connected to node that is not in this list just ignore.
		if (Connections[i]->In == FirstSocket && Connections[i]->Out == SecondSocket)
			return true;
	}

	return false;
}

void NodeArea::ProcessConnections(const std::vector<NodeSocket*>& Sockets,
										std::unordered_map<NodeSocket*, NodeSocket*>& OldToNewSocket,
										NodeArea* TargetArea, size_t NodeShift, const std::vector<Node*>& SourceNodes)
{
	NodeArea* SourceArea = SourceNodes[0]->GetParentArea();

	for (size_t i = 0; i < Sockets.size(); i++)
	{
		NodeSocket* CurrentSocket = Sockets[i];
		for (const auto& ConnectedSocket : CurrentSocket->ConnectedSockets)
		{
			if (Node::IsNodeWithIDInList(ConnectedSocket->GetParent()->GetID(), SourceNodes))
			{
				// Check maybe we already establish this connection.
				if (!IsAlreadyConnected(OldToNewSocket[CurrentSocket], OldToNewSocket[ConnectedSocket], TargetArea->Connections))
				{
					std::unordered_map<RerouteNode*, RerouteNode*> OldToNewRerouteNode;
					// Get connection info from old node area.
					Connection* OldConnection = SourceArea->GetConnection(CurrentSocket, ConnectedSocket);

					if (!TargetArea->TryToConnect(OldToNewSocket[CurrentSocket]->GetParent(), OldToNewSocket[CurrentSocket]->GetID(), OldToNewSocket[ConnectedSocket]->GetParent(), OldToNewSocket[ConnectedSocket]->GetID()))
						continue;

					Connection* NewConnection = TargetArea->Connections.back();
					// First pass to fill OldToNewRerouteNode map and other information that does not depend on OldToNewRerouteNode map.
					for (size_t j = 0; j < OldConnection->RerouteNodes.size(); j++)
					{
						RerouteNode* OldReroute = OldConnection->RerouteNodes[j];
						RerouteNode* NewReroute = new RerouteNode();
						NewReroute->ID = NODE_CORE.GetUniqueHexID();
						NewReroute->Parent = NewConnection;
						NewReroute->Position = OldReroute->Position;

						if (OldReroute->BeginSocket != nullptr)
							NewReroute->BeginSocket = OldToNewSocket[OldReroute->BeginSocket];
						if (OldReroute->EndSocket != nullptr)
							NewReroute->EndSocket = OldToNewSocket[OldReroute->EndSocket];

						// Associate old to new
						OldToNewRerouteNode[OldConnection->RerouteNodes[j]] = NewReroute;

						NewConnection->RerouteNodes.push_back(NewReroute);
					}

					// Second pass to fill all other info.
					for (size_t j = 0; j < OldConnection->RerouteNodes.size(); j++)
					{
						RerouteNode* OldReroute = OldConnection->RerouteNodes[j];

						if (OldReroute->BeginReroute != nullptr)
							OldToNewRerouteNode[OldReroute]->BeginReroute = OldToNewRerouteNode[OldReroute->BeginReroute];
						if (OldReroute->EndReroute != nullptr)
							OldToNewRerouteNode[OldReroute]->EndReroute = OldToNewRerouteNode[OldReroute->EndReroute];
					}
				}
			}
		}
	}
}

void NodeArea::CopyNodesInternal(const std::vector<Node*>& SourceNodes, NodeArea* TargetArea, const size_t NodeShift)
{
	// Copy all nodes to new node area.
	std::unordered_map<Node*, Node*> OldToNewNode;
	std::unordered_map<NodeSocket*, NodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < SourceNodes.size(); i++)
	{
		Node* CopyOfNode = NODE_FACTORY.CopyNode(SourceNodes[i]->GetType(), *SourceNodes[i]);

		if (CopyOfNode == nullptr)
			CopyOfNode = new Node(*SourceNodes[i]);
		CopyOfNode->ParentArea = TargetArea;

		TargetArea->AddNode(CopyOfNode);

		// Associate old to new
		OldToNewNode[SourceNodes[i]] = CopyOfNode;

		for (size_t j = 0; j < SourceNodes[i]->Input.size(); j++)
		{
			OldToNewSocket[SourceNodes[i]->Input[j]] = CopyOfNode->Input[j];
		}

		for (size_t j = 0; j < SourceNodes[i]->Output.size(); j++)
		{
			OldToNewSocket[SourceNodes[i]->Output[j]] = CopyOfNode->Output[j];
		}
	}

	// Recreate all connections.
	for (size_t i = 0; i < SourceNodes.size(); i++)
	{
		ProcessConnections(SourceNodes[i]->Output, OldToNewSocket, TargetArea, NodeShift + i, SourceNodes);
	}
}

NodeArea* NodeArea::CreateNodeArea(const std::vector<Node*> Nodes, const std::vector<GroupComment*> GroupComments)
{
	NodeArea* NewArea = new NodeArea();
	CopyNodesInternal(Nodes, NewArea);

	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		GroupComment* CopyOfGroupComment = new GroupComment(*GroupComments[i]);
		NewArea->AddGroupComment(CopyOfGroupComment);
	}

	return NewArea;
}

std::string NodeArea::ToJson() const
{
	Json::Value root;
	std::ofstream SaveFile;

	Json::Value NodesData;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		NodesData[std::to_string(i)] = Nodes[i]->ToJson();
	}
	root["nodes"] = NodesData;

	Json::Value ConnectionsData;
	for (size_t i = 0; i < Connections.size(); i++)
	{
		ConnectionsData[std::to_string(i)]["in"]["socket_ID"] = Connections[i]->In->GetID();
		size_t socket_index = 0;
		for (size_t j = 0; j < Connections[i]->In->GetParent()->Input.size(); j++)
		{
			if (Connections[i]->In->GetParent()->Input[j]->GetID() == Connections[i]->In->GetID())
				socket_index = j;
		}
		ConnectionsData[std::to_string(i)]["in"]["socket_index"] = socket_index;
		ConnectionsData[std::to_string(i)]["in"]["node_ID"] = Connections[i]->In->GetParent()->GetID();

		ConnectionsData[std::to_string(i)]["out"]["socket_ID"] = Connections[i]->Out->GetID();
		socket_index = 0;
		for (size_t j = 0; j < Connections[i]->Out->GetParent()->Output.size(); j++)
		{
			if (Connections[i]->Out->GetParent()->Output[j]->GetID() == Connections[i]->Out->GetID())
				socket_index = j;
		}
		ConnectionsData[std::to_string(i)]["out"]["socket_index"] = socket_index;
		ConnectionsData[std::to_string(i)]["out"]["node_ID"] = Connections[i]->Out->GetParent()->GetID();

		for (size_t j = 0; j < Connections[i]->RerouteNodes.size(); j++)
		{
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["reroute_ID"] = Connections[i]->RerouteNodes[j]->ID;
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["position_x"] = Connections[i]->RerouteNodes[j]->Position.x;
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["position_y"] = Connections[i]->RerouteNodes[j]->Position.y;

			std::string BeginSocketID = "";
			if (Connections[i]->RerouteNodes[j]->BeginSocket)
				BeginSocketID = Connections[i]->RerouteNodes[j]->BeginSocket->GetID();
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["begin_socket_ID"] = BeginSocketID;

			std::string EndSocketID = "";
			if (Connections[i]->RerouteNodes[j]->EndSocket)
				EndSocketID = Connections[i]->RerouteNodes[j]->EndSocket->GetID();
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["end_socket_ID"] = EndSocketID;

			std::string BeginRerouteID = "";
			if (Connections[i]->RerouteNodes[j]->BeginReroute)
				BeginRerouteID = Connections[i]->RerouteNodes[j]->BeginReroute->ID;
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["begin_reroute_ID"] = BeginRerouteID;

			std::string EndRerouteID = "";
			if (Connections[i]->RerouteNodes[j]->EndReroute)
				EndRerouteID = Connections[i]->RerouteNodes[j]->EndReroute->ID;
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["end_reroute_ID"] = EndRerouteID;
		}
	}
	root["connections"] = ConnectionsData;

	Json::Value GroupCommentsData;
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		GroupCommentsData[std::to_string(i)] = GroupComments[i]->ToJson();
	}
	root["GroupComments"] = GroupCommentsData;

	root["renderOffset"]["x"] = RenderOffset.x;
	root["renderOffset"]["y"] = RenderOffset.y;

	Json::StreamWriterBuilder builder;
	const std::string JsonText = Json::writeString(builder, root);

	return JsonText;
}

void NodeArea::CopyNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea)
{
	const size_t NodeShift = TargetNodeArea->Nodes.size();
	CopyNodesInternal(SourceNodeArea->Nodes, TargetNodeArea, NodeShift);

	for (size_t i = 0; i < SourceNodeArea->GroupComments.size(); i++)
	{
		GroupComment* CopyOfGroupComment = new GroupComment(*SourceNodeArea->GroupComments[i]);
		TargetNodeArea->AddGroupComment(CopyOfGroupComment);
	}
}

void NodeArea::LoadFromJson(std::string JsonText)
{
	if (JsonText.find("{") == std::string::npos || JsonText.find("}") == std::string::npos || JsonText.find(":") == std::string::npos)
		return;

	Json::Value root;
	JSONCPP_STRING err;
	Json::CharReaderBuilder builder;

	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(JsonText.c_str(), JsonText.c_str() + JsonText.size(), &root, &err))
		return;

	if (!root.isMember("nodes"))
		return;

	std::unordered_map<std::string, Node*> LoadedNodes;
	std::vector<Json::String> NodesList = root["nodes"].getMemberNames();
	for (size_t i = 0; i < NodesList.size(); i++)
	{
		std::string NodeType = root["nodes"][std::to_string(i)]["nodeType"].asCString();
		Node* NewNode = NODE_FACTORY.CreateNode(NodeType);
		if (NewNode == nullptr)
		{
			if (NodeType == "VisualNode")
			{
				NewNode = new Node();
			}
			else
			{
				continue;
			}
		}

		NewNode->FromJson(root["nodes"][std::to_string(i)]);

		if (NewNode != nullptr)
		{
			LoadedNodes[NewNode->GetID()] = NewNode;
			AddNode(NewNode);
		}
	}

	std::vector<Json::String> ConnectionsList = root["connections"].getMemberNames();
	for (size_t i = 0; i < ConnectionsList.size(); i++)
	{
		std::string InSocketID = root["connections"][ConnectionsList[i]]["in"]["socket_ID"].asCString();
		std::string InNodeID = root["connections"][ConnectionsList[i]]["in"]["node_ID"].asCString();

		std::string OutSocketID = root["connections"][ConnectionsList[i]]["out"]["socket_ID"].asCString();
		std::string OutNodeID = root["connections"][ConnectionsList[i]]["out"]["node_ID"].asCString();

		if (LoadedNodes.find(OutNodeID) != LoadedNodes.end() && LoadedNodes.find(InNodeID) != LoadedNodes.end())
			if (!TryToConnect(LoadedNodes[OutNodeID], OutSocketID, LoadedNodes[InNodeID], InSocketID))
				continue;

		Connection* NewConnection = Connections.back();

		// First pass to fill information that does not depend other reroutes.
		std::vector<Json::String> RerouteList = root["connections"][ConnectionsList[i]]["reroute_connections"].getMemberNames();
		for (size_t j = 0; j < RerouteList.size(); j++)
		{
			RerouteNode* NewReroute = new RerouteNode();
			std::string ID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["reroute_ID"].asCString();
			NewReroute->ID = ID;
			NewReroute->Parent = NewConnection;

			NewReroute->Position.x = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["position_x"].asFloat();
			NewReroute->Position.y = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["position_y"].asFloat();

			NewConnection->RerouteNodes.push_back(NewReroute);
		}

		// Second pass to fill pointers.
		for (size_t j = 0; j < RerouteList.size(); j++)
		{
			std::string BeginSocketID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["begin_socket_ID"].asCString();
			if (BeginSocketID != "")
			{
				NodeSocket* BeginSocket = NewConnection->Out;
				if (BeginSocketID == BeginSocket->GetID())
					NewConnection->RerouteNodes[j]->BeginSocket = BeginSocket;
			}

			std::string EndSocketID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["end_socket_ID"].asCString();
			if (EndSocketID != "")
			{
				NodeSocket* EndSocket = NewConnection->In;
				if (EndSocketID == EndSocket->GetID())
					NewConnection->RerouteNodes[j]->EndSocket = EndSocket;
			}

			std::string BeginRerouteID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["begin_reroute_ID"].asCString();
			if (BeginRerouteID != "")
			{
				RerouteNode* BeginReroute = nullptr;
				for (size_t k = 0; k < NewConnection->RerouteNodes.size(); k++)
				{
					if (BeginRerouteID == NewConnection->RerouteNodes[k]->ID)
						BeginReroute = NewConnection->RerouteNodes[k];
				}

				if (BeginReroute != nullptr)
					NewConnection->RerouteNodes[j]->BeginReroute = BeginReroute;
			}

			std::string EndRerouteID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["end_reroute_ID"].asCString();
			if (EndRerouteID != "")
			{
				RerouteNode* EndReroute = nullptr;
				for (size_t k = 0; k < NewConnection->RerouteNodes.size(); k++)
				{
					if (EndRerouteID == NewConnection->RerouteNodes[k]->ID)
						EndReroute = NewConnection->RerouteNodes[k];
				}

				if (EndReroute != nullptr)
					NewConnection->RerouteNodes[j]->EndReroute = EndReroute;
			}
		}
	}

	if (root.isMember("GroupComments"))
	{
		std::vector<Json::String> GroupCommentsList = root["GroupComments"].getMemberNames();
		for (size_t i = 0; i < GroupCommentsList.size(); i++)
		{
			GroupComment* NewGroupComment = new GroupComment();
			NewGroupComment->FromJson(root["GroupComments"][std::to_string(i)]);
			AddGroupComment(NewGroupComment);
		}
	}

	if (root.isMember("renderOffset"))
	{
		float OffsetX = root["renderOffset"]["x"].asFloat();
		float OffsetY = root["renderOffset"]["y"].asFloat();
		SetRenderOffset(ImVec2(OffsetX, OffsetY));
	}
}

void NodeArea::LoadFromFile(const char* FileName)
{
	std::ifstream NodesFile;
	NodesFile.open(FileName);

	const std::string FileData((std::istreambuf_iterator<char>(NodesFile)), std::istreambuf_iterator<char>());
	NodesFile.close();

	LoadFromJson(FileData);
}

Node* NodeArea::GetNodeByID(std::string NodeID) const
{
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetID() == NodeID)
			return Nodes[i];
	}

	return nullptr;
}

std::vector<Node*> NodeArea::GetNodesByName(const std::string NodeName) const
{
	std::vector<Node*> Result;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetName() == NodeName)
			Result.push_back(Nodes[i]);
	}

	return Result;
}

std::vector<Node*> NodeArea::GetNodesByType(const std::string NodeType) const
{
	std::vector<Node*> Result;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetType() == NodeType)
			Result.push_back(Nodes[i]);
	}

	return Result;
}

size_t NodeArea::GetNodeCount() const
{
	return Nodes.size();
}

bool NodeArea::IsEmptyOrFilledByNulls(const std::vector<Node*> Vector)
{
	for (size_t i = 0; i < Vector.size(); i++)
	{
		if (Vector[i] != nullptr)
			return false;
	}

	return true;
}

ConnectionStyle* NodeArea::GetConnectionStyle(const NodeSocket* ParticipantOfConnection) const
{
	if (ParticipantOfConnection == nullptr)
		return nullptr;

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i]->In == ParticipantOfConnection || Connections[i]->Out == ParticipantOfConnection)
			return &Connections[i]->Style;
	}

	return nullptr;
}

std::vector<ConnectionSegment> NodeArea::GetConnectionSegments(const Connection* Connection) const
{
	std::vector<ConnectionSegment> Result;
	ConnectionSegment CurrentSegment;
	
	ImVec2 BeginPosition;
	ImVec2 EndPosition;

	if (Connection->RerouteNodes.empty())
	{
		BeginPosition = SocketToPosition(Connection->Out);
		EndPosition = SocketToPosition(Connection->In);

		CurrentSegment.Begin = BeginPosition;
		CurrentSegment.BeginSocket = Connection->Out;
		CurrentSegment.End = EndPosition;
		CurrentSegment.EndSocket = Connection->In;
		Result.push_back(CurrentSegment);
	}
	else
	{
		for (size_t i = 0; i < Connection->RerouteNodes.size(); i++)
		{	
			// First we will add segment from start to current reroute
			if (Connection->RerouteNodes[i]->BeginSocket != nullptr)
			{
				BeginPosition = SocketToPosition(Connection->RerouteNodes[i]->BeginSocket);
				CurrentSegment.BeginSocket = Connection->RerouteNodes[i]->BeginSocket;
			}
			else
			{
				BeginPosition = LocalToScreen(Connection->RerouteNodes[i]->BeginReroute->Position);
				CurrentSegment.BeginReroute = Connection->RerouteNodes[i]->BeginReroute;
			}
			
			EndPosition = LocalToScreen(Connection->RerouteNodes[i]->Position);
			CurrentSegment.EndReroute = Connection->RerouteNodes[i];

			CurrentSegment.Begin = BeginPosition;
			CurrentSegment.End = EndPosition;
			Result.push_back(CurrentSegment);

			// Than we will add segment from current reroute to end, only if it is last reroute
			if (i == Connection->RerouteNodes.size() - 1)
			{
				CurrentSegment = ConnectionSegment();
				BeginPosition = LocalToScreen(Connection->RerouteNodes[i]->Position);
				CurrentSegment.BeginReroute = Connection->RerouteNodes[i];

				EndPosition = SocketToPosition(Connection->RerouteNodes[i]->EndSocket);
				CurrentSegment.EndSocket = Connection->RerouteNodes[i]->EndSocket;

				CurrentSegment.Begin = BeginPosition;
				CurrentSegment.End = EndPosition;
				Result.push_back(CurrentSegment);
			}
		}
	}

	return Result;
}

ImVec2 NodeArea::LocalToScreen(ImVec2 LocalPosition) const
{
	ImVec2 WindowPosition = ImVec2(0.0f, 0.0f);
	if (GetCurrentWindowImpl() != nullptr)
		WindowPosition = GetCurrentWindowImpl()->Pos;

	return WindowPosition + LocalPosition * Zoom + RenderOffset;
}

ImVec2 NodeArea::ScreenToLocal(ImVec2 ScreenPosition) const
{
	ImVec2 WindowPosition = ImVec2(0.0f, 0.0f);
	if (GetCurrentWindowImpl() != nullptr)
		WindowPosition = GetCurrentWindowImpl()->Pos;

	return (ScreenPosition - WindowPosition - RenderOffset) / Zoom;
}

std::vector<ImVec2> NodeArea::GetTangentsForLine(const ImVec2 Begin, const ImVec2 End) const
{
	std::vector<ImVec2> Result;
	Result.resize(2);

	float ScaledXTangentMagnitude = Settings.Style.GeneralConnection.LineXTangentMagnitude * Zoom;
	float ScaledYTangentMagnitude = Settings.Style.GeneralConnection.LineYTangentMagnitude * Zoom;

	Result[0] = ImVec2(ScaledXTangentMagnitude, ScaledYTangentMagnitude);
	Result[1] = ImVec2(ScaledXTangentMagnitude, ScaledYTangentMagnitude);

	if (Begin.x >= End.x && Begin.y >= End.y)
	{
		Result[0] = ImVec2(-ScaledXTangentMagnitude, ScaledYTangentMagnitude);
		Result[1] = ImVec2(-ScaledXTangentMagnitude, ScaledYTangentMagnitude);
	}

	return Result;
}

bool NodeArea::IsRectsOverlaping(ImVec2 FirstRectMin, ImVec2 FirstRectSize, ImVec2 SecondRectMin, ImVec2 SecondRectSize)
{
	if (FirstRectMin.x < (SecondRectMin.x + SecondRectSize.x) &&
		(FirstRectMin.x + FirstRectSize.x) > SecondRectMin.x &&
		FirstRectMin.y < (SecondRectMin.y + SecondRectSize.y) &&
		(FirstRectMin.y + FirstRectSize.y) > SecondRectMin.y)
	{
		return true;
	}

	return false;
}

bool NodeArea::IsSecondRectInsideFirstOne(ImVec2 FirstRectMin, ImVec2 FirstRectSize, ImVec2 SecondRectMin, ImVec2 SecondRectSize) const
{
	if (SecondRectMin.x >= FirstRectMin.x &&
	   (SecondRectMin.x + SecondRectSize.x) <= (FirstRectMin.x + FirstRectSize.x) &&
	   SecondRectMin.y >= FirstRectMin.y &&
	   (SecondRectMin.y + SecondRectSize.y) <= (FirstRectMin.y + FirstRectSize.y))
	{
		return true;
	}

	return false;
}

ImGuiWindow* NodeArea::GetCurrentWindowImpl() const
{
	ImGuiContext* Context = ImGui::GetCurrentContext();
	if (Context == nullptr)
		return nullptr;

	return Context->CurrentWindow;
}