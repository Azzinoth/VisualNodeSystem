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

SubAreaInputNode::SubAreaInputNode()
{
	Type = "SubAreaInputNode";
	bCouldBeDestroyedByUser = false;
	bCouldBeCopiedByUser = false;

	SetStyle(DEFAULT);
	SetRenderTitleBar(false);
	SetName("SubAreaInputNode");

	// Data enters the sub area through this node. From the perspective of
	// nodes inside the sub area, this node provides outputs they can read from.
	bHaveInput = false;
	bHaveOutput = true;
	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaInputNode::SubAreaInputNode(const SubAreaInputNode& Other) : VisNodeSys::SocketMirrorNode(Other)
{
	Type = "SubAreaInputNode";
	bCouldBeDestroyedByUser = false;

	SetStyle(DEFAULT);
	SetRenderTitleBar(false);

	SetName(Other.GetName());
	//LinkedAreaID = Other.LinkedAreaID;

	//// After copying node would be dangling.
	//PartnerNodeID = "";
	//bIsInputNode = Other.IsInputNode();
	//bIsInProcessOfBeingDestroyed = false;
	//SocketIDBeingModified = "";
	//bInEditMode = false;

	// Data enters the sub area through this node. From the perspective of
	// nodes inside the sub area, this node provides outputs they can read from.
	bHaveInput = false;
	bHaveOutput = true;

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaInputNode::~SubAreaInputNode()
{
	//NODE_SYSTEM.DeleteLinkRecord(ID);
}

NodeArea* SubAreaInputNode::GetParentArea() const
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

	//Result["PartnerNodeID"] = PartnerNodeID;
	//Result["bIsInputNode"] = bIsInputNode;
	//Result["LinkedAreaID"] = LinkedAreaID;

	return Result;
}

bool SubAreaInputNode::FromJson(Json::Value Json)
{
	bool bResult = SocketMirrorNode::FromJson(Json);
	if (!bResult)
		return false;

	//if (!Json.isMember("PartnerNodeID") || !Json["PartnerNodeID"].isString())
	//	return false;

	//if (!Json.isMember("bIsInputNode") || !Json["bIsInputNode"].isBool())
	//	return false;

	//if (!Json.isMember("LinkedAreaID") || !Json["LinkedAreaID"].isString())
	//	return false;

	//PartnerNodeID = Json["PartnerNodeID"].asString();
	//bIsInputNode = Json["bIsInputNode"].asBool();
	//LinkedAreaID = Json["LinkedAreaID"].asString();

	//// Here I am restoring the output data function.
	//// Because the function is not serializable, I have to set it manually.
	//for (size_t i = 0; i < Output.size(); i++)
	//{
	//	if (Output[i] == nullptr)
	//		return false;

	//	Output[i]->SetFunctionToOutputData(CreateCrossAreaDataGetter(static_cast<int>(i)));
	//}

	return true;
}

void SubAreaInputNode::Draw()
{
	SocketMirrorNode::Draw();
}

void SubAreaInputNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	for (size_t i = 0; i < OwnSocket->GetConnectedSockets().size(); i++)
		ParentArea->TriggerSocketEvent(OwnSocket, OwnSocket->GetConnectedSockets()[i], EventType);
}

std::vector<Node*> SubAreaInputNode::GetMirrorPartners() const
{
	std::vector<Node*> Result;
	Node* Partner = GetOwnerSubAreaNode();
	if (Partner == nullptr)
		return Result;

	Result.push_back(Partner);
	return Result;
}