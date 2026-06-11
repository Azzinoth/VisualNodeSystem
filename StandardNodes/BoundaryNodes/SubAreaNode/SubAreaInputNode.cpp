#include "SubAreaInputNode.h"
#include "../../../VisualNodeSystem.h"
using namespace VisNodeSys;

bool SubAreaInputNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("SubAreaInputNode",
		[]() -> Node* {
			return new SubAreaInputNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const SubAreaInputNode& NodeToCopy = static_cast<const SubAreaInputNode&>(CurrentNode);
			return new SubAreaInputNode(NodeToCopy);
		}
	);

	return true;
}();

void SubAreaInputNode::Init()
{
	Type = "SubAreaInputNode";
	bCouldBeDestroyedByUser = false;
	bCouldBeCopiedByUser = false;

	SetStyle(DEFAULT);
	SetName("FROM PARENT");

	// Data enters the sub area through this node. From the perspective of
	// nodes inside the sub area, this node provides outputs they can read from.
	bHaveInput = false;
	bHaveOutput = true;

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetTitleBarHeight(54.0f);
	SetCorrectSize();
	SetTitleBarAvailableWidth(GetSize().x - 48.0f - 8.0f);
}

void SubAreaInputNode::SetCorrectSize()
{
	SetSize(ImVec2(400.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 1.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaInputNode::SubAreaInputNode()
{
	Init();
}

SubAreaInputNode::SubAreaInputNode(const SubAreaInputNode& Other) : VisNodeSys::SocketMirrorNode(Other)
{
	Init();
	OwnerSubAreaNodeID = Other.OwnerSubAreaNodeID;
}

SubAreaInputNode::~SubAreaInputNode()
{

}

NodeArea* SubAreaInputNode::GetOwningParentArea() const
{
	Node* ParentNode = NODE_SYSTEM.GetNodeByID(OwnerSubAreaNodeID);
	if (ParentNode == nullptr)
		return nullptr;

	return ParentNode->GetParentArea();
}

Node* SubAreaInputNode::GetOwnerSubAreaNode() const
{
	return NODE_SYSTEM.GetNodeByID(OwnerSubAreaNodeID);
}

Json::Value SubAreaInputNode::ToJson()
{
	Json::Value Result = SocketMirrorNode::ToJson();
	Result["OwnerSubAreaNodeID"] = OwnerSubAreaNodeID;
	return Result;
}

bool SubAreaInputNode::FromJson(Json::Value Json)
{
	bool bResult = SocketMirrorNode::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("OwnerSubAreaNodeID") || !Json["OwnerSubAreaNodeID"].isString())
		return false;

	OwnerSubAreaNodeID = Json["OwnerSubAreaNodeID"].asString();
	return true;
}

void SubAreaInputNode::Draw()
{
	float SocketWidthFactor = 0.82f;
	if (bInEditMode)
		SocketWidthFactor = 0.57f;

	MaxOutputLabelWidth = GetSize().x * SocketWidthFactor;
	ImVec2 PositionBeforeDraw = ImGui::GetCursorScreenPos();

	SocketMirrorNode::Draw();

	if (SocketMirrorNode::SubAreaIconTextureID != 0)
	{
		float Zoom = ParentArea->GetZoomFactor();
		float IconSize = 48.0f * Zoom;

		float NodeCenterX = PositionBeforeDraw.x + GetSize().x / 2.0f * Zoom;
		float NodeCenterY = PositionBeforeDraw.y + GetSize().y / 2.0f * Zoom;

		float EditButtonVisualAsimetryShift = 5.0f;
		ImGui::SetCursorScreenPos(ImVec2(PositionBeforeDraw.x + (GetSize().x - 4.0f) * Zoom - IconSize, PositionBeforeDraw.y + 4.0f * Zoom));
		ImGui::Image(SocketMirrorNode::SubAreaIconTextureID, ImVec2(IconSize, IconSize));
	}
}

void SubAreaInputNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	if (OwnSocket == nullptr || ParentArea == nullptr)
		return;

	for (size_t i = 0; i < OwnSocket->GetConnectedSockets().size(); i++)
		ParentArea->TriggerSocketEvent(OwnSocket, OwnSocket->GetConnectedSockets()[i], EventType);
}

std::vector<Node*> SubAreaInputNode::GetMirrorPartners() const
{
	std::vector<Node*> Result;
	if (IsDangling())
		return Result;

	Node* Partner = GetOwnerSubAreaNode();
	if (Partner == nullptr)
		return Result;

	Result.push_back(Partner);
	return Result;
}

bool SubAreaInputNode::IsDangling() const
{
	Node* OwnerNode = GetOwnerSubAreaNode();
	return OwnerNode == nullptr || OwnerNode->GetType() != "SubAreaNode";
}