#include "LinkNode.h"
#include "../../VisualNodeSystem.h"
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
	SetName("Reference");

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));
}

LinkNode::LinkNode(const LinkNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	LinkedAreaID = Other.LinkedAreaID;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	//Output[0]->SetFunctionToOutputData(BoolDataGetter);
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
	return bIsInputNode;
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
	Json::Value Result = Node::ToJson();

	Result["PartnerNodeID"] = PartnerNodeID;
	Result["bIsInputNode"] = bIsInputNode;
	Result["LinkedAreaID"] = LinkedAreaID;

	return Result;
}

bool LinkNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("PartnerNodeID") || !Json["PartnerNodeID"].isString())
		return false;

	if (!Json.isMember("bIsInputNode") || !Json["bIsInputNode"].isBool())
		return false;

	if (!Json.isMember("LinkedAreaID") || !Json["LinkedAreaID"].isString())
		return false;

	PartnerNodeID = Json["PartnerNodeID"].asString();
	bIsInputNode = Json["bIsInputNode"].asBool();
	LinkedAreaID = Json["LinkedAreaID"].asString();

	//// Here I am restoring the output data function.
	//// Because the function is not serializable, I have to set it manually.
	//if (Output.size() < 1)
	//	return false;

	//if (Output[0] == nullptr)
	//	return false;

	//Output[0]->SetFunctionToOutputData(BoolDataGetter);

	return true;
}

void LinkNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));
	std::string ReferencedAreaIDText = "NULL";
	NodeArea* ReferencedArea = NODE_SYSTEM.GetNodeAreaByID(LinkedAreaID);
	ReferencedAreaIDText = ReferencedArea != nullptr ? LinkedAreaID : "Invalid ID";
	std::string TextToDisplay = "Linked Area ID: " + ReferencedAreaIDText;
	float NodeWidth = GetSize().x * 0.9f;
	TextToDisplay = NODE_CORE.TruncateText(TextToDisplay, NodeWidth);
	ImGui::Text(TextToDisplay.c_str());
}

//void BoolLiteralNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
//{
//	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);
//}
//
//bool BoolLiteralNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
//{
//	if (!Node::CanConnect(OwnSocket, CandidateSocket, nullptr))
//		return false;
//
//	return true;
//}

bool LinkNode::IsDangling() const
{
	if (GetLinkedArea() == nullptr)
		return true;

	if (GetPartnerNode() == nullptr)
		return true;

	if (NODE_SYSTEM.GetLinkDataByNodeID(ID).IsNull())
		return true;

	return false;
}

void LinkNode::AddSocket(NodeSocket* Socket)
{
	if (bIsInputNode != Socket->IsInput())
		return;

	//if (Socket->AllowedTypes() != "EXECUTE")
	//	return;

	Node::AddSocket(Socket);
}