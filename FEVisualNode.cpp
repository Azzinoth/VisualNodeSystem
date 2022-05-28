#include "FEVisualNode.h"

std::unordered_map<std::string, FEVisualNodeChildFunc> FEVisualNode::childClasses;

FEVisualNode::FEVisualNode(std::string ID)
{
	this->ID = ID;
	if (ID == "")
		this->ID = APPLICATION.getUniqueHexID();

	setSize(ImVec2(200, 80));
	setName("FEVisualNode");
	type = "FEVisualNode";
}

FEVisualNode::FEVisualNode(const FEVisualNode& src)
{
	parentArea = src.parentArea;
	ID = APPLICATION.getUniqueHexID();
	position = src.position;
	size = src.size;

	clientRegionMin = src.clientRegionMin;
	clientRegionMax = src.clientRegionMax;

	name = src.name;
	type = src.type;
	shouldBeDestroyed = false;

	leftTop = src.leftTop;
	rightBottom = src.rightBottom;

	titleBackgroundColor = src.titleBackgroundColor;
	titleBackgroundColorHovered = src.titleBackgroundColorHovered;

	for (size_t i = 0; i < src.input.size(); i++)
	{
		input.push_back(new FEVisualNodeSocket(this, src.input[i]->getType(), src.input[i]->getName()));
	}

	for (size_t i = 0; i < src.output.size(); i++)
	{
		output.push_back(new FEVisualNodeSocket(this, src.output[i]->getType(), src.output[i]->getName()));
	}
}

FEVisualNode::~FEVisualNode()
{
	for (int i = 0; i < int(input.size()); i++)
	{
		delete input[i];
		input.erase(input.begin() + i, input.begin() + i + 1);
		i--;
	}

	for (int i = 0; i < int(output.size()); i++)
	{
		delete output[i];
		output.erase(output.begin() + i, output.begin() + i + 1);
		i--;
	}
}

std::string FEVisualNode::getID()
{
	return ID;
}

ImVec2 FEVisualNode::getPosition()
{
	return position;
}

void FEVisualNode::setPosition(ImVec2 newValue)
{
	position = newValue;
}

ImVec2 FEVisualNode::getSize()
{
	if (getStyle() == FE_VISUAL_NODE_STYLE_CIRCLE)
		return ImVec2(NODE_DIAMETER, NODE_DIAMETER);
	
	return size;
}

void FEVisualNode::setSize(ImVec2 newValue)
{
	size = newValue;
}

std::string FEVisualNode::getName()
{
	return name;
}

void FEVisualNode::setName(std::string newValue)
{
	if (newValue.size() > FE_VISUAL_NODE_NAME_MAX_LENGHT)
		return;

	name = newValue;
}

void FEVisualNode::addInputSocket(FEVisualNodeSocket* socket)
{
	if (socket == nullptr || !FEVisualNode::isSocketTypeIn(socket->type))
		return;

	input.push_back(socket);
}

void FEVisualNode::addOutputSocket(FEVisualNodeSocket* socket)
{
	if (socket == nullptr || FEVisualNode::isSocketTypeIn(socket->type))
		return;

	output.push_back(socket);
}

bool FEVisualNode::isSocketTypeIn(FEVisualNodeSocketType type)
{
	static FEVisualNodeSocketType inSocketTypes[] = { FE_NODE_SOCKET_COLOR_CHANNEL_IN,
													  FE_NODE_SOCKET_FLOAT_CHANNEL_IN,
													  FE_NODE_SOCKET_COLOR_RGB_CHANNEL_IN,
													  FE_NODE_SOCKET_COLOR_RGBA_CHANNEL_IN };

	static FEVisualNodeSocketType outSocketTypes[] = { FE_NODE_SOCKET_COLOR_CHANNEL_OUT,
													   FE_NODE_SOCKET_FLOAT_CHANNEL_OUT,
													   FE_NODE_SOCKET_COLOR_RGB_CHANNEL_OUT,
													   FE_NODE_SOCKET_COLOR_RGBA_CHANNEL_OUT};

	int typesCount = (sizeof(inSocketTypes) + sizeof(outSocketTypes)) / sizeof(FEVisualNodeSocketType);
	for (size_t i = 0; i < sizeof(inSocketTypes) / sizeof(FEVisualNodeSocketType); i++)
	{
		if (inSocketTypes[i] == type)
			return true;
	}

	return false;
}

void FEVisualNode::draw()
{
}

void FEVisualNode::socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType)
{

}

bool FEVisualNode::canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser)
{
	if (ownSocket == candidateSocket)
		return false;

	// Own sockets can't be connected.
	if (candidateSocket->getParent() == this)
		return false;

	return true;
}

std::string FEVisualNode::getType() const
{
	return type;
}

Json::Value FEVisualNode::toJson()
{
	Json::Value result;

	result["ID"] = ID;
	result["nodeType"] = type;
	result["position"]["x"] = position.x;
	result["position"]["y"] = position.y;
	result["size"]["x"] = size.x;
	result["size"]["y"] = size.y;
	result["name"] = name;

	for (size_t i = 0; i < input.size(); i++)
	{
		result["input"][std::to_string(i)]["ID"] = input[i]->getID();
		result["input"][std::to_string(i)]["name"] = input[i]->getName();
		result["input"][std::to_string(i)]["type"] = input[i]->getType();
	}

	for (size_t i = 0; i < output.size(); i++)
	{
		result["output"][std::to_string(i)]["ID"] = output[i]->getID();
		result["output"][std::to_string(i)]["name"] = output[i]->getName();
		result["output"][std::to_string(i)]["type"] = output[i]->getType();
	}

	return result;
}

void FEVisualNode::fromJson(Json::Value json)
{
	ID = json["ID"].asCString();
	type = json["nodeType"].asCString();
	position.x = json["position"]["x"].asFloat();
	position.y = json["position"]["y"].asFloat();
	size.x = json["size"]["x"].asFloat();
	size.y = json["size"]["y"].asFloat();
	name = json["name"].asCString();

	std::vector<Json::String> inputsList = json["input"].getMemberNames();
	for (size_t i = 0; i < input.size(); i++)
	{
		delete input[i];
		input[i] = nullptr;
	}
	input.resize(inputsList.size());
	for (size_t i = 0; i < inputsList.size(); i++)
	{
		std::string ID = json["input"][std::to_string(i)]["ID"].asCString();
		std::string name = json["input"][std::to_string(i)]["name"].asCString();
		FEVisualNodeSocketType type = FE_NODE_SOCKET_FLOAT_CHANNEL_IN;
		if (json["input"][std::to_string(i)].isMember("type"))
			type = FEVisualNodeSocketType(json["input"][std::to_string(i)]["type"].asInt());

		input[i] = new FEVisualNodeSocket(this, type, name);
		input[i]->ID = ID;
	}

	std::vector<Json::String> outputsList = json["output"].getMemberNames();
	for (size_t i = 0; i < output.size(); i++)
	{
		delete output[i];
		output[i] = nullptr;
	}
	output.resize(outputsList.size());
	for (size_t i = 0; i < outputsList.size(); i++)
	{
		std::string ID = json["output"][std::to_string(i)]["ID"].asCString();
		std::string name = json["output"][std::to_string(i)]["name"].asCString();
		FEVisualNodeSocketType type = FE_NODE_SOCKET_FLOAT_CHANNEL_OUT;
		if (json["output"][std::to_string(i)].isMember("type"))
			type = FEVisualNodeSocketType(json["output"][std::to_string(i)]["type"].asInt());

		output[i] = new FEVisualNodeSocket(this, type, name);
		output[i]->ID = ID;
	}
}

void FEVisualNode::updateClientRegion()
{
	float longestInputSocketTextW = 0.0f;
	for (size_t i = 0; i < input.size(); i++)
	{
		ImVec2 textSize = ImGui::CalcTextSize(input[i]->getName().c_str());
		if (textSize.x > longestInputSocketTextW)
			longestInputSocketTextW = textSize.x;
	}

	float longestOutputSocketTextW = 0.0f;
	for (size_t i = 0; i < output.size(); i++)
	{
		ImVec2 textSize = ImGui::CalcTextSize(output[i]->getName().c_str());
		if (textSize.x > longestOutputSocketTextW)
			longestOutputSocketTextW = textSize.x;
	}

	clientRegionMin.x = leftTop.x + NODE_SOCKET_SIZE * 5.0f + longestInputSocketTextW + 2.0f;
	clientRegionMax.x = rightBottom.x - NODE_SOCKET_SIZE * 5.0f - longestOutputSocketTextW - 2.0f;

	clientRegionMin.y = leftTop.y + NODE_TITLE_HEIGHT + 2.0f;
	clientRegionMax.y = rightBottom.y - 2.0f;
}

ImVec2 FEVisualNode::getClientRegionSize()
{
	updateClientRegion();
	return clientRegionMax - clientRegionMin;
}

ImVec2 FEVisualNode::getClientRegionPosition()
{
	updateClientRegion();
	return clientRegionMin;
}

size_t FEVisualNode::inputSocketCount()
{
	return input.size();
}

size_t FEVisualNode::outSocketCount()
{
	return output.size();
}

bool FEVisualNode::getForcedOutSocketColor(ImColor& color, size_t socketIndex)
{
	if (socketIndex < 0 || socketIndex >= output.size())
		return false;

	return output[socketIndex]->getForcedConnectionColor(color);
}

void FEVisualNode::setForcedOutSocketColor(ImColor* newValue, size_t socketIndex)
{
	if (socketIndex < 0 || socketIndex >= output.size())
		return;

	output[socketIndex]->setForcedConnectionColor(newValue);
}

std::vector<FEVisualNode*> FEVisualNode::getConnectedNodes()
{
	std::vector<FEVisualNode*> result;
	for (size_t i = 0; i < output.size(); i++)
	{
		for (size_t j = 0; j < output[i]->connections.size(); j++)
		{
			result.push_back(output[i]->connections[j]->getParent());
		}
	}

	return result;
}

FEVisualNode* FEVisualNode::getLogicallyNextNode()
{
	auto connected = getConnectedNodes();
	if (connected.size() > 0 && connected[0] != nullptr)
		return getConnectedNodes()[0];
	
	return nullptr;
}

void FEVisualNode::registerChildNodeClass(FEVisualNodeChildFunc functions, std::string className)
{
	if (functions.JsonToObj != nullptr && functions.copyConstructor != nullptr && className != "")
	{
		childClasses[className] = functions;
	}
}

FEVisualNode* FEVisualNode::constructChild(std::string childClassName, Json::Value data)
{
	if (childClasses.find(childClassName) == childClasses.end())
		return nullptr;

	return childClasses[childClassName].JsonToObj(data);
}

FEVisualNode* FEVisualNode::copyChild(std::string childClassName, FEVisualNode* child)
{
	if (childClasses.find(childClassName) == childClasses.end())
		return nullptr;

	return childClasses[childClassName].copyConstructor(*child);
}

bool FEVisualNode::openContextMenu()
{
	return false;
}

FE_VISUAL_NODE_STYLE FEVisualNode::getStyle()
{
	return style;
}

void FEVisualNode::setStyle(FE_VISUAL_NODE_STYLE newValue)
{
	if (int(newValue) < 0 || int(newValue) >= 2)
		return;

	style = newValue;
}

bool FEVisualNode::isHovered()
{
	return hovered;
}

void FEVisualNode::setIsHovered(bool newValue)
{
	hovered = newValue;
}