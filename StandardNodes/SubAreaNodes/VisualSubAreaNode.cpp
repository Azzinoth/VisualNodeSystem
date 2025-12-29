#include "VisualSubAreaNode.h"
#include "../../VisualNodeSystem.h"
using namespace VisNodeSys;

bool VisualSubAreaNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("VisualSubAreaNode",
		[]() -> Node* {
			return new VisualSubAreaNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const VisualSubAreaNode& NodeToCopy = static_cast<const VisualSubAreaNode&>(CurrentNode);
			return new VisualSubAreaNode(NodeToCopy);
		}
	);

	return true;
}();

VisualSubAreaNode::VisualSubAreaNode(bool bCreateEmptyNodeArea) : Node()
{
	Type = "VisualSubAreaNode";
	SetStyle(DEFAULT);
	SetName("Sub Area");

	if (bCreateEmptyNodeArea)
		SetSubArea(NODE_SYSTEM.CreateNodeArea());
}

VisualSubAreaNode::VisualSubAreaNode(NodeArea* SubArea)
{
	Type = "VisualSubAreaNode";
	SetStyle(DEFAULT);
	SetName("Sub Area");

	SetSubArea(SubArea);
}

VisualSubAreaNode::VisualSubAreaNode(const VisualSubAreaNode& Other) : VisNodeSys::Node(Other)
{
	SetStyle(DEFAULT);
	SubAreaID = Other.SubAreaID;

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	//Output[0]->SetFunctionToOutputData(BoolDataGetter);
}

VisualSubAreaNode::~VisualSubAreaNode()
{
	NODE_SYSTEM.DeleteSubAreaNodeRecord(ID);
}

NodeArea* VisualSubAreaNode::GetSubArea() const
{
	return NODE_SYSTEM.GetNodeAreaByID(SubAreaID);
}

bool VisualSubAreaNode::SetSubArea(std::string NewSubAreaID)
{
	if (ParentArea != nullptr && ParentArea->GetID() == NewSubAreaID)
		return false;

	SubAreaID = NewSubAreaID;
	NODE_SYSTEM.CreateSubAreaNodeRecord(ID, ParentArea != nullptr ? ParentArea->GetID() : "", NewSubAreaID);
	return true;
}

bool VisualSubAreaNode::SetSubArea(NodeArea* NewSubArea)
{
	if (NewSubArea == nullptr)
		return false;

	return SetSubArea(NewSubArea->GetID());
}

Json::Value VisualSubAreaNode::ToJson()
{
	Json::Value Result = Node::ToJson();
	Result["SubAreaID"] = SubAreaID;
	return Result;
}

bool VisualSubAreaNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("SubAreaID"))
		return false;

	if (!Json["SubAreaID"].isString())
		return false;

	SetSubArea(Json["SubAreaID"].asString());

	//// Here I am restoring the output data function.
	//// Because the function is not serializable, I have to set it manually.
	//if (Output.size() < 1)
	//	return false;

	//if (Output[0] == nullptr)
	//	return false;

	//Output[0]->SetFunctionToOutputData(BoolDataGetter);

	return true;
}

void VisualSubAreaNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 10.0f * Zoom, ImGui::GetCursorScreenPos().y + 45.0f * Zoom));
	std::string SubAreaIDText = "NULL";
	NodeArea* SubArea = NODE_SYSTEM.GetNodeAreaByID(SubAreaID);
	SubAreaIDText = SubArea != nullptr ? SubAreaID : "Invalid ID";
	ImGui::Text(("Sub Area ID:" + SubAreaIDText).c_str());
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