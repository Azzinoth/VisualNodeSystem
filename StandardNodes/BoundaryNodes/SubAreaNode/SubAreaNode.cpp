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

SubAreaNode::SubAreaNode()
{
	Type = "SubAreaNode";
	SetStyle(DEFAULT);
	SetRenderTitleBar(false);
	SetName("SubArea");
	
	bHaveInput = true;
	bHaveOutput = true;

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaNode::SubAreaNode(NodeArea* OwnedArea)
{
	Type = "SubAreaNode";
	SetStyle(DEFAULT);
	SetRenderTitleBar(false);
	SetName("SubArea");
	OwnedAreaID = OwnedArea->GetID();

	bHaveInput = true;
	bHaveOutput = true;

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaNode::SubAreaNode(const SubAreaNode& Other) : VisNodeSys::SocketMirrorNode(Other)
{
	Type = "SubAreaNode";
	SetStyle(DEFAULT);
	SetRenderTitleBar(false);

	SetName(Other.GetName());
	OwnedAreaID = Other.OwnedAreaID;

	//// After copying node would be dangling.
	//PartnerNodeID = "";
	//bIsInputNode = Other.IsInputNode();
	//bIsInProcessOfBeingDestroyed = false;
	//SocketIDBeingModified = "";
	//bInEditMode = false;

	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

SubAreaNode::~SubAreaNode()
{
	NodeArea* OwnedArea = GetOwnedArea();
	NODE_SYSTEM.DeleteNodeArea(OwnedArea);
	//NODE_SYSTEM.DeleteLinkRecord(ID);
}

NodeArea* SubAreaNode::GetOwnedArea() const
{
	return NODE_SYSTEM.GetNodeAreaByID(OwnedAreaID);
}

bool SubAreaNode::AddSocket(NodeSocket* Socket)
{
	return SocketMirrorNode::AddSocket(Socket);
}

bool SubAreaNode::AddSocket(std::vector<std::string> AllowedTypes, std::string Name, NodeSocket::Direction SocketDirection)
{
	return SocketMirrorNode::AddSocket(AllowedTypes, Name, SocketDirection == NodeSocket::Direction::Output ? true : false);
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

	OwnedAreaID = Json["OwnedAreaID"].asString();

	if (!Json.isMember("OwnedAreaData") || !Json["OwnedAreaData"].isObject())
		return false;

	//NODE_SYSTEM.crea
	//NodeArea* OwnedArea = GetOwnedArea();
	//if (OwnedArea == nullptr)
	//	return false;

	/*if (!OwnedArea->LoadFromJson(Json["OwnedAreaData"].toStyledString()))
		return false;*/
	

	if (!Json.isMember("SubAreaInputNodeID") || !Json["SubAreaInputNodeID"].isString())
		return false;

	if (!Json.isMember("SubAreaOutputNodeID") || !Json["SubAreaOutputNodeID"].isString())
		return false;

	SubAreaInputNodeID = Json["SubAreaInputNodeID"].asString();
	SubAreaOutputNodeID = Json["SubAreaOutputNodeID"].asString();

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

void SubAreaNode::Draw()
{
	SocketMirrorNode::Draw();
}

void SubAreaNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	if (OwnSocket->IsOutput())
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

	Node* InputNode = NODE_SYSTEM.GetNodeByID(SubAreaInputNodeID);
	Result.push_back(InputNode);

	Node* OutputNode = NODE_SYSTEM.GetNodeByID(SubAreaOutputNodeID);
	Result.push_back(OutputNode);

	return Result;
}

SubAreaInputNode* SubAreaNode::GetSubAreaInputNode() const
{
	NodeArea* OwnedArea = GetOwnedArea();
	Node* Node = OwnedArea->GetNodeByID(SubAreaInputNodeID);
	if (Node == nullptr || Node->GetType() != "SubAreaInputNode")
		return nullptr;

	return static_cast<SubAreaInputNode*>(Node);
}

SubAreaOutputNode* SubAreaNode::GetSubAreaOutputNode() const
{
	NodeArea* OwnedArea = GetOwnedArea();
	Node* Node = OwnedArea->GetNodeByID(SubAreaOutputNodeID);
	if (Node == nullptr || Node->GetType() != "SubAreaOutputNode")
		return nullptr;

	return static_cast<SubAreaOutputNode*>(Node);
}