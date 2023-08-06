#include "VisualNode.h"

VisualNode::VisualNode(const std::string ID)
{
	this->ID = ID;
	if (ID.empty())
		this->ID = APPLICATION.GetUniqueHexID();

	SetSize(ImVec2(200, 80));
	SetName("VisualNode");
	Type = "VisualNode";
}

VisualNode::VisualNode(const VisualNode& Src)
{
	ParentArea = Src.ParentArea;
	ID = APPLICATION.GetUniqueHexID();
	Position = Src.Position;
	Size = Src.Size;

	ClientRegionMin = Src.ClientRegionMin;
	ClientRegionMax = Src.ClientRegionMax;

	Name = Src.Name;
	Type = Src.Type;
	Style = Src.Style;
	bShouldBeDestroyed = false;

	LeftTop = Src.LeftTop;
	RightBottom = Src.RightBottom;

	TitleBackgroundColor = Src.TitleBackgroundColor;
	TitleBackgroundColorHovered = Src.TitleBackgroundColorHovered;

	for (size_t i = 0; i < Src.Input.size(); i++)
	{
		Input.push_back(new NodeSocket(this, Src.Input[i]->GetType(), Src.Input[i]->GetName(), false, Src.Input[i]->OutputData));
	}

	for (size_t i = 0; i < Src.Output.size(); i++)
	{
		Output.push_back(new NodeSocket(this, Src.Output[i]->GetType(), Src.Output[i]->GetName(), true, Src.Output[i]->OutputData));
	}
}

VisualNode::~VisualNode()
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

std::string VisualNode::GetID()
{
	return ID;
}

ImVec2 VisualNode::GetPosition() const
{
	return Position;
}

void VisualNode::SetPosition(const ImVec2 NewValue)
{
	Position = NewValue;
}

ImVec2 VisualNode::GetSize() const
{
	if (GetStyle() == VISUAL_NODE_STYLE_CIRCLE)
		return ImVec2(NODE_DIAMETER, NODE_DIAMETER);
	
	return Size;
}

void VisualNode::SetSize(const ImVec2 NewValue)
{
	Size = NewValue;
}

std::string VisualNode::GetName()
{
	return Name;
}

void VisualNode::SetName(const std::string NewValue)
{
	if (NewValue.size() > VISUAL_NODE_NAME_MAX_LENGHT)
		return;

	Name = NewValue;
}

void VisualNode::AddSocket(NodeSocket* Socket)
{
	if (Socket == nullptr)
		return;

	if (Socket->bOutput)
		Output.push_back(Socket);
	else
		Input.push_back(Socket);
}

void VisualNode::Draw()
{
}

void VisualNode::SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType)
{

}

bool VisualNode::CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser)
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

std::string VisualNode::GetType() const
{
	return Type;
}

Json::Value VisualNode::ToJson()
{
	Json::Value result;

	result["ID"] = ID;
	result["nodeType"] = Type;
	result["nodeStyle"] = Style;
	result["position"]["x"] = Position.x;
	result["position"]["y"] = Position.y;
	result["size"]["x"] = Size.x;
	result["size"]["y"] = Size.y;
	result["name"] = Name;

	for (size_t i = 0; i < Input.size(); i++)
	{
		result["input"][std::to_string(i)]["ID"] = Input[i]->GetID();
		result["input"][std::to_string(i)]["name"] = Input[i]->GetName();
		result["input"][std::to_string(i)]["type"] = Input[i]->GetType();
	}

	for (size_t i = 0; i < Output.size(); i++)
	{
		result["output"][std::to_string(i)]["ID"] = Output[i]->GetID();
		result["output"][std::to_string(i)]["name"] = Output[i]->GetName();
		result["output"][std::to_string(i)]["type"] = Output[i]->GetType();
	}

	return result;
}

void VisualNode::FromJson(Json::Value Json)
{
	ID = Json["ID"].asCString();
	Type = Json["nodeType"].asCString();
	if (Json.isMember("nodeStyle"))
		Style = VISUAL_NODE_STYLE(Json["nodeStyle"].asInt());
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
	Input.resize(InputsList.size());
	for (size_t i = 0; i < InputsList.size(); i++)
	{
		const std::string ID = Json["input"][std::to_string(i)]["ID"].asCString();
		const std::string name = Json["input"][std::to_string(i)]["name"].asCString();

		// This is a temporary solution for compatibility with old files.
		std::string type = "FLOAT";
		if (Json["input"][std::to_string(i)]["type"].type() == Json::stringValue)
			type = Json["input"][std::to_string(i)]["type"].asCString();

		Input[i] = new NodeSocket(this, type, name, false);
		Input[i]->ID = ID;
	}

	const std::vector<Json::String> OutputsList = Json["output"].getMemberNames();
	for (size_t i = 0; i < Output.size(); i++)
	{
		delete Output[i];
		Output[i] = nullptr;
	}
	Output.resize(OutputsList.size());
	for (size_t i = 0; i < OutputsList.size(); i++)
	{
		const std::string ID = Json["output"][std::to_string(i)]["ID"].asCString();
		const std::string name = Json["output"][std::to_string(i)]["name"].asCString();

		// This is a temporary solution for compatibility with old files.
		std::string type = "FLOAT";
		if (Json["output"][std::to_string(i)]["type"].type() == Json::stringValue)
			type = Json["output"][std::to_string(i)]["type"].asCString();

		Output[i] = new NodeSocket(this, type, name, true);
		Output[i]->ID = ID;
	}
}

void VisualNode::UpdateClientRegion()
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

ImVec2 VisualNode::GetClientRegionSize()
{
	UpdateClientRegion();
	return ClientRegionMax - ClientRegionMin;
}

ImVec2 VisualNode::GetClientRegionPosition()
{
	UpdateClientRegion();
	return ClientRegionMin;
}

size_t VisualNode::InputSocketCount() const
{
	return Input.size();
}

size_t VisualNode::OutSocketCount() const
{
	return Output.size();
}

std::vector<VisualNode*> VisualNode::GetNodesConnectedToInput() const
{
	std::vector<VisualNode*> result;
	for (size_t i = 0; i < Input.size(); i++)
	{
		for (size_t j = 0; j < Input[i]->SocketConnected.size(); j++)
		{
			result.push_back(Input[i]->SocketConnected[j]->GetParent());
		}
	}

	return result;
}

std::vector<VisualNode*> VisualNode::GetNodesConnectedToOutput() const
{
	std::vector<VisualNode*> result;
	for (size_t i = 0; i < Output.size(); i++)
	{
		for (size_t j = 0; j < Output[i]->SocketConnected.size(); j++)
		{
			result.push_back(Output[i]->SocketConnected[j]->GetParent());
		}
	}

	return result;
}

bool VisualNode::OpenContextMenu()
{
	return false;
}

VISUAL_NODE_STYLE VisualNode::GetStyle() const
{
	return Style;
}

void VisualNode::SetStyle(const VISUAL_NODE_STYLE NewValue)
{
	if (static_cast<int>(NewValue) < 0 || static_cast<int>(NewValue) >= 2)
		return;

	Style = NewValue;
}

bool VisualNode::IsHovered() const
{
	return bHovered;
}

void VisualNode::SetIsHovered(const bool NewValue)
{
	bHovered = NewValue;
}

bool VisualNode::CouldBeMoved() const
{
	return bCouldBeMoved;
}

void VisualNode::SetCouldBeMoved(bool NewValue)
{
	bCouldBeMoved = NewValue;
}

VisualNodeArea* VisualNode::GetParentArea() const
{
	return ParentArea;
}