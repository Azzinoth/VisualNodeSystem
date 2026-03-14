#include "VisualLinkNode.h"
#include "../../VisualNodeSystem.h"
using namespace VisNodeSys;

bool VisualLinkNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("VisualLinkNode",
		[]() -> Node* {
			return new VisualLinkNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const VisualLinkNode& NodeToCopy = static_cast<const VisualLinkNode&>(CurrentNode);
			return new VisualLinkNode(NodeToCopy);
		}
	);

	return true;
}();

VisualLinkNode::VisualLinkNode()
{
	Type = "VisualLinkNode";
	SetStyle(DEFAULT);
	SetName("Reference");

	int R = static_cast<int>(44.0f * 1.2f);
	int G = static_cast<int>(46.0f * 1.2f);
	int B = static_cast<int>(44.0f * 1.2f);
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(static_cast<int>(R * 1.1f), static_cast<int>(G * 1.1f), static_cast<int>(B * 1.1f));
}

VisualLinkNode::VisualLinkNode(const VisualLinkNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	LinkedAreaID = Other.LinkedAreaID;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	//Output[0]->SetFunctionToOutputData(BoolDataGetter);
}

VisualLinkNode::~VisualLinkNode()
{
	NODE_SYSTEM.DeleteLinkRecord(ID);
}

NodeArea* VisualLinkNode::GetLinkedArea() const
{
	return NODE_SYSTEM.GetNodeAreaByID(LinkedAreaID);
}

bool VisualLinkNode::IsInputNode() const
{
	return bIsInputNode;
}

Node* VisualLinkNode::GetPartnerNode() const
{
	NodeArea* ReferencedArea = GetLinkedArea();
	if (ReferencedArea == nullptr)
		return nullptr;

	return ReferencedArea->GetNodeByID(PartnerNodeID);
}

Json::Value VisualLinkNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["LinkedAreaID"] = LinkedAreaID;
	return Result;
}

bool VisualLinkNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("LinkedAreaID"))
		return false;

	if (!Json["LinkedAreaID"].isString())
		return false;

	//// Here I am restoring the output data function.
	//// Because the function is not serializable, I have to set it manually.
	//if (Output.size() < 1)
	//	return false;

	//if (Output[0] == nullptr)
	//	return false;

	//Output[0]->SetFunctionToOutputData(BoolDataGetter);

	return true;
}

void VisualLinkNode::Draw()
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