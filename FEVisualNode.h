#pragma once

#include "FEVisualNodeSocket.h"

enum FE_VISUAL_NODE_STYLE
{
	FE_VISUAL_NODE_STYLE_DEFAULT = 0,
	FE_VISUAL_NODE_STYLE_CIRCLE = 1
};

#define FE_VISUAL_NODE_NAME_MAX_LENGHT 1024
#define NODE_TITLE_HEIGHT 30.0f
#define NODE_RADIUS 72.0f
#define NODE_DIAMETER NODE_RADIUS * 2.0f

enum FE_VISUAL_NODE_SOCKET_EVENT
{
	FE_VISUAL_NODE_SOCKET_CONNECTED = 0,
	FE_VISUAL_NODE_SOCKET_DISCONNECTED = 1,
	FE_VISUAL_NODE_SOCKET_DESTRUCTION = 2,
	FE_VISUAL_NODE_SOCKET_UPDATE = 3
};

class FEVisualNodeSystem;
class FEVisualNodeArea;
class FEVisualNodeSocket;

struct FEVisualNodeChildFunc
{
	std::function<FEVisualNode* (Json::Value data)> JsonToObj;
	std::function<FEVisualNode* (FEVisualNode& src)> copyConstructor;
};

#define VISUAL_NODE_CHILD_PRIVATE_PART(CLASS_NAME)			\
class CLASS_NAME##Registration;								\
class CLASS_NAME : public FEVisualNode						\
{															\
friend CLASS_NAME##Registration;							\
static FEVisualNode* constructorFromJson(Json::Value json);	\
static FEVisualNode* copyConstructor(FEVisualNode& src);	\
static CLASS_NAME##Registration* registration;				\
static FEVisualNodeChildFunc childFunctions;				

#define VISUAL_NODE_CHILD_AFTER_CLASS(CLASS_NAME)										\
class CLASS_NAME##Registration															\
{																						\
public:																					\
	CLASS_NAME##Registration()															\
	{																					\
		CLASS_NAME::childFunctions.JsonToObj = CLASS_NAME::constructorFromJson;			\
		CLASS_NAME::childFunctions.copyConstructor = CLASS_NAME::copyConstructor;		\
		FEVisualNode::registerChildNodeClass(CLASS_NAME::childFunctions, #CLASS_NAME);	\
	}																					\
};


#define VISUAL_NODE_CHILD_CPP(CLASS_NAME)										\
FEVisualNode* CLASS_NAME::constructorFromJson(Json::Value json)					\
{																				\
	CLASS_NAME* newNode = new CLASS_NAME();										\
	newNode->fromJson(json);													\
	return newNode;																\
}																				\
																				\
FEVisualNode* CLASS_NAME::copyConstructor(FEVisualNode& src)					\
{																				\
	if (src.getType() != #CLASS_NAME)											\
		return nullptr;															\
																				\
	CLASS_NAME* newNode = new CLASS_NAME(*reinterpret_cast<CLASS_NAME*>(&src));	\
	return newNode;																\
}																				\
																				\
FEVisualNodeChildFunc CLASS_NAME::childFunctions;								\
CLASS_NAME##Registration* CLASS_NAME::registration = new CLASS_NAME##Registration();

class FEVisualNode
{
protected:
	friend FEVisualNodeSystem;
	friend FEVisualNodeArea;

	virtual ~FEVisualNode();

	FEVisualNodeArea* parentArea = nullptr;
	std::string ID;
	ImVec2 position;
	ImVec2 size;

	ImVec2 clientRegionMin;
	ImVec2 clientRegionMax;

	std::string name;
	std::string type;
	bool shouldBeDestroyed = false;
	bool couldBeDestroyed = true;

	std::vector<FEVisualNodeSocket*> input;
	std::vector<FEVisualNodeSocket*> output;

	ImVec2 leftTop;
	ImVec2 rightBottom;

	ImColor titleBackgroundColor = ImColor(120, 150, 25);
	ImColor titleBackgroundColorHovered = ImColor(140, 190, 35);

	bool hovered = false;
	void setIsHovered(bool newValue);

	FE_VISUAL_NODE_STYLE style = FE_VISUAL_NODE_STYLE_DEFAULT;

	virtual void draw();
	virtual bool canConnect(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* candidateSocket, char** msgToUser = nullptr);
	virtual void socketEvent(FEVisualNodeSocket* ownSocket, FEVisualNodeSocket* connectedSocket, FE_VISUAL_NODE_SOCKET_EVENT eventType);
	virtual bool openContextMenu();

	void updateClientRegion();

	static std::unordered_map<std::string, FEVisualNodeChildFunc> childClasses;
	static FEVisualNode* constructChild(std::string childClassName, Json::Value data);
	static FEVisualNode* copyChild(std::string childClassName, FEVisualNode* child);
public:
	FEVisualNode(std::string ID = "");
	FEVisualNode(const FEVisualNode& src);

	std::string getID();

	ImVec2 getPosition();
	void setPosition(ImVec2 newValue);

	ImVec2 getSize();
	void setSize(ImVec2 newValue);

	ImVec2 getClientRegionSize();
	ImVec2 getClientRegionPosition();

	std::string getName();
	void setName(std::string newValue);

	std::string getType() const;

	void addInputSocket(FEVisualNodeSocket* socket);
	void addOutputSocket(FEVisualNodeSocket* socket);

	static bool isSocketTypeIn(FEVisualNodeSocketType type);

	virtual Json::Value toJson();
	virtual void fromJson(Json::Value json);

	bool getForcedOutSocketColor(ImColor& color, size_t socketIndex);
	void setForcedOutSocketColor(ImColor* newValue, size_t socketIndex);

	size_t inputSocketCount();
	size_t outSocketCount();

	std::vector<FEVisualNode*> getConnectedNodes();
	virtual FEVisualNode* getLogicallyNextNode();

	static void registerChildNodeClass(FEVisualNodeChildFunc functions, std::string className);

	FE_VISUAL_NODE_STYLE getStyle();
	void setStyle(FE_VISUAL_NODE_STYLE newValue);

	bool isHovered();
};