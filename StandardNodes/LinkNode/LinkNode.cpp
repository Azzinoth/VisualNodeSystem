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

ImTextureID LinkNode::LinkIconTextureID = 0;
ImTextureID LinkNode::PlusIconTextureID = 0;
ImTextureID LinkNode::EditIconTextureID = 0;
ImTextureID LinkNode::TrashBinIconTextureID = 0;

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

std::function<void* ()> LinkNode::CreateCrossAreaDataGetter(int SocketIndex)
{
	return [this, SocketIndex]() -> void* {
		LinkNode* Partner = dynamic_cast<LinkNode*>(GetPartnerNode());
		if (Partner == nullptr)
			return nullptr;

		if (SocketIndex >= static_cast<int>(Partner->Input.size()))
			return nullptr;

		auto Connected = Partner->Input[SocketIndex]->GetConnectedSockets();
		if (Connected.empty())
			return nullptr;

		return Connected[0]->GetData();
	};
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

	// Here I am restoring the output data function.
	// Because the function is not serializable, I have to set it manually.
	for (size_t i = 0; i < Output.size(); i++)
	{
		if (Output[i] == nullptr)
			return false;

		Output[i]->SetFunctionToOutputData(CreateCrossAreaDataGetter(static_cast<int>(i)));
	}

	return true;
}

void LinkNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();

	float NodeCenterX = ImGui::GetCursorScreenPos().x + GetSize().x / 2.0f * Zoom;
	float NodeCenterY = ImGui::GetCursorScreenPos().y + GetSize().y / 2.0f * Zoom;
	float EditIconX = NodeCenterX + 64.0f * Zoom - (32.0f * Zoom) / 2.0f;
	float EditIconY = NodeCenterY - (32.0f * Zoom) / 2.0f;

	float XPosition = ImGui::GetCursorScreenPos().x;
	if (bIsInputNode)
		XPosition += GetSize().x * Zoom - 32.0f * 1.5f * Zoom;
	else
		XPosition += 32.0f * 0.5f * Zoom;
	
	float YPosition = ImGui::GetCursorScreenPos().y + GetSize().y / 2.0f * Zoom - 32.0f / 2.0f * Zoom;

	std::vector<NodeSocket*>& SocketsToWorkWith = bIsInputNode ? Input : Output;

	if (LinkNode::LinkIconTextureID != 0)
	{
		ImGui::SetCursorScreenPos(ImVec2(XPosition, YPosition));
		ImGui::Image(LinkNode::LinkIconTextureID, ImVec2(32.0f, 32.0f) * Zoom);
	}

	if (LinkNode::PlusIconTextureID != 0 && bInEditMode)
	{
		size_t LastSocketIndex = SocketsToWorkWith.size() - 1;
		std::string SocketID = SocketsToWorkWith[LastSocketIndex]->GetID();
		ImVec2 SocketPosition = ParentArea->SocketToPosition(this, SocketID);
		ImGui::SetCursorScreenPos(ImVec2(EditIconX - 48.0f * Zoom - (16.0f * Zoom) / 2.0f, SocketPosition.y - (16.0f * Zoom) / 2.0f));
		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		if (ImGui::ImageButton(("PlusIcon_LinkNode_ID" + ID).c_str(), PlusIconTextureID, ImVec2(16.0f, 16.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
		{
			AddSocket(new NodeSocket(this, std::vector<std::string>{ "INT" }, "TEST", bIsInputNode));
			//NODE_SYSTEM.AddSocketToLink(ID, { "INT" }, "TEST");
		}
		NODE_CORE.ShowToolTip("Add socket");

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	if (LinkNode::EditIconTextureID != 0)
	{
		ImGui::SetCursorScreenPos(ImVec2(EditIconX, EditIconY));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		if (ImGui::ImageButton(("EditIcon_LinkNode_ID" + ID).c_str(), EditIconTextureID, ImVec2(32.0f, 32.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
		{
			bInEditMode = !bInEditMode;
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	if (LinkNode::TrashBinIconTextureID != 0 && bInEditMode)
	{
		for (size_t i = 0; i < SocketsToWorkWith.size(); i++)
		{
			if (i > 0)
			{
				std::string SocketID = SocketsToWorkWith[i]->GetID();
				ImVec2 SocketPosition = ParentArea->SocketToPosition(this, SocketID);
				ImGui::SetCursorScreenPos(ImVec2(EditIconX - 16.0f * Zoom - (16.0f * Zoom) / 2.0f, SocketPosition.y - (16.0f * Zoom) / 2.0f));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

				if (ImGui::ImageButton(("TrashBinIcon_LinkNode_ID" + SocketID).c_str(), TrashBinIconTextureID, ImVec2(16.0f, 16.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
				{
					DeleteSocket(SocketID);
					
					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					break;
				}
				NODE_CORE.ShowToolTip("Delete socket");

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}
		}
	}
}

void LinkNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	Node* PartnerNode = GetPartnerNode();
	if (PartnerNode == nullptr || PartnerNode->GetType() != "LinkNode")
		return;

	int SocketIndex = -1;
	std::vector<NodeSocket*>& SocketsToCheck = bIsInputNode ? Input : Output;
	for (size_t i = 0; i < SocketsToCheck.size(); i++)
	{
		if (SocketsToCheck[i] == OwnSocket)
		{
			SocketIndex = static_cast<int>(i);
			break;
		}
	}

	std::vector<NodeSocket*>& OutputSocketsToCheck = bIsInputNode ? PartnerNode->Output : Output;
	if (SocketIndex == -1 || SocketIndex >= static_cast<int>(OutputSocketsToCheck.size()))
		return;
	
	if (bIsInputNode)
	{
		NodeArea* LinkedArea = GetLinkedArea();
		if (LinkedArea == nullptr)
			return;
			
		NodeSocket* PartnerOutSocket = PartnerNode->Output[SocketIndex];
		LinkedArea->TriggerSocketEvent(OwnSocket, PartnerOutSocket, EventType);
	}
	else
	{
		for (size_t i = 0; i < Output[SocketIndex]->GetConnectedSockets().size(); i++)
			ParentArea->TriggerSocketEvent(Output[SocketIndex], Output[SocketIndex]->GetConnectedSockets()[i], EventType);
	}
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

bool LinkNode::AddSocket(std::vector<std::string> AllowedTypes, std::string Name)
{
	NodeSocket* NewSocket = new NodeSocket(this, AllowedTypes, Name, !IsInputNode());
	bool bResult = AddSocket(NewSocket);
	if (!bResult)
		delete NewSocket;

	return bResult;
}

bool LinkNode::AddSocket(NodeSocket* Socket)
{
	if (bIsInputNode != Socket->IsInput())
		return false;

	SocketIDBeingModified = Socket->GetID();
	if (!NODE_SYSTEM.AddSocketToLink(ID, Socket->GetAllowedTypes(), Socket->GetName()))
	{
		SocketIDBeingModified = "";
		return false;
	}

	Node::AddSocket(Socket);
	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));

	if (bIsInputNode)
	{
		SocketIDBeingModified = "";
		return true;
	}

	int SocketIndex = static_cast<int>(Output.size()) - 1;
	Output[SocketIndex]->SetFunctionToOutputData(CreateCrossAreaDataGetter(SocketIndex));

	SocketIDBeingModified = "";
	return true;
}

bool LinkNode::AddSocketInternal(std::vector<std::string> AllowedTypes, std::string Name)
{
	NodeSocket* NewSocket = new NodeSocket(this, AllowedTypes, Name, !IsInputNode());
	bool bResult =Node::AddSocket(NewSocket);
	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));

	return bResult;
}

bool LinkNode::DeleteSocket(std::string SocketID)
{
	return DeleteSocket(GetSocketByIDInternal(SocketID));
}

bool LinkNode::DeleteSocket(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return false;

	SocketIDBeingModified = Socket->GetID();
	NODE_SYSTEM.DeleteSocketFromLink(ID, Socket->GetID());
	Node::DeleteSocket(Socket);
	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));

	if (bIsInputNode)
	{
		SocketIDBeingModified = "";
		return true;
	}
		
	// After removing the socket, We need to update the output data functions of the remaining sockets.
	for (size_t i = 0; i < Output.size(); i++)
		Output[i]->SetFunctionToOutputData(CreateCrossAreaDataGetter(static_cast<int>(i)));

	SocketIDBeingModified = "";
	return true;
}