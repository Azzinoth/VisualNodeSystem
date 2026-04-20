#include "SocketMirrorNode.h"
#include "../../VisualNodeSystem.h"
using namespace VisNodeSys;

ImTextureID SocketMirrorNode::LinkIconTextureID = 0;
ImTextureID SocketMirrorNode::BrokenLinkIconTextureID = 0;
ImTextureID SocketMirrorNode::PlusIconTextureID = 0;
ImTextureID SocketMirrorNode::EditIconTextureID = 0;
ImTextureID SocketMirrorNode::TrashBinIconTextureID = 0;
ImTextureID SocketMirrorNode::RenameIconTextureID = 0;
ImTextureID SocketMirrorNode::ChangeAllowedTypesIconTextureID = 0;
ImTextureID SocketMirrorNode::SubAreaIconTextureID = 0;

bool SocketMirrorNode::bSettingAllowedTypes = false;
bool SocketMirrorNode::bShouldOpenEditWindow = false;
NodeSocket* SocketMirrorNode::SocketInEditWindow = nullptr;
std::string SocketMirrorNode::CurrentEditWindowCaption = "";
std::string SocketMirrorNode::EditWindowInputBuffer = "";

SocketMirrorNode::SocketMirrorNode(const std::string ID) : Node(ID)
{
	
}

SocketMirrorNode::SocketMirrorNode(const SocketMirrorNode& Other) : Node(Other)
{
	SocketIDBeingModified = "";

	bHaveInput = Other.bHaveInput;
	bHaveOutput = Other.bHaveOutput;

	// Output data function should be updated to mirror the new node's sockets, not the original's sockets.
	if (bHaveOutput)
	{
		for (size_t i = 0; i < Output.size(); i++)
			Output[i]->SetFunctionToOutputData(CreateCrossAreaDataGetter(static_cast<int>(i)));
	}
}

std::function<void* ()> SocketMirrorNode::CreateCrossAreaDataGetter(int SocketIndex)
{
	return [this, SocketIndex]() -> void* {
		std::vector<Node*> MirrorPartners = GetMirrorPartners();
		if (MirrorPartners.empty())
			return nullptr;
		
		SocketMirrorNode* Partner = nullptr;
		for (size_t i = 0; i < MirrorPartners.size(); i++)
		{
			if (MirrorPartners[i] == nullptr)
				continue;

			SocketMirrorNode* LocalCast = dynamic_cast<SocketMirrorNode*>(MirrorPartners[i]);
			if (LocalCast == nullptr)
				continue;

			if (LocalCast->bHaveInput)
			{
				Partner = LocalCast;
				break;
			}
		}

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

Json::Value SocketMirrorNode::ToJson()
{
	Json::Value Result = Node::ToJson();

	Result["bHaveInput"] = bHaveInput;
	Result["bHaveOutput"] = bHaveOutput;

	return Result;
}

bool SocketMirrorNode::FromJson(Json::Value Json)
{
	bool bResult = Node::FromJson(Json);
	if (!bResult)
		return false;

	if (!Json.isMember("bHaveInput") || !Json["bHaveInput"].isBool() || !Json.isMember("bHaveOutput") || !Json["bHaveOutput"].isBool())
		return false;

	bHaveInput = Json["bHaveInput"].asBool();
	bHaveOutput = Json["bHaveOutput"].asBool();

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

void SocketMirrorNode::SetCorrectSize()
{
	SetSize(ImVec2(300.0f, static_cast<float>(NODE_HEIGHT_PER_SOCKET * 0.75f * std::max(size_t(2), std::max(Input.size(), Output.size())))));
}

bool SocketMirrorNode::IsDangling() const
{
	return false;
}

void SocketMirrorNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{
	Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);

	int SocketIndex = -1;
	if (OwnSocket->GetFlowDirection() == NodeSocket::SocketFlow::Input)
	{
		std::pair<SocketMirrorNode*, NodeSocket*> PartnerData = NODE_SYSTEM.GetAppropriatePartnerAndSocket(this, OwnSocket);
		if (PartnerData.first == nullptr || PartnerData.second == nullptr)
			return;

		NodeArea* PartnerArea = PartnerData.first->GetParentArea();
		if (PartnerArea == nullptr)
			return;

		PartnerArea->TriggerSocketEvent(OwnSocket, PartnerData.second, EventType);
	}
	else
	{
		for (size_t i = 0; i < Output.size(); i++)
		{
			if (Output[i] == OwnSocket)
			{
				SocketIndex = static_cast<int>(i);
				break;
			}
		}

		if (SocketIndex == -1 || SocketIndex >= static_cast<int>(Output.size()))
			return;

		for (size_t i = 0; i < Output[SocketIndex]->GetConnectedSockets().size(); i++)
			ParentArea->TriggerSocketEvent(Output[SocketIndex], Output[SocketIndex]->GetConnectedSockets()[i], EventType);
	}
}

bool SocketMirrorNode::AddSocketInternal(std::vector<std::string> AllowedTypes, std::string Name, NodeSocket::SocketFlow FlowDirection)
{
	NodeSocket::SocketFlow NewSocketFlowDirection = bHaveInput ? NodeSocket::SocketFlow::Input : NodeSocket::SocketFlow::Output;
	// Only if this node mirrors both directions does the caller's Flow matter.
	if (bHaveInput && bHaveOutput)
	{
		if ((FlowDirection == NodeSocket::SocketFlow::Output && !bHaveOutput) ||
			(FlowDirection == NodeSocket::SocketFlow::Input && !bHaveInput))
			return false;

		NewSocketFlowDirection = FlowDirection;
	}

	NodeSocket* NewSocket = new NodeSocket(this, AllowedTypes, Name, NewSocketFlowDirection);
	bool bResult = Node::AddSocket(NewSocket);
	SetCorrectSize();

	if (NewSocketFlowDirection == NodeSocket::SocketFlow::Output)
	{
		int SocketIndex = static_cast<int>(Output.size()) - 1;
		Output[SocketIndex]->SetFunctionToOutputData(CreateCrossAreaDataGetter(SocketIndex));
	}

	return bResult;
}

bool SocketMirrorNode::AddSocket(std::vector<std::string> AllowedTypes, std::string Name, NodeSocket::SocketFlow FlowDirection)
{
	if ((FlowDirection == NodeSocket::SocketFlow::Output && !bHaveOutput) || (FlowDirection == NodeSocket::SocketFlow::Input && !bHaveInput))
		return false;

	NodeSocket* NewSocket = new NodeSocket(this, AllowedTypes, Name, FlowDirection);
	return AddSocket(NewSocket);
}

bool SocketMirrorNode::AddSocket(NodeSocket* Socket)
{
	if (Socket == nullptr || (Socket->GetFlowDirection() == NodeSocket::SocketFlow::Input && !bHaveInput) || (Socket->GetFlowDirection() == NodeSocket::SocketFlow::Output && !bHaveOutput))
		return false;

	SocketIDBeingModified = Socket->GetID();
	if (!NODE_SYSTEM.AddSocketToMirrorNode(ID, Socket->GetAllowedTypes(), Socket->GetName(), Socket->GetFlowDirection()))
	{
		SocketIDBeingModified = "";
		return false;
	}

	Node::AddSocket(Socket);
	SetCorrectSize();

	if (Socket->GetFlowDirection() == NodeSocket::SocketFlow::Output && bHaveOutput)
	{
		int SocketIndex = static_cast<int>(Output.size()) - 1;
		Output[SocketIndex]->SetFunctionToOutputData(CreateCrossAreaDataGetter(SocketIndex));
	}

	SocketIDBeingModified = "";
	return true;
}

bool SocketMirrorNode::DeleteSocket(std::string SocketID)
{
	return DeleteSocket(GetSocketByID(SocketID));
}

bool SocketMirrorNode::DeleteSocket(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return false;

	if (!Socket->GetAllowedTypes().empty() && Socket->GetAllowedTypes()[0] == "EXECUTE" && GetSocketIndexByID(Socket->GetID()) == 0)
		return false; // We should never change execution socket types.

	NodeSocket::SocketFlow DeletedSocketFlow = Socket->GetFlowDirection();
	SocketIDBeingModified = Socket->GetID();
	NODE_SYSTEM.DeleteSocketFromMirrorNode(ID, Socket->GetID());
	Node::DeleteSocket(Socket);
	SetCorrectSize();

	if (DeletedSocketFlow == NodeSocket::SocketFlow::Output && bHaveOutput)
	{
		// After removing the socket, We need to update the output data functions of the remaining sockets.
		for (size_t i = 0; i < Output.size(); i++)
			Output[i]->SetFunctionToOutputData(CreateCrossAreaDataGetter(static_cast<int>(i)));
	}

	SocketIDBeingModified = "";
	return true;
}

void SocketMirrorNode::OpenEditWindow(NodeSocket* Socket, bool bForAllowedTypes)
{
	SocketInEditWindow = Socket;
	bSettingAllowedTypes = bForAllowedTypes;
	bShouldOpenEditWindow = true;
}

void SocketMirrorNode::RenderEditWindow()
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

void SocketMirrorNode::Draw()
{
	Node::Draw();

	float Zoom = ParentArea->GetZoomFactor();
	float ButtonGap = 8.0f * Zoom;
	float SpacingAroundEditButton = 8.0f * Zoom;

	float EditButtonSize = 32.0f * Zoom;
	float PlusIconSize = 16.0f * Zoom;
	float RenameIconSize = 24.0f * Zoom;
	float ChangeTypesIconSize = 16.0f * Zoom;
	float TrashBinIconSize = 16.0f * Zoom;

	// Each value is: distance from edit button center to THIS icon's center.
	float RenameShiftMag = SpacingAroundEditButton * 2.3f + PlusIconSize / 2.0f + PlusIconSize / 2.0f + ButtonGap + 24.0f * Zoom / 2.0f;
	float TypesShiftMag = RenameShiftMag + 24.0f * Zoom / 2.0f + ButtonGap + 16.0f * Zoom / 2.0f;
	float TrashShiftMag = TypesShiftMag + 16.0f * Zoom / 2.0f + ButtonGap + 16.0f * Zoom / 2.0f;

	float EditButtonVisualAsimetryShift = 5.0f; // Because the edit icon is not visually symmetrical, we need to shift it a bit to make it look better.

	float TitleBarHeight = GetTitleBarHeight();
	TitleBarHeight = TitleBarHeight == -1 ? 0.0f : TitleBarHeight * Zoom;
	float NodeCenterX = ImGui::GetCursorScreenPos().x + GetSize().x / 2.0f * Zoom;
	float NodeCenterY = ImGui::GetCursorScreenPos().y + (GetSize().y - TitleBarHeight / Zoom) / 2.0f * Zoom + TitleBarHeight;
	float EditButtonX = 0.0f;

	int FlowMode = 0; // 1 for input, 2 for output, 3 for both.
	FlowMode = (bHaveInput ? 1 : 0) + (bHaveOutput ? 2 : 0);
	bool bShouldEditInput = (FlowMode == 1 || FlowMode == 3) && (Input.size() > 0);

	if (FlowMode == 1)
	{
		EditButtonX = ImGui::GetCursorScreenPos().x + GetSize().x * Zoom - EditButtonSize - 8.0f * Zoom;
	}
	else if (FlowMode == 2)
	{
		EditButtonX = ImGui::GetCursorScreenPos().x + 8.0f * Zoom;
	}
	else
	{
		EditButtonX = NodeCenterX - EditButtonSize / 2.0f + EditButtonSize / 4.0f;
	}
	float EditButtonY = NodeCenterY - EditButtonSize / 2.0f - EditButtonVisualAsimetryShift / 2.0f * Zoom;

	auto GetEditButtonCenter = [&](bool bInputFlowSide) -> glm::vec2 {
		float EditButtonCenterX = (EditButtonX + EditButtonSize / 2.0f) + (bInputFlowSide ? -(EditButtonSize / 3.25f) : (EditButtonSize / 3.25f));
		float EditButtonCenterY = EditButtonY + EditButtonSize / 2.0f;
		glm::vec2 Result = glm::vec2(EditButtonCenterX, EditButtonCenterY);
		// Icon is not visually centered, so I need to shift it a bit to make it look better.
		Result.x -= EditButtonVisualAsimetryShift / 2.0f * Zoom;
		Result.y += EditButtonVisualAsimetryShift / 2.0f * Zoom;

		return Result;
	};

	auto DrawGlobalButtonsBackground = [&]() {
		float Padding = 6.0f * Zoom;

		glm::vec2 EditButtonCenter = GetEditButtonCenter(true);
		// Edit button edges (use the raw EditButtonX, not the visually-shifted center).
		float EditLeft = EditButtonX;
		float EditRight = EditButtonX + EditButtonSize;

		float InputPlusShift = -(16.0f * Zoom + SpacingAroundEditButton);
		float InputPlusCenterX = EditButtonCenter.x + InputPlusShift;
		float InputPlusLeft = InputPlusCenterX - PlusIconSize / 2.0f;
		float InputPlusRight = InputPlusCenterX + PlusIconSize / 2.0f;
		
		EditButtonCenter = GetEditButtonCenter(false);
		float OutputPlusShift = 16.0f * Zoom + SpacingAroundEditButton;
		float OutputPlusCenterX = EditButtonCenter.x + OutputPlusShift;
		float OutputPlusRight = OutputPlusCenterX + PlusIconSize / 2.0f;

		float MinX = 0.0f;
		float MaxX = 0.0f;

		if (FlowMode == 1)
		{
			MinX = InputPlusLeft - Padding;
			MaxX = EditRight + Padding;
		}
		else if(FlowMode == 2)
		{
			MinX = EditLeft - Padding;
			MaxX = OutputPlusRight + Padding;
		}
		else
		{
			MinX = InputPlusLeft - Padding;
			MaxX = OutputPlusRight + Padding;
		}

		float MinY = EditButtonY - Padding;
		float MaxY = EditButtonY + EditButtonSize + EditButtonVisualAsimetryShift * Zoom + Padding;

		ImU32 FillColor = IM_COL32(255, 255, 255, 18);
		ImU32 OutlineColor = IM_COL32(255, 255, 255, 40);

		ImDrawList* DrawList = ImGui::GetWindowDrawList();
		DrawList->AddRectFilled(ImVec2(MinX, MinY), ImVec2(MaxX, MaxY), FillColor, 6.0f * Zoom);
		DrawList->AddRect(ImVec2(MinX, MinY), ImVec2(MaxX, MaxY), OutlineColor, 6.0f * Zoom, 0, 1.0f);
	};

	auto DrawAddSocketButton = [&](bool bInputFlowSide) {
		if (SocketMirrorNode::PlusIconTextureID != 0)
		{
			glm::vec2 EditButtonCenter = GetEditButtonCenter(bInputFlowSide);
			float ShiftFromEditIcon = (bInputFlowSide ? -1 : 1) * (16.0f * Zoom + SpacingAroundEditButton);
			float ButtonPositionX = EditButtonCenter.x + ShiftFromEditIcon - PlusIconSize / 2.0f;
			float ButtonPositionY = EditButtonCenter.y - PlusIconSize / 2.0f;

			ImGui::SetCursorScreenPos(ImVec2(ButtonPositionX, ButtonPositionY));

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			if (ImGui::ImageButton(("PlusIcon_LinkNode_ID" + ID + (bInputFlowSide ? "_Input" : "_Output")).c_str(), PlusIconTextureID, ImVec2(PlusIconSize, PlusIconSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
			{
				AddSocket({ "INT" }, "TEST", bInputFlowSide ? NodeSocket::SocketFlow::Input : NodeSocket::SocketFlow::Output);
			}
			NODE_CORE.ShowToolTip("Add socket");

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}
	};
	
	auto DrawRenameSocketButton = [&](bool bInputFlowSide) {
		if (SocketMirrorNode::RenameIconTextureID != 0)
		{
			std::vector<NodeSocket*>& SocketsToWorkWith = bInputFlowSide ? Input : Output;
			for (size_t i = 0; i < SocketsToWorkWith.size(); i++)
			{
				std::string SocketID = SocketsToWorkWith[i]->GetID();
				ImVec2 SocketPosition = ParentArea->SocketToPosition(this, SocketID);

				glm::vec2 EditButtonCenter = GetEditButtonCenter(bInputFlowSide);
				float ShiftFromEditIcon = (bInputFlowSide ? -1 : 1) * RenameShiftMag;
				float ButtonPositionX = EditButtonCenter.x + ShiftFromEditIcon - RenameIconSize / 2.0f;
				float ButtonPositionY = SocketPosition.y - RenameIconSize / 2.0f;

				ImGui::SetCursorScreenPos(ImVec2(ButtonPositionX, ButtonPositionY));
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
	};

	auto DrawChangeAllowedTypesSocketButton = [&](bool bInputFlowSide) {
		if (SocketMirrorNode::ChangeAllowedTypesIconTextureID != 0)
		{
			std::vector<NodeSocket*>& SocketsToWorkWith = bInputFlowSide ? Input : Output;
			for (size_t i = 0; i < SocketsToWorkWith.size(); i++)
			{
				if (i == 0)
					continue;

				std::string SocketID = SocketsToWorkWith[i]->GetID();
				ImVec2 SocketPosition = ParentArea->SocketToPosition(this, SocketID);

				glm::vec2 EditButtonCenter = GetEditButtonCenter(bInputFlowSide);
				float ShiftFromEditIcon = (bInputFlowSide ? -1 : 1) * TypesShiftMag;
				float ButtonPositionX = EditButtonCenter.x + ShiftFromEditIcon - ChangeTypesIconSize / 2.0f;
				float ButtonPositionY = SocketPosition.y - ChangeTypesIconSize / 2.0f;

				ImGui::SetCursorScreenPos(ImVec2(ButtonPositionX, ButtonPositionY));

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
	};

	auto DrawDeleteSocketButton = [&](bool bInputFlowSide) {
		if (SocketMirrorNode::TrashBinIconTextureID != 0)
		{
			std::vector<NodeSocket*>& SocketsToWorkWith = bInputFlowSide ? Input : Output;
			for (size_t i = 0; i < SocketsToWorkWith.size(); i++)
			{
				if (i == 0)
					continue;

				std::string SocketID = SocketsToWorkWith[i]->GetID();
				ImVec2 SocketPosition = ParentArea->SocketToPosition(this, SocketID);

				glm::vec2 EditButtonCenter = GetEditButtonCenter(bInputFlowSide);
				float ShiftFromEditIcon = (bInputFlowSide ? -1 : 1) * TrashShiftMag;
				float ButtonPositionX = EditButtonCenter.x + ShiftFromEditIcon - TrashBinIconSize / 2.0f;
				float ButtonPositionY = SocketPosition.y - TrashBinIconSize / 2.0f;

				ImGui::SetCursorScreenPos(ImVec2(ButtonPositionX, ButtonPositionY));

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
	};

	if (SocketMirrorNode::EditIconTextureID != 0)
	{
		ImGui::SetCursorScreenPos(ImVec2(EditButtonX, EditButtonY));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		if (ImGui::ImageButton(("EditIcon_LinkNode_ID" + ID).c_str(), EditIconTextureID, ImVec2(32.0f, 32.0f) * Zoom, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0, 0, 0, 0)))
			bInEditMode = !bInEditMode;
		NODE_CORE.ShowToolTip("Edit node");

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	if (bInEditMode)
	{
		DrawGlobalButtonsBackground();
		RenderEditWindow();

		if (FlowMode == 3)
		{
			DrawAddSocketButton(true);
			DrawAddSocketButton(false);

			DrawRenameSocketButton(true);
			DrawRenameSocketButton(false);

			DrawChangeAllowedTypesSocketButton(true);
			DrawChangeAllowedTypesSocketButton(false);

			DrawDeleteSocketButton(true);
			DrawDeleteSocketButton(false);
		}
		else
		{
			DrawAddSocketButton(FlowMode == 1);
			DrawRenameSocketButton(FlowMode == 1);
			DrawChangeAllowedTypesSocketButton(FlowMode == 1);
			DrawDeleteSocketButton(FlowMode == 1);
		}
	}
}