#include "SubAreaNode.h"
#include "../../../VisualNodeSystem.h"
using namespace VisNodeSys;

bool SubAreaNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("SubAreaNode",
		[]() -> Node* {
			return new SubAreaNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const SubAreaNode& NodeToCopy = static_cast<const SubAreaNode&>(CurrentNode);
			return new SubAreaNode(NodeToCopy);
		}
	);

	return true;
}();

void SubAreaNode::Init()
{
	Type = "SubAreaNode";
	SetStyle(DEFAULT);
	SetName("SubArea");
	
	bHaveInput = true;
	bHaveOutput = true;

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 0.75f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetTitleBarHeight(54.0f);
	SetCorrectSize();
	SetTitleBarAvailableWidth(GetSize().x - 48.0f - 8.0f);
}

void SubAreaNode::SetCorrectSize()
{
	SetSize(ImVec2(400.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 1.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaNode::SubAreaNode()
{
	Init();
}

SubAreaNode::SubAreaNode(NodeArea* OwnedArea)
{
	Init();
	OwnedAreaID = OwnedArea->GetID();
}

SubAreaNode::SubAreaNode(const SubAreaNode& Other) : VisNodeSys::SocketMirrorNode(Other)
{
	Init();

	// If the source has no owned area (e.g. default-constructed via factory), leave the copy in the same dangling state.
	NodeArea* OtherOwnedArea = Other.GetOwnedArea();
	if (OtherOwnedArea == nullptr)
	{
		OwnedAreaID = "";
		SetName(Other.GetName());
		return;
	}

	NodeArea* NewOwnedArea = NODE_SYSTEM.CreateNodeArea();
	OwnedAreaID = NewOwnedArea->GetID();
	NODE_SYSTEM.CopyNodesTo(OtherOwnedArea, NewOwnedArea);

	// Find the copied SubAreaInputNode and SubAreaOutputNode in the new area and relink IDs.
	auto InputNodes = NewOwnedArea->GetNodesByType<SubAreaInputNode>();
	auto OutputNodes = NewOwnedArea->GetNodesByType<SubAreaOutputNode>();
	if (!InputNodes.empty())
	{
		SubAreaInputNodeID = InputNodes[0]->GetID();
		InputNodes[0]->OwnerSubAreaNodeID = GetID();
	}

	if (!OutputNodes.empty())
	{
		SubAreaOutputNodeID = OutputNodes[0]->GetID();
		OutputNodes[0]->OwnerSubAreaNodeID = GetID();
	}

	SetName(Other.GetName());
}

SubAreaNode::~SubAreaNode()
{
	NodeArea* OwnedArea = GetOwnedArea();
	OwnedAreaID = "";
	NODE_SYSTEM.DeleteNodeArea(OwnedArea);
}

NodeArea* SubAreaNode::GetOwnedArea() const
{
	return NODE_SYSTEM.GetNodeAreaByID(OwnedAreaID);
}

bool SubAreaNode::AddSocket(NodeSocket* Socket)
{
	return SocketMirrorNode::AddSocket(Socket);
}

bool SubAreaNode::AddSocket(std::vector<std::string> AllowedTypes, std::string Name, NodeSocket::SocketFlow SocketDirection)
{
	return SocketMirrorNode::AddSocket(AllowedTypes, Name, SocketDirection);
}

Json::Value SubAreaNode::ToJson()
{
	Json::Value Result = SocketMirrorNode::ToJson();

	NodeArea* OwnedArea = GetOwnedArea();
	if (OwnedArea != nullptr)
	{
		Result["OwnedAreaID"] = OwnedArea->GetID();
		Result["OwnedAreaData"] = OwnedArea->ToJson();
		Result["SubAreaInputNodeID"] = SubAreaInputNodeID;
		Result["SubAreaOutputNodeID"] = SubAreaOutputNodeID;
	}
	else
	{
		Result["OwnedAreaID"] = "";
	}

	return Result;
}

bool SubAreaNode::FromJson(Json::Value Json)
{
	bool bResult = SocketMirrorNode::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("OwnedAreaID") || !Json["OwnedAreaID"].isString())
		return false;

	if (!Json.isMember("OwnedAreaData") || !Json["OwnedAreaData"].isString())
		return false;

	if (!Json.isMember("SubAreaInputNodeID") || !Json["SubAreaInputNodeID"].isString())
		return false;

	if (!Json.isMember("SubAreaOutputNodeID") || !Json["SubAreaOutputNodeID"].isString())
		return false;

	const std::string CandidateOwnedAreaID = Json["OwnedAreaID"].asString();

	// Detach current owned area if it exists.
	NodeArea* PreviouslyOwnedArea = GetOwnedArea();
	OwnedAreaID = "";

	// Refuse an area another SubAreaNode already owns.
	if (NODE_SYSTEM.FindOwnerSubAreaNode(CandidateOwnedAreaID) != nullptr)
	{
		if (PreviouslyOwnedArea != nullptr)
			OwnedAreaID = PreviouslyOwnedArea->GetID();

		return false;
	}

	NodeArea* LoadedArea = NODE_SYSTEM.GetNodeAreaByID(CandidateOwnedAreaID);
	if (LoadedArea == nullptr)
	{
		NodeArea* NewOwnedArea = NODE_SYSTEM.CreateNodeArea();
		if (!NewOwnedArea->LoadFromJson(Json["OwnedAreaData"].asString()))
		{
			NODE_SYSTEM.DeleteNodeArea(NewOwnedArea);
			if (PreviouslyOwnedArea != nullptr)
				OwnedAreaID = PreviouslyOwnedArea->GetID();

			return false;
		}

		// Reject if loaded area has ID that collides with a different existing area.
		if (NODE_SYSTEM.GetNodeAreaByID(NewOwnedArea->GetID()) != NewOwnedArea)
		{
			NODE_SYSTEM.DeleteNodeArea(NewOwnedArea);
			if (PreviouslyOwnedArea != nullptr)
				OwnedAreaID = PreviouslyOwnedArea->GetID();

			return false;
		}

		LoadedArea = NewOwnedArea;
	}

	// Delete previously owned area.
	if (PreviouslyOwnedArea != nullptr && PreviouslyOwnedArea != LoadedArea)
		NODE_SYSTEM.DeleteNodeArea(PreviouslyOwnedArea);

	OwnedAreaID = LoadedArea->GetID();

	SubAreaInputNodeID = Json["SubAreaInputNodeID"].asString();
	SubAreaOutputNodeID = Json["SubAreaOutputNodeID"].asString();

	return true;
}

void SubAreaNode::Draw()
{
	float SocketWidthFactor = 0.4f;
	if (bInEditMode)
		SocketWidthFactor = 0.14f;

	MaxInputLabelWidth = GetSize().x * SocketWidthFactor;
	// Edit icon is not visually simetrical, it is wider on the right side, so I need to give less space for output sockets.
	MaxOutputLabelWidth = GetSize().x * (SocketWidthFactor - (bInEditMode ? 0.03f : 0.0f));

	ImVec2 PositionBeforeDraw = ImGui::GetCursorScreenPos();

	SocketMirrorNode::Draw();

	if (SocketMirrorNode::SubAreaIconTextureID != 0)
	{
		float Zoom = ParentArea->GetZoomFactor();
		float IconSize = 48.0f * Zoom;

		float NodeCenterX = PositionBeforeDraw.x + GetSize().x / 2.0f * Zoom;
		float NodeCenterY = PositionBeforeDraw.y + GetSize().y / 2.0f * Zoom;

		float EditButtonVisualAsimetryShift = 5.0f;
		ImGui::SetCursorScreenPos(ImVec2(PositionBeforeDraw.x + (GetSize().x  - 4.0f) * Zoom - IconSize, PositionBeforeDraw.y + 4.0f * Zoom));
		ImGui::Image(SocketMirrorNode::SubAreaIconTextureID, ImVec2(IconSize, IconSize));
	}
}

void SubAreaNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	if (OwnSocket == nullptr)
		return;

	if (OwnSocket->GetFlowDirection() == NodeSocket::SocketFlow::Output)
	{
		for (size_t i = 0; i < OwnSocket->GetConnectedSockets().size(); i++)
			ParentArea->TriggerSocketEvent(OwnSocket, OwnSocket->GetConnectedSockets()[i], EventType);

		return;
	}

	SocketMirrorNode::SocketEvent(OwnSocket, ConnectedSocket, EventType);
}

std::vector<Node*> SubAreaNode::GetMirrorPartners() const
{
	std::vector<Node*> Result;
	if (IsDangling())
		return Result;

	Node* InputNode = NODE_SYSTEM.GetNodeByID(SubAreaInputNodeID);
	Result.push_back(InputNode);

	Node* OutputNode = NODE_SYSTEM.GetNodeByID(SubAreaOutputNodeID);
	Result.push_back(OutputNode);

	return Result;
}

SubAreaInputNode* SubAreaNode::GetSubAreaInputNode() const
{
	NodeArea* OwnedArea = GetOwnedArea();
	if (OwnedArea == nullptr)
		return nullptr;

	Node* Node = OwnedArea->GetNodeByID(SubAreaInputNodeID);
	if (Node == nullptr || Node->GetType() != "SubAreaInputNode")
		return nullptr;

	return static_cast<SubAreaInputNode*>(Node);
}

SubAreaOutputNode* SubAreaNode::GetSubAreaOutputNode() const
{
	NodeArea* OwnedArea = GetOwnedArea();
	if (OwnedArea == nullptr)
		return nullptr;

	Node* Node = OwnedArea->GetNodeByID(SubAreaOutputNodeID);
	if (Node == nullptr || Node->GetType() != "SubAreaOutputNode")
		return nullptr;

	return static_cast<SubAreaOutputNode*>(Node);
}

bool SubAreaNode::IsDangling() const
{
	return GetSubAreaInputNode() == nullptr || GetSubAreaOutputNode() == nullptr;
}

void SubAreaNode::SetNameInternal(std::string NewValue)
{
	Node::SetName(NewValue);

	SubAreaInputNode* InputNode = GetSubAreaInputNode();
	if (InputNode != nullptr)
		InputNode->SetName(NewValue + " Input");

	SubAreaOutputNode* OutputNode = GetSubAreaOutputNode();
	if (OutputNode != nullptr)
		OutputNode->SetName(NewValue + " Output");
}

void SubAreaNode::SetName(std::string NewValue)
{
	SetNameInternal(NewValue);
	if (GetOwnedArea() == nullptr)
		return;

	GetOwnedArea()->SetName(NewValue);

	SubAreaInputNode* InputNode = GetSubAreaInputNode();
	if (InputNode != nullptr)
		InputNode->SetName(NewValue + " Input");

	SubAreaOutputNode* OutputNode = GetSubAreaOutputNode();
	if (OutputNode != nullptr)
		OutputNode->SetName(NewValue + " Output");
}