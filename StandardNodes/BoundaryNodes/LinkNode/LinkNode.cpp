#include "LinkNode.h"
#include "../../../VisualNodeSystem.h"
using namespace VisNodeSys;

bool LinkNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("LinkNode",
		[]() -> Node* {
			return new LinkNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const LinkNode& NodeToCopy = static_cast<const LinkNode&>(CurrentNode);
			return new LinkNode(NodeToCopy);
		}
	);

	return true;
}();

LinkNode::LinkNode()
{
	Type = "LinkNode";
	SetStyle(DEFAULT);
	SetName("Link");

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetTitleBarHeight(54.0f);
	SetCorrectSize();
	SetTitleBarAvailableWidth(GetSize().x - 48.0f - 8.0f);
}

LinkNode::LinkNode(const LinkNode& Other) : VisNodeSys::SocketMirrorNode(Other)
{
	SetName(Other.GetName());
	LinkedAreaID = Other.LinkedAreaID;

	// After copying node would be dangling.
	PartnerNodeID = "";
	bIsInProcessOfBeingDestroyed = false;
	SocketIDBeingModified = "";
	bInEditMode = false;

	SetTitleBarHeight(54.0f);
	SetCorrectSize();
	SetTitleBarAvailableWidth(GetSize().x - 48.0f - 8.0f);
}

LinkNode::~LinkNode()
{
	auto* Record = NODE_SYSTEM.GetLinkDataByNodeID(ID);
	if (Record != nullptr)
		NODE_SYSTEM.DeleteLinkRecord(Record->ID);
}

void LinkNode::SetCorrectSize()
{
	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 1.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

NodeArea* LinkNode::GetLinkedArea() const
{
	return NODE_SYSTEM.GetNodeAreaByID(LinkedAreaID);
}

bool LinkNode::IsInputNode() const
{
	return bHaveInput;
}

Node* LinkNode::GetPartnerNode() const
{
	NodeArea* ReferencedArea = GetLinkedArea();
	if (ReferencedArea == nullptr)
		return nullptr;

	return ReferencedArea->GetNodeByID(PartnerNodeID);
}

Json::Value LinkNode::ToJson()
{
	Json::Value Result = SocketMirrorNode::ToJson();

	Result["PartnerNodeID"] = PartnerNodeID;
	Result["LinkedAreaID"] = LinkedAreaID;

	return Result;
}

bool LinkNode::FromJson(Json::Value Json)
{
	bool bResult = SocketMirrorNode::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("PartnerNodeID") || !Json["PartnerNodeID"].isString())
		return false;

	if (!Json.isMember("LinkedAreaID") || !Json["LinkedAreaID"].isString())
		return false;

	PartnerNodeID = Json["PartnerNodeID"].asString();
	LinkedAreaID = Json["LinkedAreaID"].asString();

	return true;
}

void LinkNode::Draw()
{
	float SocketWidthFactor = 0.78f;
	if (bInEditMode)
		SocketWidthFactor = 0.42f;

	MaxInputLabelWidth = GetSize().x * SocketWidthFactor;
	MaxOutputLabelWidth = MaxInputLabelWidth;
	ImVec2 PositionBeforeDraw = ImGui::GetCursorScreenPos();

	SocketMirrorNode::Draw();

	if (SocketMirrorNode::LinkIconTextureID != 0 && SocketMirrorNode::BrokenLinkIconTextureID != 0)
	{
		float Zoom = ParentArea->GetZoomFactor();
		float IconSize = 48.0f * Zoom;

		float NodeCenterX = PositionBeforeDraw.x + GetSize().x / 2.0f * Zoom;
		float NodeCenterY = PositionBeforeDraw.y + GetSize().y / 2.0f * Zoom;

		float EditButtonVisualAsimetryShift = 5.0f;
		ImGui::SetCursorScreenPos(ImVec2(PositionBeforeDraw.x + (GetSize().x - 4.0f) * Zoom - IconSize, PositionBeforeDraw.y + 4.0f * Zoom));
		ImGui::Image(IsDangling() ? SocketMirrorNode::BrokenLinkIconTextureID : SocketMirrorNode::LinkIconTextureID, ImVec2(IconSize, IconSize));
		if (IsDangling())
			NODE_CORE.ShowToolTip("This link node is dangling.\nIt means that it is not properly connected to another link node.");
	}
}

void LinkNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	SocketMirrorNode::SocketEvent(OwnSocket, ConnectedSocket, EventType);
}

bool LinkNode::IsDangling() const
{
	if (GetLinkedArea() == nullptr)
		return true;

	if (GetPartnerNode() == nullptr)
		return true;

	if (NODE_SYSTEM.GetLinkDataByNodeID(ID) == nullptr)
		return true;

	return false;
}

std::vector<Node*> LinkNode::GetMirrorPartners() const
{
	std::vector<Node*> Result;

	if (IsDangling())
		return Result;

	Node* Partner = GetPartnerNode();
	if (Partner == nullptr)
		return Result;

	Result.push_back(Partner);
	return Result;
}