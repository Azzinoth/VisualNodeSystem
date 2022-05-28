#include "FEVisualNodeArea.h"

ImVec2 FEVisualNodeArea::neededShift = ImVec2();

FEVisualNodeArea::FEVisualNodeArea() {};

void FEVisualNodeArea::addNode(FEVisualNode* newNode)
{
	if (newNode == nullptr)
		return;

	//if (newNode->parentArea != nullptr)
	//	newNode->setPosition(newNode->getPosition() + newNode->parentArea->getAreaRenderOffset());

	newNode->parentArea = this;
	//newNode->setPosition(newNode->getPosition() - getAreaRenderOffset());

	nodes.push_back(newNode);
}

FEVisualNodeArea::~FEVisualNodeArea() 
{
	clear();
}

bool ColorPicker3U32(const char* label, ImU32* color)
{
	float col[3];
	col[0] = (float)((*color) & 0xFF) / 255.0f;
	col[1] = (float)((*color >> 8) & 0xFF) / 255.0f;
	col[2] = (float)((*color >> 16) & 0xFF) / 255.0f;

	bool result = ImGui::ColorPicker3(label, col);

	*color = ((ImU32)(col[0] * 255.0f)) |
		((ImU32)(col[1] * 255.0f) << 8) |
		((ImU32)(col[2] * 255.0f) << 16);

	return result;
}

void FEVisualNodeArea::renderNode(FEVisualNode* node)
{
	if (currentDrawList == nullptr || node == nullptr)
		return;

	ImGui::PushID(node->getID().c_str());

	node->leftTop = ImGui::GetCurrentWindow()->Pos + node->getPosition() + renderOffset;
	if (node->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		node->rightBottom = node->leftTop + node->getSize();
	}
	else if (node->getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		node->rightBottom = node->leftTop + ImVec2(NODE_RADIUS, NODE_RADIUS);
	}

	if (isSelected(node))
	{
		if (node->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
		{
			ImVec2 leftTop = node->leftTop - ImVec2(4.0f, 4.0f);
			ImVec2 rightBottom = node->rightBottom + ImVec2(4.0f, 4.0f);
			ImGui::GetWindowDrawList()->AddRect(leftTop, rightBottom, IM_COL32(175, 255, 175, 255), 16.0f);
		}
		else if (node->getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
		{
			ImGui::GetWindowDrawList()->AddCircle(node->leftTop + ImVec2(NODE_RADIUS / 2.0f, NODE_RADIUS / 2.0f), NODE_RADIUS + 4.0f, IM_COL32(175, 255, 175, 255), 32, 4.0f);
		}
	}

	currentDrawList->ChannelsSetCurrent(2);
	
	if (node->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		ImGui::SetCursorScreenPos(node->leftTop);
	}
	else if (node->getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		ImGui::SetCursorScreenPos(node->leftTop - ImVec2(NODE_RADIUS / 2.0f - NODE_RADIUS / 4.0f, NODE_RADIUS / 2.0f - NODE_RADIUS / 4.0f));
	}
	node->draw();

	currentDrawList->ChannelsSetCurrent(1);
	ImGui::SetCursorScreenPos(node->leftTop);

	// Drawing node background layer.
	ImU32 nodeBackgroundColor = (hovered == node || isSelected(node)) ? IM_COL32(75, 75, 75, 125) : IM_COL32(60, 60, 60, 125);
	if (node->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		currentDrawList->AddRectFilled(node->leftTop, node->rightBottom, nodeBackgroundColor, 8.0f);
	}
	else if (node->getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		ImGui::GetWindowDrawList()->AddCircleFilled(node->leftTop + (node->rightBottom - node->leftTop) / 2.0f,
													NODE_RADIUS,
													nodeBackgroundColor, 32);
	}

	if (node->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		// Drawing caption area.
		ImVec2 titleArea = node->rightBottom;
		titleArea.y = node->leftTop.y + NODE_TITLE_HEIGHT;
		ImU32 nodeTitleBackgroundColor = (hovered == node || isSelected(node)) ? node->titleBackgroundColorHovered : node->titleBackgroundColor;

		currentDrawList->AddRectFilled(node->leftTop + ImVec2(1, 1), titleArea, nodeTitleBackgroundColor, 8.0f);
		currentDrawList->AddRect(node->leftTop, node->rightBottom, ImColor(100, 100, 100), 8.0f);

		ImVec2 textSize = ImGui::CalcTextSize(node->getName().c_str());
		ImVec2 textPosition;
		textPosition.x = node->leftTop.x + (node->getSize().x / 2) - textSize.x / 2;
		textPosition.y = node->leftTop.y + (NODE_TITLE_HEIGHT / 2) - textSize.y / 2;

		ImGui::SetCursorScreenPos(textPosition);
		ImGui::Text(node->getName().c_str());
	}
	else if (node->getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		currentDrawList->AddCircle(node->leftTop + ImVec2(NODE_RADIUS / 2.0f, NODE_RADIUS / 2.0f), NODE_RADIUS + 2.0f, ImColor(100, 100, 100), 32, 2.0f);
	}

	renderNodeSockets(node);

	ImGui::PopID();
}

void FEVisualNodeArea::renderNodeSockets(FEVisualNode* node)
{
	for (size_t i = 0; i < node->input.size(); i++)
	{
		renderNodeSocket(node->input[i]);
	}

	for (size_t i = 0; i < node->output.size(); i++)
	{
		renderNodeSocket(node->output[i]);
	}
}

void FEVisualNodeArea::renderNodeSocket(FEVisualNodeSocket* socket)
{
	ImVec2 socketPosition = socketToPosition(socket);
	if (socket->getParent()->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		bool input = FEVisualNode::isSocketTypeIn(socket->getType());
		// Socket description.
		ImVec2 textSize = ImGui::CalcTextSize(socket->getName().c_str());

		float textX = socketPosition.x;
		textX += input ? NODE_SOCKET_SIZE * 2.0f : -NODE_SOCKET_SIZE * 2.0f - textSize.x;

		ImGui::SetCursorScreenPos(ImVec2(textX, socketPosition.y - textSize.y / 2.0f));
		ImGui::Text(socket->getName().c_str());
	}

	ImColor socketColor = ImColor(150, 150, 150);
	if (socketHovered == socket)
	{
		socketColor = ImColor(200, 200, 200);
		// If potential connection can't be established we will provide visual indication.
		if (socketLookingForConnection != nullptr)
		{
			char** msg = new char*;
			*msg = nullptr;
			socketColor = socketHovered->getParent()->canConnect(socketHovered, socketLookingForConnection, msg) ?
																				ImColor(50, 200, 50) : ImColor(200, 50, 50);

			if (*msg != nullptr)
			{
				ImGui::Begin("socket connection info", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
				ImGui::Text(*msg);
				ImGui::End();

				delete msg;
			}
		}
	}

	if (socketLookingForConnection == socket)
	{
		currentDrawList->ChannelsSetCurrent(3);
		drawHermiteLine(socketPosition, ImGui::GetIO().MousePos, 12, ImColor(200, 200, 200));
	}

	// Draw socket icon.
	currentDrawList->AddCircleFilled(socketPosition, NODE_SOCKET_SIZE, socketColor);
}

void FEVisualNodeArea::renderGrid(ImVec2 currentPosition)
{
	currentDrawList->ChannelsSplit(2);

	currentPosition.x += renderOffset.x;
	currentPosition.y += renderOffset.y;
	
	// Horizontal lines
	int startingStep = int(ceil(-10000.0f / FE_VISUAL_NODE_GRID_STEP));
	int stepCount = int(ceil(10000.0f / FE_VISUAL_NODE_GRID_STEP));
	for (int i = startingStep; i < stepCount; i++)
	{
		ImVec2 from = ImVec2(currentPosition.x - 10000.0f , currentPosition.y + i * FE_VISUAL_NODE_GRID_STEP);
		ImVec2 to = ImVec2(currentPosition.x + 10000.0f, currentPosition.y + i * FE_VISUAL_NODE_GRID_STEP);

		if (i % 10 != 0)
		{
			currentDrawList->ChannelsSetCurrent(1);
			currentDrawList->AddLine(from, to, ImGui::GetColorU32(gridLinesColor), 1);
		}
		else
		{
			currentDrawList->ChannelsSetCurrent(0);
			currentDrawList->AddLine(from, to, ImGui::GetColorU32(gridBoldLinesColor), 3);
		}
	}

	// Vertical lines
	for (int i = startingStep; i < stepCount; i++)
	{
		ImVec2 from = ImVec2(currentPosition.x + i * FE_VISUAL_NODE_GRID_STEP, currentPosition.y - 10000.0f);
		ImVec2 to = ImVec2(currentPosition.x + i * FE_VISUAL_NODE_GRID_STEP, currentPosition.y + 10000.0f);

		if (i % 10 != 0)
		{
			currentDrawList->ChannelsSetCurrent(1);
			currentDrawList->AddLine(from, to, ImGui::GetColorU32(gridLinesColor), 1);
		}
		else
		{
			currentDrawList->ChannelsSetCurrent(0);
			currentDrawList->AddLine(from, to, ImGui::GetColorU32(gridBoldLinesColor), 3);
		}
	}

	currentDrawList->ChannelsMerge();
}

void FEVisualNodeArea::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, gridBackgroundColor);

	ImVec2 currentPosition = ImGui::GetCurrentWindow()->Pos + areaPosition;
	ImGui::SetNextWindowPos(currentPosition);
	ImGui::BeginChild("Nodes area", getAreaSize(), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	nodeAreaWindow = ImGui::GetCurrentWindow();
	currentDrawList = ImGui::GetWindowDrawList();

	renderGrid(currentPosition);

	// 0 - connections.
	// 1 - main node rect.
	// 2 - for custom node draw.
	// 3 - for line that represent new connection.
	currentDrawList->ChannelsSplit(4);

	currentDrawList->ChannelsSetCurrent(1);
	for (int i = 0; i < int(nodes.size()); i++)
	{
		renderNode(nodes[i]);
	}

	// Connection should be on node's top layer.
	// But with my current realization it would be better to call it after renderNode.
	currentDrawList->ChannelsSetCurrent(0);
	for (size_t i = 0; i < connections.size(); i++)
	{
		renderConnection(connections[i]);
	}

	// ************************* RENDER CONTEXT MENU *************************
	if (openMainContextMenu && mainContextMenuFunc != nullptr)
	{
		openMainContextMenu = false;
		ImGui::OpenPopup("##main_context_menu");
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginPopup("##main_context_menu"))
	{
		if (mainContextMenuFunc != nullptr)
			mainContextMenuFunc();

		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
	// ************************* RENDER CONTEXT MENU END *************************

	currentDrawList->ChannelsMerge();
	currentDrawList = nullptr;

	// Draw mouse selection region.
	if (mouseSelectRegionMin.x != FLT_MAX && mouseSelectRegionMin.y != FLT_MAX &&
		mouseSelectRegionMax.x != FLT_MAX && mouseSelectRegionMax.y != FLT_MAX)
	{
		ImGui::GetWindowDrawList()->AddRectFilled(mouseSelectRegionMin, mouseSelectRegionMax, IM_COL32(175, 175, 255, 125), 1.0f);
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

ImVec2 FEVisualNodeArea::getAreaSize()
{
	return areaSize;
}

void FEVisualNodeArea::setAreaSize(ImVec2 newValue)
{
	if (newValue.x < 1 || newValue.y < 1)
		return;

	areaSize = newValue;
}

ImVec2 FEVisualNodeArea::getAreaPosition()
{
	return areaPosition;
}

void FEVisualNodeArea::setAreaPosition(ImVec2 newValue)
{
	if (newValue.x < 0 || newValue.y < 0)
		return;

	areaPosition = newValue;
}

void FEVisualNodeArea::drawHermiteLine(ImVec2 p1, ImVec2 p2, int steps, ImColor color)
{
	ImVec2 t1 = ImVec2(80.0f, 0.0f);
	ImVec2 t2 = ImVec2(80.0f, 0.0f);

	for (int step = 0; step <= steps; step++)
	{
		float t = (float)step / (float)steps;
		float h1 = +2 * t * t * t - 3 * t * t + 1.0f;
		float h2 = -2 * t * t * t + 3 * t * t;
		float h3 = t * t * t - 2 * t * t + t;
		float h4 = t * t * t - t * t;
		currentDrawList->PathLineTo(ImVec2(h1 * p1.x + h2 * p2.x + h3 * t1.x + h4 * t2.x, h1 * p1.y + h2 * p2.y + h3 * t1.y + h4 * t2.y));
	}

	currentDrawList->PathStroke(color, false, 3.0f);
}

void FEVisualNodeArea::renderConnection(FEVisualNodeConnection* connection)
{
	if (connection->out == nullptr || connection->in == nullptr)
		return;

	ImColor connectionColor = ImColor(200, 200, 200);
	if (connection->out->forceColor != nullptr)
		connectionColor = *connection->out->forceColor;

	drawHermiteLine(socketToPosition(connection->out), socketToPosition(connection->in), 12, connectionColor);
}

ImVec2 FEVisualNodeArea::socketToPosition(FEVisualNodeSocket* socket)
{
	bool input = FEVisualNode::isSocketTypeIn(socket->type);
	float socketX = 0.0f;
	float socketY = 0.0f;

	int socketIndex = -1;
	if (input)
	{
		for (size_t i = 0; i < socket->parent->input.size(); i++)
		{
			if (socket->parent->input[i] == socket)
			{
				socketIndex = int(i);
				break;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < socket->parent->output.size(); i++)
		{
			if (socket->parent->output[i] == socket)
			{
				socketIndex = int(i);
				break;
			}
		}
	}

	if (socket->getParent()->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		socketX = input ? socket->parent->leftTop.x + NODE_SOCKET_SIZE * 3 : socket->parent->rightBottom.x - NODE_SOCKET_SIZE * 3;

		float heightForSockets = socket->parent->getSize().y - NODE_TITLE_HEIGHT;
		float socketSpacing = heightForSockets / (input ? socket->parent->input.size() : socket->parent->output.size());

		socketY = socket->parent->leftTop.y + NODE_TITLE_HEIGHT + socketSpacing * (socketIndex + 1) - socketSpacing / 2.0f;
	}
	else if (socket->getParent()->getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		size_t socketCount = input ? socket->parent->input.size() : socket->parent->output.size();
		float beginAngle = (180.0f / float(socketCount) / 2.0f);
		if (input)
			beginAngle = -beginAngle;

		float step = (180.0f / float(socketCount) * (socketIndex));
		if (input)
			step = -step;

		float angle = beginAngle + step;

		float nodeCenterX = socket->parent->leftTop.x + NODE_RADIUS / 2.0f;
		float nodeCenterY = socket->parent->leftTop.y + NODE_RADIUS / 2.0f;

		socketX = nodeCenterX + NODE_RADIUS * 0.95f * sin(glm::radians(angle));
		socketY = nodeCenterY + NODE_RADIUS * 0.95f * cos(glm::radians(angle));
	}

	return ImVec2(socketX, socketY);
}

void FEVisualNodeArea::update()
{
	inputUpdate();

	for (int i = 0; i < int(nodes.size()); i++)
	{
		if (nodes[i]->shouldBeDestroyed)
		{
			deleteNode(nodes[i]);
			i--;
			break;
		}
	}

	render();
}

std::vector<FEVisualNodeConnection*> FEVisualNodeArea::getAllConnections(FEVisualNodeSocket* socket)
{
	std::vector<FEVisualNodeConnection*> result;

	for (size_t i = 0; i < connections.size(); i++)
	{
		if (connections[i]->in == socket || connections[i]->out == socket)
			result.push_back(connections[i]);
	}

	return result;
}

void FEVisualNodeArea::disconnect(FEVisualNodeConnection*& connection)
{
	for (int i = 0; i < int(connection->in->connections.size()); i++)
	{
		if (connection->in->connections[i] == connection->out)
		{
			FEVisualNode* parent = connection->in->connections[i]->parent;
			if (!clearing)
				propagateNodeEventsCallbacks(parent, FE_VISUAL_NODE_BEFORE_DISCONNECTED);

			connection->in->connections.erase(connection->in->connections.begin() + i, connection->in->connections.begin() + i + 1);
			connection->in->parent->socketEvent(connection->in, connection->out, clearing ? FE_VISUAL_NODE_SOCKET_DESTRUCTION : FE_VISUAL_NODE_SOCKET_DISCONNECTED);
			i--;

			if (!clearing)
				propagateNodeEventsCallbacks(parent, FE_VISUAL_NODE_AFTER_DISCONNECTED);
		}
	}

	for (int i = 0; i < int(connection->out->connections.size()); i++)
	{
		if (connection->out->connections[i] == connection->in)
		{
			FEVisualNode* parent = connection->out->connections[i]->parent;
			if (!clearing)
				propagateNodeEventsCallbacks(parent, FE_VISUAL_NODE_BEFORE_DISCONNECTED);

			connection->out->connections.erase(connection->out->connections.begin() + i, connection->out->connections.begin() + i + 1);
			i--;

			if (!clearing)
				propagateNodeEventsCallbacks(parent, FE_VISUAL_NODE_AFTER_DISCONNECTED);
		}
	}

	for (size_t i = 0; i < connections.size(); i++)
	{
		if (connections[i] == connection)
		{
			connections.erase(connections.begin() + i, connections.begin() + i + 1);
			delete connection;
			connection = nullptr;
			return;
		}
	}
}

void FEVisualNodeArea::deleteNode(FEVisualNode* node)
{
	if (!node->couldBeDestroyed)
		return;

	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (nodes[i] == node)
		{
			propagateNodeEventsCallbacks(nodes[i], FE_VISUAL_NODE_REMOVED);

			for (size_t j = 0; j < nodes[i]->input.size(); j++)
			{
				auto connections = getAllConnections(nodes[i]->input[j]);
				for (size_t p = 0; p < connections.size(); p++)
				{
					disconnect(connections[p]);
				}
			}

			for (size_t j = 0; j < nodes[i]->output.size(); j++)
			{
				auto connections = getAllConnections(nodes[i]->output[j]);
				for (size_t p = 0; p < connections.size(); p++)
				{
					disconnect(connections[p]);
				}
			}

			delete nodes[i];
			nodes.erase(nodes.begin() + i, nodes.begin() + i + 1);

			break;
		}
	}
}

void FEVisualNodeArea::setMainContextMenuFunc(void(*func)())
{
	mainContextMenuFunc = func;
}

void FEVisualNodeArea::clear()
{
	clearing = true;

	for (int i = 0; i < int(nodes.size()); i++)
	{
		propagateNodeEventsCallbacks(nodes[i], FE_VISUAL_NODE_DESTROYED);
		nodes[i]->couldBeDestroyed = true;
		deleteNode(nodes[i]);
		i--;
	}

	renderOffset = ImVec2(0, 0);
	nodeAreaWindow = nullptr;
	selected.clear();
	socketLookingForConnection = nullptr;
	socketHovered = nullptr;

	mouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	mouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);

	clearing = false;
}

void FEVisualNodeArea::reset()
{
	clear();

	mainContextMenuFunc = nullptr;
	nodeEventsCallbacks.clear();
}

void FEVisualNodeArea::propagateUpdateToConnectedNodes(FEVisualNode* callerNode)
{
	if (callerNode == nullptr)
		return;

	for (size_t i = 0; i < callerNode->input.size(); i++)
	{
		auto connections = getAllConnections(callerNode->input[i]);
		for (size_t j = 0; j < connections.size(); j++)
		{
			connections[j]->in->getParent()->socketEvent(connections[j]->in, connections[j]->out, FE_VISUAL_NODE_SOCKET_UPDATE);
		}
	}

	for (size_t i = 0; i < callerNode->output.size(); i++)
	{
		auto connections = getAllConnections(callerNode->output[i]);
		for (size_t j = 0; j < connections.size(); j++)
		{
			connections[j]->in->getParent()->socketEvent(connections[j]->in, connections[j]->out, FE_VISUAL_NODE_SOCKET_UPDATE);
		}
	}
}

ImVec2 FEVisualNodeArea::getAreaRenderOffset()
{
	return renderOffset;
}

void FEVisualNodeArea::setAreaRenderOffset(ImVec2 offset)
{
	if (offset.x <= -10000.0f || offset.x >= 10000.0f ||
		offset.y <= -10000.0f || offset.y >= 10000.0f)
		return;

	renderOffset = offset;
}

bool FEVisualNodeArea::tryToConnect(FEVisualNode* outNode, size_t outNodeSocketIndex, FEVisualNode* inNode, size_t inNodeSocketIndex)
{
	if (outNode->output.size() <= outNodeSocketIndex)
		return false;

	if (inNode->input.size() <= inNodeSocketIndex)
		return false;

	FEVisualNodeSocket* outSocket = outNode->output[outNodeSocketIndex];
	FEVisualNodeSocket* inSocket = inNode->input[inNodeSocketIndex];

	char* msg = nullptr;
	bool result = inSocket->getParent()->canConnect(inSocket, outSocket, &msg);

	if (result)
	{
		propagateNodeEventsCallbacks(outSocket->getParent(), FE_VISUAL_NODE_BEFORE_CONNECTED);
		propagateNodeEventsCallbacks(inSocket->getParent(), FE_VISUAL_NODE_BEFORE_CONNECTED);

		outSocket->connections.push_back(inSocket);
		inSocket->connections.push_back(outSocket);

		connections.push_back(new FEVisualNodeConnection(outSocket, inSocket));

		outSocket->getParent()->socketEvent(outSocket, inSocket, FE_VISUAL_NODE_SOCKET_CONNECTED);
		inSocket->getParent()->socketEvent(inSocket, outSocket, FE_VISUAL_NODE_SOCKET_CONNECTED);

		propagateNodeEventsCallbacks(outSocket->getParent(), FE_VISUAL_NODE_AFTER_CONNECTED);
		propagateNodeEventsCallbacks(inSocket->getParent(), FE_VISUAL_NODE_AFTER_CONNECTED);
	}

	return result;
}

void FEVisualNodeArea::setNodeEventCallback(void(*func)(FEVisualNode*, FE_VISUAL_NODE_EVENT))
{
	if (func != nullptr)
		nodeEventsCallbacks.push_back(func);
}

void FEVisualNodeArea::propagateNodeEventsCallbacks(FEVisualNode* node, FE_VISUAL_NODE_EVENT eventToPropagate)
{
	for (size_t i = 0; i < nodeEventsCallbacks.size(); i++)
	{
		if (nodeEventsCallbacks[i] != nullptr)
			nodeEventsCallbacks[i](node, eventToPropagate);
	}
}

void FEVisualNodeArea::saveToFile(const char* fileName)
{
	const std::string json_file = toJson();
	std::ofstream saveFile;
	saveFile.open(fileName);
	saveFile << json_file;
	saveFile.close();
}

bool FEVisualNodeArea::isNodeIDInList(std::string ID, std::vector<FEVisualNode*> list)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list[i]->getID() == ID)
			return true;
	}

	return false;
}

void FEVisualNodeArea::saveNodesToFile(const char* fileName, std::vector<FEVisualNode*> nodes)
{
	if (nodes.size() == 0)
		return;

	FEVisualNodeArea* newNodeArea = FEVisualNodeArea::createNodeArea(nodes);
	const std::string json_file = newNodeArea->toJson();
	std::ofstream saveFile;
	saveFile.open(fileName);
	saveFile << json_file;
	saveFile.close();
	delete newNodeArea;
}

void FEVisualNodeArea::runOnEachNode(void(*func)(FEVisualNode*))
{
	if (func != nullptr)
		std::for_each(nodes.begin(), nodes.end(), func);
}

FEVisualNode* FEVisualNodeArea::getHovered()
{
	return hovered;
}

std::vector<FEVisualNode*> FEVisualNodeArea::getSelected()
{
	return selected;
}

void FEVisualNodeArea::inputUpdate()
{
	mouseCursorPosition = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	hovered = nullptr;
	socketHovered = nullptr;
	openMainContextMenu = false;

	mouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	mouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);

	if (ImGui::GetCurrentContext()->HoveredWindow != nodeAreaWindow)
	{
		mouseHovered = false;
		return;
	}
	mouseHovered = true;

	if (ImGui::GetIO().MouseClicked[0])
		mouseDownIn = nodeAreaWindow;

	if (ImGui::GetIO().MouseReleased[0])
		mouseDownIn = nullptr;

	for (size_t i = 0; i < nodes.size(); i++)
		nodes[i]->setIsHovered(false);

	for (size_t i = 0; i < nodes.size(); i++)
	{
		inputUpdateNode(nodes[i]);
		if (hovered != nullptr)
			break;
	}

	if (ImGui::IsMouseDragging(0) && mouseDownIn == nodeAreaWindow)
	{
		if (ImGui::IsKeyDown(GLFW_KEY_LEFT_SHIFT) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_SHIFT))
		{
			socketLookingForConnection = nullptr;

			if (abs(ImGui::GetMouseDragDelta(0).x) > 1 || abs(ImGui::GetMouseDragDelta(0).y) > 1)
			{
				mouseSelectRegionMin = ImGui::GetIO().MouseClickedPos[0];
				mouseSelectRegionMax = mouseSelectRegionMin + ImGui::GetMouseDragDelta(0);

				if (mouseSelectRegionMax.x < mouseSelectRegionMin.x)
					std::swap(mouseSelectRegionMin.x, mouseSelectRegionMax.x);

				if (mouseSelectRegionMax.y < mouseSelectRegionMin.y)
					std::swap(mouseSelectRegionMin.y, mouseSelectRegionMax.y);
			}
		}

		if (mouseSelectRegionMin.x != FLT_MAX && mouseSelectRegionMin.y != FLT_MAX &&
			mouseSelectRegionMax.x != FLT_MAX && mouseSelectRegionMax.y != FLT_MAX)
		{
			selected.clear();
			ImVec2 regionSize = mouseSelectRegionMax - mouseSelectRegionMin;

			for (size_t i = 0; i < nodes.size(); i++)
			{
				if (nodes[i]->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
				{
					if (nodes[i]->leftTop.x < mouseSelectRegionMin.x + regionSize.x &&
						nodes[i]->leftTop.x + nodes[i]->getSize().x > mouseSelectRegionMin.x &&
						nodes[i]->leftTop.y < mouseSelectRegionMin.y + regionSize.y &&
						nodes[i]->getSize().y + nodes[i]->leftTop.y > mouseSelectRegionMin.y)
					{
						addSelected(nodes[i]);
					}
				}
				else if (nodes[i]->getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
				{
					if (nodes[i]->leftTop.x < mouseSelectRegionMin.x + regionSize.x &&
						nodes[i]->leftTop.x + NODE_RADIUS > mouseSelectRegionMin.x &&
						nodes[i]->leftTop.y < mouseSelectRegionMin.y + regionSize.y &&
						NODE_RADIUS + nodes[i]->leftTop.y > mouseSelectRegionMin.y)
					{
						addSelected(nodes[i]);
					}
				}
			}
		}
		else
		{
			if (socketLookingForConnection == nullptr)
			{
				if (selected.size() == 0 && socketHovered == nullptr)
				{
					renderOffset.x += ImGui::GetIO().MouseDelta.x;
					renderOffset.y += ImGui::GetIO().MouseDelta.y;

					if (renderOffset.x > 10000.0f)
						renderOffset.x = 10000.0f;

					if (renderOffset.x < -10000.0f)
						renderOffset.x = -10000.0f;

					if (renderOffset.y > 10000.0f)
						renderOffset.y = 10000.0f;

					if (renderOffset.y < -10000.0f)
						renderOffset.y = -10000.0f;
				}
				else if (socketHovered == nullptr)
				{
					for (size_t i = 0; i < selected.size(); i++)
					{
						selected[i]->setPosition(selected[i]->getPosition() + ImGui::GetIO().MouseDelta);
					}
				}
			}
		}
	}

	if (ImGui::IsMouseClicked(0))
	{
		if (hovered != nullptr)
		{
			if (!isSelected(hovered) && !ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) && !ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL))
				selected.clear();
			addSelected(hovered);
		}
		else
		{
			selected.clear();
		}
	}

	if (ImGui::IsMouseClicked(1))
	{
		if (hovered != nullptr)
		{
			// Should we disconnect sockets
			if (socketHovered != nullptr && socketHovered->connections.size() > 0)
			{
				std::vector<FEVisualNodeConnection*> impactedConnections = getAllConnections(socketHovered);
				for (size_t i = 0; i < impactedConnections.size(); i++)
				{
					disconnect(impactedConnections[i]);
				}
			}
			else
			{
				if (selected.size() <= 1)
				{
					if (!hovered->openContextMenu())
					{
						openMainContextMenu = true;
					}
				}
				else
				{
					if (mainContextMenuFunc != nullptr)
					{
						openMainContextMenu = true;
					}
					else
					{
						hovered->openContextMenu();
					}
				}

				// If hovered node was already selected do nothing
				if (addSelected(hovered))
				{
					// But if it was not selected before deselect all other nodes.
					selected.clear();
					addSelected(hovered);
				}
			}
		}
		else
		{
			selected.clear();
			openMainContextMenu = true;
		}
	}
	
	// Should we connect two sockets ?
	if (socketLookingForConnection != nullptr &&
		socketHovered != nullptr &&
		ImGui::GetIO().MouseReleased[0])
	{
		size_t outSocketIndex = 0;
		for (size_t i = 0; i < socketLookingForConnection->getParent()->output.size(); i++)
		{
			if (socketLookingForConnection->getParent()->output[i] == socketLookingForConnection)
			{
				outSocketIndex = i;
				break;
			}
		}

		size_t inSocketIndex = 0;
		for (size_t i = 0; i < socketHovered->getParent()->input.size(); i++)
		{
			if (socketHovered->getParent()->input[i] == socketHovered)
			{
				inSocketIndex = i;
				break;
			}
		}

		tryToConnect(socketLookingForConnection->getParent(), outSocketIndex, socketHovered->getParent(), inSocketIndex);
		socketLookingForConnection = nullptr;
	}
	else if (ImGui::GetIO().MouseReleased[0] || selected.size() > 1)
	{
		socketLookingForConnection = nullptr;
	}

	if (ImGui::IsKeyDown(GLFW_KEY_DELETE))
	{
		for (size_t i = 0; i < selected.size(); i++)
		{
			deleteNode(selected[i]);
		}

		selected.clear();
	}

	static bool wasCopiedToClipboard = false;
	if (ImGui::IsKeyDown(GLFW_KEY_LEFT_CONTROL) || ImGui::IsKeyDown(GLFW_KEY_RIGHT_CONTROL))
	{
		if (ImGui::IsKeyDown(GLFW_KEY_C))
		{
			if (selected.size() > 0)
			{
				FEVisualNodeArea* newNodeArea = FEVisualNodeArea::createNodeArea(selected);
				APPLICATION.setClipboardText(newNodeArea->toJson());
				delete newNodeArea;
			}
		}
		else if (ImGui::IsKeyDown(GLFW_KEY_V))
		{
			if (!wasCopiedToClipboard)
			{
				wasCopiedToClipboard = true;

				std::string nodesToImport = APPLICATION.getClipboardText();
				Json::Value data;

				JSONCPP_STRING err;
				Json::CharReaderBuilder builder;

				const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
				if (!reader->parse(nodesToImport.c_str(), nodesToImport.c_str() + nodesToImport.size(), &data, &err))
					return;

				FEVisualNodeArea* newNodeArea = FEVisualNodeArea::fromJson(nodesToImport);

				// ***************** Place new nodes in center of a view space *****************
				ImVec2 viewCenter = getRenderedViewCenter();
				ImVec2 nodesAABBCenter = newNodeArea->getAllNodesAABBCenter();
				nodesAABBCenter -= newNodeArea->getAreaRenderOffset();

				neededShift = viewCenter - nodesAABBCenter;

				newNodeArea->runOnEachNode([](FEVisualNode* node) {
					size_t outSocketCount = node->outSocketCount();
					node->setPosition(node->getPosition() + neededShift);
				});
				// ***************** Place new nodes in center of a view space END *****************
				
				FEVisualNodeArea::copyNodesTo(newNodeArea, this);
				
				// Select pasted nodes.
				selected.clear();
				for (size_t i = nodes.size() - newNodeArea->nodes.size(); i < nodes.size(); i++)
				{
					selected.push_back(nodes[i]);
				}

				delete newNodeArea;
			}
		}
	}

	if (!ImGui::IsKeyDown(GLFW_KEY_V))
		wasCopiedToClipboard = false;
}

void FEVisualNodeArea::inputUpdateNode(FEVisualNode* node)
{
	if (node->getStyle() == FE_VISUAL_NODE_STYLE_DEFAULT)
	{
		if (node->leftTop.x < mouseCursorPosition.x + mouseCursorSize.x &&
			node->leftTop.x + node->getSize().x > mouseCursorPosition.x &&
			node->leftTop.y < mouseCursorPosition.y + mouseCursorSize.y &&
			node->getSize().y + node->leftTop.y > mouseCursorPosition.y)
		{
			hovered = node;
			node->setIsHovered(true);
		}
	}
	else if (node->getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
	{
		if (glm::distance(glm::vec2(node->leftTop.x + NODE_RADIUS / 2.0f, node->leftTop.y + NODE_RADIUS / 2.0f),
						  glm::vec2(mouseCursorPosition.x, mouseCursorPosition.y)) <= NODE_RADIUS)
		{
			hovered = node;
			node->setIsHovered(true);
		}
	}

	if (socketHovered == nullptr)
	{
		for (size_t i = 0; i < node->input.size(); i++)
		{
			inputUpdateSocket(node->input[i]);
		}

		for (size_t i = 0; i < node->output.size(); i++)
		{
			inputUpdateSocket(node->output[i]);
		}
	}
}

void FEVisualNodeArea::inputUpdateSocket(FEVisualNodeSocket* socket)
{
	ImVec2 socketPosition = socketToPosition(socket);
	if (mouseCursorPosition.x >= socketPosition.x - NODE_SOCKET_SIZE &&
		mouseCursorPosition.x <= socketPosition.x + NODE_SOCKET_SIZE &&
		mouseCursorPosition.y >= socketPosition.y - NODE_SOCKET_SIZE &&
		mouseCursorPosition.y <= socketPosition.y + NODE_SOCKET_SIZE)
	{
		socketHovered = socket;
	}

	if (socketHovered == socket && ImGui::GetIO().MouseClicked[0] && !FEVisualNode::isSocketTypeIn(socket->getType()))
		socketLookingForConnection = socket;
}

bool FEVisualNodeArea::addSelected(FEVisualNode* newNode)
{
	if (newNode == nullptr)
		return false;

	for (size_t i = 0; i < selected.size(); i++)
	{
		if (selected[i] == newNode)
			return false;
	}

	selected.push_back(newNode);
	return true;
}

bool FEVisualNodeArea::isSelected(FEVisualNode* node)
{
	if (node == nullptr)
		return false;

	for (size_t i = 0; i < selected.size(); i++)
	{
		if (selected[i] == node)
			return true;
	}

	return false;
}

void FEVisualNodeArea::clearSelection()
{
	selected.clear();
}

void FEVisualNodeArea::getAllNodesAABB(ImVec2& min, ImVec2& max)
{
	min.x = FLT_MAX;
	min.y = FLT_MAX;

	max.x = -FLT_MAX;
	max.y = -FLT_MAX;

	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->getPosition().x + renderOffset.x < min.x)
			min.x = nodes[i]->getPosition().x + renderOffset.x;

		if (nodes[i]->getPosition().x + renderOffset.x + nodes[i]->getSize().x > max.x)
			max.x = nodes[i]->getPosition().x + renderOffset.x + nodes[i]->getSize().x;

		if (nodes[i]->getPosition().y + renderOffset.y < min.y)
			min.y = nodes[i]->getPosition().y + renderOffset.y;

		if (nodes[i]->getPosition().y + renderOffset.y + nodes[i]->getSize().y > max.y)
			max.y = nodes[i]->getPosition().y + renderOffset.y + nodes[i]->getSize().y;
	}
}

ImVec2 FEVisualNodeArea::getAllNodesAABBCenter()
{
	ImVec2 min, max;
	getAllNodesAABB(min, max);

	return ImVec2(min.x + (max.x - min.x) / 2.0f, min.y + (max.y - min.y) / 2.0f);
}

ImVec2 FEVisualNodeArea::getRenderedViewCenter()
{
	if (nodeAreaWindow != nullptr)
	{
		return nodeAreaWindow->Size / 2.0f - renderOffset;
	}
	else
	{
		return ImGui::GetCurrentWindow()->Size / 2.0f - renderOffset;
	}
}

void FEVisualNodeArea::runOnEachConnectedNode(FEVisualNode* startNode, void(*func)(FEVisualNode*))
{
	if (func == nullptr)
		return;

	static std::unordered_map<FEVisualNode*, bool> seenNodes;
	seenNodes.clear();
	auto wasNodeSeen = [](FEVisualNode* node) {
		if (seenNodes.find(node) == seenNodes.end())
		{
			seenNodes[node] = true;
			return false;
		}

		return true;
	};
	
	std::vector<FEVisualNode*> currentNodes;
	currentNodes.push_back(startNode);
	if (wasNodeSeen(startNode))
		return;
	while (!emptyOrFilledByNulls(currentNodes))
	{
		for (int i = 0; i < int(currentNodes.size()); i++)
		{
			if (currentNodes[i] == nullptr)
			{
				currentNodes.erase(currentNodes.begin() + i);
				i--;
				continue;
			}

			func(currentNodes[i]);

			std::vector<FEVisualNode*> newNodes = currentNodes[i]->getConnectedNodes();
			for (size_t j = 0; j < newNodes.size(); j++)
			{
				currentNodes.push_back(newNodes[j]);
				if (wasNodeSeen(newNodes[j]))
					return;
			}

			currentNodes.erase(currentNodes.begin() + i);
			i--;
		}
	}
}

bool FEVisualNodeArea::tryToConnect(FEVisualNode* outNode, std::string outSocketID, FEVisualNode* inNode, std::string inSocketID)
{
	size_t outSocketIndex = 0;
	for (size_t i = 0; i < outNode->output.size(); i++)
	{
		if (outNode->output[i]->getID() == outSocketID)
		{
			outSocketIndex = i;
			break;
		}
	}

	size_t inSocketIndex = 0;
	for (size_t i = 0; i < inNode->input.size(); i++)
	{
		if (inNode->input[i]->getID() == inSocketID)
		{
			inSocketIndex = i;
			break;
		}
	}

	return tryToConnect(outNode, outSocketIndex, inNode, inSocketIndex);
}

FEVisualNodeArea* FEVisualNodeArea::createNodeArea(std::vector<FEVisualNode*> nodes)
{
	FEVisualNodeArea* newArea = new FEVisualNodeArea();

	// Copy all nodes to new node area.
	std::unordered_map<FEVisualNode*, FEVisualNode*> oldToNewNode;
	std::unordered_map<FEVisualNodeSocket*, FEVisualNodeSocket*> oldToNewSocket;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		FEVisualNode* copyOfNode = FEVisualNode::copyChild(nodes[i]->getType(), nodes[i]);
		if (copyOfNode == nullptr)
			copyOfNode = new FEVisualNode(*nodes[i]);
		copyOfNode->parentArea = newArea;

		//newArea->nodes.push_back(copyOfNode);
		newArea->addNode(copyOfNode);

		// Associate old to new IDs
		oldToNewNode[nodes[i]] = copyOfNode;

		for (size_t j = 0; j < nodes[i]->input.size(); j++)
		{
			oldToNewSocket[nodes[i]->input[j]] = copyOfNode->input[j];
		}

		for (size_t j = 0; j < nodes[i]->output.size(); j++)
		{
			oldToNewSocket[nodes[i]->output[j]] = copyOfNode->output[j];
		}
	}

	// Than we need to recreate all connections.
	for (size_t i = 0; i < nodes.size(); i++)
	{
		for (size_t j = 0; j < nodes[i]->input.size(); j++)
		{
			for (size_t k = 0; k < nodes[i]->input[j]->connections.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (isNodeIDInList(nodes[i]->input[j]->connections[k]->getParent()->getID(), nodes))
				{
					newArea->nodes[i]->input[j]->connections.push_back(oldToNewSocket[nodes[i]->input[j]->connections[k]]);

					// Add connection to node area.
					// Maybe we already establish this connecton.
					bool shouldAdd = true;
					for (size_t l = 0; l < newArea->connections.size(); l++)
					{
						if (newArea->connections[l]->in == oldToNewSocket[nodes[i]->input[j]] &&
							newArea->connections[l]->out == oldToNewSocket[nodes[i]->input[j]->connections[k]])
						{
							shouldAdd = false;
							break;
						}
					}

					if (shouldAdd)
						newArea->connections.push_back(new FEVisualNodeConnection(oldToNewSocket[nodes[i]->input[j]->connections[k]], oldToNewSocket[nodes[i]->input[j]]));
				}
			}
		}

		for (size_t j = 0; j < nodes[i]->output.size(); j++)
		{
			for (size_t k = 0; k < nodes[i]->output[j]->connections.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (isNodeIDInList(nodes[i]->output[j]->connections[k]->getParent()->getID(), nodes))
				{
					newArea->nodes[i]->output[j]->connections.push_back(oldToNewSocket[nodes[i]->output[j]->connections[k]]);

					// Add connection to node area.
					// Maybe we already establish this connecton.
					bool shouldAdd = true;
					for (size_t l = 0; l < newArea->connections.size(); l++)
					{
						if (newArea->connections[l]->in == oldToNewSocket[nodes[i]->output[j]->connections[k]] &&
							newArea->connections[l]->out == oldToNewSocket[nodes[i]->output[j]])
						{
							shouldAdd = false;
							break;
						}
					}

					if (shouldAdd)
						newArea->connections.push_back(new FEVisualNodeConnection(oldToNewSocket[nodes[i]->output[j]], oldToNewSocket[nodes[i]->output[j]->connections[k]]));
				}
			}
		}
	}

	return newArea;
}

std::string FEVisualNodeArea::toJson()
{
	Json::Value root;
	std::ofstream saveFile;

	Json::Value nodesData;
	for (int i = 0; i < int(nodes.size()); i++)
	{
		nodesData[std::to_string(i)] = nodes[i]->toJson();
	}
	root["nodes"] = nodesData;

	Json::Value connectionsData;
	for (int i = 0; i < int(connections.size()); i++)
	{
		connectionsData[std::to_string(i)]["in"]["socket_ID"] = connections[i]->in->getID();
		size_t socket_index = 0;
		for (size_t j = 0; j < connections[i]->in->getParent()->input.size(); j++)
		{
			if (connections[i]->in->getParent()->input[j]->getID() == connections[i]->in->getID())
				socket_index = j;
		}
		connectionsData[std::to_string(i)]["in"]["socket_index"] = socket_index;
		connectionsData[std::to_string(i)]["in"]["node_ID"] = connections[i]->in->getParent()->getID();

		connectionsData[std::to_string(i)]["out"]["socket_ID"] = connections[i]->out->getID();
		socket_index = 0;
		for (size_t j = 0; j < connections[i]->out->getParent()->output.size(); j++)
		{
			if (connections[i]->out->getParent()->output[j]->getID() == connections[i]->out->getID())
				socket_index = j;
		}
		connectionsData[std::to_string(i)]["out"]["socket_index"] = socket_index;
		connectionsData[std::to_string(i)]["out"]["node_ID"] = connections[i]->out->getParent()->getID();
	}
	root["connections"] = connectionsData;

	root["renderOffset"]["x"] = renderOffset.x;
	root["renderOffset"]["y"] = renderOffset.y;

	Json::StreamWriterBuilder builder;
	const std::string jsonText = Json::writeString(builder, root);

	return jsonText;
}

FEVisualNodeArea* FEVisualNodeArea::fromJson(std::string jsonText)
{
	FEVisualNodeArea* newArea = new FEVisualNodeArea();

	if (jsonText.find("{") == std::string::npos || jsonText.find("}") == std::string::npos || jsonText.find(":") == std::string::npos)
		return newArea;

	Json::Value root;
	JSONCPP_STRING err;
	Json::CharReaderBuilder builder;

	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(jsonText.c_str(), jsonText.c_str() + jsonText.size(), &root, &err))
		return newArea;

	if (!root.isMember("nodes"))
		return newArea;

	std::unordered_map<std::string, FEVisualNode*> loadedNodes;
	std::vector<Json::String> nodesList = root["nodes"].getMemberNames();
	for (size_t i = 0; i < nodesList.size(); i++)
	{
		std::string nodeType = root["nodes"][std::to_string(i)]["nodeType"].asCString();
		FEVisualNode* newNode = FEVisualNode::constructChild(nodeType, root["nodes"][std::to_string(i)]);

		if (newNode != nullptr)
		{
			loadedNodes[newNode->getID()] = newNode;
			newArea->addNode(newNode);
		}
	}

	std::vector<Json::String> connectionsList = root["connections"].getMemberNames();
	for (size_t i = 0; i < connectionsList.size(); i++)
	{
		std::string inSocketID = root["connections"][connectionsList[i]]["in"]["socket_ID"].asCString();
		std::string inNodeID = root["connections"][connectionsList[i]]["in"]["node_ID"].asCString();

		std::string outSocketID = root["connections"][connectionsList[i]]["out"]["socket_ID"].asCString();
		std::string outNodeID = root["connections"][connectionsList[i]]["out"]["node_ID"].asCString();

		if (loadedNodes.find(outNodeID) != loadedNodes.end() && loadedNodes.find(inNodeID) != loadedNodes.end())
			newArea->tryToConnect(loadedNodes[outNodeID], outSocketID, loadedNodes[inNodeID], inSocketID);
	}

	if (root.isMember("renderOffset"))
	{
		float offsetX = root["renderOffset"]["x"].asFloat();
		float offsetY = root["renderOffset"]["y"].asFloat();
		newArea->setAreaRenderOffset(ImVec2(offsetX, offsetY));
	}

	return newArea;
}

void FEVisualNodeArea::copyNodesTo(FEVisualNodeArea* sourceNodeArea, FEVisualNodeArea* targetNodeArea)
{
	size_t nodeShift = targetNodeArea->nodes.size();

	// Copy all nodes to new node area.
	std::unordered_map<FEVisualNode*, FEVisualNode*> oldToNewNode;
	std::unordered_map<FEVisualNodeSocket*, FEVisualNodeSocket*> oldToNewSocket;
	for (size_t i = 0; i < sourceNodeArea->nodes.size(); i++)
	{
		FEVisualNode* copyOfNode = FEVisualNode::copyChild(sourceNodeArea->nodes[i]->getType(), sourceNodeArea->nodes[i]);
		if (copyOfNode == nullptr)
			copyOfNode = new FEVisualNode(*sourceNodeArea->nodes[i]);
		copyOfNode->parentArea = sourceNodeArea;

		//targetNodeArea->nodes.push_back(copyOfNode);
		targetNodeArea->addNode(copyOfNode);

		// Associate old to new IDs
		oldToNewNode[sourceNodeArea->nodes[i]] = copyOfNode;

		for (size_t j = 0; j < sourceNodeArea->nodes[i]->input.size(); j++)
		{
			oldToNewSocket[sourceNodeArea->nodes[i]->input[j]] = copyOfNode->input[j];
		}

		for (size_t j = 0; j < sourceNodeArea->nodes[i]->output.size(); j++)
		{
			oldToNewSocket[sourceNodeArea->nodes[i]->output[j]] = copyOfNode->output[j];
		}
	}

	// Than we need to recreate all connections.
	for (size_t i = 0; i < sourceNodeArea->nodes.size(); i++)
	{
		for (size_t j = 0; j < sourceNodeArea->nodes[i]->input.size(); j++)
		{
			for (size_t k = 0; k < sourceNodeArea->nodes[i]->input[j]->connections.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (isNodeIDInList(sourceNodeArea->nodes[i]->input[j]->connections[k]->getParent()->getID(), sourceNodeArea->nodes))
				{
					targetNodeArea->nodes[nodeShift + i]->input[j]->connections.push_back(oldToNewSocket[sourceNodeArea->nodes[i]->input[j]->connections[k]]);

					// Add connection to node area.
					// Maybe we already establish this connecton.
					bool shouldAdd = true;
					for (size_t l = 0; l < targetNodeArea->connections.size(); l++)
					{
						if (targetNodeArea->connections[l]->in == oldToNewSocket[sourceNodeArea->nodes[i]->input[j]] &&
							targetNodeArea->connections[l]->out == oldToNewSocket[sourceNodeArea->nodes[i]->input[j]->connections[k]])
						{
							shouldAdd = false;
							break;
						}
					}

					if (shouldAdd)
						targetNodeArea->connections.push_back(new FEVisualNodeConnection(oldToNewSocket[sourceNodeArea->nodes[i]->input[j]->connections[k]], oldToNewSocket[sourceNodeArea->nodes[i]->input[j]]));
				}
			}
		}

		for (size_t j = 0; j < sourceNodeArea->nodes[i]->output.size(); j++)
		{
			for (size_t k = 0; k < sourceNodeArea->nodes[i]->output[j]->connections.size(); k++)
			{
				// if node is connected to node that is not in this list just ignore.
				if (isNodeIDInList(sourceNodeArea->nodes[i]->output[j]->connections[k]->getParent()->getID(), sourceNodeArea->nodes))
				{
					targetNodeArea->nodes[nodeShift + i]->output[j]->connections.push_back(oldToNewSocket[sourceNodeArea->nodes[i]->output[j]->connections[k]]);

					// Add connection to node area.
					// Maybe we already establish this connecton.
					bool shouldAdd = true;
					for (size_t l = 0; l < targetNodeArea->connections.size(); l++)
					{
						if (targetNodeArea->connections[l]->in == oldToNewSocket[sourceNodeArea->nodes[i]->output[j]->connections[k]] &&
							targetNodeArea->connections[l]->out == oldToNewSocket[sourceNodeArea->nodes[i]->output[j]])
						{
							shouldAdd = false;
							break;
						}
					}

					if (shouldAdd)
						targetNodeArea->connections.push_back(new FEVisualNodeConnection(oldToNewSocket[sourceNodeArea->nodes[i]->output[j]], oldToNewSocket[sourceNodeArea->nodes[i]->output[j]->connections[k]]));
				}
			}
		}
	}
}

void FEVisualNodeArea::loadFromFile(const char* fileName)
{
	std::ifstream nodesFile;
	nodesFile.open(fileName);

	std::string fileData((std::istreambuf_iterator<char>(nodesFile)), std::istreambuf_iterator<char>());
	nodesFile.close();

	FEVisualNodeArea* newNodeArea = FEVisualNodeArea::fromJson(fileData);
	FEVisualNodeArea::copyNodesTo(newNodeArea, this);
	delete newNodeArea;
}

std::vector<FEVisualNode*> FEVisualNodeArea::getNodesByName(std::string nodeName)
{
	std::vector<FEVisualNode*> result;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->getName() == nodeName)
			result.push_back(nodes[i]);
	}

	return result;
}

std::vector<FEVisualNode*> FEVisualNodeArea::getNodesByType(std::string nodeType)
{
	std::vector<FEVisualNode*> result;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->getType() == nodeType)
			result.push_back(nodes[i]);
	}

	return result;
}

int FEVisualNodeArea::getNodeCount()
{
	return int(nodes.size());
}

bool FEVisualNodeArea::isMouseHovered()
{
	return mouseHovered;
}