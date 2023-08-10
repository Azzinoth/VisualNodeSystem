#include "VisualNodeArea.h"

void VisualNodeArea::RenderNode(VisualNode* Node) const
{
	if (CurrentDrawList == nullptr || Node == nullptr)
		return;

	ImGui::PushID(Node->GetID().c_str());

	Node->LeftTop = LocalToScreen(Node->GetPosition());
	if (Node->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
	{
		Node->RightBottom = Node->LeftTop + Node->GetSize() * Zoom;
	}
	else if (Node->GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
	{
		Node->RightBottom = Node->LeftTop + ImVec2(NODE_DIAMETER, NODE_DIAMETER) * Zoom;
	}

	if (IsSelected(Node))
	{
		if (Node->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
		{
			const ImVec2 LeftTop = Node->LeftTop - ImVec2(4.0f, 4.0f);
			const ImVec2 RightBottom = Node->RightBottom + ImVec2(4.0f, 4.0f);
			ImGui::GetWindowDrawList()->AddRect(LeftTop, RightBottom, IM_COL32(175, 255, 175, 255), 16.0f * Zoom);
		}
		else if (Node->GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
		{
			ImGui::GetWindowDrawList()->AddCircle(Node->LeftTop + ImVec2(NODE_DIAMETER / 2.0f, NODE_DIAMETER / 2.0f) * Zoom, NODE_DIAMETER * Zoom + 4.0f, IM_COL32(175, 255, 175, 255), 32, 4.0f);
		}
	}

	CurrentDrawList->ChannelsSetCurrent(2);
	
	if (Node->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
	{
		ImGui::SetCursorScreenPos(Node->LeftTop);
	}
	else if (Node->GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
	{
		ImGui::SetCursorScreenPos(Node->LeftTop - ImVec2(NODE_DIAMETER / 2.0f - NODE_DIAMETER / 4.0f, NODE_DIAMETER / 2.0f - NODE_DIAMETER / 4.0f) * Zoom);
	}
	Node->Draw();

	CurrentDrawList->ChannelsSetCurrent(1);
	ImGui::SetCursorScreenPos(Node->LeftTop);

	// Drawing node background layer.
	const ImU32 NodeBackgroundColor = (HoveredNode == Node || IsSelected(Node)) ? IM_COL32(75, 75, 75, 125) : IM_COL32(60, 60, 60, 125);
	if (Node->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
	{
		CurrentDrawList->AddRectFilled(Node->LeftTop, Node->RightBottom, NodeBackgroundColor, 8.0f * Zoom);
	}
	else if (Node->GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(Node->LeftTop + (Node->RightBottom - Node->LeftTop) / 2.0f,
													NODE_DIAMETER * Zoom,
													NodeBackgroundColor, 32);
	}

	if (Node->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
	{
		// Drawing caption area.
		ImVec2 TitleArea = Node->RightBottom;
		TitleArea.y = Node->LeftTop.y + GetNodeTitleHeight();
		const ImU32 NodeTitleBackgroundColor = (HoveredNode == Node || IsSelected(Node)) ? Node->TitleBackgroundColorHovered : Node->TitleBackgroundColor;

		CurrentDrawList->AddRectFilled(Node->LeftTop + ImVec2(1, 1), TitleArea, NodeTitleBackgroundColor, 8.0f * Zoom);
		CurrentDrawList->AddRect(Node->LeftTop, Node->RightBottom, ImColor(100, 100, 100), 8.0f * Zoom);

		const ImVec2 TextSize = ImGui::CalcTextSize(Node->GetName().c_str());
		ImVec2 TextPosition;
		TextPosition.x = Node->LeftTop.x + (Node->GetSize().x * Zoom / 2) - TextSize.x / 2;
		TextPosition.y = Node->LeftTop.y + (GetNodeTitleHeight() / 2) - TextSize.y / 2;

		ImGui::SetCursorScreenPos(TextPosition);
		ImGui::Text(Node->GetName().c_str());
	}
	else if (Node->GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
	{
		CurrentDrawList->AddCircle(Node->LeftTop + ImVec2(NODE_DIAMETER / 2.0f, NODE_DIAMETER / 2.0f) * Zoom, NODE_DIAMETER * Zoom + 2.0f, ImColor(100, 100, 100), 32, 2.0f);
	}

	RenderNodeSockets(Node);

	ImGui::PopID();
}

void VisualNodeArea::RenderNodeSockets(const VisualNode* Node) const
{
	for (size_t i = 0; i < Node->Input.size(); i++)
	{
		RenderNodeSocket(Node->Input[i]);
	}

	for (size_t i = 0; i < Node->Output.size(); i++)
	{
		RenderNodeSocket(Node->Output[i]);
	}
}

void VisualNodeArea::RenderNodeSocket(NodeSocket* Socket) const
{
	const ImVec2 SocketPosition = SocketToPosition(Socket);
	if (Socket->GetParent()->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
	{
		const bool Input = !Socket->bOutput;
		// Socket description.
		const ImVec2 TextSize = ImGui::CalcTextSize(Socket->GetName().c_str());

		float TextX = SocketPosition.x;
		TextX += Input ? GetNodeSocketSize() * 2.0f : -GetNodeSocketSize() * 2.0f - TextSize.x;

		ImGui::SetCursorScreenPos(ImVec2(TextX, SocketPosition.y - TextSize.y / 2.0f));
		ImGui::Text(Socket->GetName().c_str());
	}

	ImColor SocketColor = DEFAULT_NODE_SOCKET_COLOR;
	if (NodeSocket::SocketTypeToColorAssosiations.find(Socket->GetType()) != NodeSocket::SocketTypeToColorAssosiations.end())
		SocketColor = NodeSocket::SocketTypeToColorAssosiations[Socket->GetType()];

	ImColor SocketInternalPartColor = ImColor(30, 30, 30);
	if (SocketHovered == Socket)
	{
		SocketColor = SocketColor.Value + ImColor(50, 50, 50).Value;
		SocketInternalPartColor = ImColor(50, 50, 50);
		// If potential connection can't be established we will provide visual indication.
		if (SocketLookingForConnection != nullptr)
		{
			char** msg = new char*;
			*msg = nullptr;
			SocketColor = SocketHovered->GetParent()->CanConnect(SocketHovered, SocketLookingForConnection, msg) ?
																				ImColor(50, 200, 50) : ImColor(200, 50, 50);

			if (*msg != nullptr)
			{
				ImGui::Begin("socket connection info", nullptr, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
				ImGui::Text(*msg);
				ImGui::End();

				delete msg;
			}
		}
	}

	if (SocketLookingForConnection == Socket)
	{
		static VisualNodeConnectionStyle DefaultConnectionStyle;

		ImColor ConnectionColor = ImColor(200, 200, 200);
		if (NodeSocket::SocketTypeToColorAssosiations.find(SocketLookingForConnection->GetType()) != NodeSocket::SocketTypeToColorAssosiations.end())
			ConnectionColor = NodeSocket::SocketTypeToColorAssosiations[SocketLookingForConnection->GetType()];
		
		CurrentDrawList->ChannelsSetCurrent(3);
		DrawHermiteLine(SocketPosition, ImGui::GetIO().MousePos, 12, ConnectionColor, &DefaultConnectionStyle);
	}

	// Draw socket icon.
	CurrentDrawList->AddCircleFilled(SocketPosition, GetNodeSocketSize(), SocketColor);
	if (Socket->SocketConnected.empty())
	{
		float InternalSocketSizeFactor = 0.6f;
		ImVec2 InternalPartShift = ImVec2(GetNodeSocketSize() * InternalSocketSizeFactor / 32.0f, GetNodeSocketSize() * InternalSocketSizeFactor / 32.0f);
		CurrentDrawList->AddCircleFilled(SocketPosition + InternalPartShift, GetNodeSocketSize() * InternalSocketSizeFactor, SocketInternalPartColor);
	}
}

void VisualNodeArea::RenderGrid(ImVec2 CurrentPosition) const
{
	CurrentDrawList->ChannelsSplit(2);

	CurrentPosition.x += RenderOffset.x;
	CurrentPosition.y += RenderOffset.y;

	// Adjust grid step size based on zoom level.
	float ZoomedGridStep = VISUAL_NODE_GRID_STEP * Zoom;
	
	// Horizontal lines
	const int StartingStep = static_cast<int>(ceil(-GRID_SIZE));
	const int StepCount = static_cast<int>(ceil(GRID_SIZE));
	for (int i = StartingStep; i < StepCount; i++)
	{
		ImVec2 from = ImVec2(CurrentPosition.x - GRID_SIZE * Zoom, CurrentPosition.y + i * ZoomedGridStep);
		ImVec2 to = ImVec2(CurrentPosition.x + GRID_SIZE * Zoom * 4, CurrentPosition.y + i * ZoomedGridStep);

		if (i % BOLD_LINE_FREQUENCY != 0)
		{
			CurrentDrawList->ChannelsSetCurrent(1);
			CurrentDrawList->AddLine(from, to, ImGui::GetColorU32(GridLinesColor), DEFAULT_LINE_WIDTH);
		}
		else
		{
			CurrentDrawList->ChannelsSetCurrent(0);
			CurrentDrawList->AddLine(from, to, ImGui::GetColorU32(GridBoldLinesColor), BOLD_LINE_WIDTH);
		}
	}

	// Vertical lines
	for (int i = StartingStep; i < StepCount; i++)
	{
		ImVec2 from = ImVec2(CurrentPosition.x + i * ZoomedGridStep, CurrentPosition.y - GRID_SIZE * Zoom);
		ImVec2 to = ImVec2(CurrentPosition.x + i * ZoomedGridStep, CurrentPosition.y + GRID_SIZE * Zoom * 4);

		if (i % BOLD_LINE_FREQUENCY != 0)
		{
			CurrentDrawList->ChannelsSetCurrent(1);
			CurrentDrawList->AddLine(from, to, ImGui::GetColorU32(GridLinesColor), DEFAULT_LINE_WIDTH);
		}
		else
		{
			CurrentDrawList->ChannelsSetCurrent(0);
			CurrentDrawList->AddLine(from, to, ImGui::GetColorU32(GridBoldLinesColor), BOLD_LINE_WIDTH);
		}
	}

	CurrentDrawList->ChannelsMerge();
}

void VisualNodeArea::Render()
{
	ImGuiStyle OriginalStyle = ImGui::GetStyle();
	ImGuiStyle& ZoomStyle = ImGui::GetStyle();
	ZoomStyle = OriginalStyle;
	ZoomStyle.ScaleAllSizes(Zoom * 2.0f);
	
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, GridBackgroundColor);

	const ImVec2 CurrentPosition = ImGui::GetCurrentWindow()->Pos + AreaPosition;
	ImGui::SetNextWindowPos(CurrentPosition);

	if (bFillWindow)
	{
		auto NodeAreaParentWindow = ImGui::GetCurrentWindow();
		SetAreaSize(NodeAreaParentWindow->Size - ImVec2(2, 2));
	}

	ImGui::BeginChild("Nodes area", GetAreaSize(), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	ImGui::SetWindowFontScale(Zoom);

	NodeAreaWindow = ImGui::GetCurrentWindow();
	CurrentDrawList = ImGui::GetWindowDrawList();

	RenderGrid(CurrentPosition);

	// 0 - connections.
	// 1 - main node rect.
	// 2 - for custom node draw.
	// 3 - for line that represent new connection.
	CurrentDrawList->ChannelsSplit(4);

	CurrentDrawList->ChannelsSetCurrent(1);
	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		RenderNode(Nodes[i]);
	}

	// Connection should be on node's top layer.
	// But with my current realization it would be better to call it after renderNode.
	CurrentDrawList->ChannelsSetCurrent(0);
	for (size_t i = 0; i < Connections.size(); i++)
	{
		RenderConnection(Connections[i]);

		/*if (Connections[i]->bHovered)
		{
			Connections[i]->Style.ForceColor = DEFAULT_NODE_SOCKET_MOUSE_HOVERED_CONNECTION_COLOR;
		}
		else
		{
			Connections[i]->Style.ForceColor = DEFAULT_NODE_SOCKET_CONNECTION_COLOR;
		}*/

		/*if (MouseSelectRegionMin.x != FLT_MAX && MouseSelectRegionMin.y != FLT_MAX &&
			MouseSelectRegionMax.x != FLT_MAX && MouseSelectRegionMax.y != FLT_MAX)
		{
			if (IsConnectionInRegion(Connections[i], 12))
			{
				ImColor* DefaultColor = new ImColor(200, 0, 0);
				Connections[i]->Out->SetForcedConnectionColor(DefaultColor);
			}
			else
			{
				if (Connections[i]->Out->ConnectionStyle.ForceColor != nullptr)
				{
					delete Connections[i]->Out->ConnectionStyle.ForceColor;
					Connections[i]->Out->ConnectionStyle.ForceColor = nullptr;
				}
			}
		}*/
	}

	// ************************* RENDER CONTEXT MENU *************************
	if (bOpenMainContextMenu && MainContextMenuFunc != nullptr)
	{
		bOpenMainContextMenu = false;
		ImGui::OpenPopup("##main_context_menu");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginPopup("##main_context_menu"))
	{
		if (MainContextMenuFunc != nullptr)
			MainContextMenuFunc();

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
	// ************************* RENDER CONTEXT MENU END *************************

	CurrentDrawList->ChannelsMerge();
	CurrentDrawList = nullptr;

	// Draw mouse selection region.
	if (MouseSelectRegionMin.x != FLT_MAX && MouseSelectRegionMin.y != FLT_MAX &&
		MouseSelectRegionMax.x != FLT_MAX && MouseSelectRegionMax.y != FLT_MAX)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(MouseSelectRegionMin, MouseSelectRegionMax, IM_COL32(175, 175, 255, 125), 1.0f);
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	// Reset to the original style before zooming.
	ImGuiStyle& CurrentStyle = ImGui::GetStyle();
	CurrentStyle = OriginalStyle;
}

void VisualNodeArea::DrawHermiteLine(const ImVec2 P1, const ImVec2 P2, const int Steps, const ImColor Color, const VisualNodeConnectionStyle* Style) const
{
	const ImVec2 t1 = ImVec2(80.0f, 0.0f);
	const ImVec2 t2 = ImVec2(80.0f, 0.0f);

	if (Style->bMarchingAntsEffect)
	{
		double Time = glfwGetTime();
		int Offset = static_cast<int>(Time * 20.0f * Style->MarchingAntsSpeed) % Steps;
		ImVec2 LastPoint = ImVec2(0, 0);

		for (int Step = 0; Step <= Steps; Step++)
		{
			const float t = static_cast<float>(Step) / static_cast<float>(Steps);
			const float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
			const float h2 = -2 * t * t * t + 3 * t * t;
			const float h3 = t * t * t - 2 * t * t + t;
			const float h4 = t * t * t - t * t;

			ImVec2 CurrentPoint = ImVec2(h1 * P1.x + h2 * P2.x + h3 * t1.x + h4 * t2.x, h1 * P1.y + h2 * P2.y + h3 * t1.y + h4 * t2.y);

			if (Step != 0) // Avoid drawing on the first step because lastPoint is not valid.
			{
				float Thickness = 0.0f;
				float Intensity = 0.0f;
				if (Style->bMarchingAntsReverseDirection)
				{
					Thickness = ((Step + Offset) % Steps < Steps / 10) ? 5.0f : 3.0f;
					Intensity = ((Step + Offset) % Steps < Steps / 10) ? 1.2f : 1.0f;
				}
				else
				{
					Thickness = ((Step - Offset + Steps) % Steps < Steps / 10) ? 5.0f : 3.0f;
					Intensity = ((Step - Offset + Steps) % Steps < Steps / 10) ? 1.2f : 1.0f;
				}

				ImColor ModifiedColor = ImColor(Color.Value.x * Intensity, Color.Value.y * Intensity, Color.Value.z * Intensity, Color.Value.w);
				CurrentDrawList->AddLine(LastPoint, CurrentPoint, ModifiedColor, Thickness * Zoom);
			}

			LastPoint = CurrentPoint;
		}

		CurrentDrawList->PathStroke(Color, false, GetConnectionThickness());
	}
	else if (Style->bPulseEffect)
	{
		for (int Step = 0; Step <= Steps; Step++)
		{
			const float t = static_cast<float>(Step) / static_cast<float>(Steps);
			const float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
			const float h2 = -2 * t * t * t + 3 * t * t;
			const float h3 = t * t * t - 2 * t * t + t;
			const float h4 = t * t * t - t * t;

			CurrentDrawList->PathLineTo(ImVec2(h1 * P1.x + h2 * P2.x + h3 * t1.x + h4 * t2.x, h1 * P1.y + h2 * P2.y + h3 * t1.y + h4 * t2.y));
		}

		double Time = glfwGetTime();
		float Pulse = static_cast<float>((sin(Time * 5 * Style->PulseSpeed) + 1.0f) / 2.0f);
		Pulse = glm::max(Style->PulseMin, Pulse);
		ImColor PulseColor = ImColor(Color.Value.x, Color.Value.y, Color.Value.z, Pulse);

		CurrentDrawList->PathStroke(PulseColor, false, GetConnectionThickness());
	}
	else
	{
		ImVec2 LastPoint = ImVec2(0, 0);
		for (int Step = 0; Step <= Steps; Step++)
		{
			const float t = static_cast<float>(Step) / static_cast<float>(Steps);
			const float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
			const float h2 = -2 * t * t * t + 3 * t * t;
			const float h3 = t * t * t - 2 * t * t + t;
			const float h4 = t * t * t - t * t;

			ImVec2 CurrentPoint = ImVec2(h1 * P1.x + h2 * P2.x + h3 * t1.x + h4 * t2.x, h1 * P1.y + h2 * P2.y + h3 * t1.y + h4 * t2.y);
			CurrentDrawList->PathLineTo(CurrentPoint);

			LastPoint = CurrentPoint;
		}

		CurrentDrawList->PathStroke(Color, false, GetConnectionThickness());
	}
}

void VisualNodeArea::DrawHermiteLine(const ImVec2 P1, const ImVec2 P2, const int Steps, const ImColor Color, const float Thickness) const
{
	const ImVec2 t1 = ImVec2(80.0f, 0.0f);
	const ImVec2 t2 = ImVec2(80.0f, 0.0f);

	ImVec2 LastPoint = ImVec2(0, 0);
	for (int Step = 0; Step <= Steps; Step++)
	{
		const float t = static_cast<float>(Step) / static_cast<float>(Steps);
		const float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
		const float h2 = -2 * t * t * t + 3 * t * t;
		const float h3 = t * t * t - 2 * t * t + t;
		const float h4 = t * t * t - t * t;

		ImVec2 CurrentPoint = ImVec2(h1 * P1.x + h2 * P2.x + h3 * t1.x + h4 * t2.x, h1 * P1.y + h2 * P2.y + h3 * t1.y + h4 * t2.y);
		CurrentDrawList->PathLineTo(CurrentPoint);

		LastPoint = CurrentPoint;
	}

	CurrentDrawList->PathStroke(Color, false, Thickness);
}

void VisualNodeArea::RenderConnection(const VisualNodeConnection* Connection) const
{
	if (Connection->Out == nullptr || Connection->In == nullptr)
		return;

	ImColor CurrentConnectionColor = Connection->Style.ForceColor;
	if (NodeSocket::SocketTypeToColorAssosiations.find(Connection->Out->GetType()) != NodeSocket::SocketTypeToColorAssosiations.end())
		CurrentConnectionColor = NodeSocket::SocketTypeToColorAssosiations[Connection->Out->GetType()];

	std::vector<VisualNodeConnectionSegment> Segments = GetConnectionSegments(Connection);
	for (size_t i = 0; i < Segments.size(); i++)
	{
		ImVec2 BeginPosition = Segments[i].Begin;
		ImVec2 EndPosition = Segments[i].End;

		if (Connection->bSelected)
		{
			DrawHermiteLine(BeginPosition, EndPosition, 12, ImColor(55, 255, 55), GetConnectionThickness() + GetConnectionThickness() * 1.2f);
		}
		else if (Connection->bHovered)
		{
			DrawHermiteLine(BeginPosition, EndPosition, 12, ImColor(55, 55, 250), GetConnectionThickness() + GetConnectionThickness() * 1.2f);
		}

		DrawHermiteLine(BeginPosition, EndPosition, 12, CurrentConnectionColor, &Connection->Style);

		// If it is reroute than we should render circle.
		if (i > 0)
			RenderReroute(Connection->RerouteConnections[i - 1]);
	}
}

void VisualNodeArea::RenderReroute(const VisualNodeRerouteNode* RerouteNode) const
{
	if (RerouteNode->bSelected)
	{
		CurrentDrawList->AddCircleFilled(LocalToScreen(RerouteNode->Position), GetRerouteNodeSize() * 1.2f, ImColor(55, 255, 55));
	}
	else if (RerouteNode->bHovered)
	{
		CurrentDrawList->AddCircleFilled(LocalToScreen(RerouteNode->Position), GetRerouteNodeSize() * 1.2f, ImColor(55, 55, 250));
	}

	CurrentDrawList->AddCircleFilled(LocalToScreen(RerouteNode->Position), GetRerouteNodeSize(), ImColor(DEFAULT_NODE_SOCKET_COLOR.Value + ImColor(15, 25, 15).Value));
}

ImVec2 VisualNodeArea::SocketToPosition(const NodeSocket* Socket) const
{
	const bool Input = !Socket->bOutput;
	float SocketX = 0.0f;
	float SocketY = 0.0f;

	int SocketIndex = -1;
	if (Input)
	{
		for (size_t i = 0; i < Socket->Parent->Input.size(); i++)
		{
			if (Socket->Parent->Input[i] == Socket)
			{
				SocketIndex = static_cast<int>(i);
				break;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < Socket->Parent->Output.size(); i++)
		{
			if (Socket->Parent->Output[i] == Socket)
			{
				SocketIndex = static_cast<int>(i);
				break;
			}
		}
	}

	if (Socket->GetParent()->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
	{
		SocketX = Input ? Socket->Parent->LeftTop.x + GetNodeSocketSize() * 3 : Socket->Parent->RightBottom.x - GetNodeSocketSize() * 3;

		const float HeightForSockets = Socket->Parent->GetSize().y * Zoom - GetNodeTitleHeight();
		const float SocketSpacing = HeightForSockets / (Input ? Socket->Parent->Input.size() : Socket->Parent->Output.size());

		SocketY = (Socket->Parent->LeftTop.y + GetNodeTitleHeight() + SocketSpacing * (SocketIndex + 1) - SocketSpacing / 2.0f);
	}
	else if (Socket->GetParent()->GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
	{
		const size_t SocketCount = Input ? Socket->Parent->Input.size() : Socket->Parent->Output.size();
		float BeginAngle = (180.0f / static_cast<float>(SocketCount) / 2.0f);
		if (Input)
			BeginAngle = -BeginAngle;

		float step = (180.0f / static_cast<float>(SocketCount) * (SocketIndex));
		if (Input)
			step = -step;

		const float angle = BeginAngle + step;

		const float NodeCenterX = Socket->Parent->LeftTop.x + NODE_DIAMETER * Zoom / 2.0f;
		const float NodeCenterY = Socket->Parent->LeftTop.y + NODE_DIAMETER * Zoom / 2.0f;

		SocketX = NodeCenterX + NODE_DIAMETER * Zoom * 0.95f * sin(glm::radians(angle));
		SocketY = NodeCenterY + NODE_DIAMETER * Zoom * 0.95f * cos(glm::radians(angle));
	}

	return {SocketX, SocketY};
}

ImVec2 VisualNodeArea::GetAreaRenderOffset() const
{
	return RenderOffset;
}

void VisualNodeArea::SetAreaRenderOffset(const ImVec2 Offset)
{
	if (Offset.x <= -GRID_SIZE || Offset.x >= GRID_SIZE ||
		Offset.y <= -GRID_SIZE || Offset.y >= GRID_SIZE)
		return;

	RenderOffset = Offset;
}

void VisualNodeArea::GetAllNodesAABB(ImVec2& Min, ImVec2& Max) const
{
	Min.x = FLT_MAX;
	Min.y = FLT_MAX;

	Max.x = -FLT_MAX;
	Max.y = -FLT_MAX;

	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetPosition().x + RenderOffset.x < Min.x)
			Min.x = Nodes[i]->GetPosition().x + RenderOffset.x;

		if (Nodes[i]->GetPosition().x + RenderOffset.x + Nodes[i]->GetSize().x > Max.x)
			Max.x = Nodes[i]->GetPosition().x + RenderOffset.x + Nodes[i]->GetSize().x;

		if (Nodes[i]->GetPosition().y + RenderOffset.y < Min.y)
			Min.y = Nodes[i]->GetPosition().y + RenderOffset.y;

		if (Nodes[i]->GetPosition().y + RenderOffset.y + Nodes[i]->GetSize().y > Max.y)
			Max.y = Nodes[i]->GetPosition().y + RenderOffset.y + Nodes[i]->GetSize().y;
	}
}

ImVec2 VisualNodeArea::GetAllNodesAABBCenter() const
{
	ImVec2 min, max;
	GetAllNodesAABB(min, max);

	return {min.x + (max.x - min.x) / 2.0f, min.y + (max.y - min.y) / 2.0f};
}

ImVec2 VisualNodeArea::GetRenderedViewCenter() const
{
	if (NodeAreaWindow != nullptr)
	{
		return (NodeAreaWindow->Size / 2.0f - RenderOffset) / Zoom;
	}
	else
	{
		return (ImGui::GetCurrentWindow()->Size / 2.0f - RenderOffset) / Zoom;
	}
}

bool VisualNodeArea::IsAreaFillingWindow()
{
	return bFillWindow;
}

void VisualNodeArea::SetIsAreaFillingWindow(bool NewValue)
{
	bFillWindow = NewValue;
}

void VisualNodeArea::ApplyZoom(float Delta)
{
	ImVec2 MousePosBeforeZoom = ScreenToLocal(ImGui::GetMousePos());

	Zoom += Delta * 0.1f;
	Zoom = std::max(Zoom, MIN_ZOOM_LEVEL);
	Zoom = std::min(Zoom, MAX_ZOOM_LEVEL);

	ImVec2 MousePosAfterZoom = ScreenToLocal(ImGui::GetMousePos());

	// Adjust render offset to keep the mouse over the same point after zooming
	RenderOffset -= (MousePosBeforeZoom - MousePosAfterZoom) * Zoom;
}

float VisualNodeArea::GetZoomFactor() const
{
	return Zoom;
}

void VisualNodeArea::SetZoomFactor(float NewValue)
{
	if (NewValue < MIN_ZOOM_LEVEL || NewValue > MAX_ZOOM_LEVEL)
		return;

	Zoom = NewValue;
}

bool VisualNodeArea::GetConnectionStyle(VisualNode* Node, bool bOutputSocket, size_t SocketIndex, VisualNodeConnectionStyle& Style) const
{
	if (Node == nullptr || SocketIndex < 0)
		return false;

	if (bOutputSocket)
	{
		if (SocketIndex >= Node->Output.size())
			return false;

		VisualNodeConnectionStyle* TempVariable = GetConnectionStyle(Node->Output[SocketIndex]);
		if (TempVariable != nullptr)
		{
			Style = *TempVariable;
			return true;
		}

		return false;
	}
	else
	{
		if (SocketIndex >= Node->Input.size())
			return false;

		VisualNodeConnectionStyle* TempVariable = GetConnectionStyle(Node->Input[SocketIndex]);
		if (TempVariable != nullptr)
		{
			Style = *TempVariable;
			return true;
		}

		return false;
	}
}

void VisualNodeArea::SetConnectionStyle(VisualNode* Node, bool bOutputSocket, size_t SocketIndex, VisualNodeConnectionStyle NewStyle)
{
	if (Node == nullptr || SocketIndex < 0)
		return;

	if (bOutputSocket)
	{
		if (SocketIndex >= Node->Output.size())
			return;

		VisualNodeConnectionStyle* TempVariable = GetConnectionStyle(Node->Output[SocketIndex]);
		if (TempVariable != nullptr)
			*TempVariable = NewStyle;
	}
	else
	{
		if (SocketIndex >= Node->Input.size())
			return;

		VisualNodeConnectionStyle* TempVariable = GetConnectionStyle(Node->Input[SocketIndex]);
		if (TempVariable != nullptr)
			*TempVariable = NewStyle;
	}
}