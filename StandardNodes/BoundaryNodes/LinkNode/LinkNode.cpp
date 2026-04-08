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
	SetRenderTitleBar(false);
	SetName("Link");

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

LinkNode::LinkNode(const LinkNode& Other) : VisNodeSys::SocketMirrorNode(Other)
{
	Type = "LinkNode";
	SetStyle(DEFAULT);
	SetRenderTitleBar(false);

	SetName(Other.GetName());
	LinkedAreaID = Other.LinkedAreaID;

	// After copying node would be dangling.
	PartnerNodeID = "";
	bIsInProcessOfBeingDestroyed = false;
	SocketIDBeingModified = "";
	bInEditMode = false;

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

LinkNode::~LinkNode()
{
	NODE_SYSTEM.DeleteLinkRecord(ID);
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
	SocketMirrorNode::Draw();
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