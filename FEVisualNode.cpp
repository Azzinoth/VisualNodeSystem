#include "FEVisualNode.h"

//std::unordered_map<std::string, FEVisualNodeChildFunc> FEVisualNode::ChildClasses;

std::unordered_map<std::string, FEVisualNodeChildFunc>& FEVisualNode::GetChildClasses()
{
	static std::unordered_map<std::string, FEVisualNodeChildFunc> ChildClasses;
	return ChildClasses;
}

FEVisualNode::FEVisualNode(const std::string ID)
{
	this->ID = ID;
	if (ID.empty())
		this->ID = APPLICATION.GetUniqueHexID();

	SetSize(ImVec2(200, 80));
	SetName("FEVisualNode");
	Type = "FEVisualNode";
}

FEVisualNode::FEVisualNode(const FEVisualNode& Src)
{
	ParentArea = Src.ParentArea;
	ID = APPLICATION.GetUniqueHexID();
	Position = Src.Position;
	Size = Src.Size;

	ClientRegionMin = Src.ClientRegionMin;
	ClientRegionMax = Src.ClientRegionMax;

	Name = Src.Name;
	Type = Src.Type;
	bShouldBeDestroyed = false;

	LeftTop = Src.LeftTop;
	RightBottom = Src.RightBottom;

	TitleBackgroundColor = Src.TitleBackgroundColor;
	TitleBackgroundColorHovered = Src.TitleBackgroundColorHovered;

	for (size_t i = 0; i < Src.Input.size(); i++)
	{
		Input.push_back(new FEVisualNodeSocket(this, Src.Input[i]->GetType(), Src.Input[i]->GetName()));
	}

	for (size_t i = 0; i < Src.Output.size(); i++)
	{
		Output.push_back(new FEVisualNodeSocket(this, Src.Output[i]->GetType(), Src.Output[i]->GetName()));
	}
}

FEVisualNode::~FEVisualNode()
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

std::string FEVisualNode::GetID()
{
	return ID;
}

ImVec2 FEVisualNode::GetPosition() const
{
	return Position;
}

void FEVisualNode::SetPosition(const ImVec2 NewValue)
{
	Position = NewValue;
}

ImVec2 FEVisualNode::GetSize() const
{
	if (GetStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
		return ImVec2(NODE_DIAMETER, NODE_DIAMETER);
	
	return Size;
}

void FEVisualNode::SetSize(const ImVec2 NewValue)
{
	Size = NewValue;
}

std::string FEVisualNode::GetName()
{
	return Name;
}

void FEVisualNode::SetName(const std::string NewValue)
{
	if (NewValue.size() > FE_VISUAL_NODE_NAME_MAX_LENGHT)
		return;

	Name = NewValue;
}

void FEVisualNode::AddInputSocket(FEVisualNodeSocket* Socket)
{
	if (Socket == nullptr || !FEVisualNode::IsSocketTypeIn(Socket->Type))
		return;

	Input.push_back(Socket);
}

void FEVisualNode::AddOutputSocket(FEVisualNodeSocket* Socket)
{
	if (Socket == nullptr || FEVisualNode::IsSocketTypeIn(Socket->Type))
		return;

	Output.push_back(Socket);
}

bool FEVisualNode::IsSocketTypeIn(const FE_VISUAL_NODE_SOCKET_TYPE Type)
{
	static FE_VISUAL_NODE_SOCKET_TYPE InSocketTypes[] = { FE_NODE_SOCKET_COLOR_CHANNEL_IN,
													  FE_NODE_SOCKET_FLOAT_CHANNEL_IN,
													  FE_NODE_SOCKET_COLOR_RGB_CHANNEL_IN,
													  FE_NODE_SOCKET_COLOR_RGBA_CHANNEL_IN };

	static FE_VISUAL_NODE_SOCKET_TYPE OutSocketTypes[] = { FE_NODE_SOCKET_COLOR_CHANNEL_OUT,
													   FE_NODE_SOCKET_FLOAT_CHANNEL_OUT,
													   FE_NODE_SOCKET_COLOR_RGB_CHANNEL_OUT,
													   FE_NODE_SOCKET_COLOR_RGBA_CHANNEL_OUT};

	for (size_t i = 0; i < sizeof(InSocketTypes) / sizeof(FE_VISUAL_NODE_SOCKET_TYPE); i++)
	{
		if (InSocketTypes[i] == Type)
			return true;
	}

	return false;
}

void FEVisualNode::Draw()
{
}

void FEVisualNode::SocketEvent(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* ConnectedSocket, FE_VISUAL_NODE_SOCKET_EVENT EventType)
{

}

bool FEVisualNode::CanConnect(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* CandidateSocket, char** MsgToUser)
{
	if (OwnSocket == CandidateSocket)
		return false;

	// Own sockets can't be connected.
	if (CandidateSocket->GetParent() == this)
		return false;

	return true;
}

std::string FEVisualNode::GetType() const
{
	return Type;
}

Json::Value FEVisualNode::ToJson()
{
	Json::Value result;

	result["ID"] = ID;
	result["nodeType"] = Type;
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

void FEVisualNode::FromJson(Json::Value Json)
{
	ID = Json["ID"].asCString();
	Type = Json["nodeType"].asCString();
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
		FE_VISUAL_NODE_SOCKET_TYPE type = FE_NODE_SOCKET_FLOAT_CHANNEL_IN;
		if (Json["input"][std::to_string(i)].isMember("type"))
			type = static_cast<FE_VISUAL_NODE_SOCKET_TYPE>(Json["input"][std::to_string(i)]["type"].asInt());

		Input[i] = new FEVisualNodeSocket(this, type, name);
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
		FE_VISUAL_NODE_SOCKET_TYPE type = FE_NODE_SOCKET_FLOAT_CHANNEL_OUT;
		if (Json["output"][std::to_string(i)].isMember("type"))
			type = static_cast<FE_VISUAL_NODE_SOCKET_TYPE>(Json["output"][std::to_string(i)]["type"].asInt());

		Output[i] = new FEVisualNodeSocket(this, type, name);
		Output[i]->ID = ID;
	}
}

void FEVisualNode::UpdateClientRegion()
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

ImVec2 FEVisualNode::GetClientRegionSize()
{
	UpdateClientRegion();
	return ClientRegionMax - ClientRegionMin;
}

ImVec2 FEVisualNode::GetClientRegionPosition()
{
	UpdateClientRegion();
	return ClientRegionMin;
}

size_t FEVisualNode::InputSocketCount() const
{
	return Input.size();
}

size_t FEVisualNode::OutSocketCount() const
{
	return Output.size();
}

bool FEVisualNode::GetForcedOutSocketColor(ImColor& Color, const size_t SocketIndex) const
{
	if (SocketIndex < 0 || SocketIndex >= Output.size())
		return false;

	return Output[SocketIndex]->GetForcedConnectionColor(Color);
}

void FEVisualNode::SetForcedOutSocketColor(ImColor* NewValue, const size_t SocketIndex) const
{
	if (SocketIndex < 0 || SocketIndex >= Output.size())
		return;

	Output[SocketIndex]->SetForcedConnectionColor(NewValue);
}

std::vector<FEVisualNode*> FEVisualNode::GetConnectedNodes() const
{
	std::vector<FEVisualNode*> result;
	for (size_t i = 0; i < Output.size(); i++)
	{
		for (size_t j = 0; j < Output[i]->Connections.size(); j++)
		{
			result.push_back(Output[i]->Connections[j]->GetParent());
		}
	}

	return result;
}

FEVisualNode* FEVisualNode::GetLogicallyNextNode()
{
	const auto Connected = GetConnectedNodes();
	if (!Connected.empty() && Connected[0] != nullptr)
		return GetConnectedNodes()[0];
	
	return nullptr;
}

void FEVisualNode::RegisterChildNodeClass(const FEVisualNodeChildFunc Functions, const std::string ClassName)
{
	if (Functions.JsonToObj != nullptr && Functions.CopyConstructor != nullptr && !ClassName.empty())
	{
		GetChildClasses()[ClassName] = Functions;
	}
}

FEVisualNode* FEVisualNode::ConstructChild(const std::string ChildClassName, const Json::Value Data)
{
	if (GetChildClasses().find(ChildClassName) == GetChildClasses().end())
		return nullptr;

	return GetChildClasses()[ChildClassName].JsonToObj(Data);
}

FEVisualNode* FEVisualNode::CopyChild(const std::string ChildClassName, FEVisualNode* Child)
{
	if (GetChildClasses().find(ChildClassName) == GetChildClasses().end())
		return nullptr;

	return GetChildClasses()[ChildClassName].CopyConstructor(*Child);
}

bool FEVisualNode::OpenContextMenu()
{
	return false;
}

FE_VISUAL_NODE_STYLE FEVisualNode::GetStyle() const
{
	return Style;
}

void FEVisualNode::SetStyle(const FE_VISUAL_NODE_STYLE NewValue)
{
	if (static_cast<int>(NewValue) < 0 || static_cast<int>(NewValue) >= 2)
		return;

	Style = NewValue;
}

bool FEVisualNode::IsHovered() const
{
	return bHovered;
}

void FEVisualNode::SetIsHovered(const bool NewValue)
{
	bHovered = NewValue;
}