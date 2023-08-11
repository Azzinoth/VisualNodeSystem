#include "VisualNodeArea.h"

ImVec2 VisualNodeArea::NeededShift = ImVec2();

VisualNodeArea::VisualNodeArea()
{
	SetAreaSize(ImVec2(256, 256));
};

VisualNodeArea::~VisualNodeArea() 
{
	Clear();
}

ImVec2 VisualNodeArea::GetAreaSize() const
{
	return AreaSize;
}

void VisualNodeArea::SetAreaSize(const ImVec2 NewValue)
{
	if (NewValue.x < 1 || NewValue.y < 1)
		return;

	AreaSize = NewValue;
}

ImVec2 VisualNodeArea::GetAreaPosition() const
{
	return AreaPosition;
}

void VisualNodeArea::SetAreaPosition(const ImVec2 NewValue)
{
	if (NewValue.x < 0 || NewValue.y < 0)
		return;

	AreaPosition = NewValue;
}

void VisualNodeArea::Update()
{
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
	Render();
}

void VisualNodeArea::SetMainContextMenuFunc(void(*Func)())
{
	MainContextMenuFunc = Func;
}

void VisualNodeArea::Clear()
{
	bClearing = true;

	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		PropagateNodeEventsCallbacks(Nodes[i], VISUAL_NODE_DESTROYED);
		Nodes[i]->bCouldBeDestroyed = true;
		DeleteNode(Nodes[i]);
		i--;
	}

	RenderOffset = ImVec2(0, 0);
	NodeAreaWindow = nullptr;
	SelectedNodes.clear();
	SocketLookingForConnection = nullptr;
	SocketHovered = nullptr;

	MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);

	bClearing = false;
}

void VisualNodeArea::Reset()
{
	Clear();

	MainContextMenuFunc = nullptr;
	NodeEventsCallbacks.clear();
}

void VisualNodeArea::SetNodeEventCallback(void(*Func)(VisualNode*, VISUAL_NODE_EVENT))
{
	if (Func != nullptr)
		NodeEventsCallbacks.push_back(Func);
}

void VisualNodeArea::PropagateNodeEventsCallbacks(VisualNode* Node, const VISUAL_NODE_EVENT EventToPropagate) const
{
	for (size_t i = 0; i < NodeEventsCallbacks.size(); i++)
	{
		if (NodeEventsCallbacks[i] != nullptr)
			NodeEventsCallbacks[i](Node, EventToPropagate);
	}
}

void VisualNodeArea::SaveToFile(const char* FileName) const
{
	const std::string json_file = ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << json_file;
	SaveFile.close();
}

bool VisualNodeArea::IsNodeIDInList(const std::string ID, const std::vector<VisualNode*> List)
{
	for (size_t i = 0; i < List.size(); i++)
	{
		if (List[i]->GetID() == ID)
			return true;
	}

	return false;
}

void VisualNodeArea::SaveNodesToFile(const char* FileName, std::vector<VisualNode*> Nodes)
{
	if (Nodes.empty())
		return;

	const VisualNodeArea* NewNodeArea = VisualNodeArea::CreateNodeArea(Nodes);
	const std::string json_file = NewNodeArea->ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << json_file;
	SaveFile.close();
	delete NewNodeArea;
}

bool VisualNodeArea::IsAlreadyConnected(NodeSocket* FirstSocket, NodeSocket* SecondSocket, const std::vector<VisualNodeConnection*>& Connections)
{
	for (size_t i = 0; i < Connections.size(); i++)
	{
		// if node is connected to node that is not in this list just ignore.
		if (Connections[i]->In == FirstSocket && Connections[i]->Out == SecondSocket)
			return true;
	}

	return false;
}

void VisualNodeArea::ProcessConnections(const std::vector<NodeSocket*>& Sockets,
										std::unordered_map<NodeSocket*, NodeSocket*>& OldToNewSocket,
										VisualNodeArea* TargetArea, size_t NodeShift, const std::vector<VisualNode*>& SourceNodes)
{
	VisualNodeArea* SourceArea = SourceNodes[0]->GetParentArea();

	for (size_t i = 0; i < Sockets.size(); i++)
	{
		NodeSocket* CurrentSocket = Sockets[i];
		for (const auto& ConnectedSocket : CurrentSocket->ConnectedSockets)
		{
			if (IsNodeIDInList(ConnectedSocket->GetParent()->GetID(), SourceNodes))
			{
				// Record the connection from the perspective of the output node
				TargetArea->Nodes[NodeShift]->Output[i]->ConnectedSockets.push_back(OldToNewSocket[ConnectedSocket]);

				// Check maybe we already establish this connection.
				if (!IsAlreadyConnected(OldToNewSocket[CurrentSocket], OldToNewSocket[ConnectedSocket], TargetArea->Connections))
				{
					std::unordered_map<VisualNodeRerouteNode*, VisualNodeRerouteNode*> OldToNewRerouteNode;
					// Get connection info from old node area.
					VisualNodeConnection* OldConnection = SourceArea->GetAllConnections(CurrentSocket, ConnectedSocket);

					TargetArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[CurrentSocket], OldToNewSocket[ConnectedSocket]));
					VisualNodeConnection* NewConnection = TargetArea->Connections.back();

					// First pass to fill OldToNewRerouteNode map and other information that does not depend on OldToNewRerouteNode map.
					for (size_t j = 0; j < OldConnection->RerouteConnections.size(); j++)
					{
						VisualNodeRerouteNode* OldReroute = OldConnection->RerouteConnections[j];
						VisualNodeRerouteNode* NewReroute = new VisualNodeRerouteNode();
						NewReroute->ID = APPLICATION.GetUniqueHexID();
						NewReroute->Parent = NewConnection;
						NewReroute->Position = OldReroute->Position;

						if (OldReroute->BeginSocket != nullptr)
							NewReroute->BeginSocket = OldToNewSocket[OldReroute->BeginSocket];
						if (OldReroute->EndSocket != nullptr)
							NewReroute->EndSocket = OldToNewSocket[OldReroute->EndSocket];

						// Associate old to new
						OldToNewRerouteNode[OldConnection->RerouteConnections[j]] = NewReroute;

						NewConnection->RerouteConnections.push_back(NewReroute);
					}

					// Second pass to fill all other info.
					for (size_t j = 0; j < OldConnection->RerouteConnections.size(); j++)
					{
						VisualNodeRerouteNode* OldReroute = OldConnection->RerouteConnections[j];

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

void VisualNodeArea::CopyNodesInternal(const std::vector<VisualNode*>& SourceNodes, VisualNodeArea* TargetArea, const size_t NodeShift)
{
	// Copy all nodes to new node area.
	std::unordered_map<VisualNode*, VisualNode*> OldToNewNode;
	std::unordered_map<NodeSocket*, NodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < SourceNodes.size(); i++)
	{
		VisualNode* CopyOfNode = NODE_FACTORY.CopyNode(SourceNodes[i]->GetType(), *SourceNodes[i]);

		if (CopyOfNode == nullptr)
			CopyOfNode = new VisualNode(*SourceNodes[i]);
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


VisualNodeArea* VisualNodeArea::CreateNodeArea(const std::vector<VisualNode*> Nodes)
{
	VisualNodeArea* NewArea = new VisualNodeArea();
	CopyNodesInternal(Nodes, NewArea);

	return NewArea;
}

std::string VisualNodeArea::ToJson() const
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

		for (size_t j = 0; j < Connections[i]->RerouteConnections.size(); j++)
		{
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["reroute_ID"] = Connections[i]->RerouteConnections[j]->ID;
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["position_x"] = Connections[i]->RerouteConnections[j]->Position.x;
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["position_y"] = Connections[i]->RerouteConnections[j]->Position.y;

			std::string BeginSocketID = "";
			if (Connections[i]->RerouteConnections[j]->BeginSocket)
				BeginSocketID = Connections[i]->RerouteConnections[j]->BeginSocket->GetID();
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["begin_socket_ID"] = BeginSocketID;

			std::string EndSocketID = "";
			if (Connections[i]->RerouteConnections[j]->EndSocket)
				EndSocketID = Connections[i]->RerouteConnections[j]->EndSocket->GetID();
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["end_socket_ID"] = EndSocketID;

			std::string BeginRerouteID = "";
			if (Connections[i]->RerouteConnections[j]->BeginReroute)
				BeginRerouteID = Connections[i]->RerouteConnections[j]->BeginReroute->ID;
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["begin_reroute_ID"] = BeginRerouteID;

			std::string EndRerouteID = "";
			if (Connections[i]->RerouteConnections[j]->EndReroute)
				EndRerouteID = Connections[i]->RerouteConnections[j]->EndReroute->ID;
			ConnectionsData[std::to_string(i)]["reroute_connections"][std::to_string(j)]["end_reroute_ID"] = EndRerouteID;
		}
	}
	root["connections"] = ConnectionsData;

	root["renderOffset"]["x"] = RenderOffset.x;
	root["renderOffset"]["y"] = RenderOffset.y;

	Json::StreamWriterBuilder builder;
	const std::string JsonText = Json::writeString(builder, root);

	return JsonText;
}

VisualNodeArea* VisualNodeArea::FromJson(std::string JsonText)
{
	VisualNodeArea* NewArea = new VisualNodeArea();

	if (JsonText.find("{") == std::string::npos || JsonText.find("}") == std::string::npos || JsonText.find(":") == std::string::npos)
		return NewArea;

	Json::Value root;
	JSONCPP_STRING err;
	Json::CharReaderBuilder builder;

	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(JsonText.c_str(), JsonText.c_str() + JsonText.size(), &root, &err))
		return NewArea;

	if (!root.isMember("nodes"))
		return NewArea;

	std::unordered_map<std::string, VisualNode*> LoadedNodes;
	std::vector<Json::String> NodesList = root["nodes"].getMemberNames();
	for (size_t i = 0; i < NodesList.size(); i++)
	{
		std::string NodeType = root["nodes"][std::to_string(i)]["nodeType"].asCString();
		VisualNode* NewNode = NODE_FACTORY.CreateNode(NodeType);
		if (NewNode == nullptr)
		{
			if (NodeType == "VisualNode")
			{
				NewNode = new VisualNode();
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
			NewArea->AddNode(NewNode);
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
			if (!NewArea->TryToConnect(LoadedNodes[OutNodeID], OutSocketID, LoadedNodes[InNodeID], InSocketID))
				continue;

		VisualNodeConnection* NewConnection = NewArea->Connections.back();

		// First pass to fill information that does not depend other reroutes.
		std::vector<Json::String> RerouteList = root["connections"][ConnectionsList[i]]["reroute_connections"].getMemberNames();
		for (size_t j = 0; j < RerouteList.size(); j++)
		{
			VisualNodeRerouteNode* NewReroute = new VisualNodeRerouteNode();
			std::string ID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["reroute_ID"].asCString();
			NewReroute->ID = ID;
			NewReroute->Parent = NewConnection;

			NewReroute->Position.x = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["position_x"].asFloat();
			NewReroute->Position.y = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["position_y"].asFloat();

			NewConnection->RerouteConnections.push_back(NewReroute);
		}

		// Second pass to fill pointers.
		for (size_t j = 0; j < RerouteList.size(); j++)
		{
			std::string BeginSocketID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["begin_socket_ID"].asCString();
			if (BeginSocketID != "")
			{
				NodeSocket* BeginSocket = NewConnection->Out;
				if (BeginSocketID == BeginSocket->GetID())
					NewConnection->RerouteConnections[j]->BeginSocket = BeginSocket;
			}

			std::string EndSocketID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["end_socket_ID"].asCString();
			if (EndSocketID != "")
			{
				NodeSocket* EndSocket = NewConnection->In;
				if (EndSocketID == EndSocket->GetID())
					NewConnection->RerouteConnections[j]->EndSocket = EndSocket;
			}

			std::string BeginRerouteID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["begin_reroute_ID"].asCString();
			if (BeginRerouteID != "")
			{
				VisualNodeRerouteNode* BeginReroute = nullptr;
				for (size_t k = 0; k < NewConnection->RerouteConnections.size(); k++)
				{
					if (BeginRerouteID == NewConnection->RerouteConnections[k]->ID)
						BeginReroute = NewConnection->RerouteConnections[k];
				}
				
				if (BeginReroute != nullptr)
					NewConnection->RerouteConnections[j]->BeginReroute = BeginReroute;
			}

			std::string EndRerouteID = root["connections"][ConnectionsList[i]]["reroute_connections"][std::to_string(j)]["end_reroute_ID"].asCString();
			if (EndRerouteID != "")
			{
				VisualNodeRerouteNode* EndReroute = nullptr;
				for (size_t k = 0; k < NewConnection->RerouteConnections.size(); k++)
				{
					if (EndRerouteID == NewConnection->RerouteConnections[k]->ID)
						EndReroute = NewConnection->RerouteConnections[k];
				}

				if (EndReroute != nullptr)
					NewConnection->RerouteConnections[j]->EndReroute = EndReroute;
			}
		}
	}

	if (root.isMember("renderOffset"))
	{
		float OffsetX = root["renderOffset"]["x"].asFloat();
		float OffsetY = root["renderOffset"]["y"].asFloat();
		NewArea->SetAreaRenderOffset(ImVec2(OffsetX, OffsetY));
	}

	return NewArea;
}

void VisualNodeArea::CopyNodesTo(VisualNodeArea* SourceNodeArea, VisualNodeArea* TargetNodeArea)
{
	const size_t NodeShift = TargetNodeArea->Nodes.size();
	CopyNodesInternal(SourceNodeArea->Nodes, TargetNodeArea, NodeShift);
}

void VisualNodeArea::LoadFromFile(const char* FileName)
{
	std::ifstream NodesFile;
	NodesFile.open(FileName);

	const std::string FileData((std::istreambuf_iterator<char>(NodesFile)), std::istreambuf_iterator<char>());
	NodesFile.close();

	VisualNodeArea* NewNodeArea = VisualNodeArea::FromJson(FileData);
	VisualNodeArea::CopyNodesTo(NewNodeArea, this);
	delete NewNodeArea;
}

std::vector<VisualNode*> VisualNodeArea::GetNodesByName(const std::string NodeName) const
{
	std::vector<VisualNode*> result;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetName() == NodeName)
			result.push_back(Nodes[i]);
	}

	return result;
}

std::vector<VisualNode*> VisualNodeArea::GetNodesByType(const std::string NodeType) const
{
	std::vector<VisualNode*> result;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetType() == NodeType)
			result.push_back(Nodes[i]);
	}

	return result;
}

int VisualNodeArea::GetNodeCount() const
{
	return static_cast<int>(Nodes.size());
}

bool VisualNodeArea::EmptyOrFilledByNulls(const std::vector<VisualNode*> Vector)
{
	for (size_t i = 0; i < Vector.size(); i++)
	{
		if (Vector[i] != nullptr)
			return false;
	}

	return true;
}

VisualNodeConnectionStyle* VisualNodeArea::GetConnectionStyle(const NodeSocket* ParticipantOfConnection) const
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

std::vector<VisualNodeConnectionSegment> VisualNodeArea::GetConnectionSegments(const VisualNodeConnection* Connection) const
{
	std::vector<VisualNodeConnectionSegment> Result;
	VisualNodeConnectionSegment CurrentSegment;
	
	ImVec2 BeginPosition;
	ImVec2 EndPosition;

	if (Connection->RerouteConnections.empty())
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
		for (size_t i = 0; i < Connection->RerouteConnections.size(); i++)
		{	
			// First we will add segment from start to current reroute
			if (Connection->RerouteConnections[i]->BeginSocket != nullptr)
			{
				BeginPosition = SocketToPosition(Connection->RerouteConnections[i]->BeginSocket);
				CurrentSegment.BeginSocket = Connection->RerouteConnections[i]->BeginSocket;
			}
			else
			{
				BeginPosition = LocalToScreen(Connection->RerouteConnections[i]->BeginReroute->Position);
				CurrentSegment.BeginReroute = Connection->RerouteConnections[i]->BeginReroute;
			}
			
			EndPosition = LocalToScreen(Connection->RerouteConnections[i]->Position);
			CurrentSegment.EndReroute = Connection->RerouteConnections[i];

			CurrentSegment.Begin = BeginPosition;
			CurrentSegment.End = EndPosition;
			Result.push_back(CurrentSegment);

			// Than we will add segment from current reroute to end, only if it is last reroute
			if (i == Connection->RerouteConnections.size() - 1)
			{
				CurrentSegment = VisualNodeConnectionSegment();
				BeginPosition = LocalToScreen(Connection->RerouteConnections[i]->Position);
				CurrentSegment.BeginReroute = Connection->RerouteConnections[i]/*->BeginReroute*/;

				EndPosition = SocketToPosition(Connection->RerouteConnections[i]->EndSocket);
				CurrentSegment.EndSocket = Connection->RerouteConnections[i]->EndSocket;

				CurrentSegment.Begin = BeginPosition;
				CurrentSegment.End = EndPosition;
				Result.push_back(CurrentSegment);
			}
		}
	}

	return Result;
}

ImVec2 VisualNodeArea::LocalToScreen(ImVec2 LocalPosition) const
{
	return ImGui::GetCurrentWindow()->Pos + LocalPosition * Zoom + RenderOffset;
}

ImVec2 VisualNodeArea::ScreenToLocal(ImVec2 ScreenPosition) const
{
	return (ScreenPosition - ImGui::GetCurrentWindow()->Pos - RenderOffset) / Zoom;
}