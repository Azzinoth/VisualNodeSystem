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

	if (!NODE_CORE.bIsInTestMode)
		Render();
}

void NodeArea::SetMainContextMenuFunction(void(*Function)())
{
	MainContextMenuFunction = Function;
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

	MainContextMenuFunction = nullptr;
	NodeEventsCallbacks.clear();
}

void NodeArea::AddNodeEventCallback(std::function<void(Node*, NODE_EVENT)> Function)
{
	if (Function != nullptr)
		NodeEventsCallbacks.push_back(Function);
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
	const std::string JsonFile = ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << JsonFile;
	SaveFile.close();
}

void NodeArea::SaveNodesToFile(const char* FileName, std::vector<Node*> Nodes)
{
	if (Nodes.empty())
		return;

	const NodeArea* NewNodeArea = NodeArea::CreateNodeArea(Nodes, std::vector<GroupComment*>());
	const std::string JsonFile = NewNodeArea->ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << JsonFile;
	SaveFile.close();
	delete NewNodeArea;
}

bool NodeArea::IsAlreadyConnected(NodeSocket* FirstSocket, NodeSocket* SecondSocket, const std::vector<Connection*>& Connections)
{
	for (size_t i = 0; i < Connections.size(); i++)
	{
		// If the node is connected to a node that is not in this list, just ignore.
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
				// Check if we have already established this connection.
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
	Json::Value Root;
	std::ofstream SaveFile;

	Json::Value NodesData;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		NodesData[std::to_string(i)] = Nodes[i]->ToJson();
	}
#ifdef VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES
	NodesData["ExecutionEntryNodeID"] = ExecutionEntryNodeID;
#endif

	Root["Nodes"] = NodesData;

	Json::Value ConnectionsData;
	for (size_t i = 0; i < Connections.size(); i++)
	{
		ConnectionsData[std::to_string(i)]["In"]["SocketID"] = Connections[i]->In->GetID();
		size_t SocketIndex = 0;
		for (size_t j = 0; j < Connections[i]->In->GetParent()->Input.size(); j++)
		{
			if (Connections[i]->In->GetParent()->Input[j]->GetID() == Connections[i]->In->GetID())
				SocketIndex = j;
		}
		ConnectionsData[std::to_string(i)]["In"]["SocketIndex"] = SocketIndex;
		ConnectionsData[std::to_string(i)]["In"]["NodeID"] = Connections[i]->In->GetParent()->GetID();

		ConnectionsData[std::to_string(i)]["Out"]["SocketID"] = Connections[i]->Out->GetID();
		SocketIndex = 0;
		for (size_t j = 0; j < Connections[i]->Out->GetParent()->Output.size(); j++)
		{
			if (Connections[i]->Out->GetParent()->Output[j]->GetID() == Connections[i]->Out->GetID())
				SocketIndex = j;
		}
		ConnectionsData[std::to_string(i)]["Out"]["SocketIndex"] = SocketIndex;
		ConnectionsData[std::to_string(i)]["Out"]["NodeID"] = Connections[i]->Out->GetParent()->GetID();

		for (size_t j = 0; j < Connections[i]->RerouteNodes.size(); j++)
		{
			ConnectionsData[std::to_string(i)]["RerouteConnections"][std::to_string(j)]["RerouteID"] = Connections[i]->RerouteNodes[j]->ID;
			ConnectionsData[std::to_string(i)]["RerouteConnections"][std::to_string(j)]["PositionX"] = Connections[i]->RerouteNodes[j]->Position.x;
			ConnectionsData[std::to_string(i)]["RerouteConnections"][std::to_string(j)]["PositionY"] = Connections[i]->RerouteNodes[j]->Position.y;

			std::string BeginSocketID = "";
			if (Connections[i]->RerouteNodes[j]->BeginSocket)
				BeginSocketID = Connections[i]->RerouteNodes[j]->BeginSocket->GetID();
			ConnectionsData[std::to_string(i)]["RerouteConnections"][std::to_string(j)]["BeginSocketID"] = BeginSocketID;

			std::string EndSocketID = "";
			if (Connections[i]->RerouteNodes[j]->EndSocket)
				EndSocketID = Connections[i]->RerouteNodes[j]->EndSocket->GetID();
			ConnectionsData[std::to_string(i)]["RerouteConnections"][std::to_string(j)]["EndSocketID"] = EndSocketID;

			std::string BeginRerouteID = "";
			if (Connections[i]->RerouteNodes[j]->BeginReroute)
				BeginRerouteID = Connections[i]->RerouteNodes[j]->BeginReroute->ID;
			ConnectionsData[std::to_string(i)]["RerouteConnections"][std::to_string(j)]["BeginRerouteID"] = BeginRerouteID;

			std::string EndRerouteID = "";
			if (Connections[i]->RerouteNodes[j]->EndReroute)
				EndRerouteID = Connections[i]->RerouteNodes[j]->EndReroute->ID;
			ConnectionsData[std::to_string(i)]["RerouteConnections"][std::to_string(j)]["EndRerouteID"] = EndRerouteID;
		}
	}
	Root["Connections"] = ConnectionsData;

	Json::Value GroupCommentsData;
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		GroupCommentsData[std::to_string(i)] = GroupComments[i]->ToJson();
	}
	Root["GroupComments"] = GroupCommentsData;

	Root["RenderOffset"]["X"] = RenderOffset.x;
	Root["RenderOffset"]["Y"] = RenderOffset.y;

	Json::StreamWriterBuilder Builder;
	const std::string JsonText = Json::writeString(Builder, Root);

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

std::pair<int, int> NodeArea::FindOutOfOrderConnectionPair(Json::Value& Root, std::vector<Json::String>& ConnectionList, std::unordered_map<std::string, Node*>& LoadedNodes)
{
	for (size_t i = 0; i < ConnectionList.size(); i++)
	{
		const Json::Value& ConnectionData = Root["Connections"][ConnectionList[i]];

		std::string OutSocketID = ConnectionData["Out"]["SocketID"].asCString();
		std::string OutNodeID = ConnectionData["Out"]["NodeID"].asCString();

		Node* ConnectionOutNode = LoadedNodes[OutNodeID];

		// Check against subsequent connections in the list.
		for (size_t j = i + 1; j < ConnectionList.size(); j++)
		{
			const Json::Value& NextConnectionData = Root["Connections"][ConnectionList[j]];

			std::string NextInSocketID = NextConnectionData["In"]["SocketID"].asCString();
			std::string NextInNodeID = NextConnectionData["In"]["NodeID"].asCString();

			Node* NextConnectionInNode = LoadedNodes[NextInNodeID];

			// If the output node of the current connection is the same as
			// the input node of a connection appearing later, we have a dependency.
			if (ConnectionOutNode == NextConnectionInNode)
				return std::make_pair(static_cast<int>(i), static_cast<int>(j));
		}
	}

	return std::make_pair(-1, -1);
}

bool NodeArea::LoadFromJson(std::string JsonText)
{
	if (JsonText.find("{") == std::string::npos || JsonText.find("}") == std::string::npos || JsonText.find(":") == std::string::npos)
		return false;

	Json::Value Root;
	JSONCPP_STRING Error;
	Json::CharReaderBuilder Builder;

	const std::unique_ptr<Json::CharReader> Reader(Builder.newCharReader());
	if (!Reader->parse(JsonText.c_str(), JsonText.c_str() + JsonText.size(), &Root, &Error))
		return false;

	if (!Root.isMember("Nodes"))
		return false;

	if (!Root["Nodes"].isObject())
		return false;

	std::unordered_map<std::string, Node*> LoadedNodes;
	std::vector<Json::String> NodesList = Root["Nodes"].getMemberNames();
	for (size_t i = 0; i < NodesList.size(); i++)
	{
		std::string NodeKey = std::to_string(i);
		if (!Root["Nodes"][NodeKey].isMember("NodeType"))
			continue;

		if (!Root["Nodes"][NodeKey]["NodeType"].isString())
			continue;

		std::string NodeType = Root["Nodes"][NodeKey]["NodeType"].asCString();
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

		bool bResult = NewNode->FromJson(Root["Nodes"][NodeKey]);
		if (!bResult)
		{
			// TO-DO: Implement a more robust user notification system (e.g., logging, UI warning).
			delete NewNode;
			NewNode = nullptr;
			continue;
		}

		if (NewNode != nullptr)
		{
			LoadedNodes[NewNode->GetID()] = NewNode;
			AddNode(NewNode);
		}
	}

#ifdef VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES
	if (Root["Nodes"].isMember("ExecutionEntryNodeID") && Root["Nodes"]["ExecutionEntryNodeID"].isString())
		SetExecutionEntryNode(Root["Nodes"]["ExecutionEntryNodeID"].asString());
#endif

	if (Root.isMember("Connections"))
	{
		if (!Root["Connections"].isObject())
			return false;

		std::vector<Json::String> ConnectionsList = Root["Connections"].getMemberNames();
		std::vector<Json::String> ValidConnectionKeys;
		for (size_t i = 0; i < ConnectionsList.size(); i++)
		{
			if (!Root["Connections"][ConnectionsList[i]].isObject())
				continue;

			const Json::Value& ConnectionData = Root["Connections"][ConnectionsList[i]];
			if (!ConnectionData.isMember("In") || !ConnectionData.isMember("Out") || !ConnectionData["In"].isObject() || !ConnectionData["Out"].isObject())
				continue;

			// TO-DO: Implement a more robust user notification system (e.g., logging, UI warning).
			if (!ConnectionData["In"].isMember("SocketID") || !ConnectionData["In"]["SocketID"].isString() ||
				!ConnectionData["In"].isMember("NodeID") || !ConnectionData["In"]["NodeID"].isString() ||
				!ConnectionData["Out"].isMember("SocketID") || !ConnectionData["Out"]["SocketID"].isString() ||
				!ConnectionData["Out"].isMember("NodeID") || !ConnectionData["Out"]["NodeID"].isString())
				continue;

			std::string InSocketID = ConnectionData["In"]["SocketID"].asCString();
			std::string InNodeID = ConnectionData["In"]["NodeID"].asCString();

			std::string OutSocketID = ConnectionData["Out"]["SocketID"].asCString();
			std::string OutNodeID = ConnectionData["Out"]["NodeID"].asCString();

			if (LoadedNodes.find(InNodeID) == LoadedNodes.end() || LoadedNodes.find(OutNodeID) == LoadedNodes.end())
				continue;

			ValidConnectionKeys.push_back(ConnectionsList[i]);
		}

		// Process connections.
		std::vector<Json::String> FailedConnectionsList;
		for (size_t i = 0; i < ValidConnectionKeys.size(); i++)
		{
			const Json::Value& ConnectionData = Root["Connections"][ValidConnectionKeys[i]];

			if (!WorkOnLoadedConnection(Root, ConnectionData, LoadedNodes))
				FailedConnectionsList.push_back(ValidConnectionKeys[i]);
		}

		if (!FailedConnectionsList.empty())
		{
			// Reorder failed connections for type resolution.
			// Required because multi-type socket outputs depend on connected inputs to determine their final type.
			// This loop enforces that inputs to a node are processed before its outputs, enabling type deduction.
			// It ensures this order via repeated swapping based on the detected dependency (e.g., process A->B before B->C).
			size_t MaxSwaps = FailedConnectionsList.size() * FailedConnectionsList.size();
			size_t SwapCount = 0;
			while (SwapCount < MaxSwaps)
			{
				std::pair<int, int> SwapIndices = FindOutOfOrderConnectionPair(Root, FailedConnectionsList, LoadedNodes);
				if (SwapIndices.first == -1 || SwapIndices.second == -1)
					break;

				std::string KeyToSwap = FailedConnectionsList[SwapIndices.first];
				FailedConnectionsList[SwapIndices.first] = FailedConnectionsList[SwapIndices.second];
				FailedConnectionsList[SwapIndices.second] = KeyToSwap;

				SwapCount++;
			}

			for (size_t i = 0; i < FailedConnectionsList.size(); i++)
			{
				const Json::Value& ConnectionData = Root["Connections"][FailedConnectionsList[i]];
				if (!WorkOnLoadedConnection(Root, ConnectionData, LoadedNodes))
					continue;
			}
		}
	}

	if (Root.isMember("GroupComments") && !Root["GroupComments"].isNull())
	{
		if (!Root["GroupComments"].isObject())
			return false;

		std::vector<Json::String> GroupCommentsList = Root["GroupComments"].getMemberNames();
		for (size_t i = 0; i < GroupCommentsList.size(); i++)
		{
			if (!Root["GroupComments"][std::to_string(i)].isObject())
				return false;

			GroupComment* NewGroupComment = new GroupComment();
			NewGroupComment->FromJson(Root["GroupComments"][std::to_string(i)]);
			AddGroupComment(NewGroupComment);
		}
	}

	if (Root.isMember("RenderOffset"))
	{
		if (!Root["RenderOffset"].isMember("X") || !Root["RenderOffset"].isMember("Y"))
			return false;

		float OffsetX = Root["RenderOffset"]["X"].asFloat();
		float OffsetY = Root["RenderOffset"]["Y"].asFloat();
		SetRenderOffset(ImVec2(OffsetX, OffsetY));
	}

	return true;
}

bool NodeArea::WorkOnLoadedConnection(Json::Value& Root, const Json::Value& ConnectionData, std::unordered_map<std::string, Node*>& LoadedNodes)
{
	std::string InSocketID = ConnectionData["In"]["SocketID"].asCString();
	std::string InNodeID = ConnectionData["In"]["NodeID"].asCString();

	std::string OutSocketID = ConnectionData["Out"]["SocketID"].asCString();
	std::string OutNodeID = ConnectionData["Out"]["NodeID"].asCString();

	if (!TryToConnect(LoadedNodes[OutNodeID], OutSocketID, LoadedNodes[InNodeID], InSocketID))
		return false;

	Connection* NewConnection = Connections.back();
	if (NewConnection == nullptr)
		return false;

	if (!ConnectionData.isMember("RerouteConnections"))
		return true;

	// First pass to fill information that does not depend on other reroutes.
	std::vector<Json::String> RerouteList = ConnectionData["RerouteConnections"].getMemberNames();
	for (size_t j = 0; j < RerouteList.size(); j++)
	{
		const Json::Value& CurrentReroute = ConnectionData["RerouteConnections"][std::to_string(j)];
		if (!CurrentReroute.isMember("RerouteID") || !CurrentReroute.isMember("PositionX") || !CurrentReroute.isMember("PositionY"))
			continue;

		if (!CurrentReroute["RerouteID"].isString() || !CurrentReroute["PositionX"].isNumeric() || !CurrentReroute["PositionY"].isNumeric())
			continue;

		RerouteNode* NewReroute = new RerouteNode();
		NewReroute->ID = CurrentReroute["RerouteID"].asCString();
		NewReroute->Parent = NewConnection;

		NewReroute->Position.x = CurrentReroute["PositionX"].asFloat();
		NewReroute->Position.y = CurrentReroute["PositionY"].asFloat();

		NewConnection->RerouteNodes.push_back(NewReroute);
	}

	// Second pass to fill pointers.
	for (size_t j = 0; j < RerouteList.size(); j++)
	{
		const Json::Value& CurrentReroute = ConnectionData["RerouteConnections"][std::to_string(j)];
		if (!CurrentReroute.isMember("BeginSocketID") || !CurrentReroute.isMember("EndSocketID") || !CurrentReroute.isMember("BeginRerouteID") || !CurrentReroute.isMember("EndRerouteID"))
			continue;

		if (!CurrentReroute["BeginSocketID"].isString() || !CurrentReroute["EndSocketID"].isString() || !CurrentReroute["BeginRerouteID"].isString() || !CurrentReroute["EndRerouteID"].isString())
			continue;

		std::string BeginSocketID = CurrentReroute["BeginSocketID"].asCString();
		if (!BeginSocketID.empty())
		{
			NodeSocket* BeginSocket = NewConnection->Out;
			if (BeginSocketID == BeginSocket->GetID())
				NewConnection->RerouteNodes[j]->BeginSocket = BeginSocket;
		}

		std::string EndSocketID = CurrentReroute["EndSocketID"].asCString();
		if (!EndSocketID.empty())
		{
			NodeSocket* EndSocket = NewConnection->In;
			if (EndSocketID == EndSocket->GetID())
				NewConnection->RerouteNodes[j]->EndSocket = EndSocket;
		}

		std::string BeginRerouteID = CurrentReroute["BeginRerouteID"].asCString();
		if (!BeginRerouteID.empty())
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

		std::string EndRerouteID = CurrentReroute["EndRerouteID"].asCString();
		if (!EndRerouteID.empty())
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

	for (size_t i = 0; i < NewConnection->RerouteNodes.size(); i++)
	{
		// If any of the reroute nodes are invalid, we need to delete all of them.
		if (!IsRerouteNodeValid(NewConnection->RerouteNodes[i]))
		{
			for (size_t j = 0; j < NewConnection->RerouteNodes.size(); j++)
			{
				delete NewConnection->RerouteNodes[j];
				NewConnection->RerouteNodes.erase(NewConnection->RerouteNodes.begin() + j, NewConnection->RerouteNodes.begin() + j + 1);
				j--;
			}
			break;
		}
	}

	return true;
}

bool NodeArea::LoadFromFile(const char* FileName)
{
	std::ifstream NodesFile;
	NodesFile.open(FileName);
	if (!NodesFile.is_open())
		return false;

	const std::string FileData((std::istreambuf_iterator<char>(NodesFile)), std::istreambuf_iterator<char>());
	NodesFile.close();

	return LoadFromJson(FileData);
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

			// Then we will add segment from current reroute to end, only if it is last reroute
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

bool NodeArea::IsRectsOverlapping(ImVec2 FirstRectMin, ImVec2 FirstRectSize, ImVec2 SecondRectMin, ImVec2 SecondRectSize)
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

#ifdef VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES
Node* NodeArea::GetExecutionEntryNode() const
{
	return GetNodeByID(ExecutionEntryNodeID);
}

bool NodeArea::SetExecutionEntryNode(Node* TargetNode)
{
	if (TargetNode == nullptr)
		return false;

	return SetExecutionEntryNode(TargetNode->GetID());
}

bool NodeArea::SetExecutionEntryNode(std::string NewEntryNode)
{
	if (NewEntryNode.empty())
		return false;

	Node* EntryNode = GetNodeByID(NewEntryNode);
	if (EntryNode == nullptr)
		return false;

	// Such node should have at least one output socket of type "EXECUTE".
	bool bAppropriateSocketFound = false;
	for (size_t i = 0; i < EntryNode->Output.size(); i++)
	{
		std::vector<std::string> AllowedTypes = EntryNode->Output[i]->GetAllowedTypes();
		for (size_t j = 0; j < AllowedTypes.size(); j++)
		{
			if (AllowedTypes[j] == "EXECUTE")
			{
				bAppropriateSocketFound = true;
				break;
			}
		}
	}

	if (!bAppropriateSocketFound)
		return false;

	ExecutionEntryNodeID = EntryNode->GetID();
	return true;
}

bool NodeArea::ExecuteNodeNetwork()
{
	Node* EntryNode = GetExecutionEntryNode();
	if (EntryNode == nullptr)
		return false;

	LastExecutedNodes.clear();
	TriggerOrphanSocketEvent(EntryNode, EXECUTE);
	return true;
}

std::vector<Node*> NodeArea::GetLastExecutedNodes() const
{
	return LastExecutedNodes;
}

bool NodeArea::IsSaveExecutedNodes() const
{
	return Settings.bSaveExecutedNodes;
}

void NodeArea::SetSaveExecutedNodes(bool NewValue)
{
	Settings.bSaveExecutedNodes = NewValue;
}
#endif