#include "VisualNodeArea.h"

ImVec2 VisualNodeArea::NeededShift = ImVec2();

VisualNodeArea::VisualNodeArea()
{
	SetAreaSize(ImVec2(256, 256));
};

void VisualNodeArea::AddNode(VisualNode* NewNode)
{
	if (NewNode == nullptr)
		return;

	NewNode->ParentArea = this;
	Nodes.push_back(NewNode);
}

VisualNodeArea::~VisualNodeArea() 
{
	Clear();
}

bool ColorPicker3U32(const char* Label, ImU32* Color)
{
	float col[3];
	col[0] = static_cast<float>((*Color) & 0xFF) / 255.0f;
	col[1] = static_cast<float>((*Color >> 8) & 0xFF) / 255.0f;
	col[2] = static_cast<float>((*Color >> 16) & 0xFF) / 255.0f;

	const bool result = ImGui::ColorPicker3(Label, col);

	*Color = static_cast<ImU32>(col[0] * 255.0f) |
		(static_cast<ImU32>(col[1] * 255.0f) << 8) |
		(static_cast<ImU32>(col[2] * 255.0f) << 16);

	return result;
}

void VisualNodeArea::RenderNode(VisualNode* Node) const
{
	if (CurrentDrawList == nullptr || Node == nullptr)
		return;

	ImGui::PushID(Node->GetID().c_str());

	Node->LeftTop = ImGui::GetCurrentWindow()->Pos + Node->GetPosition() * Zoom + RenderOffset;
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
	const ImU32 NodeBackgroundColor = (Hovered == Node || IsSelected(Node)) ? IM_COL32(75, 75, 75, 125) : IM_COL32(60, 60, 60, 125);
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
		const ImU32 NodeTitleBackgroundColor = (Hovered == Node || IsSelected(Node)) ? Node->TitleBackgroundColorHovered : Node->TitleBackgroundColor;

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

	ImColor SocketColor = ImColor(150, 150, 150);
	if (SocketHovered == Socket)
	{
		SocketColor = ImColor(200, 200, 200);
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
		ImColor ConnectionColor = ImColor(200, 200, 200);
		if (NodeSocket::SocketTypeToColorAssosiations.find(SocketLookingForConnection->GetType()) != NodeSocket::SocketTypeToColorAssosiations.end())
			ConnectionColor = NodeSocket::SocketTypeToColorAssosiations[SocketLookingForConnection->GetType()];

		CurrentDrawList->ChannelsSetCurrent(3);
		DrawHermiteLine(SocketPosition / Zoom, ImGui::GetIO().MousePos / Zoom, 12, ConnectionColor, &Socket->ConnectionStyle);
	}

	// Draw socket icon.
	CurrentDrawList->AddCircleFilled(SocketPosition, GetNodeSocketSize(), SocketColor);
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

ImVec2 VisualNodeArea::GetAreaSize() const
{
	return AreaSize;
}

void VisualNodeArea::SetAreaSize(const ImVec2 NewValue)
{
	if (NewValue.x < 1 || NewValue.y < 1)
		return;

	AreaSize = NewValue;
}

ImVec2 VisualNodeArea::GetAreaPosition() const
{
	return AreaPosition;
}

void VisualNodeArea::SetAreaPosition(const ImVec2 NewValue)
{
	if (NewValue.x < 0 || NewValue.y < 0)
		return;

	AreaPosition = NewValue;
}

void VisualNodeArea::DrawHermiteLine(const ImVec2 P1, const ImVec2 P2, const int Steps, const ImColor Color, VisualNodeConnectionStyle* Style) const
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

			ImVec2 CurrentPoint = ImVec2(Zoom, Zoom) * ImVec2(h1 * P1.x + h2 * P2.x + h3 * t1.x + h4 * t2.x, h1 * P1.y + h2 * P2.y + h3 * t1.y + h4 * t2.y);

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

			CurrentDrawList->PathLineTo(ImVec2(Zoom, Zoom) * ImVec2(h1 * P1.x + h2 * P2.x + h3 * t1.x + h4 * t2.x, h1 * P1.y + h2 * P2.y + h3 * t1.y + h4 * t2.y));
		}

		double Time = glfwGetTime();
		float Pulse = (sin(Time * 5 * Style->PulseSpeed) + 1.0f) / 2.0f;
		Pulse = glm::max(Style->PulseMin, Pulse);
		ImColor PulseColor = ImColor(Color.Value.x, Color.Value.y, Color.Value.z, Pulse);

		CurrentDrawList->PathStroke(PulseColor, false, GetConnectionThickness());
	}
	else
	{
		for (int Step = 0; Step <= Steps; Step++)
		{
			const float t = static_cast<float>(Step) / static_cast<float>(Steps);
			const float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
			const float h2 = -2 * t * t * t + 3 * t * t;
			const float h3 = t * t * t - 2 * t * t + t;
			const float h4 = t * t * t - t * t;

			CurrentDrawList->PathLineTo(ImVec2(Zoom, Zoom) * ImVec2(h1 * P1.x + h2 * P2.x + h3 * t1.x + h4 * t2.x, h1 * P1.y + h2 * P2.y + h3 * t1.y + h4 * t2.y));
		}

		CurrentDrawList->PathStroke(Color, false, GetConnectionThickness());
	}
}

void VisualNodeArea::RenderConnection(const VisualNodeConnection* Connection) const
{
	if (Connection->Out == nullptr || Connection->In == nullptr)
		return;

	ImColor ConnectionColor = ImColor(200, 200, 200);
	if (NodeSocket::SocketTypeToColorAssosiations.find(Connection->Out->GetType()) != NodeSocket::SocketTypeToColorAssosiations.end())
		ConnectionColor = NodeSocket::SocketTypeToColorAssosiations[Connection->Out->GetType()];

	if (Connection->Out->ConnectionStyle.ForceColor != nullptr)
		ConnectionColor = *Connection->Out->ConnectionStyle.ForceColor;

	DrawHermiteLine(SocketToPosition(Connection->Out) / Zoom, SocketToPosition(Connection->In) / Zoom, 12, ConnectionColor, &Connection->Out->ConnectionStyle);
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

void VisualNodeArea::Update()
{
	InputUpdate();

	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		if (Nodes[i]->bShouldBeDestroyed)
		{
			DeleteNode(Nodes[i]);
			i--;
			break;
		}
	}

	ProcessSocketEventQueue();
	Render();
}

std::vector<VisualNodeConnection*> VisualNodeArea::GetAllConnections(const NodeSocket* Socket) const
{
	std::vector<VisualNodeConnection*> result;

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i]->In == Socket || Connections[i]->Out == Socket)
			result.push_back(Connections[i]);
	}

	return result;
}

void VisualNodeArea::Disconnect(VisualNodeConnection*& Connection)
{
	for (int i = 0; i < static_cast<int>(Connection->In->Connections.size()); i++)
	{
		if (Connection->In->Connections[i] == Connection->Out)
		{
			VisualNode* parent = Connection->In->Connections[i]->Parent;
			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, VISUAL_NODE_BEFORE_DISCONNECTED);

			Connection->In->Connections.erase(Connection->In->Connections.begin() + i, Connection->In->Connections.begin() + i + 1);
			Connection->In->Parent->SocketEvent(Connection->In, Connection->Out, bClearing ? VISUAL_NODE_SOCKET_DESTRUCTION : VISUAL_NODE_SOCKET_DISCONNECTED);
			i--;

			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, VISUAL_NODE_AFTER_DISCONNECTED);
		}
	}

	for (int i = 0; i < static_cast<int>(Connection->Out->Connections.size()); i++)
	{
		if (Connection->Out->Connections[i] == Connection->In)
		{
			VisualNode* parent = Connection->Out->Connections[i]->Parent;
			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, VISUAL_NODE_BEFORE_DISCONNECTED);

			Connection->Out->Connections.erase(Connection->Out->Connections.begin() + i, Connection->Out->Connections.begin() + i + 1);
			i--;

			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, VISUAL_NODE_AFTER_DISCONNECTED);
		}
	}

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i] == Connection)
		{
			Connections.erase(Connections.begin() + i, Connections.begin() + i + 1);
			delete Connection;
			Connection = nullptr;
			return;
		}
	}
}

void VisualNodeArea::DeleteNode(const VisualNode* Node)
{
	if (!Node->bCouldBeDestroyed)
		return;

	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i] == Node)
		{
			PropagateNodeEventsCallbacks(Nodes[i], VISUAL_NODE_REMOVED);

			for (size_t j = 0; j < Nodes[i]->Input.size(); j++)
			{
				auto connections = GetAllConnections(Nodes[i]->Input[j]);
				for (size_t p = 0; p < connections.size(); p++)
				{
					Disconnect(connections[p]);
				}
			}

			for (size_t j = 0; j < Nodes[i]->Output.size(); j++)
			{
				auto connections = GetAllConnections(Nodes[i]->Output[j]);
				for (size_t p = 0; p < connections.size(); p++)
				{
					Disconnect(connections[p]);
				}
			}

			delete Nodes[i];
			Nodes.erase(Nodes.begin() + i, Nodes.begin() + i + 1);

			break;
		}
	}
}

void VisualNodeArea::SetMainContextMenuFunc(void(*Func)())
{
	MainContextMenuFunc = Func;
}

void VisualNodeArea::Clear()
{
	bClearing = true;

	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		PropagateNodeEventsCallbacks(Nodes[i], VISUAL_NODE_DESTROYED);
		Nodes[i]->bCouldBeDestroyed = true;
		DeleteNode(Nodes[i]);
		i--;
	}

	RenderOffset = ImVec2(0, 0);
	NodeAreaWindow = nullptr;
	Selected.clear();
	SocketLookingForConnection = nullptr;
	SocketHovered = nullptr;

	MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);

	bClearing = false;
}

void VisualNodeArea::Reset()
{
	Clear();

	MainContextMenuFunc = nullptr;
	NodeEventsCallbacks.clear();
}

void VisualNodeArea::PropagateUpdateToConnectedNodes(const VisualNode* CallerNode) const
{
	if (CallerNode == nullptr)
		return;

	for (size_t i = 0; i < CallerNode->Input.size(); i++)
	{
		auto connections = GetAllConnections(CallerNode->Input[i]);
		for (size_t j = 0; j < connections.size(); j++)
		{
			connections[j]->In->GetParent()->SocketEvent(connections[j]->In, connections[j]->Out, VISUAL_NODE_SOCKET_UPDATE);
		}
	}

	for (size_t i = 0; i < CallerNode->Output.size(); i++)
	{
		auto connections = GetAllConnections(CallerNode->Output[i]);
		for (size_t j = 0; j < connections.size(); j++)
		{
			connections[j]->In->GetParent()->SocketEvent(connections[j]->In, connections[j]->Out, VISUAL_NODE_SOCKET_UPDATE);
		}
	}
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

bool VisualNodeArea::TryToConnect(const VisualNode* OutNode, const size_t OutNodeSocketIndex, const VisualNode* InNode, const size_t InNodeSocketIndex)
{
	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return false;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return false;

	NodeSocket* OutSocket = OutNode->Output[OutNodeSocketIndex];
	NodeSocket* InSocket = InNode->Input[InNodeSocketIndex];

	char* msg = nullptr;
	const bool result = InSocket->GetParent()->CanConnect(InSocket, OutSocket, &msg);

	if (result)
	{
		PropagateNodeEventsCallbacks(OutSocket->GetParent(), VISUAL_NODE_BEFORE_CONNECTED);
		PropagateNodeEventsCallbacks(InSocket->GetParent(), VISUAL_NODE_BEFORE_CONNECTED);

		OutSocket->Connections.push_back(InSocket);
		InSocket->Connections.push_back(OutSocket);

		Connections.push_back(new VisualNodeConnection(OutSocket, InSocket));

		OutSocket->GetParent()->SocketEvent(OutSocket, InSocket, VISUAL_NODE_SOCKET_CONNECTED);
		InSocket->GetParent()->SocketEvent(InSocket, OutSocket, VISUAL_NODE_SOCKET_CONNECTED);

		PropagateNodeEventsCallbacks(OutSocket->GetParent(), VISUAL_NODE_AFTER_CONNECTED);
		PropagateNodeEventsCallbacks(InSocket->GetParent(), VISUAL_NODE_AFTER_CONNECTED);
	}

	return result;
}

void VisualNodeArea::SetNodeEventCallback(void(*Func)(VisualNode*, VISUAL_NODE_EVENT))
{
	if (Func != nullptr)
		NodeEventsCallbacks.push_back(Func);
}

void VisualNodeArea::PropagateNodeEventsCallbacks(VisualNode* Node, const VISUAL_NODE_EVENT EventToPropagate) const
{
	for (size_t i = 0; i < NodeEventsCallbacks.size(); i++)
	{
		if (NodeEventsCallbacks[i] != nullptr)
			NodeEventsCallbacks[i](Node, EventToPropagate);
	}
}

void VisualNodeArea::SaveToFile(const char* FileName) const
{
	const std::string json_file = ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << json_file;
	SaveFile.close();
}

bool VisualNodeArea::IsNodeIDInList(const std::string ID, const std::vector<VisualNode*> List)
{
	for (size_t i = 0; i < List.size(); i++)
	{
		if (List[i]->GetID() == ID)
			return true;
	}

	return false;
}

void VisualNodeArea::SaveNodesToFile(const char* FileName, std::vector<VisualNode*> Nodes)
{
	if (Nodes.empty())
		return;

	const VisualNodeArea* NewNodeArea = VisualNodeArea::CreateNodeArea(Nodes);
	const std::string json_file = NewNodeArea->ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << json_file;
	SaveFile.close();
	delete NewNodeArea;
}

void VisualNodeArea::RunOnEachNode(void(*Func)(VisualNode*))
{
	if (Func != nullptr)
		std::for_each(Nodes.begin(), Nodes.end(), Func);
}

VisualNode* VisualNodeArea::GetHovered() const
{
	return Hovered;
}

std::vector<VisualNode*> VisualNodeArea::GetSelected()
{
	return Selected;
}

void VisualNodeArea::InputUpdate()
{
	MouseCursorPosition = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	Hovered = nullptr;
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
		if (Hovered != nullptr)
			break;
	}

	if (ImGui::IsMouseDragging(0) && MouseDownIn == NodeAreaWindow)
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

		if (MouseSelectRegionMin.x != FLT_MAX && MouseSelectRegionMin.y != FLT_MAX &&
			MouseSelectRegionMax.x != FLT_MAX && MouseSelectRegionMax.y != FLT_MAX)
		{
			Selected.clear();
			const ImVec2 RegionSize = MouseSelectRegionMax - MouseSelectRegionMin;

			for (size_t i = 0; i < Nodes.size(); i++)
			{
				if (Nodes[i]->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
				{
					if (Nodes[i]->LeftTop.x < MouseSelectRegionMin.x + RegionSize.x &&
						Nodes[i]->LeftTop.x + Nodes[i]->GetSize().x * Zoom > MouseSelectRegionMin.x &&
						Nodes[i]->LeftTop.y < MouseSelectRegionMin.y + RegionSize.y &&
						Nodes[i]->GetSize().y * Zoom + Nodes[i]->LeftTop.y > MouseSelectRegionMin.y)
					{
						AddSelected(Nodes[i]);
					}
				}
				else if (Nodes[i]->GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
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
			if (SocketLookingForConnection == nullptr)
			{
				if (Selected.empty() && SocketHovered == nullptr)
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
				else if (SocketHovered == nullptr)
				{
					for (size_t i = 0; i < Selected.size(); i++)
					{
						if (Selected[i]->CouldBeMoved())
							Selected[i]->SetPosition(Selected[i]->GetPosition() + GetMouseDelta());
					}
				}
			}
		}
	}

	if (ImGui::IsMouseClicked(0))
	{
		if (Hovered != nullptr)
		{
			if (!IsSelected(Hovered) && !ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_LEFT_CONTROL)) && !ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_RIGHT_CONTROL)))
				Selected.clear();
			AddSelected(Hovered);
		}
		else
		{
			Selected.clear();
		}
	}

	if (ImGui::IsMouseClicked(1))
	{
		if (Hovered != nullptr)
		{
			// Should we disconnect sockets
			if (SocketHovered != nullptr && !SocketHovered->Connections.empty())
			{
				std::vector<VisualNodeConnection*> ImpactedConnections = GetAllConnections(SocketHovered);
				for (size_t i = 0; i < ImpactedConnections.size(); i++)
				{
					Disconnect(ImpactedConnections[i]);
				}
			}
			else
			{
				if (Selected.size() <= 1)
				{
					if (!Hovered->OpenContextMenu())
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
						Hovered->OpenContextMenu();
					}
				}

				// If hovered node was already selected do nothing
				if (AddSelected(Hovered))
				{
					// But if it was not selected before deselect all other nodes.
					Selected.clear();
					AddSelected(Hovered);
				}
			}
		}
		else
		{
			Selected.clear();
			bOpenMainContextMenu = true;
		}
	}
	
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
	else if (ImGui::GetIO().MouseReleased[0] || Selected.size() > 1)
	{
		SocketLookingForConnection = nullptr;
	}

	if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_DELETE)))
	{
		for (size_t i = 0; i < Selected.size(); i++)
		{
			DeleteNode(Selected[i]);
		}

		Selected.clear();
	}

	static bool WasCopiedToClipboard = false;
	if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_LEFT_CONTROL)) || ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_RIGHT_CONTROL)))
	{
		if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_C)))
		{
			if (!Selected.empty())
			{
				const VisualNodeArea* NewNodeArea = VisualNodeArea::CreateNodeArea(Selected);
				APPLICATION.SetClipboardText(NewNodeArea->ToJson());
				delete NewNodeArea;
			}
		}
		else if (ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_V)))
		{
			if (!WasCopiedToClipboard)
			{
				WasCopiedToClipboard = true;

				const std::string NodesToImport = APPLICATION.GetClipboardText();
				Json::Value data;

				JSONCPP_STRING err;
				const Json::CharReaderBuilder builder;

				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				if (!reader->parse(NodesToImport.c_str(), NodesToImport.c_str() + NodesToImport.size(), &data, &err))
					return;

				VisualNodeArea* NewNodeArea = VisualNodeArea::FromJson(NodesToImport);

				// ***************** Place new nodes in center of a view space *****************
				const ImVec2 ViewCenter = GetRenderedViewCenter();
				ImVec2 NodesAABBCenter = NewNodeArea->GetAllNodesAABBCenter();
				NodesAABBCenter -= NewNodeArea->GetAreaRenderOffset();

				NeededShift = ViewCenter - NodesAABBCenter;

				NewNodeArea->RunOnEachNode([](VisualNode* Node) {
					Node->SetPosition(Node->GetPosition() + NeededShift);
				});
				// ***************** Place new nodes in center of a view space END *****************
				
				VisualNodeArea::CopyNodesTo(NewNodeArea, this);
				
				// Select pasted nodes.
				Selected.clear();
				for (size_t i = Nodes.size() - NewNodeArea->Nodes.size(); i < Nodes.size(); i++)
				{
					Selected.push_back(Nodes[i]);
				}

				delete NewNodeArea;
			}
		}
	}

	if (!ImGui::IsKeyDown(static_cast<ImGuiKey>(GLFW_KEY_V)))
		WasCopiedToClipboard = false;

	if (ImGui::GetIO().MouseWheel > 0)
	{
		ApplyZoom(1.0f);
	}
	else if (ImGui::GetIO().MouseWheel < 0)
	{
		ApplyZoom(-1.0f);
	}
}

void VisualNodeArea::InputUpdateNode(VisualNode* Node)
{
	if (Node->GetStyle() == VISUAL_NODE_STYLE_DEFAULT)
	{
		if (Node->LeftTop.x < MouseCursorPosition.x + MouseCursorSize.x &&
			Node->LeftTop.x + Node->GetSize().x * Zoom > MouseCursorPosition.x &&
			Node->LeftTop.y < MouseCursorPosition.y + MouseCursorSize.y &&
			Node->GetSize().y * Zoom + Node->LeftTop.y > MouseCursorPosition.y)
		{
			Hovered = Node;
			Node->SetIsHovered(true);
		}
	}
	else if (Node->GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
	{
		if (glm::distance(glm::vec2(Node->LeftTop.x + NODE_DIAMETER / 2.0f * Zoom, Node->LeftTop.y + NODE_DIAMETER / 2.0f * Zoom),
						  glm::vec2(MouseCursorPosition.x, MouseCursorPosition.y)) <= NODE_DIAMETER * Zoom)
		{
			Hovered = Node;
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

void VisualNodeArea::InputUpdateSocket(NodeSocket* Socket)
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

bool VisualNodeArea::AddSelected(VisualNode* NewNode)
{
	if (NewNode == nullptr)
		return false;

	for (size_t i = 0; i < Selected.size(); i++)
	{
		if (Selected[i] == NewNode)
			return false;
	}

	Selected.push_back(NewNode);
	return true;
}

bool VisualNodeArea::IsSelected(const VisualNode* Node) const
{
	if (Node == nullptr)
		return false;

	for (size_t i = 0; i < Selected.size(); i++)
	{
		if (Selected[i] == Node)
			return true;
	}

	return false;
}

void VisualNodeArea::ClearSelection()
{
	Selected.clear();
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
		return NodeAreaWindow->Size / 2.0f - RenderOffset;
	}
	else
	{
		return ImGui::GetCurrentWindow()->Size / 2.0f - RenderOffset;
	}
}

void VisualNodeArea::RunOnEachConnectedNode(VisualNode* StartNode, void(*Func)(VisualNode*))
{
	if (Func == nullptr)
		return;

	static std::unordered_map<VisualNode*, bool> SeenNodes;
	SeenNodes.clear();
	auto bWasNodeSeen = [](VisualNode* Node) {
		if (SeenNodes.find(Node) == SeenNodes.end())
		{
			SeenNodes[Node] = true;
			return false;
		}

		return true;
	};
	
	std::vector<VisualNode*> CurrentNodes;
	CurrentNodes.push_back(StartNode);
	if (bWasNodeSeen(StartNode))
		return;
	while (!EmptyOrFilledByNulls(CurrentNodes))
	{
		for (int i = 0; i < static_cast<int>(CurrentNodes.size()); i++)
		{
			if (CurrentNodes[i] == nullptr)
			{
				CurrentNodes.erase(CurrentNodes.begin() + i);
				i--;
				continue;
			}

			Func(CurrentNodes[i]);

			std::vector<VisualNode*> NewNodes = CurrentNodes[i]->GetNodesConnectedToOutput();
			for (size_t j = 0; j < NewNodes.size(); j++)
			{
				CurrentNodes.push_back(NewNodes[j]);
				if (bWasNodeSeen(NewNodes[j]))
					return;
			}

			CurrentNodes.erase(CurrentNodes.begin() + i);
			i--;
		}
	}
}

bool VisualNodeArea::TryToConnect(const VisualNode* OutNode, const std::string OutSocketID, const VisualNode* InNode, const std::string InSocketID)
{
	size_t OutSocketIndex = 0;
	for (size_t i = 0; i < OutNode->Output.size(); i++)
	{
		if (OutNode->Output[i]->GetID() == OutSocketID)
		{
			OutSocketIndex = i;
			break;
		}
	}

	size_t InSocketIndex = 0;
	for (size_t i = 0; i < InNode->Input.size(); i++)
	{
		if (InNode->Input[i]->GetID() == InSocketID)
		{
			InSocketIndex = i;
			break;
		}
	}

	return TryToConnect(OutNode, OutSocketIndex, InNode, InSocketIndex);
}

VisualNodeArea* VisualNodeArea::CreateNodeArea(const std::vector<VisualNode*> Nodes)
{
	VisualNodeArea* NewArea = new VisualNodeArea();

	// Copy all nodes to new node area.
	std::unordered_map<VisualNode*, VisualNode*> OldToNewNode;
	std::unordered_map<NodeSocket*, NodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		//VisualNode* CopyOfNode = VisualNode::CopyChild(Nodes[i]->GetType(), Nodes[i]);
		VisualNode* CopyOfNode = NODE_FACTORY.CopyNode(Nodes[i]->GetType(), *Nodes[i]);

		if (CopyOfNode == nullptr)
			CopyOfNode = new VisualNode(*Nodes[i]);
		CopyOfNode->ParentArea = NewArea;

		//newArea->nodes.push_back(copyOfNode);
		NewArea->AddNode(CopyOfNode);

		// Associate old to new IDs
		OldToNewNode[Nodes[i]] = CopyOfNode;

		for (size_t j = 0; j < Nodes[i]->Input.size(); j++)
		{
			OldToNewSocket[Nodes[i]->Input[j]] = CopyOfNode->Input[j];
		}

		for (size_t j = 0; j < Nodes[i]->Output.size(); j++)
		{
			OldToNewSocket[Nodes[i]->Output[j]] = CopyOfNode->Output[j];
		}
	}

	// Than we need to recreate all connections.
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		for (size_t j = 0; j < Nodes[i]->Input.size(); j++)
		{
			for (size_t k = 0; k < Nodes[i]->Input[j]->Connections.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (IsNodeIDInList(Nodes[i]->Input[j]->Connections[k]->GetParent()->GetID(), Nodes))
				{
					NewArea->Nodes[i]->Input[j]->Connections.push_back(OldToNewSocket[Nodes[i]->Input[j]->Connections[k]]);

					// Add connection to node area.
					// Maybe we already establish this connection.
					bool bShouldAdd = true;
					for (size_t l = 0; l < NewArea->Connections.size(); l++)
					{
						if (NewArea->Connections[l]->In == OldToNewSocket[Nodes[i]->Input[j]] &&
							NewArea->Connections[l]->Out == OldToNewSocket[Nodes[i]->Input[j]->Connections[k]])
						{
							bShouldAdd = false;
							break;
						}
					}

					if (bShouldAdd)
						NewArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[Nodes[i]->Input[j]->Connections[k]], OldToNewSocket[Nodes[i]->Input[j]]));
				}
			}
		}

		for (size_t j = 0; j < Nodes[i]->Output.size(); j++)
		{
			for (size_t k = 0; k < Nodes[i]->Output[j]->Connections.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (IsNodeIDInList(Nodes[i]->Output[j]->Connections[k]->GetParent()->GetID(), Nodes))
				{
					NewArea->Nodes[i]->Output[j]->Connections.push_back(OldToNewSocket[Nodes[i]->Output[j]->Connections[k]]);

					// Add connection to node area.
					// Maybe we already establish this connection.
					bool bShouldAdd = true;
					for (size_t l = 0; l < NewArea->Connections.size(); l++)
					{
						if (NewArea->Connections[l]->In == OldToNewSocket[Nodes[i]->Output[j]->Connections[k]] &&
							NewArea->Connections[l]->Out == OldToNewSocket[Nodes[i]->Output[j]])
						{
							bShouldAdd = false;
							break;
						}
					}

					if (bShouldAdd)
						NewArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[Nodes[i]->Output[j]], OldToNewSocket[Nodes[i]->Output[j]->Connections[k]]));
				}
			}
		}
	}

	return NewArea;
}

std::string VisualNodeArea::ToJson() const
{
	Json::Value root;
	std::ofstream SaveFile;

	Json::Value NodesData;
	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		NodesData[std::to_string(i)] = Nodes[i]->ToJson();
	}
	root["nodes"] = NodesData;

	Json::Value ConnectionsData;
	for (int i = 0; i < static_cast<int>(Connections.size()); i++)
	{
		ConnectionsData[std::to_string(i)]["in"]["socket_ID"] = Connections[i]->In->GetID();
		size_t socket_index = 0;
		for (size_t j = 0; j < Connections[i]->In->GetParent()->Input.size(); j++)
		{
			if (Connections[i]->In->GetParent()->Input[j]->GetID() == Connections[i]->In->GetID())
				socket_index = j;
		}
		ConnectionsData[std::to_string(i)]["in"]["socket_index"] = socket_index;
		ConnectionsData[std::to_string(i)]["in"]["node_ID"] = Connections[i]->In->GetParent()->GetID();

		ConnectionsData[std::to_string(i)]["out"]["socket_ID"] = Connections[i]->Out->GetID();
		socket_index = 0;
		for (size_t j = 0; j < Connections[i]->Out->GetParent()->Output.size(); j++)
		{
			if (Connections[i]->Out->GetParent()->Output[j]->GetID() == Connections[i]->Out->GetID())
				socket_index = j;
		}
		ConnectionsData[std::to_string(i)]["out"]["socket_index"] = socket_index;
		ConnectionsData[std::to_string(i)]["out"]["node_ID"] = Connections[i]->Out->GetParent()->GetID();
	}
	root["connections"] = ConnectionsData;

	root["renderOffset"]["x"] = RenderOffset.x;
	root["renderOffset"]["y"] = RenderOffset.y;

	Json::StreamWriterBuilder builder;
	const std::string JsonText = Json::writeString(builder, root);

	return JsonText;
}

VisualNodeArea* VisualNodeArea::FromJson(std::string JsonText)
{
	VisualNodeArea* NewArea = new VisualNodeArea();

	if (JsonText.find("{") == std::string::npos || JsonText.find("}") == std::string::npos || JsonText.find(":") == std::string::npos)
		return NewArea;

	Json::Value root;
	JSONCPP_STRING err;
	Json::CharReaderBuilder builder;

	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(JsonText.c_str(), JsonText.c_str() + JsonText.size(), &root, &err))
		return NewArea;

	if (!root.isMember("nodes"))
		return NewArea;

	std::unordered_map<std::string, VisualNode*> LoadedNodes;
	std::vector<Json::String> NodesList = root["nodes"].getMemberNames();
	for (size_t i = 0; i < NodesList.size(); i++)
	{
		std::string NodeType = root["nodes"][std::to_string(i)]["nodeType"].asCString();
		VisualNode* NewNode = NODE_FACTORY.CreateNode(NodeType);
		if (NewNode == nullptr)
		{
			if (NodeType == "VisualNode")
			{
				NewNode = new VisualNode();
			}
			else
			{
				continue;
			}
		}
			
		NewNode->FromJson(root["nodes"][std::to_string(i)]);

		if (NewNode != nullptr)
		{
			LoadedNodes[NewNode->GetID()] = NewNode;
			NewArea->AddNode(NewNode);
		}
	}

	std::vector<Json::String> ConnectionsList = root["connections"].getMemberNames();
	for (size_t i = 0; i < ConnectionsList.size(); i++)
	{
		std::string InSocketID = root["connections"][ConnectionsList[i]]["in"]["socket_ID"].asCString();
		std::string InNodeID = root["connections"][ConnectionsList[i]]["in"]["node_ID"].asCString();

		std::string OutSocketID = root["connections"][ConnectionsList[i]]["out"]["socket_ID"].asCString();
		std::string OutNodeID = root["connections"][ConnectionsList[i]]["out"]["node_ID"].asCString();

		if (LoadedNodes.find(OutNodeID) != LoadedNodes.end() && LoadedNodes.find(InNodeID) != LoadedNodes.end())
			NewArea->TryToConnect(LoadedNodes[OutNodeID], OutSocketID, LoadedNodes[InNodeID], InSocketID);
	}

	if (root.isMember("renderOffset"))
	{
		float OffsetX = root["renderOffset"]["x"].asFloat();
		float OffsetY = root["renderOffset"]["y"].asFloat();
		NewArea->SetAreaRenderOffset(ImVec2(OffsetX, OffsetY));
	}

	return NewArea;
}

void VisualNodeArea::CopyNodesTo(VisualNodeArea* SourceNodeArea, VisualNodeArea* TargetNodeArea)
{
	const size_t NodeShift = TargetNodeArea->Nodes.size();

	// Copy all nodes to new node area.
	std::unordered_map<VisualNode*, VisualNode*> OldToNewNode;
	std::unordered_map<NodeSocket*, NodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < SourceNodeArea->Nodes.size(); i++)
	{
		VisualNode* CopyOfNode = NODE_FACTORY.CopyNode(SourceNodeArea->Nodes[i]->GetType(), *SourceNodeArea->Nodes[i]);
		if (CopyOfNode == nullptr)
			CopyOfNode = new VisualNode(*SourceNodeArea->Nodes[i]);
		CopyOfNode->ParentArea = SourceNodeArea;

		TargetNodeArea->AddNode(CopyOfNode);

		// Associate old to new IDs
		OldToNewNode[SourceNodeArea->Nodes[i]] = CopyOfNode;

		for (size_t j = 0; j < SourceNodeArea->Nodes[i]->Input.size(); j++)
		{
			OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]] = CopyOfNode->Input[j];
		}

		for (size_t j = 0; j < SourceNodeArea->Nodes[i]->Output.size(); j++)
		{
			OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]] = CopyOfNode->Output[j];
		}
	}

	// Than we need to recreate all connections.
	for (size_t i = 0; i < SourceNodeArea->Nodes.size(); i++)
	{
		for (size_t j = 0; j < SourceNodeArea->Nodes[i]->Input.size(); j++)
		{
			for (size_t k = 0; k < SourceNodeArea->Nodes[i]->Input[j]->Connections.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (IsNodeIDInList(SourceNodeArea->Nodes[i]->Input[j]->Connections[k]->GetParent()->GetID(), SourceNodeArea->Nodes))
				{
					TargetNodeArea->Nodes[NodeShift + i]->Input[j]->Connections.push_back(OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]->Connections[k]]);

					// Add connection to node area.
					// Maybe we already establish this connection.
					bool bShouldAdd = true;
					for (size_t l = 0; l < TargetNodeArea->Connections.size(); l++)
					{
						if (TargetNodeArea->Connections[l]->In == OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]] &&
							TargetNodeArea->Connections[l]->Out == OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]->Connections[k]])
						{
							bShouldAdd = false;
							break;
						}
					}

					if (bShouldAdd)
						TargetNodeArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]->Connections[k]], OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]]));
				}
			}
		}

		for (size_t j = 0; j < SourceNodeArea->Nodes[i]->Output.size(); j++)
		{
			for (size_t k = 0; k < SourceNodeArea->Nodes[i]->Output[j]->Connections.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (IsNodeIDInList(SourceNodeArea->Nodes[i]->Output[j]->Connections[k]->GetParent()->GetID(), SourceNodeArea->Nodes))
				{
					TargetNodeArea->Nodes[NodeShift + i]->Output[j]->Connections.push_back(OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]->Connections[k]]);

					// Add connection to node area.
					// Maybe we already establish this connection.
					bool bShouldAdd = true;
					for (size_t l = 0; l < TargetNodeArea->Connections.size(); l++)
					{
						if (TargetNodeArea->Connections[l]->In == OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]->Connections[k]] &&
							TargetNodeArea->Connections[l]->Out == OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]])
						{
							bShouldAdd = false;
							break;
						}
					}

					if (bShouldAdd)
						TargetNodeArea->Connections.push_back(new VisualNodeConnection(OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]], OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]->Connections[k]]));
				}
			}
		}
	}
}

void VisualNodeArea::LoadFromFile(const char* FileName)
{
	std::ifstream NodesFile;
	NodesFile.open(FileName);

	const std::string FileData((std::istreambuf_iterator<char>(NodesFile)), std::istreambuf_iterator<char>());
	NodesFile.close();

	VisualNodeArea* NewNodeArea = VisualNodeArea::FromJson(FileData);
	VisualNodeArea::CopyNodesTo(NewNodeArea, this);
	delete NewNodeArea;
}

std::vector<VisualNode*> VisualNodeArea::GetNodesByName(const std::string NodeName) const
{
	std::vector<VisualNode*> result;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetName() == NodeName)
			result.push_back(Nodes[i]);
	}

	return result;
}

std::vector<VisualNode*> VisualNodeArea::GetNodesByType(const std::string NodeType) const
{
	std::vector<VisualNode*> result;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetType() == NodeType)
			result.push_back(Nodes[i]);
	}

	return result;
}

int VisualNodeArea::GetNodeCount() const
{
	return static_cast<int>(Nodes.size());
}

bool VisualNodeArea::IsMouseHovered() const
{
	return bMouseHovered;
}

bool VisualNodeArea::IsAreaFillingWindow()
{
	return bFillWindow;
}

void VisualNodeArea::SetIsAreaFillingWindow(bool NewValue)
{
	bFillWindow = NewValue;
}

bool VisualNodeArea::TriggerSocketEvent(NodeSocket* CallerNodeSocket, NodeSocket* TriggeredNodeSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{
	if (CallerNodeSocket == nullptr || TriggeredNodeSocket == nullptr)
		return false;

	if (EventType != VISUAL_NODE_SOCKET_EXECUTE && EventType != VISUAL_NODE_SOCKET_UPDATE)
		return false;

	if (TriggeredNodeSocket->GetParent() == nullptr)
		return false;

	SocketEventQueue.push({ TriggeredNodeSocket, CallerNodeSocket, EventType });

	return true;
}

bool VisualNodeArea::TriggerOrphanSocketEvent(VisualNode* Node, VISUAL_NODE_SOCKET_EVENT EventType)
{
	if (Node == nullptr)
		return false;

	if (EventType != VISUAL_NODE_SOCKET_EXECUTE)
		return false;

	Node->SocketEvent(nullptr, nullptr, EventType);

	return true;
}

void VisualNodeArea::ProcessSocketEventQueue()
{
	while (!SocketEventQueue.empty())
	{
		SocketEvent Event = SocketEventQueue.front();
		SocketEventQueue.pop();

		Event.TriggeredNodeSocket->GetParent()->SocketEvent(Event.TriggeredNodeSocket, Event.CallerNodeSocket, Event.EventType);
	}
}

bool VisualNodeArea::EmptyOrFilledByNulls(const std::vector<VisualNode*> Vector)
{
	for (size_t i = 0; i < Vector.size(); i++)
	{
		if (Vector[i] != nullptr)
			return false;
	}

	return true;
}

void VisualNodeArea::ApplyZoom(float Delta)
{
	ImVec2 MousePosBeforeZoom = (ImGui::GetMousePos() - ImGui::GetCurrentWindow()->Pos - RenderOffset) / Zoom;
	Zoom += Delta * 0.1f;
	Zoom = std::max(Zoom, MIN_ZOOM_LEVEL);
	Zoom = std::min(Zoom, MAX_ZOOM_LEVEL);
	ImVec2 MousePosAfterZoom = (ImGui::GetMousePos() - ImGui::GetCurrentWindow()->Pos - RenderOffset) / Zoom;

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