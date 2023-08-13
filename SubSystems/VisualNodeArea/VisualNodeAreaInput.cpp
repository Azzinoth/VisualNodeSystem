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
	HoveredNode = nullptr;
	SocketHovered = nullptr;
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

	for (size_t i = 0; i < Nodes.size(); i++)
		Nodes[i]->SetIsHovered(false);

	for (size_t i = 0; i < Nodes.size(); i++)
	{
		InputUpdateNode(Nodes[i]);
		if (HoveredNode != nullptr)
			break;
	}

	if (ImGui::IsMouseDragging(0) && MouseDownIn == NodeAreaWindow)
		MouseDragging();

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
}

void NodeArea::LeftMouseClick()
{
	LeftMouseClickNodesUpdate();
	LeftMouseClickConnectionsUpdate();
	LeftMouseClickRerouteUpdate();
}

void NodeArea::LeftMouseClickNodesUpdate()
{
	if (HoveredNode != nullptr)
	{
		if (!IsSelected(HoveredNode) && !ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_LEFT_CONTROL)) && !ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_RIGHT_CONTROL)))
			SelectedNodes.clear();

		// If it is new selection
		if (!IsSelected(HoveredNode))
			UnSelectAllRerouteNodes();

		AddSelected(HoveredNode);
	}
	else
	{
		SelectedNodes.clear();
	}
}

void NodeArea::LeftMouseClickConnectionsUpdate()
{
	if (HoveredConnection != nullptr)
	{
		if (!IsSelected(HoveredConnection) && !ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_LEFT_CONTROL)) && !ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_RIGHT_CONTROL)))
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
	if (RerouteNodeHovered != nullptr || (HoveredNode != nullptr && IsSelected(HoveredNode)))
	{
		if (!IsSelected(RerouteNodeHovered) && !ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_LEFT_CONTROL)) && !ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_RIGHT_CONTROL)))
		{
			if (!(HoveredNode != nullptr && IsSelected(HoveredNode)))
				UnSelectAllRerouteNodes();
		}

		AddSelected(RerouteNodeHovered);
	}
	else
	{
		UnSelectAllRerouteNodes();
	}
}

void NodeArea::RightMouseClick()
{
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
	else
	{
		SelectedNodes.clear();
		bOpenMainContextMenu = true;
	}
}

void NodeArea::RightMouseClickConnectionsUpdate()
{

}

void NodeArea::RightMouseClickRerouteUpdate()
{

}

void NodeArea::MouseDragging()
{
	if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_LEFT_SHIFT)) || ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_RIGHT_SHIFT)))
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

	if (!IsMouseRegionSelectionActive())
	{
		if (SocketLookingForConnection == nullptr)
		{
			if (SelectedNodes.empty() && SocketHovered == nullptr && SelectedRerouteNodes.empty() && RerouteNodeHovered == nullptr)
			{
				RenderOffset.x += GetMouseDelta().x * Zoom;
				RenderOffset.y += GetMouseDelta().y * Zoom;

				if (RenderOffset.x > GRID_SIZE * Zoom)
					RenderOffset.x = GRID_SIZE * Zoom;

				if (RenderOffset.x < -GRID_SIZE * Zoom)
					RenderOffset.x = -GRID_SIZE * Zoom;

				if (RenderOffset.y > GRID_SIZE * Zoom)
					RenderOffset.y = GRID_SIZE * Zoom;

				if (RenderOffset.y < -GRID_SIZE * Zoom)
					RenderOffset.y = -GRID_SIZE * Zoom;
			}
		}
	}

	MouseDraggingNodesUpdate();
	MouseDraggingConnectionsUpdate();
	MouseDraggingRerouteUpdate();
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
				if (Nodes[i]->LeftTop.x < MouseSelectRegionMin.x + RegionSize.x &&
					Nodes[i]->LeftTop.x + Nodes[i]->GetSize().x * Zoom > MouseSelectRegionMin.x &&
					Nodes[i]->LeftTop.y < MouseSelectRegionMin.y + RegionSize.y &&
					Nodes[i]->GetSize().y * Zoom + Nodes[i]->LeftTop.y > MouseSelectRegionMin.y)
				{
					AddSelected(Nodes[i]);
				}
			}
			else if (Nodes[i]->GetStyle() == CIRCLE)
			{
				if (Nodes[i]->LeftTop.x < MouseSelectRegionMin.x + RegionSize.x &&
					Nodes[i]->LeftTop.x + NODE_DIAMETER * Zoom > MouseSelectRegionMin.x &&
					Nodes[i]->LeftTop.y < MouseSelectRegionMin.y + RegionSize.y &&
					NODE_DIAMETER * Zoom + Nodes[i]->LeftTop.y > MouseSelectRegionMin.y)
				{
					AddSelected(Nodes[i]);
				}
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
	const ImVec2 RegionSize = MouseSelectRegionMax - MouseSelectRegionMin;
	if (IsMouseRegionSelectionActive())
	{
		UnSelectAllRerouteNodes();
		for (size_t i = 0; i < Connections.size(); i++)
		{
			for (size_t j = 0; j < Connections[i]->RerouteNodes.size(); j++)
			{

				const ImVec2 ReroutePosition = LocalToScreen(Connections[i]->RerouteNodes[j]->Position);
				if (ReroutePosition.x < MouseSelectRegionMin.x + RegionSize.x &&
					ReroutePosition.x + GetRerouteNodeSize() > MouseSelectRegionMin.x &&
					ReroutePosition.y < MouseSelectRegionMin.y + RegionSize.y &&
					GetRerouteNodeSize() + ReroutePosition.y > MouseSelectRegionMin.y)
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

void NodeArea::KeyboardInputUpdate()
{
	if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_DELETE)))
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
	}

	static bool WasCopiedToClipboard = false;
	if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_LEFT_CONTROL)) || ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_RIGHT_CONTROL)))
	{
		if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_C)))
		{
			if (!SelectedNodes.empty())
			{
				const NodeArea* NewNodeArea = NodeArea::CreateNodeArea(SelectedNodes);
				FocalEngine::APPLICATION.SetClipboardText(NewNodeArea->ToJson());
				delete NewNodeArea;
			}
		}
		else if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_V)))
		{
			if (!WasCopiedToClipboard)
			{
				WasCopiedToClipboard = true;

				const std::string NodesToImport = FocalEngine::APPLICATION.GetClipboardText();
				Json::Value data;

				JSONCPP_STRING err;
				const Json::CharReaderBuilder builder;

				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				if (!reader->parse(NodesToImport.c_str(), NodesToImport.c_str() + NodesToImport.size(), &data, &err))
					return;

				NodeArea* NewNodeArea = NodeArea::FromJson(NodesToImport);

				// ***************** Place new nodes in center of a view space *****************
				const ImVec2 ViewCenter = GetRenderedViewCenter();
				ImVec2 NodesAABBCenter = NewNodeArea->GetAllNodesAABBCenter();
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
				// ***************** Place new nodes in center of a view space END *****************

				NodeArea::CopyNodesTo(NewNodeArea, this);

				// Unselect all elements.
				SelectedNodes.clear();
				UnSelectAllConnections();
				UnSelectAllRerouteNodes();

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

				delete NewNodeArea;
			}
		}
	}

	if (!ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_V)))
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

void NodeArea::ClearSelection()
{
	SelectedNodes.clear();
	SelectedConnections.clear();
	UnSelectAllRerouteNodes();
}

void NodeArea::InputUpdateNode(Node* Node)
{
	if (Node->GetStyle() == DEFAULT)
	{
		if (Node->LeftTop.x < MouseCursorPosition.x + MouseCursorSize.x &&
			Node->LeftTop.x + Node->GetSize().x * Zoom > MouseCursorPosition.x &&
			Node->LeftTop.y < MouseCursorPosition.y + MouseCursorSize.y &&
			Node->GetSize().y * Zoom + Node->LeftTop.y > MouseCursorPosition.y)
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

bool NodeArea::IsMouseOverConnection(Connection* Connection, const int Steps, const float MaxDistance, ImVec2& CollisionPoint)
{
	if (Connection->RerouteNodes.empty())
		return IsMouseOverSegment(SocketToPosition(Connection->Out), SocketToPosition(Connection->In), LineSegments, 10.0f);

	std::vector<ConnectionSegment> Segments = GetConnectionSegments(Connection);
	for (size_t i = 0; i < Segments.size(); i++)
	{
		if (IsMouseOverSegment(Segments[i].Begin, Segments[i].End, LineSegments, 10.0f))
			return true;
	}

	return false;
}

bool NodeArea::IsMouseOverSegment(ImVec2 Begin, ImVec2 End, const int Steps, const float maxDistance, ImVec2& CollisionPoint)
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
			CollisionPoint = Projection;
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

int Orientation(const ImVec2& p, const ImVec2& q, const ImVec2& r)
{
	float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0; // Collinear
	return (val > 0) ? 1 : 2; // Clockwise or Counterclockwise
}

bool OnSegment(const ImVec2& p, const ImVec2& q, const ImVec2& r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;
	return false;
}

bool IsLineSegmentIntersecting(const ImVec2& p1, const ImVec2& q1, const ImVec2& p2, const ImVec2& q2)
{
	// Find the four orientations
	int o1 = Orientation(p1, q1, p2);
	int o2 = Orientation(p1, q1, q2);
	int o3 = Orientation(p2, q2, p1);
	int o4 = Orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special cases: check if the segments are collinear and overlap
	if (o1 == 0 && OnSegment(p1, p2, q1)) return true;
	if (o2 == 0 && OnSegment(p1, q2, q1)) return true;
	if (o3 == 0 && OnSegment(p2, p1, q2)) return true;
	if (o4 == 0 && OnSegment(p2, q1, q2)) return true;

	return false; // If none of the above cases, return false
}

bool NodeArea::IsConnectionInRegion(Connection* Connection, const int Steps)
{
	if (Connection->RerouteNodes.empty())
		return IsSegmentInRegion(SocketToPosition(Connection->Out), SocketToPosition(Connection->In), LineSegments);

	std::vector<ConnectionSegment> Segments = GetConnectionSegments(Connection);
	for (size_t i = 0; i < Segments.size(); i++)
	{
		if (IsSegmentInRegion(Segments[i].Begin, Segments[i].End, LineSegments))
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

		ImVec2 segmentStart = ImVec2(h1 * Begin.x + h2 * End.x + h3 * t1.x + h4 * t2.x, h1 * Begin.y + h2 * End.y + h3 * t1.y + h4 * t2.y);

		t = static_cast<float>(Step + 1) / static_cast<float>(Steps); // Update t for the end segment.
		h1 = +2 * t * t * t - 3 * t * t + 1.0f;
		h2 = -2 * t * t * t + 3 * t * t;
		h3 = t * t * t - 2 * t * t + t;
		h4 = t * t * t - t * t;

		ImVec2 segmentEnd = ImVec2(h1 * Begin.x + h2 * End.x + h3 * t1.x + h4 * t2.x, h1 * Begin.y + h2 * End.y + h3 * t1.y + h4 * t2.y);

		// If either of the segment's points are in the region, the connection is in the region.
		if (IsPointInRegion(segmentStart, MouseSelectRegionMin, MouseSelectRegionMax) ||
			IsPointInRegion(segmentEnd, MouseSelectRegionMin, MouseSelectRegionMax))
		{
			return true;
		}

		// Check if the segment intersects with any of the region's edges.
		for (int i = 0; i < 4; i++)
		{
			if (IsLineSegmentIntersecting(regionCorners[i], regionCorners[(i + 1) % 4], segmentStart, segmentEnd))
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
			if (HoveredConnection == nullptr && IsMouseOverConnection(Connections[i], LineSegments, 10.0f))
			{
				Connections[i]->bHovered = true;
				HoveredConnection = Connections[i];
			}

			if (IsMouseRegionSelectionActive() && SelectedNodes.empty())
			{
				if (IsConnectionInRegion(Connections[i], LineSegments))
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
	{
		ConnectionsDoubleMouseClick();
	}
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
				if (IsMouseOverSegment(Segments[i].Begin, Segments[i].End, LineSegments, 10.0f))
				{
					AddRerouteNode(HoveredConnection, i, ScreenToLocal(MouseCursorPosition));
					break;
				}
			}
		}
	}
}