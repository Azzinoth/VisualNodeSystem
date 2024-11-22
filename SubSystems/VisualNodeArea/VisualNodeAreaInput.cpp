#include "VisualNodeArea.h"
using namespace VisNodeSys;

void NodeArea::InputUpdate()
{
	MouseInputUpdate();
	KeyboardInputUpdate();
}

void NodeArea::MouseInputUpdate()
{
	MouseCursorPosition = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	
	bOpenMainContextMenu = false;

	MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);

	if (ImGui::GetCurrentContext()->HoveredWindow != NodeAreaWindow)
	{
		bMouseHovered = false;
		return;
	}
	bMouseHovered = true;

	if (ImGui::GetIO().MouseClicked[0])
		MouseDownIn = NodeAreaWindow;

	if (ImGui::GetIO().MouseReleased[0])
		MouseDownIn = nullptr;

	MouseInputUpdateNodes();
	MouseInputUpdateGroupComments();

	if (ImGui::IsMouseDragging(0) && MouseDownIn == NodeAreaWindow && ImGui::GetHoveredID() == 0)
		MouseDragging();

	if (ImGui::IsMouseDown(0))
		LeftMouseDown();

	if (ImGui::IsMouseReleased(0))
		LeftMouseReleased();

	if (ImGui::IsMouseClicked(0))
		LeftMouseClick();
	
	MouseInputUpdateConnections();

	if (ImGui::IsMouseClicked(1))
		RightMouseClick();

	// Should we connect two sockets ?
	if (SocketLookingForConnection != nullptr &&
		SocketHovered != nullptr &&
		ImGui::GetIO().MouseReleased[0])
	{
		size_t OutSocketIndex = 0;
		for (size_t i = 0; i < SocketLookingForConnection->GetParent()->Output.size(); i++)
		{
			if (SocketLookingForConnection->GetParent()->Output[i] == SocketLookingForConnection)
			{
				OutSocketIndex = i;
				break;
			}
		}

		size_t InSocketIndex = 0;
		for (size_t i = 0; i < SocketHovered->GetParent()->Input.size(); i++)
		{
			if (SocketHovered->GetParent()->Input[i] == SocketHovered)
			{
				InSocketIndex = i;
				break;
			}
		}

		TryToConnect(SocketLookingForConnection->GetParent(), OutSocketIndex, SocketHovered->GetParent(), InSocketIndex);
		SocketLookingForConnection = nullptr;
	}
	else if (ImGui::GetIO().MouseReleased[0] || SelectedNodes.size() > 1)
	{
		SocketLookingForConnection = nullptr;
	}

	if (ImGui::GetIO().MouseWheel > 0)
	{
		ApplyZoom(1.0f);
	}
	else if (ImGui::GetIO().MouseWheel < 0)
	{
		ApplyZoom(-1.0f);
	}

	if (!SelectedNodes.empty() || !SelectedRerouteNodes.empty() || !SelectedGroupComments.empty())
		UnSelectAllConnections();
}

void NodeArea::LeftMouseDown()
{
}

void NodeArea::LeftMouseReleased()
{
	LeftMouseReleasedGroupCommentUpdate();
}

void NodeArea::LeftMouseReleasedGroupCommentUpdate()
{
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		GroupComments[i]->bHorizontalResizeActive = false;
		GroupComments[i]->bVerticalResizeActive = false;
	}
}

void NodeArea::MouseInputUpdateNodes()
{
	HoveredNode = nullptr;
	SocketHovered = nullptr;

	for (size_t i = 0; i < Nodes.size(); i++)
		Nodes[i]->SetIsHovered(false);

	for (size_t i = 0; i < Nodes.size(); i++)
	{
		InputUpdateNode(Nodes[i]);
		if (HoveredNode != nullptr)
			break;
	}
}

void NodeArea::MouseInputUpdateGroupComments()
{
	GroupCommentHovered = nullptr;
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		GroupComments[i]->bHovered = false;
		GroupComments[i]->bCaptionHovered = false;
	}

	if (HoveredNode)
		return;

	// Going backwards because of the render order.
	for (int i = static_cast<int>(GroupComments.size()) - 1; i >= 0; i--)
	{
		if (IsRectUnderMouse(LocalToScreen(GroupComments[i]->GetPosition()), GroupComments[i]->GetSize() * Zoom))
		{
			GroupCommentHovered = GroupComments[i];
			GroupCommentHovered->bHovered = true;

			if (IsGroupCommentCaptionUnderMouse(GroupComments[i]))
				GroupCommentHovered->bCaptionHovered = true;

			break;
		}
	}
}

void NodeArea::LeftMouseClick()
{
	LeftMouseClickNodesUpdate();
	LeftMouseClickConnectionsUpdate();
	LeftMouseClickRerouteUpdate();
	LeftMouseClickGroupCommentsUpdate();
}

void NodeArea::LeftMouseClickNodesUpdate()
{
	bool bNothingElseIsSelected = SelectedNodes.size() == 0 && SelectedRerouteNodes.size() == 0 && SelectedGroupComments.size() == 0;
	bool bCtrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
	bool bHoveredCorrectly = HoveredNode != nullptr;

	if (bNothingElseIsSelected && bHoveredCorrectly)
	{
		AddSelected(HoveredNode);
		return;
	}

	if (!IsMouseAboveSomethingSelected())
	{
		if ((bCtrlPressed && bHoveredCorrectly))
		{
			AddSelected(HoveredNode);
			return;
		}
		else if (!bCtrlPressed && bHoveredCorrectly)
		{
			UnSelectAll();
			AddSelected(HoveredNode);
			return;
		}
		else if (!bCtrlPressed)
		{
			SelectedNodes.clear();
			return;
		}
	}
}

void NodeArea::LeftMouseClickConnectionsUpdate()
{
	if (HoveredConnection != nullptr)
	{
		if (!IsSelected(HoveredConnection) && !ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && !ImGui::IsKeyDown(ImGuiKey_RightCtrl))
			UnSelectAllConnections();

		// If it is new selection
		if (!IsSelected(HoveredConnection))
			UnSelectAllConnections();

		AddSelected(HoveredConnection);
	}
	else
	{
		UnSelectAllConnections();
	}
}

void NodeArea::LeftMouseClickRerouteUpdate()
{
	bool bNothingElseIsSelected = SelectedNodes.size() == 0 && SelectedRerouteNodes.size() == 0 && SelectedGroupComments.size() == 0;
	bool bCtrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
	bool bHoveredCorrectly = RerouteNodeHovered != nullptr;

	if (bNothingElseIsSelected && bHoveredCorrectly)
	{
		AddSelected(RerouteNodeHovered);
		return;
	}

	if (!IsMouseAboveSomethingSelected())
	{
		if (bCtrlPressed && bHoveredCorrectly)
		{
			AddSelected(RerouteNodeHovered);
			return;
		}
		else if (!bCtrlPressed && bHoveredCorrectly)
		{
			UnSelectAll();
			AddSelected(RerouteNodeHovered);
			return;
		}
		else if (!bCtrlPressed)
		{
			UnSelectAllRerouteNodes();
			return;
		}
	}
}

void NodeArea::LeftMouseClickGroupCommentsUpdate()
{
	if (HoveredNode == nullptr && RerouteNodeHovered == nullptr)
	{
		if (!IsAnyGroupCommentInResizeMode())
		{
			for (size_t i = 0; i < GroupComments.size(); i++)
			{
				if (IsGroupCommentRightPartUnderMouse(GroupComments[i]))
					GroupComments[i]->bHorizontalResizeActive = true;

				if (IsGroupCommentBottomPartUnderMouse(GroupComments[i]))
					GroupComments[i]->bVerticalResizeActive = true;

				if (GroupComments[i]->bHorizontalResizeActive || GroupComments[i]->bVerticalResizeActive)
					break;
			}
		}
	}

	bool bNothingElseIsSelected = SelectedNodes.size() == 0 && SelectedRerouteNodes.size() == 0 && SelectedGroupComments.size() == 0;
	bool bCtrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
	bool bHoveredCorrectly = GroupCommentHovered != nullptr && GroupCommentHovered->bCaptionHovered;

	if (bNothingElseIsSelected && bHoveredCorrectly)
	{
		AddSelected(GroupCommentHovered);
		return;
	}

	if (!IsMouseAboveSomethingSelected())
	{
		if (bCtrlPressed && bHoveredCorrectly)
		{
			AddSelected(GroupCommentHovered);
			return;
		}
		else if (!bCtrlPressed && bHoveredCorrectly)
		{
			UnSelectAll();
			AddSelected(GroupCommentHovered);
			return;
		}
		else if (!bCtrlPressed)
		{
			UnSelectAllGroupComments();
			return;
		}
	}
}

void NodeArea::RightMouseClick()
{
	if (HoveredNode == nullptr)
	{
		SelectedNodes.clear();
		bOpenMainContextMenu = true;
	}

	RightMouseClickNodesUpdate();
	RightMouseClickConnectionsUpdate();
	RightMouseClickRerouteUpdate();
}
void NodeArea::RightMouseClickNodesUpdate()
{
	if (HoveredNode != nullptr)
	{
		// Should we disconnect sockets
		if (SocketHovered != nullptr && !SocketHovered->ConnectedSockets.empty())
		{
			std::vector<Connection*> ImpactedConnections = GetAllConnections(SocketHovered);
			for (size_t i = 0; i < ImpactedConnections.size(); i++)
			{
				Delete(ImpactedConnections[i]);
			}
		}
		else
		{
			if (SelectedNodes.size() <= 1)
			{
				if (!HoveredNode->OpenContextMenu())
				{
					bOpenMainContextMenu = true;
				}
			}
			else
			{
				if (MainContextMenuFunc != nullptr)
				{
					bOpenMainContextMenu = true;
				}
				else
				{
					HoveredNode->OpenContextMenu();
				}
			}

			// If hovered node was already selected do nothing
			if (AddSelected(HoveredNode))
			{
				// But if it was not selected before deselect all other nodes.
				SelectedNodes.clear();
				AddSelected(HoveredNode);
			}
		}
	}
}

void NodeArea::RightMouseClickConnectionsUpdate()
{

}

void NodeArea::RightMouseClickRerouteUpdate()
{

}

bool NodeArea::ShouldDragGrid()
{
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i]->bIsRenamingActive)
			return false;

		if (GroupComments[i]->bHorizontalResizeActive || GroupComments[i]->bVerticalResizeActive)
			return false;
	}

	if (IsMouseRegionSelectionActive())
		return false;

	if (SocketLookingForConnection != nullptr)
		return false;

	if (!SelectedNodes.empty())
		return false;

	if (SocketHovered != nullptr)
		return false;

	if (!SelectedRerouteNodes.empty())
		return false;

	if (RerouteNodeHovered != nullptr)
		return false;

	if (!SelectedGroupComments.empty())
		return false;

	if (GroupCommentHovered != nullptr)
		return false;

	return true;
}

void NodeArea::MouseDragging()
{
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i]->bIsRenamingActive)
			return;
	}

	if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
	{
		SocketLookingForConnection = nullptr;

		if (abs(GetMouseDragDelta().x) > 1 || abs(GetMouseDragDelta().y) > 1)
		{
			MouseSelectRegionMin = ImGui::GetIO().MouseClickedPos[0];
			MouseSelectRegionMax = MouseSelectRegionMin + GetMouseDragDelta() / Zoom;

			if (MouseSelectRegionMax.x < MouseSelectRegionMin.x)
				std::swap(MouseSelectRegionMin.x, MouseSelectRegionMax.x);

			if (MouseSelectRegionMax.y < MouseSelectRegionMin.y)
				std::swap(MouseSelectRegionMin.y, MouseSelectRegionMax.y);
		}
	}

	if (ShouldDragGrid())
	{
		RenderOffset.x += GetMouseDelta().x * Zoom;
		RenderOffset.y += GetMouseDelta().y * Zoom;

		if (RenderOffset.x > Settings.Style.Grid.GRID_SIZE * Zoom)
			RenderOffset.x = Settings.Style.Grid.GRID_SIZE * Zoom;

		if (RenderOffset.x < -Settings.Style.Grid.GRID_SIZE * Zoom)
			RenderOffset.x = -Settings.Style.Grid.GRID_SIZE * Zoom;

		if (RenderOffset.y > Settings.Style.Grid.GRID_SIZE * Zoom)
			RenderOffset.y = Settings.Style.Grid.GRID_SIZE * Zoom;

		if (RenderOffset.y < -Settings.Style.Grid.GRID_SIZE * Zoom)
			RenderOffset.y = -Settings.Style.Grid.GRID_SIZE * Zoom;
	}

	if (!IsAnyGroupCommentInResizeMode())
	{
		MouseDraggingNodesUpdate();
		MouseDraggingConnectionsUpdate();
		MouseDraggingRerouteUpdate();
	}

	MouseDraggingGroupCommentUpdate();
}

bool NodeArea::IsRectInMouseSelectionRegion(ImVec2 RectMin, ImVec2 RectSize)
{
	// Define the max corner of the rectangle using RectMin and RectSize.
	ImVec2 RectMax = RectMin + RectSize;

	if (Settings.bRequireFullOverlapToSelect)
	{
		// Check if the entire Rect is inside MouseSelectRegion.
		return IsSecondRectInsideFirstOne(MouseSelectRegionMin, MouseSelectRegionMax - MouseSelectRegionMin, RectMin, RectSize);
	}
	else
	{
		// Check if a part of the Rect is inside MouseSelectRegion.
		return IsRectsOverlaping(RectMin, RectSize, MouseSelectRegionMin, MouseSelectRegionMax - MouseSelectRegionMin);
	}

	return false;
}

bool NodeArea::IsRectUnderMouse(ImVec2 RectMin, ImVec2 RectSize)
{
	static ImVec2 MouseCursorSize = ImVec2(5, 5);
	return IsRectsOverlaping(RectMin, RectSize, MouseCursorPosition, MouseCursorSize);
}

void NodeArea::MouseDraggingNodesUpdate()
{
	if (IsMouseRegionSelectionActive())
	{
		SelectedNodes.clear();
		const ImVec2 RegionSize = MouseSelectRegionMax - MouseSelectRegionMin;

		for (size_t i = 0; i < Nodes.size(); i++)
		{
			if (Nodes[i]->GetStyle() == DEFAULT)
			{
				if (IsRectInMouseSelectionRegion(Nodes[i]->LeftTop, Nodes[i]->GetSize() * Zoom))
					AddSelected(Nodes[i]);
			}
			else if (Nodes[i]->GetStyle() == CIRCLE)
			{
				if (IsRectInMouseSelectionRegion(Nodes[i]->LeftTop, ImVec2(NODE_DIAMETER, NODE_DIAMETER) * Zoom))
					AddSelected(Nodes[i]);
			}
		}
	}
	else
	{
		if (SocketLookingForConnection == nullptr && SocketHovered == nullptr)
		{
			for (size_t i = 0; i < SelectedNodes.size(); i++)
			{
				if (SelectedNodes[i]->CouldBeMoved())
					SelectedNodes[i]->SetPosition(SelectedNodes[i]->GetPosition() + GetMouseDelta());
			}
		}
	}
}

void NodeArea::MouseDraggingConnectionsUpdate()
{

}
void NodeArea::MouseDraggingRerouteUpdate()
{
	// Reroute nodes could be selected with nodes
	if (IsMouseRegionSelectionActive())
	{
		UnSelectAllRerouteNodes();
		for (size_t i = 0; i < Connections.size(); i++)
		{
			for (size_t j = 0; j < Connections[i]->RerouteNodes.size(); j++)
			{
				const ImVec2 ReroutePosition = LocalToScreen(Connections[i]->RerouteNodes[j]->Position);
				if (IsRectInMouseSelectionRegion(ReroutePosition, ImVec2(GetRerouteNodeSize(), GetRerouteNodeSize())))
				{
					Connections[i]->RerouteNodes[j]->bSelected = true;
					AddSelected(Connections[i]->RerouteNodes[j]);
				}
				else
				{
					if (Connections[i]->RerouteNodes[j]->bSelected)
						UnSelect(Connections[i]->RerouteNodes[j]);
				}
			}
		}
	}
	else
	{
		if (SocketLookingForConnection == nullptr && SocketHovered == nullptr)
		{
			for (size_t i = 0; i < SelectedRerouteNodes.size(); i++)
			{
				SelectedRerouteNodes[i]->Position += GetMouseDelta();
			}
		}
	}
}

void NodeArea::MouseDraggingGroupCommentUpdate()
{
	if (IsMouseRegionSelectionActive())
	{
		SelectedGroupComments.clear();
		const ImVec2 RegionSize = MouseSelectRegionMax - MouseSelectRegionMin;

		for (size_t i = 0; i < GroupComments.size(); i++)
		{
			// Check if the entire Rect is inside MouseSelectRegion.
			if (IsSecondRectInsideFirstOne(MouseSelectRegionMin, MouseSelectRegionMax - MouseSelectRegionMin, LocalToScreen(GroupComments[i]->GetPosition()), GroupComments[i]->GetSize() * Zoom))
				AddSelected(GroupComments[i]);
		}
	}
	else
	{
		bool bNothingElseIsSelected = SelectedNodes.size() == 0 && SelectedRerouteNodes.size() == 0 && SelectedGroupComments.size() == 0;
		int SelectedElementsCount = static_cast<int>(SelectedNodes.size() + SelectedRerouteNodes.size() + SelectedGroupComments.size());

		GroupComment* GroupCommentThatShouldBeResized = nullptr;
		if (SelectedElementsCount <= 1)
		{
			for (size_t i = 0; i < GroupComments.size(); i++)
			{
				if (GroupComments[i]->bHorizontalResizeActive || GroupComments[i]->bVerticalResizeActive)
				{
					if (bNothingElseIsSelected || GroupComments[i]->bSelected)
					{
						GroupCommentThatShouldBeResized = GroupComments[i];
						break;
					}
				}
			}
		}

		if (GroupCommentThatShouldBeResized != nullptr)
		{
			if (GroupCommentThatShouldBeResized->bHorizontalResizeActive)
				GroupCommentThatShouldBeResized->SetSize(ImVec2(GroupCommentThatShouldBeResized->GetSize().x + GetMouseDelta().x, GroupCommentThatShouldBeResized->GetSize().y));

			if (GroupCommentThatShouldBeResized->bVerticalResizeActive)
				GroupCommentThatShouldBeResized->SetSize(ImVec2(GroupCommentThatShouldBeResized->GetSize().x, GroupCommentThatShouldBeResized->GetSize().y + GetMouseDelta().y));

			if (GroupCommentThatShouldBeResized->GetSize().x < 100)
				GroupCommentThatShouldBeResized->SetSize(ImVec2(100, GroupCommentThatShouldBeResized->GetSize().y));

			if (GroupCommentThatShouldBeResized->GetSize().y < 100)
				GroupCommentThatShouldBeResized->SetSize(ImVec2(GroupCommentThatShouldBeResized->GetSize().x, 100));
		}
		else
		{
			for (size_t i = 0; i < SelectedGroupComments.size(); i++)
			{
				MoveGroupComment(SelectedGroupComments[i], GetMouseDelta());
			}
		}
	}
}

void NodeArea::MoveGroupComment(GroupComment* GroupComment, ImVec2 Delta)
{
	GroupComment->SetPosition(GroupComment->GetPosition() + Delta);

	if (!GroupComment->bMoveElementsWithComment)
		return;

	for (size_t i = 0; i < GroupComment->AttachedNodes.size(); i++)
	{
		if (!IsSelected(GroupComment->AttachedNodes[i]))
			GroupComment->AttachedNodes[i]->SetPosition(GroupComment->AttachedNodes[i]->GetPosition() + Delta);
	}

	for (size_t i = 0; i < GroupComment->AttachedRerouteNodes.size(); i++)
	{
		if (!IsSelected(GroupComment->AttachedRerouteNodes[i]))
			GroupComment->AttachedRerouteNodes[i]->Position += Delta;
	}

	for (size_t i = 0; i < GroupComment->AttachedGroupComments.size(); i++)
	{
		if (!IsSelected(GroupComment->AttachedGroupComments[i]))
			GroupComment->AttachedGroupComments[i]->SetPosition(GroupComment->AttachedGroupComments[i]->GetPosition() + Delta);
	}
}

void NodeArea::KeyboardInputUpdate()
{
	if (ImGui::IsKeyDown(ImGuiKey_Delete))
	{
		for (size_t i = 0; i < SelectedNodes.size(); i++)
		{
			DeleteNode(SelectedNodes[i]);
		}
		SelectedNodes.clear();

		for (size_t i = 0; i < SelectedConnections.size(); i++)
		{
			Delete(SelectedConnections[i]);
			i--;
		}

		for (size_t i = 0; i < SelectedRerouteNodes.size(); i++)
		{
			Delete(SelectedRerouteNodes[i]);
			i--;
		}

		for (size_t i = 0; i < SelectedGroupComments.size(); i++)
		{
			Delete(SelectedGroupComments[i]);
			i--;
		}
	}

	static bool WasCopiedToClipboard = false;
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
	{
		if (ImGui::IsKeyDown(ImGuiKey_C))
		{
			if (!SelectedNodes.empty() || !SelectedGroupComments.empty())
			{
				const NodeArea* NewNodeArea = NodeArea::CreateNodeArea(SelectedNodes, SelectedGroupComments);
				NODE_CORE.SetClipboardText(NewNodeArea->ToJson());
				delete NewNodeArea;
			}
		}
		else if (ImGui::IsKeyDown(ImGuiKey_V))
		{
			if (!WasCopiedToClipboard)
			{
				WasCopiedToClipboard = true;

				const std::string NodesToImport = NODE_CORE.GetClipboardText();
				Json::Value data;

				JSONCPP_STRING err;
				const Json::CharReaderBuilder builder;

				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				if (!reader->parse(NodesToImport.c_str(), NodesToImport.c_str() + NodesToImport.size(), &data, &err))
					return;

				NodeArea* NewNodeArea = new NodeArea();
				NewNodeArea->LoadFromJson(NodesToImport);

				// ***************** Place new nodes in center of a view space *****************
				const ImVec2 ViewCenter = GetRenderedViewCenter();
				ImVec2 NodesAABBCenter = NewNodeArea->GetAllElementsAABBCenter();
				NodesAABBCenter -= NewNodeArea->GetRenderOffset();

				NeededShift = ViewCenter - NodesAABBCenter;

				NewNodeArea->RunOnEachNode([](Node* Node) {
					Node->SetPosition(Node->GetPosition() + NeededShift);
				});

				for (size_t i = 0; i < NewNodeArea->Connections.size(); i++)
				{
					for (size_t j = 0; j < NewNodeArea->Connections[i]->RerouteNodes.size(); j++)
					{
						NewNodeArea->Connections[i]->RerouteNodes[j]->Position += NeededShift;
					}
				}

				for (size_t i = 0; i < NewNodeArea->GroupComments.size(); i++)
				{
					NewNodeArea->GroupComments[i]->SetPosition(NewNodeArea->GroupComments[i]->GetPosition() + NeededShift);
				}
				// ***************** Place new nodes in center of a view space END *****************

				NodeArea::CopyNodesTo(NewNodeArea, this);

				// Unselect all elements.
				UnSelectAll();

				// Select all pasted nodes.
				for (size_t i = Nodes.size() - NewNodeArea->Nodes.size(); i < Nodes.size(); i++)
				{
					SelectedNodes.push_back(Nodes[i]);
				}

				// Select all pasted reroute nodes.
				for (size_t i = Connections.size() - NewNodeArea->Connections.size(); i < Connections.size(); i++)
				{
					for (size_t j = 0; j < Connections[i]->RerouteNodes.size(); j++)
					{
						AddSelected(Connections[i]->RerouteNodes[j]);
					}
				}

				// Select all pasted group comments.
				for (size_t i = GroupComments.size() - NewNodeArea->GroupComments.size(); i < GroupComments.size(); i++)
				{
					AddSelected(GroupComments[i]);
				}

				delete NewNodeArea;
			}
		}
	}

	if (!ImGui::IsKeyDown(ImGuiKey_V))
		WasCopiedToClipboard = false;
}

Node* NodeArea::GetHovered() const
{
	return HoveredNode;
}

std::vector<Node*> NodeArea::GetSelected()
{
	return SelectedNodes;
}

bool NodeArea::AddSelected(Node* Node)
{
	if (Node == nullptr)
		return false;

	if (IsSelected(Node))
		return false;

	SelectedNodes.push_back(Node);
	return true;
}

bool NodeArea::IsSelected(const Node* Node) const
{
	if (Node == nullptr)
		return false;

	for (size_t i = 0; i < SelectedNodes.size(); i++)
	{
		if (SelectedNodes[i] == Node)
			return true;
	}

	return false;
}

bool NodeArea::AddSelected(Connection* Connection)
{
	if (Connection == nullptr)
		return false;

	if (IsSelected(Connection))
		return false;

	Connection->bSelected = true;
	SelectedConnections.push_back(Connection);
	return true;
}

bool NodeArea::IsSelected(const Connection* Connection) const
{
	if (Connection == nullptr)
		return false;

	for (size_t i = 0; i < SelectedConnections.size(); i++)
	{
		if (SelectedConnections[i] == Connection)
			return true;
	}

	return false;
}

bool NodeArea::UnSelect(const Connection* Connection)
{
	if (Connection == nullptr)
		return false;

	for (size_t i = 0; i < SelectedConnections.size(); i++)
	{
		if (SelectedConnections[i] == Connection)
		{
			SelectedConnections[i]->bSelected = false;
			SelectedConnections.erase(SelectedConnections.begin() + i);
			return true;
		}
	}

	return false;
}

void NodeArea::UnSelectAllConnections()
{
	for (size_t i = 0; i < SelectedConnections.size(); i++)
	{
		SelectedConnections[i]->bSelected = false;
	}

	SelectedConnections.clear();
}

bool NodeArea::AddSelected(GroupComment* GroupComment)
{
	if (GroupComment == nullptr)
		return false;

	if (IsSelected(GroupComment))
		return false;

	GroupComment->bSelected = true;
	SelectedGroupComments.push_back(GroupComment);
	AttachElemetnsToGroupComment(GroupComment);

	return true;
}

bool NodeArea::IsSelected(const GroupComment* GroupComment) const
{
	if (GroupComment == nullptr)
		return false;

	for (size_t i = 0; i < SelectedGroupComments.size(); i++)
	{
		if (SelectedGroupComments[i] == GroupComment)
			return true;
	}

	return false;
}

bool NodeArea::UnSelect(GroupComment* GroupComment)
{
	if (GroupComment == nullptr)
		return false;

	for (size_t i = 0; i < SelectedGroupComments.size(); i++)
	{
		if (SelectedGroupComments[i] == GroupComment)
		{
			SelectedGroupComments[i]->bSelected = false;
			SelectedGroupComments.erase(SelectedGroupComments.begin() + i);

			GroupComment->AttachedNodes.clear();
			GroupComment->AttachedRerouteNodes.clear();
			GroupComment->AttachedGroupComments.clear();

			return true;
		}
	}

	return false;
}

void NodeArea::UnSelectAllGroupComments()
{
	for (size_t i = 0; i < SelectedGroupComments.size(); i++)
	{
		SelectedGroupComments[i]->bSelected = false;
	}

	SelectedGroupComments.clear();
}

bool NodeArea::AddSelected(RerouteNode* RerouteNode)
{
	if (RerouteNode == nullptr)
		return false;

	if (IsSelected(RerouteNode))
		return false;

	RerouteNode->bSelected = true;
	SelectedRerouteNodes.push_back(RerouteNode);
	return true;
}

bool NodeArea::IsSelected(const RerouteNode* RerouteNode) const
{
	if (RerouteNode == nullptr)
		return false;

	for (size_t i = 0; i < SelectedRerouteNodes.size(); i++)
	{
		if (SelectedRerouteNodes[i] == RerouteNode)
			return true;
	}

	return false;
}

bool NodeArea::UnSelect(const RerouteNode* RerouteNode)
{
	if (RerouteNode == nullptr)
		return false;

	for (size_t i = 0; i < SelectedRerouteNodes.size(); i++)
	{
		if (SelectedRerouteNodes[i] == RerouteNode)
		{
			SelectedRerouteNodes[i]->bSelected = false;
			SelectedRerouteNodes.erase(SelectedRerouteNodes.begin() + i);
			return true;
		}
	}

	return false;
}

void NodeArea::UnSelectAllRerouteNodes()
{
	for (size_t i = 0; i < SelectedRerouteNodes.size(); i++)
	{
		SelectedRerouteNodes[i]->bSelected = false;
	}

	SelectedRerouteNodes.clear();
}

void NodeArea::UnSelectAll()
{
	SelectedNodes.clear();
	SelectedConnections.clear();
	UnSelectAllRerouteNodes();
	UnSelectAllGroupComments();
}

void NodeArea::InputUpdateNode(Node* Node)
{
	if (Node->GetStyle() == DEFAULT)
	{
		if (IsRectUnderMouse(Node->LeftTop, Node->GetSize() * Zoom))
		{
			HoveredNode = Node;
			Node->SetIsHovered(true);
		}
	}
	else if (Node->GetStyle() == CIRCLE)
	{
		if (glm::distance(glm::vec2(Node->LeftTop.x + NODE_DIAMETER / 2.0f * Zoom, Node->LeftTop.y + NODE_DIAMETER / 2.0f * Zoom),
						  glm::vec2(MouseCursorPosition.x, MouseCursorPosition.y)) <= NODE_DIAMETER * Zoom)
		{
			HoveredNode = Node;
			Node->SetIsHovered(true);
		}
	}

	if (SocketHovered == nullptr)
	{
		for (size_t i = 0; i < Node->Input.size(); i++)
		{
			InputUpdateSocket(Node->Input[i]);
		}

		for (size_t i = 0; i < Node->Output.size(); i++)
		{
			InputUpdateSocket(Node->Output[i]);
		}
	}
}

bool NodeArea::IsMouseHovered() const
{
	return bMouseHovered;
}

void NodeArea::InputUpdateSocket(NodeSocket* Socket)
{
	const ImVec2 SocketPosition = SocketToPosition(Socket);
	if (MouseCursorPosition.x >= SocketPosition.x - GetNodeSocketSize() &&
		MouseCursorPosition.x <= SocketPosition.x + GetNodeSocketSize() &&
		MouseCursorPosition.y >= SocketPosition.y - GetNodeSocketSize() &&
		MouseCursorPosition.y <= SocketPosition.y + GetNodeSocketSize())
	{
		SocketHovered = Socket;
	}

	if (SocketHovered == Socket && ImGui::GetIO().MouseClicked[0] && Socket->bOutput)
		SocketLookingForConnection = Socket;
}

void NodeArea::InputUpdateReroute(RerouteNode* Reroute)
{
	const ImVec2 ReroutePosition = LocalToScreen(Reroute->Position);
	if (MouseCursorPosition.x >= ReroutePosition.x - GetRerouteNodeSize() &&
		MouseCursorPosition.x <= ReroutePosition.x + GetRerouteNodeSize() &&
		MouseCursorPosition.y >= ReroutePosition.y - GetRerouteNodeSize() &&
		MouseCursorPosition.y <= ReroutePosition.y + GetRerouteNodeSize())
	{
		RerouteNodeHovered = Reroute;
		Reroute->bHovered = true;
	}
}

bool NodeArea::IsMouseOverConnection(Connection* Connection, const int Steps, const float MaxDistance, ImVec2* CollisionPoint)
{
	if (Connection->RerouteNodes.empty())
		return IsMouseOverSegment(SocketToPosition(Connection->Out), SocketToPosition(Connection->In), Settings.Style.GeneralConnection.LineSegments, 10.0f);

	std::vector<ConnectionSegment> Segments = GetConnectionSegments(Connection);
	for (size_t i = 0; i < Segments.size(); i++)
	{
		if (IsMouseOverSegment(Segments[i].Begin, Segments[i].End, Settings.Style.GeneralConnection.LineSegments, 10.0f))
			return true;
	}

	return false;
}

bool NodeArea::IsMouseOverSegment(ImVec2 Begin, ImVec2 End, const int Steps, const float maxDistance, ImVec2* CollisionPoint)
{
	std::vector<ImVec2> LineTangents = GetTangentsForLine(Begin, End);

	for (int Step = 0; Step <= Steps; Step++)
	{
		float t = static_cast<float>(Step) / static_cast<float>(Steps);
		float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
		float h2 = -2 * t * t * t + 3 * t * t;
		float h3 = t * t * t - 2 * t * t + t;
		float h4 = t * t * t - t * t;

		ImVec2 SegmentStart = ImVec2(h1 * Begin.x + h2 * End.x + h3 * LineTangents[0].x + h4 * LineTangents[1].x, h1 * Begin.y + h2 * End.y + h3 * LineTangents[0].y + h4 * LineTangents[1].y);

		t = static_cast<float>(Step + 1) / static_cast<float>(Steps); // Update t for the end segment.
		h1 = +2 * t * t * t - 3 * t * t + 1.0f;
		h2 = -2 * t * t * t + 3 * t * t;
		h3 = t * t * t - 2 * t * t + t;
		h4 = t * t * t - t * t;

		ImVec2 SegmentEnd = ImVec2(h1 * Begin.x + h2 * End.x + h3 * LineTangents[0].x + h4 * LineTangents[1].x, h1 * Begin.y + h2 * End.y + h3 * LineTangents[0].y + h4 * LineTangents[1].y);

		// Compute the shortest distance from mousePos to the line defined by the segment.
		ImVec2 SegmentDirection = SegmentEnd - SegmentStart;
		float SegmentLengthSq = ImDot(SegmentDirection, SegmentDirection);
		ImVec2 ToMousePos = MouseCursorPosition - SegmentStart;
		float TProj = ImClamp(ImDot(ToMousePos, SegmentDirection) / SegmentLengthSq, 0.0f, 1.0f);

		ImVec2 Projection = SegmentStart + ImVec2(TProj, TProj) * SegmentDirection;
		float Distance = ImLengthSqr(MouseCursorPosition - Projection);

		// If the distance is less than the threshold, the mouse is over the connection.
		if (Distance < maxDistance * maxDistance * Zoom)
		{
			if(CollisionPoint != nullptr) *CollisionPoint = Projection;
			//ImGui::GetWindowDrawList()->AddRectFilled(projection, projection + ImVec2(5, 5), IM_COL32(175, 175, 255, 125), 1.0f);
			return true;
		}
	}

	return false;
}

bool NodeArea::IsPointInRegion(const ImVec2& Point, const ImVec2& RegionMin, const ImVec2& RegionMax)
{
	return (Point.x >= RegionMin.x && Point.x <= RegionMax.x && Point.y >= RegionMin.y && Point.y <= RegionMax.y);
}

int Orientation(const ImVec2& FirstPoint, const ImVec2& SecondPoint, const ImVec2& ThirdPoint)
{
	const float Value = (SecondPoint.y - FirstPoint.y) * (ThirdPoint.x - SecondPoint.x) - (SecondPoint.x - FirstPoint.x) * (ThirdPoint.y - SecondPoint.y);

	// Collinear
	if (Value == 0)
		return 0;

	return (Value > 0) ? 1 : 2; // Clockwise or Counterclockwise
}

bool OnSegment(const ImVec2& SegmentStart, const ImVec2& SegmentEnd, const ImVec2& PointToCheck)
{
	if (PointToCheck.x <= std::max(SegmentStart.x, SegmentEnd.x) && PointToCheck.x >= std::min(SegmentStart.x, SegmentEnd.x) &&
		PointToCheck.y <= std::max(SegmentStart.y, SegmentEnd.y) && PointToCheck.y >= std::min(SegmentStart.y, SegmentEnd.y))
		return true;

	return false;
}

bool IsLineSegmentIntersecting(const ImVec2& FirstSegmentStart, const ImVec2& FirstSegmentEnd, const ImVec2& SecondSegmentStart, const ImVec2& SecondSegmentEnd)
{
	// Find the four orientations
	int FirstLineToSecondStart = Orientation(FirstSegmentStart, FirstSegmentEnd, SecondSegmentStart);
	int FirstLineToSecondEnd = Orientation(FirstSegmentStart, FirstSegmentEnd, SecondSegmentEnd);
	int SecondLineToFirstStart = Orientation(SecondSegmentStart, SecondSegmentEnd, FirstSegmentStart);
	int SecondLineToFirstEnd = Orientation(SecondSegmentStart, SecondSegmentEnd, FirstSegmentEnd);

	// General case
	if (FirstLineToSecondStart != FirstLineToSecondEnd && SecondLineToFirstStart != SecondLineToFirstEnd)
		return true;

	// Special cases: check if the segments are collinear and overlap
	if (FirstLineToSecondStart == 0 && OnSegment(FirstSegmentStart, SecondSegmentStart, FirstSegmentEnd)) return true;
	if (FirstLineToSecondEnd == 0 && OnSegment(FirstSegmentStart, SecondSegmentEnd, FirstSegmentEnd)) return true;
	if (SecondLineToFirstStart == 0 && OnSegment(SecondSegmentStart, FirstSegmentStart, SecondSegmentEnd)) return true;
	if (SecondLineToFirstEnd == 0 && OnSegment(SecondSegmentStart, FirstSegmentEnd, SecondSegmentEnd)) return true;

	return false; // If none of the above cases, return false
}

bool NodeArea::IsConnectionInRegion(Connection* Connection, const int Steps)
{
	if (Connection->RerouteNodes.empty())
		return IsSegmentInRegion(SocketToPosition(Connection->Out), SocketToPosition(Connection->In), Settings.Style.GeneralConnection.LineSegments);

	std::vector<ConnectionSegment> Segments = GetConnectionSegments(Connection);
	for (size_t i = 0; i < Segments.size(); i++)
	{
		if (IsSegmentInRegion(Segments[i].Begin, Segments[i].End, Settings.Style.GeneralConnection.LineSegments))
			return true;
	}

	return false;
}

bool NodeArea::IsSegmentInRegion(ImVec2 Begin, ImVec2 End, const int Steps)
{
	const ImVec2 t1 = ImVec2(80.0f, 0.0f);
	const ImVec2 t2 = ImVec2(80.0f, 0.0f);

	ImVec2 regionCorners[4] = {
		MouseSelectRegionMin,
		ImVec2(MouseSelectRegionMax.x, MouseSelectRegionMin.y),
		MouseSelectRegionMax,
		ImVec2(MouseSelectRegionMin.x, MouseSelectRegionMax.y)
	};

	for (int Step = 0; Step <= Steps; Step++)
	{
		float t = static_cast<float>(Step) / static_cast<float>(Steps);
		float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
		float h2 = -2 * t * t * t + 3 * t * t;
		float h3 = t * t * t - 2 * t * t + t;
		float h4 = t * t * t - t * t;

		ImVec2 SegmentStart = ImVec2(h1 * Begin.x + h2 * End.x + h3 * t1.x + h4 * t2.x, h1 * Begin.y + h2 * End.y + h3 * t1.y + h4 * t2.y);

		t = static_cast<float>(Step + 1) / static_cast<float>(Steps); // Update t for the end segment.
		h1 = +2 * t * t * t - 3 * t * t + 1.0f;
		h2 = -2 * t * t * t + 3 * t * t;
		h3 = t * t * t - 2 * t * t + t;
		h4 = t * t * t - t * t;

		ImVec2 SegmentEnd = ImVec2(h1 * Begin.x + h2 * End.x + h3 * t1.x + h4 * t2.x, h1 * Begin.y + h2 * End.y + h3 * t1.y + h4 * t2.y);

		// If either of the segment's points are in the region, the connection is in the region.
		if (IsPointInRegion(SegmentStart, MouseSelectRegionMin, MouseSelectRegionMax) ||
			IsPointInRegion(SegmentEnd, MouseSelectRegionMin, MouseSelectRegionMax))
		{
			return true;
		}

		// Check if the segment intersects with any of the region's edges.
		for (int i = 0; i < 4; i++)
		{
			if (IsLineSegmentIntersecting(regionCorners[i], regionCorners[(i + 1) % 4], SegmentStart, SegmentEnd))
			{
				return true;
			}
		}
	}

	return false;
}

bool NodeArea::IsMouseRegionSelectionActive() const
{
	return MouseSelectRegionMin.x != FLT_MAX && MouseSelectRegionMin.y != FLT_MAX && MouseSelectRegionMax.x != FLT_MAX && MouseSelectRegionMax.y != FLT_MAX;
}

void NodeArea::MouseInputUpdateConnections()
{
	for (size_t i = 0; i < Connections.size(); i++)
		Connections[i]->bHovered = false;
	HoveredConnection = nullptr;

	RerouteNodeHovered = nullptr;
	if (HoveredNode == nullptr)
	{
		for (size_t i = 0; i < Connections.size(); i++)
		{
			for (size_t j = 0; j < Connections[i]->RerouteNodes.size(); j++)
			{
				Connections[i]->RerouteNodes[j]->bHovered = false;
				InputUpdateReroute(Connections[i]->RerouteNodes[j]);

				if (RerouteNodeHovered != nullptr)
					break;
			}

			if (RerouteNodeHovered != nullptr)
				break;
		}
	}

	// If any node is hovered, we don't want to check for hovered connections
	if (HoveredNode == nullptr && RerouteNodeHovered == nullptr)
	{
		for (size_t i = 0; i < Connections.size(); i++)
		{
			if (HoveredConnection == nullptr && IsMouseOverConnection(Connections[i], Settings.Style.GeneralConnection.LineSegments, 10.0f))
			{
				Connections[i]->bHovered = true;
				HoveredConnection = Connections[i];
			}

			if (IsMouseRegionSelectionActive() && SelectedNodes.empty())
			{
				if (IsConnectionInRegion(Connections[i], Settings.Style.GeneralConnection.LineSegments))
				{
					Connections[i]->bSelected = true;
					AddSelected(Connections[i]);
				}
				else
				{
					if (Connections[i]->bSelected)
						UnSelect(Connections[i]);
				}
			}
		}
	}

	if (!SelectedNodes.empty())
	{
		for (size_t i = 0; i < Connections.size(); i++)
			Connections[i]->bSelected = false;

		SelectedConnections.clear();
	}

	if (ImGui::IsMouseDoubleClicked(0))
		DoubleMouseClick();
}

void NodeArea::DoubleMouseClick()
{
	ConnectionsDoubleMouseClick();
	GroupCommentDoubleMouseClick();
}

void NodeArea::ConnectionsDoubleMouseClick()
{
	if (HoveredConnection != nullptr)
	{
		if (HoveredConnection->RerouteNodes.empty())
		{
			AddRerouteNode(HoveredConnection, 0, ScreenToLocal(MouseCursorPosition));
		}
		else
		{
			std::vector<ConnectionSegment> Segments = GetConnectionSegments(HoveredConnection);
			for (size_t i = 0; i < Segments.size(); i++)
			{
				if (IsMouseOverSegment(Segments[i].Begin, Segments[i].End, Settings.Style.GeneralConnection.LineSegments, 10.0f))
				{
					AddRerouteNode(HoveredConnection, i, ScreenToLocal(MouseCursorPosition));
					break;
				}
			}
		}
	}
}

bool NodeArea::IsGroupCommentCaptionUnderMouse(GroupComment* GroupComment)
{
	ImVec2 LocalPosition = LocalToScreen(GroupComment->GetPosition());
	ImVec2 CommentSize = GroupComment->GetSize() * Zoom;

	ImVec2 CaptionSize = GroupComment->GetCaptionSize(Zoom);
	ImVec2 CaptionPosition = LocalPosition + ImVec2(4.0f, 4.0f) * Zoom;

	if (IsRectUnderMouse(CaptionPosition, CaptionSize))
		return true;

	return false;
}

bool NodeArea::IsGroupCommentRightPartUnderMouse(GroupComment* GroupComment)
{
	if (IsGroupCommentCaptionUnderMouse(GroupComment))
		return false;

	ImVec2 LocalPosition = LocalToScreen(GroupComment->GetPosition());
	ImVec2 CommentSize = GroupComment->GetSize() * Zoom;

	ImVec2 CaptionSize = GroupComment->GetCaptionSize(Zoom);
	ImVec2 CaptionPosition = LocalPosition + ImVec2(4.0f, 4.0f) * Zoom;

	const float LineLength = 7.0f * Zoom;
	ImVec2 Begining = CaptionPosition + ImVec2(CaptionSize.x - LineLength, CaptionSize.y);
	
	if (IsRectUnderMouse(Begining, ImVec2(LineLength, CommentSize.y - CaptionSize.y)))
		return true;

	return false;
}

bool NodeArea::IsGroupCommentBottomPartUnderMouse(GroupComment* GroupComment)
{
	if (IsGroupCommentCaptionUnderMouse(GroupComment))
		return false;

	ImVec2 LocalPosition = LocalToScreen(GroupComment->GetPosition());
	ImVec2 CommentSize = GroupComment->GetSize() * Zoom;

	ImVec2 CaptionSize = GroupComment->GetCaptionSize(Zoom);
	ImVec2 CaptionPosition = LocalPosition + ImVec2(4.0f, 4.0f) * Zoom;

	const float LineLength = 7.0f * Zoom;
	ImVec2 Begining = CaptionPosition + ImVec2(0.0f, CommentSize.y - LineLength * 2);

	if (IsRectUnderMouse(Begining, ImVec2(CommentSize.x, LineLength)))
		return true;

	return false;
}

void NodeArea::GroupCommentDoubleMouseClick()
{
	if (GroupCommentHovered != nullptr)
	{
		if (GroupCommentHovered->bHovered)
		{
			for (size_t i = 0; i < GroupComments.size(); i++)
				GroupComments[i]->bIsRenamingActive = false;

			if (GroupCommentHovered->bCaptionHovered)
				GroupCommentHovered->bIsRenamingActive = true;
		}
	}
}

bool NodeArea::IsMouseAboveSomethingSelected() const
{
	if (HoveredNode != nullptr && IsSelected(HoveredNode))
		return true;

	if (RerouteNodeHovered != nullptr && IsSelected(RerouteNodeHovered))
		return true;

	if (GroupCommentHovered != nullptr && IsSelected(GroupCommentHovered))
		return true;

	return false;
}

bool NodeArea::IsAnyGroupCommentInResizeMode()
{
	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i]->bHorizontalResizeActive || GroupComments[i]->bVerticalResizeActive)
			return true;
	}
	
	return false;
}