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

SubAreaOutputNode::SubAreaOutputNode()
{
	Type = "SubAreaOutputNode";
	bCouldBeDestroyedByUser = false;
	bCouldBeCopiedByUser = false;

	SetStyle(DEFAULT);
	SetRenderTitleBar(false);
	SetName("SubAreaOutputNode");

	// Data exits the sub area through this node. From the perspective of
	// nodes inside the sub area, this node provides inputs they can write to.
	bHaveInput = true;
	bHaveOutput = false;

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaOutputNode::SubAreaOutputNode(const SubAreaOutputNode& Other) : VisNodeSys::SocketMirrorNode(Other)
{
	Type = "SubAreaOutputNode";
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
	
	// Data exits the sub area through this node. From the perspective of
	// nodes inside the sub area, this node provides inputs they can write to.
	bHaveInput = true;
	bHaveOutput = false;

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaOutputNode::~SubAreaOutputNode()
{
	//NODE_SYSTEM.DeleteLinkRecord(ID);
}

NodeArea* SubAreaOutputNode::GetParentArea() const
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

	//Result["PartnerNodeID"] = PartnerNodeID;
	//Result["bIsInputNode"] = bIsInputNode;
	//Result["LinkedAreaID"] = LinkedAreaID;

	return Result;
}

bool SubAreaOutputNode::FromJson(Json::Value Json)
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

void SubAreaOutputNode::Draw()
{
	Node::Draw();

}

void SubAreaOutputNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	SocketMirrorNode::SocketEvent(OwnSocket, ConnectedSocket, EventType);
}

std::vector<Node*> SubAreaOutputNode::GetMirrorPartners() const
{
	std::vector<Node*> Result;

	Node* Partner = GetOwnerSubAreaNode();
	if (Partner == nullptr)
		return Result;

	Result.push_back(Partner);
	return Result;
}