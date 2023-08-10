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

VisualNodeArea* VisualNodeArea::CreateNodeArea(const std::vector<VisualNode*> Nodes)
{
	VisualNodeArea* NewArea = new VisualNodeArea();

	// Copy all nodes to new node area.
	std::unordered_map<VisualNode*, VisualNode*> OldToNewNode;
	std::unordered_map<NodeSocket*, NodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		//VisualNode* CopyOfNode = VisualNode::CopyChild(Nodes[i]->GetType(), Nodes[i]);
		VisualNode* CopyOfNode = NODE_FACTORY.CopyNode(Nodes[i]->GetType(), *Nodes[i]);

		if (CopyOfNode == nullptr)
			CopyOfNode = new VisualNode(*Nodes[i]);
		CopyOfNode->ParentArea = NewArea;

		//newArea->nodes.push_back(copyOfNode);
		NewArea->AddNode(CopyOfNode);

		// Associate old to new IDs
		OldToNewNode[Nodes[i]] = CopyOfNode;

		for (size_t j = 0; j < Nodes[i]->Input.size(); j++)
		{
			OldToNewSocket[Nodes[i]->Input[j]] = CopyOfNode->Input[j];
		}

		for (size_t j = 0; j < Nodes[i]->Output.size(); j++)
		{
			OldToNewSocket[Nodes[i]->Output[j]] = CopyOfNode->Output[j];
		}
	}

	// Than we need to recreate all connections.
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		for (size_t j = 0; j < Nodes[i]->Input.size(); j++)
		{
			for (size_t k = 0; k < Nodes[i]->Input[j]->SocketConnected.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (IsNodeIDInList(Nodes[i]->Input[j]->SocketConnected[k]->GetParent()->GetID(), Nodes))
				{
					NewArea->Nodes[i]->Input[j]->SocketConnected.push_back(OldToNewSocket[Nodes[i]->Input[j]->SocketConnected[k]]);

					// Add connection to node area.
					// Maybe we already establish this connection.
					bool bShouldAdd = true;
					for (size_t l = 0; l < NewArea->Connections.size(); l++)
					{
						if (NewArea->Connections[l]->In == OldToNewSocket[Nodes[i]->Input[j]] &&
							NewArea->Connections[l]->Out == OldToNewSocket[Nodes[i]->Input[j]->SocketConnected[k]])
						{
							bShouldAdd = false;
							break;
						}
					}

					if (bShouldAdd)
						NewArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[Nodes[i]->Input[j]->SocketConnected[k]], OldToNewSocket[Nodes[i]->Input[j]]));
				}
			}
		}

		for (size_t j = 0; j < Nodes[i]->Output.size(); j++)
		{
			for (size_t k = 0; k < Nodes[i]->Output[j]->SocketConnected.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (IsNodeIDInList(Nodes[i]->Output[j]->SocketConnected[k]->GetParent()->GetID(), Nodes))
				{
					NewArea->Nodes[i]->Output[j]->SocketConnected.push_back(OldToNewSocket[Nodes[i]->Output[j]->SocketConnected[k]]);

					// Add connection to node area.
					// Maybe we already establish this connection.
					bool bShouldAdd = true;
					for (size_t l = 0; l < NewArea->Connections.size(); l++)
					{
						if (NewArea->Connections[l]->In == OldToNewSocket[Nodes[i]->Output[j]->SocketConnected[k]] &&
							NewArea->Connections[l]->Out == OldToNewSocket[Nodes[i]->Output[j]])
						{
							bShouldAdd = false;
							break;
						}
					}

					if (bShouldAdd)
						NewArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[Nodes[i]->Output[j]], OldToNewSocket[Nodes[i]->Output[j]->SocketConnected[k]]));
				}
			}
		}
	}

	return NewArea;
}

std::string VisualNodeArea::ToJson() const
{
	Json::Value root;
	std::ofstream SaveFile;

	Json::Value NodesData;
	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		NodesData[std::to_string(i)] = Nodes[i]->ToJson();
	}
	root["nodes"] = NodesData;

	Json::Value ConnectionsData;
	for (int i = 0; i < static_cast<int>(Connections.size()); i++)
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
			NewArea->TryToConnect(LoadedNodes[OutNodeID], OutSocketID, LoadedNodes[InNodeID], InSocketID);
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

	// Copy all nodes to new node area.
	std::unordered_map<VisualNode*, VisualNode*> OldToNewNode;
	std::unordered_map<NodeSocket*, NodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < SourceNodeArea->Nodes.size(); i++)
	{
		VisualNode* CopyOfNode = NODE_FACTORY.CopyNode(SourceNodeArea->Nodes[i]->GetType(), *SourceNodeArea->Nodes[i]);
		if (CopyOfNode == nullptr)
			CopyOfNode = new VisualNode(*SourceNodeArea->Nodes[i]);
		CopyOfNode->ParentArea = SourceNodeArea;

		TargetNodeArea->AddNode(CopyOfNode);

		// Associate old to new IDs
		OldToNewNode[SourceNodeArea->Nodes[i]] = CopyOfNode;

		for (size_t j = 0; j < SourceNodeArea->Nodes[i]->Input.size(); j++)
		{
			OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]] = CopyOfNode->Input[j];
		}

		for (size_t j = 0; j < SourceNodeArea->Nodes[i]->Output.size(); j++)
		{
			OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]] = CopyOfNode->Output[j];
		}
	}

	// Than we need to recreate all connections.
	for (size_t i = 0; i < SourceNodeArea->Nodes.size(); i++)
	{
		for (size_t j = 0; j < SourceNodeArea->Nodes[i]->Input.size(); j++)
		{
			for (size_t k = 0; k < SourceNodeArea->Nodes[i]->Input[j]->SocketConnected.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (IsNodeIDInList(SourceNodeArea->Nodes[i]->Input[j]->SocketConnected[k]->GetParent()->GetID(), SourceNodeArea->Nodes))
				{
					TargetNodeArea->Nodes[NodeShift + i]->Input[j]->SocketConnected.push_back(OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]->SocketConnected[k]]);

					// Add connection to node area.
					// Maybe we already establish this connection.
					bool bShouldAdd = true;
					for (size_t l = 0; l < TargetNodeArea->Connections.size(); l++)
					{
						if (TargetNodeArea->Connections[l]->In == OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]] &&
							TargetNodeArea->Connections[l]->Out == OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]->SocketConnected[k]])
						{
							bShouldAdd = false;
							break;
						}
					}

					if (bShouldAdd)
						TargetNodeArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]->SocketConnected[k]], OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]]));
				}
			}
		}

		for (size_t j = 0; j < SourceNodeArea->Nodes[i]->Output.size(); j++)
		{
			for (size_t k = 0; k < SourceNodeArea->Nodes[i]->Output[j]->SocketConnected.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (IsNodeIDInList(SourceNodeArea->Nodes[i]->Output[j]->SocketConnected[k]->GetParent()->GetID(), SourceNodeArea->Nodes))
				{
					TargetNodeArea->Nodes[NodeShift + i]->Output[j]->SocketConnected.push_back(OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]->SocketConnected[k]]);

					// Add connection to node area.
					// Maybe we already establish this connection.
					bool bShouldAdd = true;
					for (size_t l = 0; l < TargetNodeArea->Connections.size(); l++)
					{
						if (TargetNodeArea->Connections[l]->In == OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]->SocketConnected[k]] &&
							TargetNodeArea->Connections[l]->Out == OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]])
						{
							bShouldAdd = false;
							break;
						}
					}

					if (bShouldAdd)
						TargetNodeArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]], OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]->SocketConnected[k]]));
				}
			}
		}
	}
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