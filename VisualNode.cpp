#include "VisualNode.h"
#include "VisualNodeFactory.h"
#include "VisualNodeSystem.h"
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
		Input.push_back(new NodeSocket(this, Other.Input[i]->GetAllowedTypes(), Other.Input[i]->GetName(), false, Other.Input[i]->OutputData));
	}

	for (size_t i = 0; i < Other.Output.size(); i++)
	{
		Output.push_back(new NodeSocket(this, Other.Output[i]->GetAllowedTypes(), Other.Output[i]->GetName(), true, Other.Output[i]->OutputData));
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

bool Node::AddSocket(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return false;

	if (Socket->bOutput)
		Output.push_back(Socket);
	else
		Input.push_back(Socket);

	return true;
}

bool Node::DeleteSocket(std::string SocketID)
{
	return DeleteSocket(GetSocketByIDInternal(SocketID));
}

bool Node::DeleteSocket(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return false;

	if (Socket->GetParent() != nullptr)
	{
		NODE_SYSTEM.DeleteSocket(GetID(), Socket->GetID());
		return true;
	}

	std::vector<NodeSocket*>& SocketList = Socket->IsOutput() ? Output : Input;
	for (size_t i = 0; i < SocketList.size(); i++)
	{
		if (SocketList[i]->GetID() == Socket->GetID())
		{
			delete SocketList[i];
			SocketList.erase(SocketList.begin() + i, SocketList.begin() + i + 1);
			return true;
		}
	}

	return false;
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

	// Iterate through all allowed types of the candidate socket and check if the type of the own socket is in the list.
	bool bFound = false;
	for (size_t i = 0; i < CandidateSocket->GetAllowedTypes().size(); i++)
	{
		for (size_t j = 0; j < OwnSocket->GetAllowedTypes().size(); j++)
		{
			if (CandidateSocket->GetAllowedTypes()[i] == OwnSocket->GetAllowedTypes()[j])
			{
				bFound = true;
				break;
			}
		}

		if (bFound)
			break;
	}

	if (!bFound)
		return false;

	return true;
}

std::string Node::GetType() const
{
	return Type;
}

bool Node::GetRenderTitleBar() const
{
	return bRenderTitleBar;
}

void Node::SetRenderTitleBar(bool bNewValue)
{
	bRenderTitleBar = bNewValue;
}

Json::Value Node::ToJson()
{
	Json::Value Result;

	Result["ID"] = ID;
	Result["NodeType"] = Type;
	Result["NodeStyle"] = Style;
	Result["Position"]["X"] = Position.x;
	Result["Position"]["Y"] = Position.y;
	Result["Size"]["X"] = Size.x;
	Result["Size"]["Y"] = Size.y;
	Result["Name"] = Name;

	for (size_t i = 0; i < Input.size(); i++)
	{
		Result["Input"][std::to_string(i)]["ID"] = Input[i]->GetID();
		Result["Input"][std::to_string(i)]["Name"] = Input[i]->GetName();
		for (size_t j = 0; j < Input[i]->GetAllowedTypes().size(); j++)
			Result["Input"][std::to_string(i)]["AllowedTypes"].append(Input[i]->GetAllowedTypes()[j]);
	}

	for (size_t i = 0; i < Output.size(); i++)
	{
		Result["Output"][std::to_string(i)]["ID"] = Output[i]->GetID();
		Result["Output"][std::to_string(i)]["Name"] = Output[i]->GetName();
		for (size_t j = 0; j < Output[i]->GetAllowedTypes().size(); j++)
			Result["Output"][std::to_string(i)]["AllowedTypes"].append(Output[i]->GetAllowedTypes()[j]);
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
		!Json.isMember("NodeType") || !Json["NodeType"].isString() ||
		!Json.isMember("Position") || !Json["Position"].isObject() ||
		!Json["Position"].isMember("X") || !Json["Position"]["X"].isNumeric() ||
		!Json["Position"].isMember("Y") || !Json["Position"]["Y"].isNumeric() ||
		!Json.isMember("Size") || !Json["Size"].isObject() ||
		!Json["Size"].isMember("X") || !Json["Size"]["X"].isNumeric() ||
		!Json["Size"].isMember("Y") || !Json["Size"]["Y"].isNumeric() ||
		!Json.isMember("Name") || !Json["Name"].isString())
	{
		// FE_TO_DO: Implement a more robust user notification system (e.g., logging, UI warning).
		SetToDefaultState();
		return false;
	}

	ID = Json["ID"].asCString();
	Type = Json["NodeType"].asCString();
	if (Json.isMember("NodeStyle"))
		Style = NODE_STYLE(Json["NodeStyle"].asInt());
	Position.x = Json["Position"]["X"].asFloat();
	Position.y = Json["Position"]["Y"].asFloat();
	Size.x = Json["Size"]["X"].asFloat();
	Size.y = Json["Size"]["Y"].asFloat();
	Name = Json["Name"].asCString();

	const std::vector<Json::String> InputsList = Json["Input"].getMemberNames();
	for (size_t i = 0; i < Input.size(); i++)
	{
		delete Input[i];
		Input[i] = nullptr;
	}

	std::pair<size_t, size_t> SocketCountInClassDefinition = NODE_FACTORY.GetSocketCount(Type);
	Input.resize(InputsList.size());

	for (size_t i = 0; i < Input.size(); i++)
	{
		const std::string Key = std::to_string(i);
		if (!Json["Input"].isMember(Key) || !Json["Input"][Key].isObject() ||
			!Json["Input"][Key].isMember("ID") || !Json["Input"][Key]["ID"].isString() ||
			!Json["Input"][Key].isMember("Name") || !Json["Input"][Key]["Name"].isString() ||
			!Json["Input"][Key].isMember("AllowedTypes") || !Json["Input"][Key]["AllowedTypes"].isArray())
		{
			// TO-DO: Implement a more robust user notification system (e.g., logging, UI warning).

			// Safest recovery strategy: Reset the node to a default state.
			// This avoids potential crashes or undefined behavior from mismatched data.
			SetToDefaultState();
			return false;
		}

		const std::string ID = Json["Input"][Key]["ID"].asCString();
		const std::string Name = Json["Input"][Key]["Name"].asCString();

		Json::Value AllowedTypesArray = Json["Input"][Key]["AllowedTypes"];
		std::vector<std::string> AllowedTypes;
		for (unsigned int j = 0; j < AllowedTypesArray.size(); j++)
			AllowedTypes.push_back(AllowedTypesArray[j].asString());
		
		Input[i] = new NodeSocket(this, AllowedTypes, Name, false);
		Input[i]->ID = ID;
	}

	const std::vector<Json::String> OutputsList = Json["Output"].getMemberNames();
	for (size_t i = 0; i < Output.size(); i++)
	{
		delete Output[i];
		Output[i] = nullptr;
	}

	Output.resize(OutputsList.size());
	for (size_t i = 0; i < Output.size(); i++)
	{
		const std::string Key = std::to_string(i);
		if (!Json["Output"].isMember(Key) || !Json["Output"][Key].isObject() ||
			!Json["Output"][Key].isMember("ID") || !Json["Output"][Key]["ID"].isString() ||
			!Json["Output"][Key].isMember("Name") || !Json["Output"][Key]["Name"].isString() ||
			!Json["Output"][Key].isMember("AllowedTypes") || !Json["Output"][Key]["AllowedTypes"].isArray())
		{
			// FE_TO_DO: Implement a more robust user notification system (e.g., logging, UI warning).

			// Safest recovery strategy: Reset the node to a default state.
			// This avoids potential crashes or undefined behavior from mismatched data.
			SetToDefaultState();
			return false;
		}

		const std::string ID = Json["Output"][Key]["ID"].asCString();
		const std::string Name = Json["Output"][Key]["Name"].asCString();

		Json::Value AllowedTypesArray = Json["Output"][Key]["AllowedTypes"];
		std::vector<std::string> AllowedTypes;
		for (unsigned int j = 0; j < AllowedTypesArray.size(); j++)
			AllowedTypes.push_back(AllowedTypesArray[j].asString());

		Output[i] = new NodeSocket(this, AllowedTypes, Name, true);
		Output[i]->ID = ID;
	}

	return true;
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

std::vector<std::pair<size_t, std::vector<std::string>>> Node::GetInputSocketTypes() const
{
	std::vector<std::pair<size_t, std::vector<std::string>>> Result;
	for (size_t i = 0; i < Input.size(); i++)
		Result.push_back(std::make_pair(i, Input[i]->GetAllowedTypes()));

	return Result;
}

size_t Node::GetOutputSocketCount() const
{
	return Output.size();
}

std::vector<std::pair<size_t, std::vector<std::string>>> Node::GetOutputSocketTypes() const
{
	std::vector<std::pair<size_t, std::vector<std::string>>> Result;
	for (size_t i = 0; i < Output.size(); i++)
		Result.push_back(std::make_pair(i, Output[i]->GetAllowedTypes()));

	return Result;
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

void Node::SetIsHovered(const bool bNewValue)
{
	bHovered = bNewValue;
}

bool Node::CouldBeMoved() const
{
	return bCouldBeMoved;
}

void Node::SetCouldBeMoved(bool bNewValue)
{
	bCouldBeMoved = bNewValue;
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

NodeSocket* Node::GetSocketByIDInternal(std::string SocketID) const
{
	for (size_t i = 0; i < Input.size(); i++)
	{
		if (Input[i]->GetID() == SocketID)
			return Input[i];
	}

	for (size_t i = 0; i < Output.size(); i++)
	{
		if (Output[i]->GetID() == SocketID)
			return Output[i];
	}

	return nullptr;
}

const NodeSocket* Node::GetSocketByID(std::string SocketID) const
{
	for (size_t i = 0; i < Input.size(); i++)
	{
		if (Input[i]->GetID() == SocketID)
			return Input[i];
	}

	for (size_t i = 0; i < Output.size(); i++)
	{
		if (Output[i]->GetID() == SocketID)
			return Output[i];
	}

	return nullptr;
}

size_t Node::GetSocketIndexByID(std::string SocketID) const
{
	for (size_t i = 0; i < Input.size(); i++)
	{
		if (Input[i]->GetID() == SocketID)
			return i;
	}

	for (size_t i = 0; i < Output.size(); i++)
	{
		if (Output[i]->GetID() == SocketID)
			return i;
	}

	return -1;
}

const NodeSocket* Node::GetSocketByIndex(size_t SocketIndex, bool bOutput) const
{
	std::vector<NodeSocket*> SocketList = bOutput ? Output : Input;
	if (SocketIndex >= SocketList.size())
		return nullptr;

	return SocketList[SocketIndex];
}

std::string Node::GetSocketIDByIndex(size_t SocketIndex, bool bOutput) const
{
	const NodeSocket* Socket = GetSocketByIndex(SocketIndex, bOutput);
	if (!Socket)
		return "";

	return Socket->GetID();
}