#include "VisualReferenceNode.h"
#include "../../VisualNodeSystem.h"
using namespace VisNodeSys;

bool VisualReferenceNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("VisualReferenceNode",
		[]() -> Node* {
			return new VisualReferenceNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const VisualReferenceNode& NodeToCopy = static_cast<const VisualReferenceNode&>(CurrentNode);
			return new VisualReferenceNode(NodeToCopy);
		}
	);

	return true;
}();

VisualReferenceNode::VisualReferenceNode(bool bCreateEmptyNodeArea) : Node()
{
	Type = "VisualReferenceNode";
	SetStyle(DEFAULT);
	SetName("Reference");

	int R = 44 * 1.2f;
	int G = 46 * 1.2f;
	int B = 44 * 1.2f;
	TitleBackgroundColor = ImColor(R, G, B);
	TitleBackgroundColorHovered = ImColor(int(R * 1.1f), int(G * 1.1f), int(B * 1.1f));

	if (bCreateEmptyNodeArea)
		SetReferencedArea(NODE_SYSTEM.CreateNodeArea());
}

VisualReferenceNode::VisualReferenceNode(NodeArea* ReferencedArea)
{
	Type = "VisualReferenceNode";
	SetStyle(DEFAULT);
	SetName("Reference");

	SetReferencedArea(ReferencedArea);
}

VisualReferenceNode::VisualReferenceNode(const VisualReferenceNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	ReferencedAreaID = Other.ReferencedAreaID;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	//Output[0]->SetFunctionToOutputData(BoolDataGetter);
}

VisualReferenceNode::~VisualReferenceNode()
{
	NODE_SYSTEM.DeleteReferenceNodeRecord(ID);
}

NodeArea* VisualReferenceNode::GetReferencedArea() const
{
	return NODE_SYSTEM.GetNodeAreaByID(ReferencedAreaID);
}

bool VisualReferenceNode::SetReferencedArea(std::string NewReferencedAreaID)
{
	if (ParentArea != nullptr && ParentArea->GetID() == NewReferencedAreaID)
		return false;

	ReferencedAreaID = NewReferencedAreaID;
	NODE_SYSTEM.CreateReferenceNodeRecord(ID, ParentArea != nullptr ? ParentArea->GetID() : "", NewReferencedAreaID);
	return true;
}

bool VisualReferenceNode::SetReferencedArea(NodeArea* NewReferencedArea)
{
	if (NewReferencedArea == nullptr)
		return false;

	return SetReferencedArea(NewReferencedArea->GetID());
}

Json::Value VisualReferenceNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["ReferencedAreaID"] = ReferencedAreaID;
	return Result;
}

bool VisualReferenceNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("ReferencedAreaID"))
		return false;

	if (!Json["ReferencedAreaID"].isString())
		return false;

	SetReferencedArea(Json["ReferencedAreaID"].asString());

	//// Here I am restoring the output data function.
	//// Because the function is not serializable, I have to set it manually.
	//if (Output.size() < 1)
	//	return false;

	//if (Output[0] == nullptr)
	//	return false;

	//Output[0]->SetFunctionToOutputData(BoolDataGetter);

	return true;
}

void VisualReferenceNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));
	std::string ReferencedAreaIDText = "NULL";
	NodeArea* ReferencedArea = NODE_SYSTEM.GetNodeAreaByID(ReferencedAreaID);
	ReferencedAreaIDText = ReferencedArea != nullptr ? ReferencedAreaID : "Invalid ID";
	std::string TextToDisplay = "Area ID: " + ReferencedAreaIDText;
	int NodeWidth = GetSize().x * 0.9f;
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