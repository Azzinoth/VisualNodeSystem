#include "FEVisualNodeArea.h"

ImVec2 FEVisualNodeArea::NeededShift = ImVec2();

FEVisualNodeArea::FEVisualNodeArea() {};

void FEVisualNodeArea::AddNode(FEVisualNode* NewNode)
{
	if (NewNode == nullptr)
		return;

	//if (newNode->parentArea != nullptr)
	//	newNode->setPosition(newNode->getPosition() + newNode->parentArea->getAreaRenderOffset());

	NewNode->ParentArea = this;
	//newNode->setPosition(newNode->getPosition() - getAreaRenderOffset());

	Nodes.push_back(NewNode);
}

FEVisualNodeArea::~FEVisualNodeArea() 
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

void FEVisualNodeArea::RenderNode(FEVisualNode* Node) const
{
	if (CurrentDrawList == nullptr || Node == nullptr)
		return;

	ImGui::PushID(Node->GetID().c_str());

	Node->LeftTop = ImGui::GetCurrentWindow()->Pos + Node->GetPosition() + RenderOffset;
	if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		Node->RightBottom = Node->LeftTop + Node->GetSize();
	}
	else if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		Node->RightBottom = Node->LeftTop + ImVec2(NODE_RADIUS, NODE_RADIUS);
	}

	if (IsSelected(Node))
	{
		if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
		{
			const ImVec2 LeftTop = Node->LeftTop - ImVec2(4.0f, 4.0f);
			const ImVec2 RightBottom = Node->RightBottom + ImVec2(4.0f, 4.0f);
			ImGui::GetWindowDrawList()->AddRect(LeftTop, RightBottom, IM_COL32(175, 255, 175, 255), 16.0f);
		}
		else if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
		{
			ImGui::GetWindowDrawList()->AddCircle(Node->LeftTop + ImVec2(NODE_RADIUS / 2.0f, NODE_RADIUS / 2.0f), NODE_RADIUS + 4.0f, IM_COL32(175, 255, 175, 255), 32, 4.0f);
		}
	}

	CurrentDrawList->ChannelsSetCurrent(2);
	
	if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		ImGui::SetCursorScreenPos(Node->LeftTop);
	}
	else if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		ImGui::SetCursorScreenPos(Node->LeftTop - ImVec2(NODE_RADIUS / 2.0f - NODE_RADIUS / 4.0f, NODE_RADIUS / 2.0f - NODE_RADIUS / 4.0f));
	}
	Node->Draw();

	CurrentDrawList->ChannelsSetCurrent(1);
	ImGui::SetCursorScreenPos(Node->LeftTop);

	// Drawing node background layer.
	const ImU32 NodeBackgroundColor = (Hovered == Node || IsSelected(Node)) ? IM_COL32(75, 75, 75, 125) : IM_COL32(60, 60, 60, 125);
	if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		CurrentDrawList->AddRectFilled(Node->LeftTop, Node->RightBottom, NodeBackgroundColor, 8.0f);
	}
	else if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(Node->LeftTop + (Node->RightBottom - Node->LeftTop) / 2.0f,
													NODE_RADIUS,
													NodeBackgroundColor, 32);
	}

	if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		// Drawing caption area.
		ImVec2 TitleArea = Node->RightBottom;
		TitleArea.y = Node->LeftTop.y + NODE_TITLE_HEIGHT;
		const ImU32 NodeTitleBackgroundColor = (Hovered == Node || IsSelected(Node)) ? Node->TitleBackgroundColorHovered : Node->TitleBackgroundColor;

		CurrentDrawList->AddRectFilled(Node->LeftTop + ImVec2(1, 1), TitleArea, NodeTitleBackgroundColor, 8.0f);
		CurrentDrawList->AddRect(Node->LeftTop, Node->RightBottom, ImColor(100, 100, 100), 8.0f);

		const ImVec2 TextSize = ImGui::CalcTextSize(Node->GetName().c_str());
		ImVec2 TextPosition;
		TextPosition.x = Node->LeftTop.x + (Node->GetSize().x / 2) - TextSize.x / 2;
		TextPosition.y = Node->LeftTop.y + (NODE_TITLE_HEIGHT / 2) - TextSize.y / 2;

		ImGui::SetCursorScreenPos(TextPosition);
		ImGui::Text(Node->GetName().c_str());
	}
	else if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		CurrentDrawList->AddCircle(Node->LeftTop + ImVec2(NODE_RADIUS / 2.0f, NODE_RADIUS / 2.0f), NODE_RADIUS + 2.0f, ImColor(100, 100, 100), 32, 2.0f);
	}

	RenderNodeSockets(Node);

	ImGui::PopID();
}

void FEVisualNodeArea::RenderNodeSockets(const FEVisualNode* Node) const
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

void FEVisualNodeArea::RenderNodeSocket(FEVisualNodeSocket* Socket) const
{
	const ImVec2 SocketPosition = SocketToPosition(Socket);
	if (Socket->GetParent()->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		const bool Input = FEVisualNode::IsSocketTypeIn(Socket->GetType());
		// Socket description.
		const ImVec2 TextSize = ImGui::CalcTextSize(Socket->GetName().c_str());

		float TextX = SocketPosition.x;
		TextX += Input ? NODE_SOCKET_SIZE * 2.0f : -NODE_SOCKET_SIZE * 2.0f - TextSize.x;

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
		CurrentDrawList->ChannelsSetCurrent(3);
		DrawHermiteLine(SocketPosition, ImGui::GetIO().MousePos, 12, ImColor(200, 200, 200));
	}

	// Draw socket icon.
	CurrentDrawList->AddCircleFilled(SocketPosition, NODE_SOCKET_SIZE, SocketColor);
}

void FEVisualNodeArea::RenderGrid(ImVec2 CurrentPosition) const
{
	CurrentDrawList->ChannelsSplit(2);

	CurrentPosition.x += RenderOffset.x;
	CurrentPosition.y += RenderOffset.y;
	
	// Horizontal lines
	const int StartingStep = static_cast<int>(ceil(-10000.0f / FE_VISUAL_NODE_GRID_STEP));
	const int StepCount = static_cast<int>(ceil(10000.0f / FE_VISUAL_NODE_GRID_STEP));
	for (int i = StartingStep; i < StepCount; i++)
	{
		ImVec2 from = ImVec2(CurrentPosition.x - 10000.0f , CurrentPosition.y + i * FE_VISUAL_NODE_GRID_STEP);
		ImVec2 to = ImVec2(CurrentPosition.x + 10000.0f, CurrentPosition.y + i * FE_VISUAL_NODE_GRID_STEP);

		if (i % 10 != 0)
		{
			CurrentDrawList->ChannelsSetCurrent(1);
			CurrentDrawList->AddLine(from, to, ImGui::GetColorU32(GridLinesColor), 1);
		}
		else
		{
			CurrentDrawList->ChannelsSetCurrent(0);
			CurrentDrawList->AddLine(from, to, ImGui::GetColorU32(GridBoldLinesColor), 3);
		}
	}

	// Vertical lines
	for (int i = StartingStep; i < StepCount; i++)
	{
		ImVec2 from = ImVec2(CurrentPosition.x + i * FE_VISUAL_NODE_GRID_STEP, CurrentPosition.y - 10000.0f);
		ImVec2 to = ImVec2(CurrentPosition.x + i * FE_VISUAL_NODE_GRID_STEP, CurrentPosition.y + 10000.0f);

		if (i % 10 != 0)
		{
			CurrentDrawList->ChannelsSetCurrent(1);
			CurrentDrawList->AddLine(from, to, ImGui::GetColorU32(GridLinesColor), 1);
		}
		else
		{
			CurrentDrawList->ChannelsSetCurrent(0);
			CurrentDrawList->AddLine(from, to, ImGui::GetColorU32(GridBoldLinesColor), 3);
		}
	}

	CurrentDrawList->ChannelsMerge();
}

void FEVisualNodeArea::Render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, GridBackgroundColor);

	const ImVec2 CurrentPosition = ImGui::GetCurrentWindow()->Pos + AreaPosition;
	ImGui::SetNextWindowPos(CurrentPosition);
	ImGui::BeginChild("Nodes area", GetAreaSize(), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

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
}

ImVec2 FEVisualNodeArea::GetAreaSize() const
{
	return AreaSize;
}

void FEVisualNodeArea::SetAreaSize(const ImVec2 NewValue)
{
	if (NewValue.x < 1 || NewValue.y < 1)
		return;

	AreaSize = NewValue;
}

ImVec2 FEVisualNodeArea::GetAreaPosition() const
{
	return AreaPosition;
}

void FEVisualNodeArea::SetAreaPosition(const ImVec2 NewValue)
{
	if (NewValue.x < 0 || NewValue.y < 0)
		return;

	AreaPosition = NewValue;
}

void FEVisualNodeArea::DrawHermiteLine(const ImVec2 P1, const ImVec2 P2, const int Steps, const ImColor Color) const
{
	const ImVec2 t1 = ImVec2(80.0f, 0.0f);
	const ImVec2 t2 = ImVec2(80.0f, 0.0f);

	for (int step = 0; step <= Steps; step++)
	{
		const float t = static_cast<float>(step) / static_cast<float>(Steps);
		const float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
		const float h2 = -2 * t * t * t + 3 * t * t;
		const float h3 = t * t * t - 2 * t * t + t;
		const float h4 = t * t * t - t * t;
		CurrentDrawList->PathLineTo(ImVec2(h1 * P1.x + h2 * P2.x + h3 * t1.x + h4 * t2.x, h1 * P1.y + h2 * P2.y + h3 * t1.y + h4 * t2.y));
	}

	CurrentDrawList->PathStroke(Color, false, 3.0f);
}

void FEVisualNodeArea::RenderConnection(const FEVisualNodeConnection* Connection) const
{
	if (Connection->Out == nullptr || Connection->In == nullptr)
		return;

	ImColor ConnectionColor = ImColor(200, 200, 200);
	if (Connection->Out->ForceColor != nullptr)
		ConnectionColor = *Connection->Out->ForceColor;

	DrawHermiteLine(SocketToPosition(Connection->Out), SocketToPosition(Connection->In), 12, ConnectionColor);
}

ImVec2 FEVisualNodeArea::SocketToPosition(const FEVisualNodeSocket* Socket) const
{
	const bool Input = FEVisualNode::IsSocketTypeIn(Socket->Type);
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

	if (Socket->GetParent()->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		SocketX = Input ? Socket->Parent->LeftTop.x + NODE_SOCKET_SIZE * 3 : Socket->Parent->RightBottom.x - NODE_SOCKET_SIZE * 3;

		const float HeightForSockets = Socket->Parent->GetSize().y - NODE_TITLE_HEIGHT;
		const float SocketSpacing = HeightForSockets / (Input ? Socket->Parent->Input.size() : Socket->Parent->Output.size());

		SocketY = Socket->Parent->LeftTop.y + NODE_TITLE_HEIGHT + SocketSpacing * (SocketIndex + 1) - SocketSpacing / 2.0f;
	}
	else if (Socket->GetParent()->GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		const size_t SocketCount = Input ? Socket->Parent->Input.size() : Socket->Parent->Output.size();
		float BeginAngle = (180.0f / static_cast<float>(SocketCount) / 2.0f);
		if (Input)
			BeginAngle = -BeginAngle;

		float step = (180.0f / static_cast<float>(SocketCount) * (SocketIndex));
		if (Input)
			step = -step;

		const float angle = BeginAngle + step;

		const float NodeCenterX = Socket->Parent->LeftTop.x + NODE_RADIUS / 2.0f;
		const float NodeCenterY = Socket->Parent->LeftTop.y + NODE_RADIUS / 2.0f;

		SocketX = NodeCenterX + NODE_RADIUS * 0.95f * sin(glm::radians(angle));
		SocketY = NodeCenterY + NODE_RADIUS * 0.95f * cos(glm::radians(angle));
	}

	return {SocketX, SocketY};
}

void FEVisualNodeArea::Update()
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

	Render();
}

std::vector<FEVisualNodeConnection*> FEVisualNodeArea::GetAllConnections(const FEVisualNodeSocket* Socket) const
{
	std::vector<FEVisualNodeConnection*> result;

	for (size_t i = 0; i < Connections.size(); i++)
	{
		if (Connections[i]->In == Socket || Connections[i]->Out == Socket)
			result.push_back(Connections[i]);
	}

	return result;
}

void FEVisualNodeArea::Disconnect(FEVisualNodeConnection*& Connection)
{
	for (int i = 0; i < static_cast<int>(Connection->In->Connections.size()); i++)
	{
		if (Connection->In->Connections[i] == Connection->Out)
		{
			FEVisualNode* parent = Connection->In->Connections[i]->Parent;
			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, FE_VISUAL_NODE_BEFORE_DISCONNECTED);

			Connection->In->Connections.erase(Connection->In->Connections.begin() + i, Connection->In->Connections.begin() + i + 1);
			Connection->In->Parent->SocketEvent(Connection->In, Connection->Out, bClearing ? FE_VISUAL_NODE_SOCKET_DESTRUCTION : FE_VISUAL_NODE_SOCKET_DISCONNECTED);
			i--;

			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, FE_VISUAL_NODE_AFTER_DISCONNECTED);
		}
	}

	for (int i = 0; i < static_cast<int>(Connection->Out->Connections.size()); i++)
	{
		if (Connection->Out->Connections[i] == Connection->In)
		{
			FEVisualNode* parent = Connection->Out->Connections[i]->Parent;
			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, FE_VISUAL_NODE_BEFORE_DISCONNECTED);

			Connection->Out->Connections.erase(Connection->Out->Connections.begin() + i, Connection->Out->Connections.begin() + i + 1);
			i--;

			if (!bClearing)
				PropagateNodeEventsCallbacks(parent, FE_VISUAL_NODE_AFTER_DISCONNECTED);
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

void FEVisualNodeArea::DeleteNode(const FEVisualNode* Node)
{
	if (!Node->bCouldBeDestroyed)
		return;

	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i] == Node)
		{
			PropagateNodeEventsCallbacks(Nodes[i], FE_VISUAL_NODE_REMOVED);

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

void FEVisualNodeArea::SetMainContextMenuFunc(void(*Func)())
{
	MainContextMenuFunc = Func;
}

void FEVisualNodeArea::Clear()
{
	bClearing = true;

	for (int i = 0; i < static_cast<int>(Nodes.size()); i++)
	{
		PropagateNodeEventsCallbacks(Nodes[i], FE_VISUAL_NODE_DESTROYED);
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

void FEVisualNodeArea::Reset()
{
	Clear();

	MainContextMenuFunc = nullptr;
	NodeEventsCallbacks.clear();
}

void FEVisualNodeArea::PropagateUpdateToConnectedNodes(const FEVisualNode* CallerNode) const
{
	if (CallerNode == nullptr)
		return;

	for (size_t i = 0; i < CallerNode->Input.size(); i++)
	{
		auto connections = GetAllConnections(CallerNode->Input[i]);
		for (size_t j = 0; j < connections.size(); j++)
		{
			connections[j]->In->GetParent()->SocketEvent(connections[j]->In, connections[j]->Out, FE_VISUAL_NODE_SOCKET_UPDATE);
		}
	}

	for (size_t i = 0; i < CallerNode->Output.size(); i++)
	{
		auto connections = GetAllConnections(CallerNode->Output[i]);
		for (size_t j = 0; j < connections.size(); j++)
		{
			connections[j]->In->GetParent()->SocketEvent(connections[j]->In, connections[j]->Out, FE_VISUAL_NODE_SOCKET_UPDATE);
		}
	}
}

ImVec2 FEVisualNodeArea::GetAreaRenderOffset() const
{
	return RenderOffset;
}

void FEVisualNodeArea::SetAreaRenderOffset(const ImVec2 Offset)
{
	if (Offset.x <= -10000.0f || Offset.x >= 10000.0f ||
		Offset.y <= -10000.0f || Offset.y >= 10000.0f)
		return;

	RenderOffset = Offset;
}

bool FEVisualNodeArea::TryToConnect(const FEVisualNode* OutNode, const size_t OutNodeSocketIndex, const FEVisualNode* InNode, const size_t InNodeSocketIndex)
{
	if (OutNode->Output.size() <= OutNodeSocketIndex)
		return false;

	if (InNode->Input.size() <= InNodeSocketIndex)
		return false;

	FEVisualNodeSocket* OutSocket = OutNode->Output[OutNodeSocketIndex];
	FEVisualNodeSocket* InSocket = InNode->Input[InNodeSocketIndex];

	char* msg = nullptr;
	const bool result = InSocket->GetParent()->CanConnect(InSocket, OutSocket, &msg);

	if (result)
	{
		PropagateNodeEventsCallbacks(OutSocket->GetParent(), FE_VISUAL_NODE_BEFORE_CONNECTED);
		PropagateNodeEventsCallbacks(InSocket->GetParent(), FE_VISUAL_NODE_BEFORE_CONNECTED);

		OutSocket->Connections.push_back(InSocket);
		InSocket->Connections.push_back(OutSocket);

		Connections.push_back(new FEVisualNodeConnection(OutSocket, InSocket));

		OutSocket->GetParent()->SocketEvent(OutSocket, InSocket, FE_VISUAL_NODE_SOCKET_CONNECTED);
		InSocket->GetParent()->SocketEvent(InSocket, OutSocket, FE_VISUAL_NODE_SOCKET_CONNECTED);

		PropagateNodeEventsCallbacks(OutSocket->GetParent(), FE_VISUAL_NODE_AFTER_CONNECTED);
		PropagateNodeEventsCallbacks(InSocket->GetParent(), FE_VISUAL_NODE_AFTER_CONNECTED);
	}

	return result;
}

void FEVisualNodeArea::SetNodeEventCallback(void(*Func)(FEVisualNode*, FE_VISUAL_NODE_EVENT))
{
	if (Func != nullptr)
		NodeEventsCallbacks.push_back(Func);
}

void FEVisualNodeArea::PropagateNodeEventsCallbacks(FEVisualNode* Node, const FE_VISUAL_NODE_EVENT EventToPropagate) const
{
	for (size_t i = 0; i < NodeEventsCallbacks.size(); i++)
	{
		if (NodeEventsCallbacks[i] != nullptr)
			NodeEventsCallbacks[i](Node, EventToPropagate);
	}
}

void FEVisualNodeArea::SaveToFile(const char* FileName) const
{
	const std::string json_file = ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << json_file;
	SaveFile.close();
}

bool FEVisualNodeArea::IsNodeIDInList(const std::string ID, const std::vector<FEVisualNode*> List)
{
	for (size_t i = 0; i < List.size(); i++)
	{
		if (List[i]->GetID() == ID)
			return true;
	}

	return false;
}

void FEVisualNodeArea::SaveNodesToFile(const char* FileName, std::vector<FEVisualNode*> Nodes)
{
	if (Nodes.empty())
		return;

	const FEVisualNodeArea* NewNodeArea = FEVisualNodeArea::CreateNodeArea(Nodes);
	const std::string json_file = NewNodeArea->ToJson();
	std::ofstream SaveFile;
	SaveFile.open(FileName);
	SaveFile << json_file;
	SaveFile.close();
	delete NewNodeArea;
}

void FEVisualNodeArea::RunOnEachNode(void(*Func)(FEVisualNode*))
{
	if (Func != nullptr)
		std::for_each(Nodes.begin(), Nodes.end(), Func);
}

FEVisualNode* FEVisualNodeArea::GetHovered() const
{
	return Hovered;
}

std::vector<FEVisualNode*> FEVisualNodeArea::GetSelected()
{
	return Selected;
}

void FEVisualNodeArea::InputUpdate()
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
		if (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT))
		{
			SocketLookingForConnection = nullptr;

			if (abs(ImGui::GetMouseDragDelta(0).x) > 1 || abs(ImGui::GetMouseDragDelta(0).y) > 1)
			{
				MouseSelectRegionMin = ImGui::GetIO().MouseClickedPos[0];
				MouseSelectRegionMax = MouseSelectRegionMin + ImGui::GetMouseDragDelta(0);

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
				if (Nodes[i]->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
				{
					if (Nodes[i]->LeftTop.x < MouseSelectRegionMin.x + RegionSize.x &&
						Nodes[i]->LeftTop.x + Nodes[i]->GetSize().x > MouseSelectRegionMin.x &&
						Nodes[i]->LeftTop.y < MouseSelectRegionMin.y + RegionSize.y &&
						Nodes[i]->GetSize().y + Nodes[i]->LeftTop.y > MouseSelectRegionMin.y)
					{
						AddSelected(Nodes[i]);
					}
				}
				else if (Nodes[i]->GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
				{
					if (Nodes[i]->LeftTop.x < MouseSelectRegionMin.x + RegionSize.x &&
						Nodes[i]->LeftTop.x + NODE_RADIUS > MouseSelectRegionMin.x &&
						Nodes[i]->LeftTop.y < MouseSelectRegionMin.y + RegionSize.y &&
						NODE_RADIUS + Nodes[i]->LeftTop.y > MouseSelectRegionMin.y)
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
					RenderOffset.x += ImGui::GetIO().MouseDelta.x;
					RenderOffset.y += ImGui::GetIO().MouseDelta.y;

					if (RenderOffset.x > 10000.0f)
						RenderOffset.x = 10000.0f;

					if (RenderOffset.x < -10000.0f)
						RenderOffset.x = -10000.0f;

					if (RenderOffset.y > 10000.0f)
						RenderOffset.y = 10000.0f;

					if (RenderOffset.y < -10000.0f)
						RenderOffset.y = -10000.0f;
				}
				else if (SocketHovered == nullptr)
				{
					for (size_t i = 0; i < Selected.size(); i++)
					{
						Selected[i]->SetPosition(Selected[i]->GetPosition() + ImGui::GetIO().MouseDelta);
					}
				}
			}
		}
	}

	if (ImGui::IsMouseClicked(0))
	{
		if (Hovered != nullptr)
		{
			if (!IsSelected(Hovered) && !ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && !ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL))
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
				std::vector<FEVisualNodeConnection*> ImpactedConnections = GetAllConnections(SocketHovered);
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

	if (ImGui::IsKeyDown(GLFW_KEY_DELETE))
	{
		for (size_t i = 0; i < Selected.size(); i++)
		{
			DeleteNode(Selected[i]);
		}

		Selected.clear();
	}

	static bool WasCopiedToClipboard = false;
	if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL))
	{
		if (ImGui::IsKeyDown(GLFW_KEY_C))
		{
			if (!Selected.empty())
			{
				const FEVisualNodeArea* NewNodeArea = FEVisualNodeArea::CreateNodeArea(Selected);
				APPLICATION.SetClipboardText(NewNodeArea->ToJson());
				delete NewNodeArea;
			}
		}
		else if (ImGui::IsKeyDown(GLFW_KEY_V))
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

				FEVisualNodeArea* NewNodeArea = FEVisualNodeArea::FromJson(NodesToImport);

				// ***************** Place new nodes in center of a view space *****************
				const ImVec2 ViewCenter = GetRenderedViewCenter();
				ImVec2 NodesAABBCenter = NewNodeArea->GetAllNodesAABBCenter();
				NodesAABBCenter -= NewNodeArea->GetAreaRenderOffset();

				NeededShift = ViewCenter - NodesAABBCenter;

				NewNodeArea->RunOnEachNode([](FEVisualNode* Node) {
					Node->SetPosition(Node->GetPosition() + NeededShift);
				});
				// ***************** Place new nodes in center of a view space END *****************
				
				FEVisualNodeArea::CopyNodesTo(NewNodeArea, this);
				
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

	if (!ImGui::IsKeyDown(GLFW_KEY_V))
		WasCopiedToClipboard = false;
}

void FEVisualNodeArea::InputUpdateNode(FEVisualNode* Node)
{
	if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		if (Node->LeftTop.x < MouseCursorPosition.x + MouseCursorSize.x &&
			Node->LeftTop.x + Node->GetSize().x > MouseCursorPosition.x &&
			Node->LeftTop.y < MouseCursorPosition.y + MouseCursorSize.y &&
			Node->GetSize().y + Node->LeftTop.y > MouseCursorPosition.y)
		{
			Hovered = Node;
			Node->SetIsHovered(true);
		}
	}
	else if (Node->GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		if (glm::distance(glm::vec2(Node->LeftTop.x + NODE_RADIUS / 2.0f, Node->LeftTop.y + NODE_RADIUS / 2.0f),
						  glm::vec2(MouseCursorPosition.x, MouseCursorPosition.y)) <= NODE_RADIUS)
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

void FEVisualNodeArea::InputUpdateSocket(FEVisualNodeSocket* Socket)
{
	const ImVec2 SocketPosition = SocketToPosition(Socket);
	if (MouseCursorPosition.x >= SocketPosition.x - NODE_SOCKET_SIZE &&
		MouseCursorPosition.x <= SocketPosition.x + NODE_SOCKET_SIZE &&
		MouseCursorPosition.y >= SocketPosition.y - NODE_SOCKET_SIZE &&
		MouseCursorPosition.y <= SocketPosition.y + NODE_SOCKET_SIZE)
	{
		SocketHovered = Socket;
	}

	if (SocketHovered == Socket && ImGui::GetIO().MouseClicked[0] && !FEVisualNode::IsSocketTypeIn(Socket->GetType()))
		SocketLookingForConnection = Socket;
}

bool FEVisualNodeArea::AddSelected(FEVisualNode* NewNode)
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

bool FEVisualNodeArea::IsSelected(const FEVisualNode* Node) const
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

void FEVisualNodeArea::ClearSelection()
{
	Selected.clear();
}

void FEVisualNodeArea::GetAllNodesAABB(ImVec2& Min, ImVec2& Max) const
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

ImVec2 FEVisualNodeArea::GetAllNodesAABBCenter() const
{
	ImVec2 min, max;
	GetAllNodesAABB(min, max);

	return {min.x + (max.x - min.x) / 2.0f, min.y + (max.y - min.y) / 2.0f};
}

ImVec2 FEVisualNodeArea::GetRenderedViewCenter() const
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

void FEVisualNodeArea::RunOnEachConnectedNode(FEVisualNode* StartNode, void(*Func)(FEVisualNode*))
{
	if (Func == nullptr)
		return;

	static std::unordered_map<FEVisualNode*, bool> SeenNodes;
	SeenNodes.clear();
	auto bWasNodeSeen = [](FEVisualNode* Node) {
		if (SeenNodes.find(Node) == SeenNodes.end())
		{
			SeenNodes[Node] = true;
			return false;
		}

		return true;
	};
	
	std::vector<FEVisualNode*> CurrentNodes;
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

			std::vector<FEVisualNode*> NewNodes = CurrentNodes[i]->GetConnectedNodes();
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

bool FEVisualNodeArea::TryToConnect(const FEVisualNode* OutNode, const std::string OutSocketID, const FEVisualNode* InNode, const std::string InSocketID)
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

FEVisualNodeArea* FEVisualNodeArea::CreateNodeArea(const std::vector<FEVisualNode*> Nodes)
{
	FEVisualNodeArea* NewArea = new FEVisualNodeArea();

	// Copy all nodes to new node area.
	std::unordered_map<FEVisualNode*, FEVisualNode*> OldToNewNode;
	std::unordered_map<FEVisualNodeSocket*, FEVisualNodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		FEVisualNode* CopyOfNode = FEVisualNode::CopyChild(Nodes[i]->GetType(), Nodes[i]);
		if (CopyOfNode == nullptr)
			CopyOfNode = new FEVisualNode(*Nodes[i]);
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
						NewArea->Connections.push_back(new FEVisualNodeConnection(OldToNewSocket[Nodes[i]->Input[j]->Connections[k]], OldToNewSocket[Nodes[i]->Input[j]]));
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
						NewArea->Connections.push_back(new FEVisualNodeConnection(OldToNewSocket[Nodes[i]->Output[j]], OldToNewSocket[Nodes[i]->Output[j]->Connections[k]]));
				}
			}
		}
	}

	return NewArea;
}

std::string FEVisualNodeArea::ToJson() const
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

FEVisualNodeArea* FEVisualNodeArea::FromJson(std::string JsonText)
{
	FEVisualNodeArea* NewArea = new FEVisualNodeArea();

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

	std::unordered_map<std::string, FEVisualNode*> LoadedNodes;
	std::vector<Json::String> NodesList = root["nodes"].getMemberNames();
	for (size_t i = 0; i < NodesList.size(); i++)
	{
		std::string NodeType = root["nodes"][std::to_string(i)]["nodeType"].asCString();
		FEVisualNode* NewNode = FEVisualNode::ConstructChild(NodeType, root["nodes"][std::to_string(i)]);

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

void FEVisualNodeArea::CopyNodesTo(FEVisualNodeArea* SourceNodeArea, FEVisualNodeArea* TargetNodeArea)
{
	const size_t NodeShift = TargetNodeArea->Nodes.size();

	// Copy all nodes to new node area.
	std::unordered_map<FEVisualNode*, FEVisualNode*> OldToNewNode;
	std::unordered_map<FEVisualNodeSocket*, FEVisualNodeSocket*> OldToNewSocket;
	for (size_t i = 0; i < SourceNodeArea->Nodes.size(); i++)
	{
		FEVisualNode* CopyOfNode = FEVisualNode::CopyChild(SourceNodeArea->Nodes[i]->GetType(), SourceNodeArea->Nodes[i]);
		if (CopyOfNode == nullptr)
			CopyOfNode = new FEVisualNode(*SourceNodeArea->Nodes[i]);
		CopyOfNode->ParentArea = SourceNodeArea;

		//targetNodeArea->nodes.push_back(copyOfNode);
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
						TargetNodeArea->Connections.push_back(new FEVisualNodeConnection(OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]->Connections[k]], OldToNewSocket[SourceNodeArea->Nodes[i]->Input[j]]));
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
						TargetNodeArea->Connections.push_back(new FEVisualNodeConnection(OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]], OldToNewSocket[SourceNodeArea->Nodes[i]->Output[j]->Connections[k]]));
				}
			}
		}
	}
}

void FEVisualNodeArea::LoadFromFile(const char* FileName)
{
	std::ifstream NodesFile;
	NodesFile.open(FileName);

	const std::string FileData((std::istreambuf_iterator<char>(NodesFile)), std::istreambuf_iterator<char>());
	NodesFile.close();

	FEVisualNodeArea* NewNodeArea = FEVisualNodeArea::FromJson(FileData);
	FEVisualNodeArea::CopyNodesTo(NewNodeArea, this);
	delete NewNodeArea;
}

std::vector<FEVisualNode*> FEVisualNodeArea::GetNodesByName(const std::string NodeName) const
{
	std::vector<FEVisualNode*> result;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetName() == NodeName)
			result.push_back(Nodes[i]);
	}

	return result;
}

std::vector<FEVisualNode*> FEVisualNodeArea::GetNodesByType(const std::string NodeType) const
{
	std::vector<FEVisualNode*> result;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->GetType() == NodeType)
			result.push_back(Nodes[i]);
	}

	return result;
}

int FEVisualNodeArea::GetNodeCount() const
{
	return static_cast<int>(Nodes.size());
}

bool FEVisualNodeArea::IsMouseHovered() const
{
	return bMouseHovered;
}