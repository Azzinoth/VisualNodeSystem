#include "VisualNode.h"
#include "VisualNodeFactory.h"
using namespace VisNodeSys;

Node::Node(const std::string ID)
{
	this->ID = ID;
	if (ID.empty())
		this->ID = NODE_CORE.GetUniqueHexID();

	SetSize(ImVec2(200, 80));
	SetName("VisualNode");
	Type = "VisualNode";
}

Node::Node(const Node& Other)
{
	ParentArea = Other.ParentArea;
	ID = NODE_CORE.GetUniqueHexID();
	Position = Other.Position;
	Size = Other.Size;

	ClientRegionMin = Other.ClientRegionMin;
	ClientRegionMax = Other.ClientRegionMax;

	Name = Other.Name;
	Type = Other.Type;
	Style = Other.Style;
	bShouldBeDestroyed = false;

	LeftTop = Other.LeftTop;
	RightBottom = Other.RightBottom;

	TitleBackgroundColor = Other.TitleBackgroundColor;
	TitleBackgroundColorHovered = Other.TitleBackgroundColorHovered;

	for (size_t i = 0; i < Other.Input.size(); i++)
	{
		Input.push_back(new NodeSocket(this, Other.Input[i]->GetType(), Other.Input[i]->GetName(), false, Other.Input[i]->OutputData));
	}

	for (size_t i = 0; i < Other.Output.size(); i++)
	{
		Output.push_back(new NodeSocket(this, Other.Output[i]->GetType(), Other.Output[i]->GetName(), true, Other.Output[i]->OutputData));
	}
}

Node::~Node()
{
	for (int i = 0; i < static_cast<int>(Input.size()); i++)
	{
		delete Input[i];
		Input.erase(Input.begin() + i, Input.begin() + i + 1);
		i--;
	}

	for (int i = 0; i < static_cast<int>(Output.size()); i++)
	{
		delete Output[i];
		Output.erase(Output.begin() + i, Output.begin() + i + 1);
		i--;
	}
}

std::string Node::GetID()
{
	return ID;
}

ImVec2 Node::GetPosition() const
{
	return Position;
}

void Node::SetPosition(const ImVec2 NewValue)
{
	Position = NewValue;
}

ImVec2 Node::GetSize() const
{
	if (GetStyle() == CIRCLE)
		return ImVec2(NODE_DIAMETER, NODE_DIAMETER);
	
	return Size;
}

void Node::SetSize(const ImVec2 NewValue)
{
	Size = NewValue;
}

std::string Node::GetName()
{
	return Name;
}

void Node::SetName(const std::string NewValue)
{
	if (NewValue.size() > NODE_NAME_MAX_LENGTH)
		return;

	Name = NewValue;
}

void Node::AddSocket(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return;

	if (Socket->bOutput)
		Output.push_back(Socket);
	else
		Input.push_back(Socket);
}

void Node::Draw()
{
}

void Node::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType)
{

}

bool Node::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
{
	// Socket can't connect to itself.
	if (OwnSocket == CandidateSocket)
		return false;

	// Nodes can't connect to themselves.
	if (CandidateSocket->GetParent() == this)
		return false;

	// Output can't connect to output and input can't connect to input.
	if (OwnSocket->bOutput == CandidateSocket->bOutput)
		return false;

	// Types must match.
	if (OwnSocket->GetType() != CandidateSocket->GetType())
		return false;

	return true;
}

std::string Node::GetType() const
{
	return Type;
}

Json::Value Node::ToJson()
{
	Json::Value Result;

	Result["ID"] = ID;
	Result["nodeType"] = Type;
	Result["nodeStyle"] = Style;
	Result["position"]["x"] = Position.x;
	Result["position"]["y"] = Position.y;
	Result["size"]["x"] = Size.x;
	Result["size"]["y"] = Size.y;
	Result["name"] = Name;

	for (size_t i = 0; i < Input.size(); i++)
	{
		Result["input"][std::to_string(i)]["ID"] = Input[i]->GetID();
		Result["input"][std::to_string(i)]["name"] = Input[i]->GetName();
		Result["input"][std::to_string(i)]["type"] = Input[i]->GetType();
	}

	for (size_t i = 0; i < Output.size(); i++)
	{
		Result["output"][std::to_string(i)]["ID"] = Output[i]->GetID();
		Result["output"][std::to_string(i)]["name"] = Output[i]->GetName();
		Result["output"][std::to_string(i)]["type"] = Output[i]->GetType();
	}

	return Result;
}

void Node::SetToDefaultState()
{
	Node* NewNode = new Node();
	std::string CurrentID = ID;
	*this = *NewNode;
	delete NewNode;

	// To preserve the ID of the node, we set it back after copying.
	ID = CurrentID;
}

bool Node::FromJson(Json::Value Json)
{
	if (!Json.isMember("ID") || !Json["ID"].isString() ||
		!Json.isMember("nodeType") || !Json["nodeType"].isString() ||
		!Json.isMember("position") || !Json["position"].isObject() ||
		!Json["position"].isMember("x") || !Json["position"]["x"].isNumeric() ||
		!Json["position"].isMember("y") || !Json["position"]["y"].isNumeric() ||
		!Json.isMember("size") || !Json["size"].isObject() ||
		!Json["size"].isMember("x") || !Json["size"]["x"].isNumeric() ||
		!Json["size"].isMember("y") || !Json["size"]["y"].isNumeric() ||
		!Json.isMember("name") || !Json["name"].isString())
	{
		// TO-DO: Implement a more robust user notification system (e.g., logging, UI warning).
		SetToDefaultState();
		return false;
	}

	ID = Json["ID"].asCString();
	Type = Json["nodeType"].asCString();
	if (Json.isMember("nodeStyle"))
		Style = NODE_STYLE(Json["nodeStyle"].asInt());
	Position.x = Json["position"]["x"].asFloat();
	Position.y = Json["position"]["y"].asFloat();
	Size.x = Json["size"]["x"].asFloat();
	Size.y = Json["size"]["y"].asFloat();
	Name = Json["name"].asCString();

	const std::vector<Json::String> InputsList = Json["input"].getMemberNames();
	for (size_t i = 0; i < Input.size(); i++)
	{
		delete Input[i];
		Input[i] = nullptr;
	}

	std::pair<int, int> SocketCountInClassDefinition = NODE_FACTORY.GetSocketCount(Type);
	Input.resize(InputsList.size());

	// Validate if the number of input sockets in the JSON data matches the current class definition.
	// This prevents errors if the node class was modified (e.g., sockets added/removed)
	// after the JSON file was saved.
	if (Type != "VisualNode" && SocketCountInClassDefinition.first != Input.size()) // For now VisualNode would be exempt from this check.
	{
		// TO-DO: Implement a more robust user notification system (e.g., logging, UI warning).

		// Safest recovery strategy: Reset the node to a default state.
		// This avoids potential crashes or undefined behavior from mismatched data.
		SetToDefaultState();
		return false;
	}

	for (size_t i = 0; i < Input.size(); i++)
	{
		const std::string Key = std::to_string(i);
		if (!Json["input"].isMember(Key) || !Json["input"][Key].isObject() ||
			!Json["input"][Key].isMember("ID") || !Json["input"][Key]["ID"].isString() ||
			!Json["input"][Key].isMember("name") || !Json["input"][Key]["name"].isString() ||
			!Json["input"][Key].isMember("type") || !Json["input"][Key]["type"].isString())
		{
			// TO-DO: Implement a more robust user notification system (e.g., logging, UI warning).

			// Safest recovery strategy: Reset the node to a default state.
			// This avoids potential crashes or undefined behavior from mismatched data.
			SetToDefaultState();
			return false;
		}

		const std::string ID = Json["input"][Key]["ID"].asCString();
		const std::string Name = Json["input"][Key]["name"].asCString();
		const std::string Type = Json["input"][Key]["type"].asString();

		Input[i] = new NodeSocket(this, Type, Name, false);
		Input[i]->ID = ID;
	}

	const std::vector<Json::String> OutputsList = Json["output"].getMemberNames();
	for (size_t i = 0; i < Output.size(); i++)
	{
		delete Output[i];
		Output[i] = nullptr;
	}

	Output.resize(OutputsList.size());
	// Validate output socket count.
	if (Type != "VisualNode" && SocketCountInClassDefinition.second != Output.size()) // For now VisualNode would be exempt from this check.
	{
		// TO-DO: Implement a more robust user notification system (e.g., logging, UI warning).

		// Safest recovery strategy: Reset the node to a default state.
		// This avoids potential crashes or undefined behavior from mismatched data.
		SetToDefaultState();
		return false;
	}

	for (size_t i = 0; i < Output.size(); i++)
	{
		const std::string Key = std::to_string(i);
		if (!Json["output"].isMember(Key) || !Json["output"][Key].isObject() ||
			!Json["output"][Key].isMember("ID") || !Json["output"][Key]["ID"].isString() ||
			!Json["output"][Key].isMember("name") || !Json["output"][Key]["name"].isString() ||
			!Json["output"][Key].isMember("type") || !Json["output"][Key]["type"].isString())
		{
			// TO-DO: Implement a more robust user notification system (e.g., logging, UI warning).

			// Safest recovery strategy: Reset the node to a default state.
			// This avoids potential crashes or undefined behavior from mismatched data.
			SetToDefaultState();
			return false;
		}

		const std::string ID = Json["output"][Key]["ID"].asCString();
		const std::string Name = Json["output"][Key]["name"].asCString();
		const std::string Type = Json["output"][Key]["type"].asString();

		Output[i] = new NodeSocket(this, Type, Name, true);
		Output[i]->ID = ID;
	}
}

void Node::UpdateClientRegion()
{
	float LongestInputSocketTextW = 0.0f;
	for (size_t i = 0; i < Input.size(); i++)
	{
		const ImVec2 TextSize = ImGui::CalcTextSize(Input[i]->GetName().c_str());
		if (TextSize.x > LongestInputSocketTextW)
			LongestInputSocketTextW = TextSize.x;
	}

	float LongestOutputSocketTextW = 0.0f;
	for (size_t i = 0; i < Output.size(); i++)
	{
		const ImVec2 TextSize = ImGui::CalcTextSize(Output[i]->GetName().c_str());
		if (TextSize.x > LongestOutputSocketTextW)
			LongestOutputSocketTextW = TextSize.x;
	}

	ClientRegionMin.x = LeftTop.x + NODE_SOCKET_SIZE * 5.0f + LongestInputSocketTextW + 2.0f;
	ClientRegionMax.x = RightBottom.x - NODE_SOCKET_SIZE * 5.0f - LongestOutputSocketTextW - 2.0f;

	ClientRegionMin.y = LeftTop.y + NODE_TITLE_HEIGHT + 2.0f;
	ClientRegionMax.y = RightBottom.y - 2.0f;
}

ImVec2 Node::GetClientRegionSize()
{
	UpdateClientRegion();
	return ClientRegionMax - ClientRegionMin;
}

ImVec2 Node::GetClientRegionPosition()
{
	UpdateClientRegion();
	return ClientRegionMin;
}

size_t Node::GetInputSocketCount() const
{
	return Input.size();
}

size_t Node::GetOutputSocketCount() const
{
	return Output.size();
}

std::vector<Node*> Node::GetNodesConnectedToInput() const
{
	std::vector<Node*> Result;
	for (size_t i = 0; i < Input.size(); i++)
	{
		for (size_t j = 0; j < Input[i]->ConnectedSockets.size(); j++)
		{
			if (IsNodeWithIDInList(Input[i]->ConnectedSockets[j]->GetParent()->GetID(), Result))
				continue;

			Result.push_back(Input[i]->ConnectedSockets[j]->GetParent());
		}
	}

	return Result;
}

std::vector<Node*> Node::GetNodesConnectedToOutput() const
{
	std::vector<Node*> Result;
	for (size_t i = 0; i < Output.size(); i++)
	{
		for (size_t j = 0; j < Output[i]->ConnectedSockets.size(); j++)
		{
			if (IsNodeWithIDInList(Output[i]->ConnectedSockets[j]->GetParent()->GetID(), Result))
				continue;

			Result.push_back(Output[i]->ConnectedSockets[j]->GetParent());
		}
	}

	return Result;
}

bool Node::OpenContextMenu()
{
	return false;
}

NODE_STYLE Node::GetStyle() const
{
	return Style;
}

void Node::SetStyle(const NODE_STYLE NewValue)
{
	if (static_cast<int>(NewValue) < 0 || static_cast<int>(NewValue) >= 2)
		return;

	Style = NewValue;
}

bool Node::IsHovered() const
{
	return bHovered;
}

void Node::SetIsHovered(const bool NewValue)
{
	bHovered = NewValue;
}

bool Node::CouldBeMoved() const
{
	return bCouldBeMoved;
}

void Node::SetCouldBeMoved(bool NewValue)
{
	bCouldBeMoved = NewValue;
}

NodeArea* Node::GetParentArea() const
{
	return ParentArea;
}

bool Node::CouldBeDestroyed() const
{
	return bShouldBeDestroyed;
}

bool Node::IsNodeWithIDInList(const std::string ID, const std::vector<Node*> List)
{
	for (size_t i = 0; i < List.size(); i++)
	{
		if (List[i]->GetID() == ID)
			return true;
	}

	return false;
}