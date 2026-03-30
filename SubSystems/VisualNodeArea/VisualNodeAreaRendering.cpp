#include "VisualNodeArea.h"
using namespace VisNodeSys;

void NodeArea::RenderNode(Node* Node) const
{
	if (CurrentDrawList == nullptr || Node == nullptr)
		return;

	ImGui::PushID(Node->GetID().c_str());

	Node->LeftTop = LocalToScreen(Node->GetPosition());
	if (Node->GetStyle() == DEFAULT)
	{
		Node->RightBottom = Node->LeftTop + Node->GetSize() * Zoom;
	}
	else if (Node->GetStyle() == CIRCLE)
	{
		Node->RightBottom = Node->LeftTop + ImVec2(NODE_DIAMETER, NODE_DIAMETER) * Zoom;
	}

	if (IsSelected(Node))
	{
		if (Node->GetStyle() == DEFAULT)
		{
			const ImVec2 LeftTop = Node->LeftTop - ImVec2(4.0f, 4.0f);
			const ImVec2 RightBottom = Node->RightBottom + ImVec2(4.0f, 4.0f);
			ImGui::GetWindowDrawList()->AddRect(LeftTop, RightBottom, ImGui::GetColorU32(Settings.Style.NodeSelectionColor), 8.0f * Zoom);
		}
		else if (Node->GetStyle() == CIRCLE)
		{
			ImGui::GetWindowDrawList()->AddCircle(Node->LeftTop + ImVec2(NODE_DIAMETER / 2.0f, NODE_DIAMETER / 2.0f) * Zoom, NODE_DIAMETER * Zoom + 4.0f, ImGui::GetColorU32(Settings.Style.NodeSelectionColor), 32, 4.0f);
		}
	}

	CurrentDrawList->ChannelsSetCurrent(2);
	
	if (Node->GetStyle() == DEFAULT)
	{
		ImGui::SetCursorScreenPos(Node->LeftTop);
	}
	else if (Node->GetStyle() == CIRCLE)
	{
		ImGui::SetCursorScreenPos(Node->LeftTop - ImVec2(NODE_DIAMETER / 2.0f - NODE_DIAMETER / 4.0f, NODE_DIAMETER / 2.0f - NODE_DIAMETER / 4.0f) * Zoom);
	}
	Node->Draw();

	CurrentDrawList->ChannelsSetCurrent(1);
	ImGui::SetCursorScreenPos(Node->LeftTop);

	// Drawing node background layer.
	const ImU32 NodeBackgroundColor = (HoveredNode == Node || IsSelected(Node)) ? ImGui::GetColorU32(Settings.Style.NodeBackgroundColor) : ImGui::GetColorU32(Settings.Style.HoveredNodeBackgroundColor);
	if (Node->GetStyle() == DEFAULT)
	{
		CurrentDrawList->AddRectFilled(Node->LeftTop, Node->RightBottom, NodeBackgroundColor, 8.0f * Zoom);
	}
	else if (Node->GetStyle() == CIRCLE)
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(Node->LeftTop + (Node->RightBottom - Node->LeftTop) / 2.0f,
													NODE_DIAMETER * Zoom,
													NodeBackgroundColor, 32);
	}

	if (Node->GetStyle() == DEFAULT)
	{
		if (Node->GetRenderTitleBar())
		{
			// Drawing caption area.
			ImVec2 TitleArea = Node->RightBottom;
			TitleArea.y = Node->LeftTop.y + GetNodeTitleHeight();
			const ImU32 NodeTitleBackgroundColor = (HoveredNode == Node || IsSelected(Node)) ? Node->TitleBackgroundColorHovered : Node->TitleBackgroundColor;

			CurrentDrawList->AddRectFilled(Node->LeftTop + ImVec2(1, 1), TitleArea, NodeTitleBackgroundColor, 8.0f * Zoom);
			CurrentDrawList->AddRect(Node->LeftTop, Node->RightBottom, ImColor(100, 100, 100), 8.0f * Zoom);

			std::string NodeNameToRender = Node->GetName();
			float AvailableWidth = Node->GetSize().x * Zoom;
			NodeNameToRender = NODE_CORE.TruncateText(NodeNameToRender, AvailableWidth);
			ImVec2 TextSize = ImGui::CalcTextSize(NodeNameToRender.c_str());

			ImVec2 TextPosition;
			TextPosition.x = Node->LeftTop.x + (Node->GetSize().x * Zoom / 2) - TextSize.x / 2;
			TextPosition.y = Node->LeftTop.y + (GetNodeTitleHeight() / 2) - TextSize.y / 2;

			ImGui::SetCursorScreenPos(TextPosition);
			ImGui::Text("%s", NodeNameToRender.c_str());
		}
	}
	else if (Node->GetStyle() == CIRCLE)
	{
		CurrentDrawList->AddCircle(Node->LeftTop + ImVec2(NODE_DIAMETER / 2.0f, NODE_DIAMETER / 2.0f) * Zoom, NODE_DIAMETER * Zoom + 2.0f, ImColor(100, 100, 100), 32, 2.0f);
	}

	RenderNodeSockets(Node);

	ImGui::PopID();
}

void NodeArea::RenderNodeSockets(const Node* Node) const
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

void NodeArea::RenderNodeSocket(NodeSocket* Socket) const
{
	const ImVec2 SocketPosition = SocketToPosition(Socket);
	if (Socket->GetParent()->GetStyle() == DEFAULT)
	{
		const bool bIsInput = !Socket->bOutput;
		// Socket description.
		const ImVec2 TextSize = ImGui::CalcTextSize(Socket->GetName().c_str());

		float TextX = SocketPosition.x;
		TextX += bIsInput ? GetNodeSocketSize() * 2.0f : -GetNodeSocketSize() * 2.0f - TextSize.x;

		ImGui::SetCursorScreenPos(ImVec2(TextX, SocketPosition.y - TextSize.y / 2.0f));
		ImGui::Text("%s",Socket->GetName().c_str());
	}

	ImColor SocketColor = DEFAULT_NODE_SOCKET_COLOR;
	std::vector<std::string> SocketAllowedTypes = Socket->GetAllowedTypes();
	// TO-DO: Add support for multiple socket types.
	if (SocketAllowedTypes.size() == 1)
	{
		if (NodeSocket::SocketTypeToColorAssociations.find(SocketAllowedTypes[0]) != NodeSocket::SocketTypeToColorAssociations.end())
			SocketColor = NodeSocket::SocketTypeToColorAssociations[SocketAllowedTypes[0]];
	}

	ImColor SocketInternalPartColor = ImColor(30, 30, 30);
	if (SocketHovered == Socket)
	{
		SocketColor = SocketColor.Value + ImColor(50, 50, 50).Value;
		SocketInternalPartColor = ImColor(50, 50, 50);
		// If potential connection can't be established we will provide visual indication.
		if (SocketLookingForConnection != nullptr)
		{
			char** Message = new char*;
			*Message = nullptr;
			SocketColor = SocketHovered->GetParent()->CanConnect(SocketHovered, SocketLookingForConnection, Message) ?
																				ImColor(50, 200, 50) : ImColor(200, 50, 50);

			if (*Message != nullptr)
			{
				ImGui::Begin("socket connection info", nullptr, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
				ImGui::Text("%s",*Message);
				ImGui::End();

				delete Message;
			}
		}
	}

	if (SocketLookingForConnection == Socket)
	{
		static ConnectionStyle DefaultConnectionStyle;

		ImColor ConnectionColor = ImColor(200, 200, 200);
		std::vector<std::string> SocketAllowedTypes = SocketLookingForConnection->GetAllowedTypes();
		// TO-DO: Add support for multiple socket types.
		if (SocketAllowedTypes.size() == 1)
		{
			if (NodeSocket::SocketTypeToColorAssociations.find(SocketAllowedTypes[0]) != NodeSocket::SocketTypeToColorAssociations.end())
				ConnectionColor = NodeSocket::SocketTypeToColorAssociations[SocketAllowedTypes[0]];
		}
		
		CurrentDrawList->ChannelsSetCurrent(3);
		DrawHermiteLine(SocketPosition, ImGui::GetIO().MousePos, Settings.Style.GeneralConnection.LineSegments, ConnectionColor, &DefaultConnectionStyle);
	}

	// Change socket transparency if it can't be connected to.
	if (Settings.bReduceTransparencyForUnconnectableSockets && SocketLookingForConnection != nullptr)
	{
		bool bCanConnect = Socket->GetParent()->CanConnect(Socket, SocketLookingForConnection);
		SocketColor.Value.w = bCanConnect ? 1.0f : 0.25f;
		SocketInternalPartColor.Value.w = bCanConnect ? 1.0f : 0.25f;
	}
	
	// Draw socket icon.
	CurrentDrawList->AddCircleFilled(SocketPosition, GetNodeSocketSize(), SocketColor);
	if (Socket->ConnectedSockets.empty())
	{
		float InternalSocketSizeFactor = 0.6f;
		ImVec2 InternalPartShift = ImVec2(GetNodeSocketSize() * InternalSocketSizeFactor / 32.0f, GetNodeSocketSize() * InternalSocketSizeFactor / 32.0f);
		CurrentDrawList->AddCircleFilled(SocketPosition + InternalPartShift, GetNodeSocketSize() * InternalSocketSizeFactor, SocketInternalPartColor);
	}
	
}

void NodeArea::RenderGrid(ImVec2 CurrentPosition) const
{
	CurrentDrawList->ChannelsSplit(2);

	CurrentPosition.x += RenderOffset.x;
	CurrentPosition.y += RenderOffset.y;

	// Adjust grid step size based on zoom level.
	float ZoomedGridStep = NODE_GRID_STEP * Zoom;
	
	// Horizontal lines
	const int StartingStep = static_cast<int>(ceil(-Settings.Style.Grid.GRID_SIZE));
	const int StepCount = static_cast<int>(ceil(Settings.Style.Grid.GRID_SIZE));
	for (int i = StartingStep; i < StepCount; i++)
	{
		ImVec2 LineStart = ImVec2(CurrentPosition.x - Settings.Style.Grid.GRID_SIZE * Zoom, CurrentPosition.y + i * ZoomedGridStep);
		ImVec2 LineEnd = ImVec2(CurrentPosition.x + Settings.Style.Grid.GRID_SIZE * Zoom * 4, CurrentPosition.y + i * ZoomedGridStep);

		if (i % Settings.Style.Grid.BOLD_LINE_FREQUENCY != 0)
		{
			CurrentDrawList->ChannelsSetCurrent(1);
			CurrentDrawList->AddLine(LineStart, LineEnd, ImGui::GetColorU32(Settings.Style.Grid.GridLinesColor), Settings.Style.Grid.DEFAULT_LINE_WIDTH);
		}
		else
		{
			CurrentDrawList->ChannelsSetCurrent(0);
			CurrentDrawList->AddLine(LineStart, LineEnd, ImGui::GetColorU32(Settings.Style.Grid.GridBoldLinesColor), Settings.Style.Grid.BOLD_LINE_WIDTH);
		}
	}

	// Vertical lines
	for (int i = StartingStep; i < StepCount; i++)
	{
		ImVec2 LineStart = ImVec2(CurrentPosition.x + i * ZoomedGridStep, CurrentPosition.y - Settings.Style.Grid.GRID_SIZE * Zoom);
		ImVec2 LineEnd = ImVec2(CurrentPosition.x + i * ZoomedGridStep, CurrentPosition.y + Settings.Style.Grid.GRID_SIZE * Zoom * 4);

		if (i % Settings.Style.Grid.BOLD_LINE_FREQUENCY != 0)
		{
			CurrentDrawList->ChannelsSetCurrent(1);
			CurrentDrawList->AddLine(LineStart, LineEnd, ImGui::GetColorU32(Settings.Style.Grid.GridLinesColor), Settings.Style.Grid.DEFAULT_LINE_WIDTH);
		}
		else
		{
			CurrentDrawList->ChannelsSetCurrent(0);
			CurrentDrawList->AddLine(LineStart, LineEnd, ImGui::GetColorU32(Settings.Style.Grid.GridBoldLinesColor), Settings.Style.Grid.BOLD_LINE_WIDTH);
		}
	}

	CurrentDrawList->ChannelsMerge();
}

void NodeArea::SelectFontSettings() const
{
	if (Zoom < 0.25f)
	{
		ImGui::PushFont(NODE_CORE.Fonts[0]);
		ImGui::SetWindowFontScale(Zoom * 4);
	}
	else if (Zoom >= 0.25f && Zoom < 0.5f)
	{
		ImGui::PushFont(NODE_CORE.Fonts[1]);
		ImGui::SetWindowFontScale(Zoom * 2);
	}
	else if (Zoom >= 1.0f && Zoom < 1.5f)
	{
		ImGui::PushFont(NODE_CORE.Fonts[2]);
		ImGui::SetWindowFontScale(Zoom);
	}
	else if (Zoom >= 1.5f && Zoom < 3.0f)
	{
		ImGui::PushFont(NODE_CORE.Fonts[3]);
		ImGui::SetWindowFontScale(Zoom / 2.0f);
	}
	else if (Zoom >= 3.0f)
	{
		ImGui::PushFont(NODE_CORE.Fonts[4]);
		ImGui::SetWindowFontScale(Zoom / 3.0f);
	}
	else
	{
		ImGui::PushFont(NODE_CORE.Fonts[2]);
		ImGui::SetWindowFontScale(Zoom);
	}
}

void NodeArea::Render()
{
	ImVec2 LocalMousePosition = ScreenToLocal(ImGui::GetMousePos());
	ImGuiStyle OriginalStyle = ImGui::GetStyle();
	ImGuiStyle& ZoomStyle = ImGui::GetStyle();
	ZoomStyle = OriginalStyle;
	ZoomStyle.ScaleAllSizes(Zoom * 2.0f);
	
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, Settings.Style.Grid.GridBackgroundColor);

	const ImVec2 CurrentPosition = GetCurrentWindowImpl()->Pos + Position;
	ImGui::SetNextWindowPos(CurrentPosition);

	if (bFillWindow)
	{
		auto NodeAreaParentWindow = GetCurrentWindowImpl();
		SetSize(NodeAreaParentWindow->Size - ImVec2(2, 2));
	}

	ImGui::BeginChild("Nodes area", GetSize(), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	SetFocused(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

	NodeAreaWindow = GetCurrentWindowImpl();
	CurrentDrawList = ImGui::GetWindowDrawList();

	RenderGrid(CurrentPosition);

	// 0 - connections.
	// 1 - main node rect.
	// 2 - for custom node draw.
	// 3 - for line that represents new connection.
	CurrentDrawList->ChannelsSplit(4);

	// We need to render comments first, because they should be on top layer.
	// Also, the font for comments should be bigger than the font for nodes.
	Zoom *= 2.0f;
	SelectFontSettings();
	Zoom /= 2.0f;
	for (size_t i = 0; i < GroupComments.size(); i++)
		RenderGroupComment(GroupComments[i]);
	ImGui::PopFont();

	// Then we apply usual font settings.
	SelectFontSettings();

	CurrentDrawList->ChannelsSetCurrent(1);
	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		RenderNode(Nodes[i]);
	}

	// Connection should be on node's top layer.
	// But with my current implementation it would be better to call it after RenderNode.
	CurrentDrawList->ChannelsSetCurrent(0);
	for (size_t i = 0; i < Connections.size(); i++)
	{
		RenderConnection(Connections[i]);
	}

	CurrentDrawList->ChannelsMerge();
	CurrentDrawList = nullptr;

	// Draw mouse selection region.
	if (MouseSelectRegionMin.x != FLT_MAX && MouseSelectRegionMin.y != FLT_MAX &&
		MouseSelectRegionMax.x != FLT_MAX && MouseSelectRegionMax.y != FLT_MAX)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(MouseSelectRegionMin, MouseSelectRegionMax, ImGui::GetColorU32(Settings.Style.MouseSelectRegionColor), 1.0f);
	}

	ImGui::PopFont();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	// Reset to the original style before zooming.
	ImGuiStyle& CurrentStyle = ImGui::GetStyle();
	CurrentStyle = OriginalStyle;

	// ************************* RENDER CONTEXT MENU *************************
	if (bOpenMainContextMenu && (MainContextMenuFunction != nullptr || Settings.bShowDefaultMainContextMenu))
	{
		bOpenMainContextMenu = false;
		ImGui::OpenPopup("##main_context_menu");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginPopup("##main_context_menu"))
	{
		if (Settings.bShowDefaultMainContextMenu)
			RenderDefaultMainContextMenu();

		if (MainContextMenuFunction != nullptr)
			MainContextMenuFunction();

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
	// ************************* RENDER CONTEXT MENU END *************************

	if (bShowGroupCommentColorPicker)
	{
		bShowGroupCommentColorPicker = false;
		GroupComment* Comment = ContextMenuOpenState.GetGroupComment();
		if (Comment != nullptr)
		{
			ColorPickerStartValue = Settings.Style.GroupCommentDefaultBackgroundColor;
			if (Comment->BackgroundColor.x != 0.0f ||
				Comment->BackgroundColor.y != 0.0f ||
				Comment->BackgroundColor.z != 0.0f ||
				Comment->BackgroundColor.w != 0.0f)
				ColorPickerStartValue = Comment->BackgroundColor;

			ImGui::OpenPopup("ColorPickerPopup");
		}
	}

	if (ImGui::BeginPopupModal("ColorPickerPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Pick a color:");
		ImGui::ColorPicker4("##picker", (float*)&ColorPickerStartValue, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();

		if (ImGui::Button("Apply"))
		{
			GroupComment* Comment = ContextMenuOpenState.GetGroupComment();
			if (Comment != nullptr)
				Comment->BackgroundColor = ColorPickerStartValue;

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

ImVec2 NodeArea::EvaluateHermiteSpline(float NormalizedParameter, ImVec2 StartPoint, ImVec2 EndPoint, const std::vector<ImVec2>& Tangents) const
{
	const float SquaredParameter = NormalizedParameter * NormalizedParameter;
	const float CubedParameter = SquaredParameter * NormalizedParameter;
	const float BasisStart = 2.0f * CubedParameter - 3.0f * SquaredParameter + 1.0f;
	const float BasisEnd = -2.0f * CubedParameter + 3.0f * SquaredParameter;
	const float BasisTangentStart = CubedParameter - 2.0f * SquaredParameter + NormalizedParameter;
	const float BasisTangentEnd = CubedParameter - SquaredParameter;

	return ImVec2(BasisStart * StartPoint.x + BasisEnd * EndPoint.x + BasisTangentStart * Tangents[0].x + BasisTangentEnd * Tangents[1].x,
				  BasisStart * StartPoint.y + BasisEnd * EndPoint.y + BasisTangentStart * Tangents[0].y + BasisTangentEnd * Tangents[1].y);
}

void NodeArea::DrawHermiteLine(const ImVec2 Begin, const ImVec2 End, const int Steps, const ImVec4 Color, const ConnectionStyle* Style) const
{
	std::vector<ImVec2> LineTangents = GetTangentsForLine(Begin, End);

	if (Style->bMarchingAntsEffect)
	{
		double Time = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
		int Offset = static_cast<int>(Time * 20.0f * Style->MarchingAntsSpeed) % Steps;
		ImVec2 LastPoint = ImVec2(0, 0);

		for (int Step = 0; Step <= Steps; Step++)
		{
			ImVec2 CurrentPoint = EvaluateHermiteSpline(static_cast<float>(Step) / static_cast<float>(Steps), Begin, End, LineTangents);

			if (Step != 0) // Avoid drawing on the first step because LastPoint is not valid.
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

				ImColor ModifiedColor = ImColor(Color.x * Intensity, Color.y * Intensity, Color.z * Intensity, Color.w);
				CurrentDrawList->AddLine(LastPoint, CurrentPoint, ModifiedColor, Thickness * Zoom);
			}

			LastPoint = CurrentPoint;
		}

		CurrentDrawList->PathStroke(ImGui::GetColorU32(Color), false, GetConnectionThickness());
	}
	else if (Style->bPulseEffect)
	{
		for (int Step = 0; Step <= Steps; Step++)
			CurrentDrawList->PathLineTo(EvaluateHermiteSpline(static_cast<float>(Step) / static_cast<float>(Steps), Begin, End, LineTangents));
		
		double Time = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
		float Pulse = static_cast<float>((sin(Time * 5 * Style->PulseSpeed) + 1.0f) / 2.0f);
		Pulse = glm::max(Style->PulseMin, Pulse);
		ImColor PulseColor = ImColor(Color.x, Color.y, Color.z, Pulse);

		CurrentDrawList->PathStroke(PulseColor, false, GetConnectionThickness());
	}
	else
	{
		ImVec2 LastPoint = ImVec2(0, 0);
		for (int Step = 0; Step <= Steps; Step++)
		{
			ImVec2 CurrentPoint = EvaluateHermiteSpline(static_cast<float>(Step) / static_cast<float>(Steps), Begin, End, LineTangents);
			CurrentDrawList->PathLineTo(CurrentPoint);

			LastPoint = CurrentPoint;
		}

		CurrentDrawList->PathStroke(ImGui::GetColorU32(Color), false, GetConnectionThickness());
	}
}

void NodeArea::DrawHermiteLine(const ImVec2 Begin, const ImVec2 End, const int Steps, const ImVec4 Color, const float Thickness) const
{
	std::vector<ImVec2> LineTangents = GetTangentsForLine(Begin, End);

	ImVec2 LastPoint = ImVec2(0, 0);
	for (int Step = 0; Step <= Steps; Step++)
	{
		ImVec2 CurrentPoint = EvaluateHermiteSpline(static_cast<float>(Step) / static_cast<float>(Steps), Begin, End, LineTangents);
		CurrentDrawList->PathLineTo(CurrentPoint);

		LastPoint = CurrentPoint;
	}

	CurrentDrawList->PathStroke(ImGui::GetColorU32(Color), false, Thickness);
}

void NodeArea::RenderConnection(const Connection* Connection) const
{
	if (Connection->Out == nullptr || Connection->In == nullptr)
		return;

	ImColor CurrentConnectionColor = Connection->Style.ForceColor;

	std::vector<std::string> ConnectOutSocketAllowedTypes = Connection->Out->GetAllowedTypes();
	// FE_TO_DO: Add support for multiple socket types.
	if (ConnectOutSocketAllowedTypes.size() == 1)
	{
		if (NodeSocket::SocketTypeToColorAssociations.find(ConnectOutSocketAllowedTypes[0]) != NodeSocket::SocketTypeToColorAssociations.end())
			CurrentConnectionColor = NodeSocket::SocketTypeToColorAssociations[ConnectOutSocketAllowedTypes[0]];
	}

	std::vector<std::string> ConnectInSocketAllowedTypes = Connection->In->GetAllowedTypes();
	// FE_TO_DO: Add support for multiple socket types.
	if (ConnectInSocketAllowedTypes.size() == 1)
	{
		if (NodeSocket::SocketTypeToColorAssociations.find(ConnectInSocketAllowedTypes[0]) != NodeSocket::SocketTypeToColorAssociations.end())
			CurrentConnectionColor = NodeSocket::SocketTypeToColorAssociations[ConnectInSocketAllowedTypes[0]];
	}

	std::vector<ConnectionSegment> Segments = GetConnectionSegments(Connection);
	for (size_t i = 0; i < Segments.size(); i++)
	{
		ImVec2 BeginPosition = Segments[i].Begin;
		ImVec2 EndPosition = Segments[i].End;

		if (Connection->bSelected)
		{
			DrawHermiteLine(BeginPosition, EndPosition, Settings.Style.GeneralConnection.LineSegments, Settings.Style.GeneralConnection.SelectionOutlineColor, GetConnectionThickness() + GetConnectionThickness() * 1.2f);
		}
		else if (Connection->bHovered)
		{
			DrawHermiteLine(BeginPosition, EndPosition, Settings.Style.GeneralConnection.LineSegments, Settings.Style.GeneralConnection.HoveredOutlineColor, GetConnectionThickness() + GetConnectionThickness() * 1.2f);
		}

		DrawHermiteLine(BeginPosition, EndPosition, Settings.Style.GeneralConnection.LineSegments, CurrentConnectionColor, &Connection->Style);

		// If it is a reroute node, then we should render a circle.
		if (i > 0)
			RenderReroute(Connection->RerouteNodes[i - 1]);
	}
}

void NodeArea::RenderReroute(const RerouteNode* RerouteNode) const
{
	if (RerouteNode->bSelected)
	{
		CurrentDrawList->AddCircleFilled(LocalToScreen(RerouteNode->Position), GetRerouteNodeSize() * 1.2f, ImGui::GetColorU32(Settings.Style.GeneralConnection.SelectionOutlineColor));
	}
	else if (RerouteNode->bHovered)
	{
		CurrentDrawList->AddCircleFilled(LocalToScreen(RerouteNode->Position), GetRerouteNodeSize() * 1.2f, ImGui::GetColorU32(Settings.Style.GeneralConnection.HoveredOutlineColor));
	}

	CurrentDrawList->AddCircleFilled(LocalToScreen(RerouteNode->Position), GetRerouteNodeSize(), ImColor(DEFAULT_NODE_SOCKET_COLOR.Value + ImColor(15, 25, 15).Value));
}

ImVec2 NodeArea::SocketToPosition(Node* Node, const std::string& SocketID) const
{
	return SocketToPosition(Node->GetSocketByID(SocketID));
}

ImVec2 NodeArea::SocketToPosition(const NodeSocket* Socket) const
{
	const bool bIsInput = !Socket->bOutput;
	float SocketX = 0.0f;
	float SocketY = 0.0f;
	Node* SocketParent = Socket->GetParent();

	int SocketIndex = -1;
	if (bIsInput)
	{
		for (size_t i = 0; i < SocketParent->Input.size(); i++)
		{
			if (SocketParent->Input[i] == Socket)
			{
				SocketIndex = static_cast<int>(i);
				break;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < SocketParent->Output.size(); i++)
		{
			if (SocketParent->Output[i] == Socket)
			{
				SocketIndex = static_cast<int>(i);
				break;
			}
		}
	}

	if (SocketParent->GetStyle() == DEFAULT)
	{
		SocketX = bIsInput ? SocketParent->LeftTop.x + GetNodeSocketSize() * 3 : SocketParent->RightBottom.x - GetNodeSocketSize() * 3;

		const float HeightForSockets = SocketParent->GetSize().y * Zoom - (SocketParent->GetRenderTitleBar() ? GetNodeTitleHeight() : 0.0f);
		const float SocketSpacing = HeightForSockets / (bIsInput ? SocketParent->Input.size() : SocketParent->Output.size());

		SocketY = (SocketParent->LeftTop.y + (SocketParent->GetRenderTitleBar() ? GetNodeTitleHeight() : 0.0f) + SocketSpacing * (SocketIndex + 1) - SocketSpacing / 2.0f);
	}
	else if (SocketParent->GetStyle() == CIRCLE)
	{
		const size_t SocketCount = bIsInput ? SocketParent->Input.size() : SocketParent->Output.size();
		float BeginAngle = (180.0f / static_cast<float>(SocketCount) / 2.0f);
		if (bIsInput)
			BeginAngle = -BeginAngle;

		float RotationOffset = (180.0f / static_cast<float>(SocketCount) * (SocketIndex));
		if (bIsInput)
			RotationOffset = -RotationOffset;

		const float SocketAngle = BeginAngle + RotationOffset;

		const float NodeCenterX = SocketParent->LeftTop.x + NODE_DIAMETER * Zoom / 2.0f;
		const float NodeCenterY = SocketParent->LeftTop.y + NODE_DIAMETER * Zoom / 2.0f;

		SocketX = NodeCenterX + NODE_DIAMETER * Zoom * 0.95f * sin(glm::radians(SocketAngle));
		SocketY = NodeCenterY + NODE_DIAMETER * Zoom * 0.95f * cos(glm::radians(SocketAngle));
	}

	return {SocketX, SocketY};
}

ImVec2 NodeArea::GetRenderOffset() const
{
	return RenderOffset;
}

void NodeArea::SetRenderOffset(const ImVec2 Offset)
{
	if (Offset.x <= -Settings.Style.Grid.GRID_SIZE || Offset.x >= Settings.Style.Grid.GRID_SIZE ||
		Offset.y <= -Settings.Style.Grid.GRID_SIZE || Offset.y >= Settings.Style.Grid.GRID_SIZE)
		return;

	RenderOffset = Offset;
}

void NodeArea::GetAllElementsAABB(ImVec2& Min, ImVec2& Max) const
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

	for (size_t i = 0; i < GroupComments.size(); i++)
	{
		if (GroupComments[i]->GetPosition().x + RenderOffset.x < Min.x)
			Min.x = GroupComments[i]->GetPosition().x + RenderOffset.x;

		if (GroupComments[i]->GetPosition().x + RenderOffset.x + GroupComments[i]->GetSize().x > Max.x)
			Max.x = GroupComments[i]->GetPosition().x + RenderOffset.x + GroupComments[i]->GetSize().x;

		if (GroupComments[i]->GetPosition().y + RenderOffset.y < Min.y)
			Min.y = GroupComments[i]->GetPosition().y + RenderOffset.y;

		if (GroupComments[i]->GetPosition().y + RenderOffset.y + GroupComments[i]->GetSize().y > Max.y)
			Max.y = GroupComments[i]->GetPosition().y + RenderOffset.y + GroupComments[i]->GetSize().y;
	}
}

ImVec2 NodeArea::GetAllElementsAABBCenter() const
{
	ImVec2 Min, Max;
	GetAllElementsAABB(Min, Max);

	return {Min.x + (Max.x - Min.x) / 2.0f, Min.y + (Max.y - Min.y) / 2.0f};
}

ImVec2 NodeArea::GetRenderedViewCenter() const
{
	if (NodeAreaWindow != nullptr)
	{
		return (NodeAreaWindow->Size / 2.0f - RenderOffset) / Zoom;
	}
	else
	{
		return (GetCurrentWindowImpl()->Size / 2.0f - RenderOffset) / Zoom;
	}
}

bool NodeArea::IsFillingWindow()
{
	return bFillWindow;
}

void NodeArea::SetIsFillingWindow(bool NewValue)
{
	bFillWindow = NewValue;
}

void NodeArea::ApplyZoom(float Delta)
{
	ImVec2 MousePosBeforeZoom = ScreenToLocal(ImGui::GetMousePos());

	Zoom += Delta * Settings.ZoomSpeed;
	Zoom = std::max(Zoom, MIN_ZOOM_LEVEL);
	Zoom = std::min(Zoom, MAX_ZOOM_LEVEL);

	ImVec2 MousePosAfterZoom = ScreenToLocal(ImGui::GetMousePos());

	// Adjust render offset to keep the mouse over the same point after zooming
	RenderOffset -= (MousePosBeforeZoom - MousePosAfterZoom) * Zoom;

	Settings.Style.GeneralConnection.LineSegments = static_cast<int>(16 * Zoom);
}

float NodeArea::GetZoomFactor() const
{
	return Zoom;
}

void NodeArea::SetZoomFactor(float NewValue)
{
	if (NewValue < MIN_ZOOM_LEVEL || NewValue > MAX_ZOOM_LEVEL)
		return;

	Zoom = NewValue;
}

bool NodeArea::GetConnectionStyle(Node* Node, bool bOutputSocket, size_t SocketIndex, ConnectionStyle& Style) const
{
	if (Node == nullptr || SocketIndex < 0)
		return false;

	if (bOutputSocket)
	{
		if (SocketIndex >= Node->Output.size())
			return false;

		ConnectionStyle* TempVariable = GetConnectionStyle(Node->Output[SocketIndex]);
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

		ConnectionStyle* TempVariable = GetConnectionStyle(Node->Input[SocketIndex]);
		if (TempVariable != nullptr)
		{
			Style = *TempVariable;
			return true;
		}

		return false;
	}
}

void NodeArea::SetConnectionStyle(Node* Node, bool bOutputSocket, size_t SocketIndex, ConnectionStyle NewStyle)
{
	if (Node == nullptr || SocketIndex < 0)
		return;

	if (bOutputSocket)
	{
		if (SocketIndex >= Node->Output.size())
			return;

		ConnectionStyle* TempVariable = GetConnectionStyle(Node->Output[SocketIndex]);
		if (TempVariable != nullptr)
			*TempVariable = NewStyle;
	}
	else
	{
		if (SocketIndex >= Node->Input.size())
			return;

		ConnectionStyle* TempVariable = GetConnectionStyle(Node->Input[SocketIndex]);
		if (TempVariable != nullptr)
			*TempVariable = NewStyle;
	}
}

void NodeArea::RenderGroupComment(GroupComment* GroupComment)
{
	if (CurrentDrawList == nullptr || GroupComment == nullptr)
		return;

	ImGui::PushID(GroupComment->ID.c_str());

	ImVec2 LocalPosition = LocalToScreen(GroupComment->GetPosition());
	ImVec2 CommentSize = GroupComment->GetSize() * Zoom;

	// *****************************************************************************************************************************
	// Render the background.
	// *****************************************************************************************************************************

	ImVec4 BackgroundColor = Settings.Style.GroupCommentDefaultBackgroundColor;
	if (GroupComment->BackgroundColor.x != 0.0f ||
		GroupComment->BackgroundColor.y != 0.0f ||
		GroupComment->BackgroundColor.z != 0.0f ||
		GroupComment->BackgroundColor.w != 0.0f)
		BackgroundColor = GroupComment->BackgroundColor;

	CurrentDrawList->AddRectFilled(LocalPosition, LocalPosition + CommentSize, ImGui::GetColorU32(BackgroundColor), 2.0f * Zoom);

	// Compute the size of the caption background.
	ImVec2 CaptionSize = GroupComment->GetCaptionSize(Zoom);
	ImVec2 CaptionPosition = LocalPosition + ImVec2(4.0f, 4.0f) * Zoom;

	if (IsSelected(GroupComment))
	{
		const ImVec2 LeftTop = LocalPosition - ImVec2(4.0f, 4.0f);
		const ImVec2 RightBottom = LocalPosition + CommentSize + ImVec2(4.0f, 4.0f);
		ImGui::GetWindowDrawList()->AddRect(LeftTop, RightBottom, ImGui::GetColorU32(Settings.Style.NodeSelectionColor), 8.0f * Zoom);
	}

	// *****************************************************************************************************************************
	// Render the caption.
	// *****************************************************************************************************************************

	const ImU32 CaptionBackgroundColor = ImGui::GetColorU32(ImVec4(BackgroundColor.x, BackgroundColor.y, BackgroundColor.z, 1.0f));
	const ImU32 CaptionTextColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Render the caption background.
	CurrentDrawList->AddRectFilled(CaptionPosition, CaptionPosition + CaptionSize, CaptionBackgroundColor, 2.0f * Zoom);

	if (!GroupComment->bLastFrameRenameEditWasVisible)
	{
		ImGui::SetKeyboardFocusHere(0);
		ImGui::SetFocusID(ImGui::GetID("##newCommentEditor"), GetCurrentWindowImpl());
		ImGui::SetItemDefaultFocus();
		GroupComment->bLastFrameRenameEditWasVisible = true;

		snprintf(VisNodeSys::GroupComment::GroupCommentRename,GroupComment->GetCaption().size() + 1, "%s", GroupComment->GetCaption().c_str());
	}

	ImVec2 TextOffset = ImVec2(4.0f, 10.0f) * Zoom;
	if (GroupComment->bIsRenamingActive)
	{
		ImGui::SetCursorScreenPos(CaptionPosition + TextOffset);
		ImGui::SetNextItemWidth(GroupComment->GetSize().x * Zoom - TextOffset.x * 4.0f);
		if (ImGui::InputText("##newCommentEditor", GroupComment->GroupCommentRename, IM_ARRAYSIZE(GroupComment->GroupCommentRename), ImGuiInputTextFlags_EnterReturnsTrue) ||
			ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered() || !ImGui::IsItemFocused())
		{
			GroupComment->bIsRenamingActive = false;
			GroupComment->SetCaption(GroupComment->GroupCommentRename);
		}
	}
	else
	{
		std::string CaptionToRender = GroupComment->GetCaption();
		CaptionToRender = NODE_CORE.TruncateText(CaptionToRender, CaptionSize.x - 4.0f * Zoom + TextOffset.x);
		CurrentDrawList->AddText(CaptionPosition + TextOffset, CaptionTextColor, CaptionToRender.c_str());
	}
	
	// *****************************************************************************************************************************
	// Render resize indicator.
	// *****************************************************************************************************************************

	const ImU32 ResizeIndicatorColor = ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
	const float LineLength = 7.0f * Zoom;
	const float LineSpacing = 5.0f * Zoom;

	ImVec2 EndOfComment = LocalPosition + CommentSize - ImVec2(2.0f, 2.0f) * Zoom;

	for (int i = 0; i < 3; i++)
	{
		ImVec2 LineStart = ImVec2(EndOfComment.x - LineLength - i * LineSpacing, EndOfComment.y);
		ImVec2 LineEnd = ImVec2(EndOfComment.x, EndOfComment.y - LineLength - i * LineSpacing);

		CurrentDrawList->AddLine(LineStart, LineEnd, ResizeIndicatorColor, 2.0f * Zoom);
	}

	// *****************************************************************************************************************************
	// Render shadow.
	// *****************************************************************************************************************************

	//EndOfComment += ImVec2(2.0f, 2.0f) * Zoom;

	//const ImU32 ShadowColor = IM_COL32(0, 0, 0, 20);
	//const float ShadowSize = 4.0f * Zoom;

	//// Right shadow rectangle.
	//ImVec2 RightShadowTopLeft = ImVec2(EndOfComment.x, LocalPosition.y);
	//ImVec2 RightShadowBottomRight = ImVec2(EndOfComment.x + ShadowSize, EndOfComment.y + ShadowSize);
	//CurrentDrawList->AddRectFilled(RightShadowTopLeft, RightShadowBottomRight, ShadowColor);

	//// Bottom shadow rectangle (without overlapping the right shadow).
	//ImVec2 BottomShadowTopLeft = ImVec2(LocalPosition.x, EndOfComment.y);
	//ImVec2 BottomShadowBottomRight = ImVec2(EndOfComment.x, EndOfComment.y + ShadowSize);
	//CurrentDrawList->AddRectFilled(BottomShadowTopLeft, BottomShadowBottomRight, ShadowColor);

	ImGui::PopID();
}

void NodeArea::RenderDefaultMainContextMenu()
{
	if (ContextMenuOpenState.GetGroupComment() != nullptr)
	{
		GroupComment* CurrentGroupComment = ContextMenuOpenState.GetGroupComment();
		if (ImGui::MenuItem("Change background color..."))
		{
			bShowGroupCommentColorPicker = true;
		}

		bool bMoveElementsWithComment = CurrentGroupComment->MoveElementsWithComment();
		if (ImGui::MenuItem("Group movement", NULL, &bMoveElementsWithComment))
		{
			CurrentGroupComment->SetMoveElementsWithComment(bMoveElementsWithComment);
		}

		if (ImGui::MenuItem("Bring Forward"))
		{
			for (size_t i = 0; i < GroupComments.size(); i++)
			{
				if (GroupComments[i] == CurrentGroupComment)
				{
					if (i < GroupComments.size() - 1)
					{
						std::swap(GroupComments[i], GroupComments[i + 1]);
					}
					break;
				}
			}
		}

		if (ImGui::MenuItem("Send Backward"))
		{
			for (size_t i = 0; i < GroupComments.size(); i++)
			{
				if (GroupComments[i] == CurrentGroupComment)
				{
					if (i > 0)
					{
						std::swap(GroupComments[i], GroupComments[i - 1]);
					}
					break;
				}
			}
		}
	}
	else if (ContextMenuOpenState.GetNode() != nullptr && GetSelected().size() == 1)
	{
		if (ImGui::MenuItem("Delete Node"))
		{
			Delete(ContextMenuOpenState.GetNode());
		}
	}
	else if (GetSelected().size() > 1)
	{
		auto SelectedList = GetSelected();
		if (ImGui::MenuItem("Delete Selected Nodes"))
		{
			for (size_t i = 0; i < SelectedList.size(); i++)
				Delete(SelectedList[i]);
			
			UnSelectAll();
		}
	}
	else if (ContextMenuOpenState.GetGroupComment() == nullptr && ContextMenuOpenState.GetNode() == nullptr && GetSelected().empty())
	{
		if (ImGui::MenuItem("Add Group Comment"))
		{
			GroupComment* NewGroupComment = new GroupComment();
			NewGroupComment->SetCaption("Group Comment");
			NewGroupComment->SetPosition(ContextMenuOpenState.MousePositionRecorded);
			NewGroupComment->bIsRenamingActive = true;
			GroupComments.push_back(NewGroupComment);
		}

#ifdef VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES
		if (ImGui::BeginMenu("Add Standard Node"))
		{
			VisNodeSys::Node* NewNode = nullptr;

			if (ImGui::BeginMenu("Literals"))
			{
				if (ImGui::MenuItem("Bool"))
				{
					NewNode = new BoolLiteralNode();
				}

				if (ImGui::MenuItem("Int"))
				{
					NewNode = new IntegerLiteralNode();
				}

				if (ImGui::MenuItem("Float"))
				{
					NewNode = new FloatLiteralNode();
				}

				if (ImGui::MenuItem("Vec2"))
				{
					NewNode = new Vec2LiteralNode();
				}

				if (ImGui::MenuItem("Bool Vec2"))
				{
					NewNode = new BoolVec2LiteralNode();
				}

				if (ImGui::MenuItem("Vec3"))
				{
					NewNode = new Vec3LiteralNode();
				}

				if (ImGui::MenuItem("Bool Vec3"))
				{
					NewNode = new BoolVec3LiteralNode();
				}

				if (ImGui::MenuItem("Vec4"))
				{
					NewNode = new Vec4LiteralNode();
				}

				if (ImGui::MenuItem("Bool Vec4"))
				{
					NewNode = new BoolVec4LiteralNode();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Variables"))
			{
				if (ImGui::MenuItem("Bool"))
				{
					NewNode = new BoolVariableNode();
				}

				if (ImGui::MenuItem("Int"))
				{
					NewNode = new IntegerVariableNode();
				}

				if (ImGui::MenuItem("Float"))
				{
					NewNode = new FloatVariableNode();
				}

				if (ImGui::MenuItem("Vec2"))
				{
					NewNode = new Vec2VariableNode();
				}

				if (ImGui::MenuItem("Bool Vec2"))
				{
					NewNode = new BoolVec2VariableNode();
				}

				if (ImGui::MenuItem("Vec3"))
				{
					NewNode = new Vec3VariableNode();
				}

				if (ImGui::MenuItem("Bool Vec3"))
				{
					NewNode = new BoolVec3VariableNode();
				}

				if (ImGui::MenuItem("Vec4"))
				{
					NewNode = new Vec4VariableNode();
				}

				if (ImGui::MenuItem("Bool Vec4"))
				{
					NewNode = new BoolVec4VariableNode();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Flow Control"))
			{
				if (ImGui::MenuItem("Branch"))
				{
					NewNode = new BranchNode();
				}

				if (ImGui::MenuItem("Sequence"))
				{
					NewNode = new SequenceNode();
				}

				if (ImGui::MenuItem("For Loop"))
				{
					NewNode = new LoopNode();
				}

				if (ImGui::MenuItem("While Loop"))
				{
					NewNode = new WhileLoopNode();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Operations"))
			{
				if (ImGui::BeginMenu("Arithmetic"))
				{
					if (ImGui::MenuItem("Add"))
					{
						NewNode = new ArithmeticAddNode();
					}

					if (ImGui::MenuItem("Subtract"))
					{
						NewNode = new ArithmeticSubtractNode();
					}

					if (ImGui::MenuItem("Multiply"))
					{
						NewNode = new ArithmeticMultiplyNode();
					}

					if (ImGui::MenuItem("Divide"))
					{
						NewNode = new ArithmeticDivideNode();
					}

					if (ImGui::MenuItem("Modulus"))
					{
						NewNode = new ArithmeticModulusNode();
					}

					if (ImGui::MenuItem("Power"))
					{
						NewNode = new ArithmeticPowerNode();
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Comparison"))
				{
					if (ImGui::MenuItem("Equal"))
						NewNode = new EqualNode();

					if (ImGui::MenuItem("NotEqual"))
						NewNode = new NotEqualNode();

					if (ImGui::MenuItem("Greater Than or Equal"))
						NewNode = new GreaterThanOrEqualNode();

					if (ImGui::MenuItem("Less Than or Equal"))
						NewNode = new LessThanOrEqualNode();

					if (ImGui::MenuItem("Greater Than"))
						NewNode = new GreaterThanNode();

					if (ImGui::MenuItem("Less Than"))
						NewNode = new LessThanNode();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Logical"))
				{
					if (ImGui::MenuItem("AND"))
						NewNode = new LogicalANDOperatorNode();

					if (ImGui::MenuItem("OR"))
						NewNode = new LogicalOROperatorNode();

					if (ImGui::MenuItem("NOT"))
						NewNode = new LogicalNOTOperatorNode();

					if (ImGui::MenuItem("XOR"))
						NewNode = new LogicalXOROperatorNode();

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (NewNode != nullptr)
			{
				NewNode->SetPosition(ContextMenuOpenState.MousePositionRecorded);
				AddNode(NewNode);
			}

			ImGui::EndMenu();
		}
#endif
	}
}