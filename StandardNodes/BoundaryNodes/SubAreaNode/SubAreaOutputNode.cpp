#include "SubAreaOutputNode.h"
#include "../../../VisualNodeSystem.h"
using namespace VisNodeSys;

bool SubAreaOutputNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("SubAreaOutputNode",
		[]() -> Node* {
			return new SubAreaOutputNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const SubAreaOutputNode& NodeToCopy = static_cast<const SubAreaOutputNode&>(CurrentNode);
			return new SubAreaOutputNode(NodeToCopy);
		}
	);

	return true;
}();

void SubAreaOutputNode::Init()
{
	Type = "SubAreaOutputNode";
	bCouldBeDestroyedByUser = false;
	bCouldBeCopiedByUser = false;

	SetStyle(DEFAULT);
	SetName("TO PARENT");

	// Data exits the sub area through this node. From the perspective of
	// nodes inside the sub area, this node provides inputs they can write to.
	bHaveInput = true;
	bHaveOutput = false;

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetTitleBarHeight(54.0f);
	SetCorrectSize();
	SetTitleBarAvailableWidth(GetSize().x - 48.0f - 8.0f);
}

void SubAreaOutputNode::SetCorrectSize()
{
	SetSize(ImVec2(400.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 1.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaOutputNode::SubAreaOutputNode()
{
	Init();
}

SubAreaOutputNode::SubAreaOutputNode(const SubAreaOutputNode& Other) : VisNodeSys::SocketMirrorNode(Other)
{
	Init();
	OwnerSubAreaNodeID = Other.OwnerSubAreaNodeID;
}

SubAreaOutputNode::~SubAreaOutputNode()
{
}

NodeArea* SubAreaOutputNode::GetOwningParentArea() const
{
	Node* ParentNode = NODE_SYSTEM.GetNodeByID(OwnerSubAreaNodeID);
	if (ParentNode == nullptr)
		return nullptr;

	return ParentNode->GetParentArea();
}

Node* SubAreaOutputNode::GetOwnerSubAreaNode() const
{
	return NODE_SYSTEM.GetNodeByID(OwnerSubAreaNodeID);
}

Json::Value SubAreaOutputNode::ToJson()
{
	Json::Value Result = SocketMirrorNode::ToJson();
	Result["OwnerSubAreaNodeID"] = OwnerSubAreaNodeID;
	return Result;
}

bool SubAreaOutputNode::FromJson(Json::Value Json)
{
	bool bResult = SocketMirrorNode::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("OwnerSubAreaNodeID") || !Json["OwnerSubAreaNodeID"].isString())
		return false;

	OwnerSubAreaNodeID = Json["OwnerSubAreaNodeID"].asString();
	return true;
}

void SubAreaOutputNode::Draw()
{
	float SocketWidthFactor = 0.82f;
	if (bInEditMode)
		SocketWidthFactor = 0.57f;

	MaxInputLabelWidth = GetSize().x * SocketWidthFactor;
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

void SubAreaOutputNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	SocketMirrorNode::SocketEvent(OwnSocket, ConnectedSocket, EventType);
}

std::vector<Node*> SubAreaOutputNode::GetMirrorPartners() const
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

bool SubAreaOutputNode::IsDangling() const
{
	Node* OwnerNode = GetOwnerSubAreaNode();
	return OwnerNode == nullptr || OwnerNode->GetType() != "SubAreaNode";
}