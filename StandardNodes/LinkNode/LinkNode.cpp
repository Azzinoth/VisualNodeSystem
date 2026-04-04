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
ImTextureID LinkNode::RenameIconTextureID = 0;
ImTextureID LinkNode::ChangeAllowedTypesIconTextureID = 0;

bool LinkNode::bSettingAllowedTypes = false;
bool LinkNode::bShouldOpenEditWindow = false;
NodeSocket* LinkNode::SocketInEditWindow = nullptr;
std::string LinkNode::CurrentEditWindowCaption = "";
std::string LinkNode::EditWindowInputBuffer = "";

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
	float LinkIconSize = 32.0f * Zoom;

	float NodeCenterX = ImGui::GetCursorScreenPos().x + GetSize().x / 2.0f * Zoom;
	float NodeCenterY = ImGui::GetCursorScreenPos().y + GetSize().y / 2.0f * Zoom;
	float EditButtonX = 0.0f;
	if (bIsInputNode)
		EditButtonX = NodeCenterX + 64.0f * Zoom - LinkIconSize / 2.0f;
	else
		EditButtonX = NodeCenterX - 64.0f * Zoom - LinkIconSize / 2.0f;

	float EditButtonY = NodeCenterY - LinkIconSize / 2.0f;

	float LinkIconX = ImGui::GetCursorScreenPos().x;
	if (bIsInputNode)
		LinkIconX += GetSize().x * Zoom - LinkIconSize * 1.25f;
	else
		LinkIconX += LinkIconSize * 0.25f;
	
	float LinkIconY = ImGui::GetCursorScreenPos().y + GetSize().y / 2.0f * Zoom - LinkIconSize / 2.0f;

	if (LinkNode::EditIconTextureID != 0)
	{
		ImGui::SetCursorScreenPos(ImVec2(EditButtonX, EditButtonY));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		if (ImGui::ImageButton(("EditIcon_LinkNode_ID" + ID).c_str(), EditIconTextureID, ImVec2(32.0f, 32.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
		{
			bInEditMode = !bInEditMode;
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	if (bInEditMode)
	{
		RenderEditWindow();

		if (LinkNode::PlusIconTextureID != 0)
		{
			float PlusIconSize = 16.0f * Zoom;
			// Horizontally: midpoint between EditIcon right edge and Link icon left edge.
			float EditIconRightEdge = EditButtonX + 32.0f * Zoom;
			float PlusPosX = EditIconRightEdge + (LinkIconX - EditIconRightEdge) / 2.0f - PlusIconSize / 2.0f;
			float PlusPosY = EditButtonY + (32.0f * Zoom) / 2.0f - PlusIconSize / 2.0f;

			ImGui::SetCursorScreenPos(ImVec2(PlusPosX, PlusPosY));

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::ImageButton(("PlusIcon_LinkNode_ID" + ID).c_str(), PlusIconTextureID, ImVec2(PlusIconSize, PlusIconSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
			{
				AddSocket({ "INT" }, "TEST");
			}
			NODE_CORE.ShowToolTip("Add socket");

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		std::vector<NodeSocket*>& SocketsToWorkWith = bIsInputNode ? Input : Output;
		if (LinkNode::RenameIconTextureID != 0)
		{
			float RenameIconSize = 24.0f * Zoom;
			for (size_t i = 0; i < SocketsToWorkWith.size(); i++)
			{
				std::string SocketID = SocketsToWorkWith[i]->GetID();
				ImVec2 SocketPosition = ParentArea->SocketToPosition(this, SocketID);
				float ButtonXShift = bIsInputNode ? -40.0f * Zoom : 48.0f * Zoom;
				float IconXShift = bIsInputNode ? -RenameIconSize / 2.0f : RenameIconSize / 2.0f;
				ImGui::SetCursorScreenPos(ImVec2(EditButtonX + ButtonXShift + IconXShift, SocketPosition.y - RenameIconSize / 2.0f));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

				if (ImGui::ImageButton(("Rename_LinkNode_ID" + SocketID).c_str(), RenameIconTextureID, ImVec2(RenameIconSize, RenameIconSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
				{
					OpenEditWindow(SocketsToWorkWith[i], false);
				}
				NODE_CORE.ShowToolTip("Rename socket");

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}
		}

		if (LinkNode::ChangeAllowedTypesIconTextureID != 0)
		{
			float ChangeTypesIconSize = 16.0f * Zoom;
			for (size_t i = 0; i < SocketsToWorkWith.size(); i++)
			{
				if (i == 0)
					continue;

				std::string SocketID = SocketsToWorkWith[i]->GetID();
				ImVec2 SocketPosition = ParentArea->SocketToPosition(this, SocketID);
				float ButtonXShift = bIsInputNode ? -64.0f * Zoom : 82.0f * Zoom;
				float IconXShift = bIsInputNode ? -ChangeTypesIconSize / 2.0f : ChangeTypesIconSize / 2.0f;
				ImGui::SetCursorScreenPos(ImVec2(EditButtonX + ButtonXShift + IconXShift, SocketPosition.y - ChangeTypesIconSize / 2.0f));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

				if (ImGui::ImageButton(("ChangeAllowedTypes_LinkNode_ID" + SocketID).c_str(), ChangeAllowedTypesIconTextureID, ImVec2(ChangeTypesIconSize, ChangeTypesIconSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
				{
					OpenEditWindow(SocketsToWorkWith[i], true);
				}
				NODE_CORE.ShowToolTip("Change allowed types");
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}
		}

		if (LinkNode::TrashBinIconTextureID != 0)
		{
			float TrashBinIconSize = 16.0f * Zoom;
			for (size_t i = 0; i < SocketsToWorkWith.size(); i++)
			{
				if (i == 0)
					continue;
				
				std::string SocketID = SocketsToWorkWith[i]->GetID();
				ImVec2 SocketPosition = ParentArea->SocketToPosition(this, SocketID);
				float ButtonXShift = bIsInputNode ? -16.0f * Zoom : 30.0f * Zoom;
				float IconXShift = bIsInputNode ? -TrashBinIconSize / 2.0f : TrashBinIconSize / 2.0f;
				ImGui::SetCursorScreenPos(ImVec2(EditButtonX + ButtonXShift + IconXShift, SocketPosition.y - TrashBinIconSize / 2.0f));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

				if (ImGui::ImageButton(("TrashBinIcon_LinkNode_ID" + SocketID).c_str(), TrashBinIconTextureID, ImVec2(TrashBinIconSize, TrashBinIconSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
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

	if (LinkNode::LinkIconTextureID != 0)
	{
		ImGui::SetCursorScreenPos(ImVec2(LinkIconX, LinkIconY));
		ImGui::Image(LinkNode::LinkIconTextureID, ImVec2(LinkIconSize, LinkIconSize));
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
	return AddSocket(NewSocket);
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
	return DeleteSocket(GetSocketByID(SocketID));
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

void LinkNode::OpenEditWindow(NodeSocket* Socket, bool bForAllowedTypes)
{
	SocketInEditWindow = Socket;
	bSettingAllowedTypes = bForAllowedTypes;
	bShouldOpenEditWindow = true;
}

void LinkNode::RenderEditWindow()
{
	if (bShouldOpenEditWindow)
	{
		if (SocketInEditWindow == nullptr)
		{
			bShouldOpenEditWindow = false;
			return;
		}

		if (bSettingAllowedTypes)
		{
			CurrentEditWindowCaption = "Set allowed types for \"" + SocketInEditWindow->GetName() + "\" socket";
			EditWindowInputBuffer.clear();
			std::vector<std::string> CurrentTypes = SocketInEditWindow->GetAllowedTypes();
			for (size_t i = 0; i < CurrentTypes.size(); i++)
			{
				if (i > 0)
					EditWindowInputBuffer += " ";
				EditWindowInputBuffer += CurrentTypes[i];
			}
		}
		else
		{
			CurrentEditWindowCaption = "Edit socket name for \"" + SocketInEditWindow->GetName() + "\" socket";
			EditWindowInputBuffer = SocketInEditWindow->GetName();
		}
		
		ImGui::OpenPopup(CurrentEditWindowCaption.c_str());
		bShouldOpenEditWindow = false;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginPopupModal(CurrentEditWindowCaption.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (SocketInEditWindow == nullptr)
		{
			ImGui::PopStyleVar();
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			return;
		}

		if (bSettingAllowedTypes)
		{
			ImGui::Text("Enter allowed types separated by spaces:");
			ImGui::SetNextItemWidth(300.0f);
			ImGui::InputText("##AllowedTypes", &EditWindowInputBuffer);

			// Parse current input to show preview.
			std::vector<std::string> ParsedTypes;
			std::istringstream StringStream(EditWindowInputBuffer);
			std::string CurrentType;
			while (StringStream >> CurrentType)
				ParsedTypes.push_back(CurrentType);

			if (ParsedTypes.empty())
			{
				ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "At least one type is required.");
			}
			else
			{
				ImGui::Text("Types to apply: %d", static_cast<int>(ParsedTypes.size()));
			}
		}
		else
		{
			ImGui::Text("Enter new socket name:");
			ImGui::SetNextItemWidth(300.0f);
			ImGui::InputText("##SocketName", &EditWindowInputBuffer);

			if (EditWindowInputBuffer.empty())
				ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Socket name cannot be empty.");
		}

		ImGui::Spacing();

		// Determine if Apply should be enabled.
		bool bCanApply = true;
		if (bSettingAllowedTypes)
		{
			std::istringstream CheckStream(EditWindowInputBuffer);
			std::string CheckType;
			if (!(CheckStream >> CheckType))
				bCanApply = false;
		}
		else
		{
			if (EditWindowInputBuffer.empty())
				bCanApply = false;
		}

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 4.0f - 120 / 2.0f);

		if (!bCanApply)
			ImGui::BeginDisabled();

		if (ImGui::Button("Apply", ImVec2(120, 0)))
		{
			if (SocketInEditWindow != nullptr)
			{
				if (bSettingAllowedTypes)
				{
					std::vector<std::string> NewTypes;
					std::istringstream StringStream(EditWindowInputBuffer);
					std::string CurrentType;
					while (StringStream >> CurrentType)
						NewTypes.push_back(CurrentType);

					SocketInEditWindow->SetAllowedTypes(NewTypes);
				}
				else
				{
					SocketInEditWindow->SetName(EditWindowInputBuffer);
				}
			}

			SocketInEditWindow = nullptr;
			ImGui::CloseCurrentPopup();
		}

		if (!bCanApply)
			ImGui::EndDisabled();
		
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f + ImGui::GetWindowWidth() / 4.0f - 120.0f / 2.0f);
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
			ImGui::CloseCurrentPopup();
		
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	else
	{
		ImGui::PopStyleVar();
	}
}