#pragma once

#include "FEEditorNodeSocket.h"

enum FE_EDITOR_NODE_VISUAL_STYLE
{
	FE_EDITOR_NODE_VISUAL_STYLE_DEFAULT = 0,
	FE_EDITOR_NODE_VISUAL_STYLE_CIRCLE = 1
};

#define FEEDITOR_NODE_NAME_MAX_LENGHT 1024
#define NODE_TITLE_HEIGHT 30.0f
#define NODE_RADIUS 72.0f
#define NODE_DIAMETER NODE_RADIUS * 2.0f

enum FE_EDITOR_NODE_SOCKET_EVENT
{
	FE_EDITOR_NODE_SOCKET_CONNECTED = 0,
	FE_EDITOR_NODE_SOCKET_DISCONNECTED = 1,
	FE_EDITOR_NODE_SOCKET_DESTRUCTION = 2,
	FE_EDITOR_NODE_SOCKET_UPDATE = 3
};

class FEEditorNodeSystem;
class FEEditorNodeArea;
class FEEditorNodeSocket;

struct FEEditorNodeChildFunc
{
	std::function<FEEditorNode* (Json::Value data)> JsonToObj;
	std::function<FEEditorNode* (FEEditorNode& src)> copyConstructor;
};

#define EDITOR_NODE_CHILD_PRIVATE_PART(CLASS_NAME)			\
class CLASS_NAME##Registration;								\
class CLASS_NAME : public FEEditorNode						\
{															\
friend CLASS_NAME##Registration;							\
static FEEditorNode* constructorFromJson(Json::Value json);	\
static FEEditorNode* copyConstructor(FEEditorNode& src);	\
static CLASS_NAME##Registration* registration;				\
static FEEditorNodeChildFunc childFunctions;				

#define EDITOR_NODE_CHILD_AFTER_CLASS(CLASS_NAME)										\
class CLASS_NAME##Registration															\
{																						\
public:																					\
	CLASS_NAME##Registration()															\
	{																					\
		CLASS_NAME::childFunctions.JsonToObj = CLASS_NAME::constructorFromJson;			\
		CLASS_NAME::childFunctions.copyConstructor = CLASS_NAME::copyConstructor;		\
		FEEditorNode::registerChildNodeClass(CLASS_NAME::childFunctions, #CLASS_NAME);	\
	}																					\
};


#define EDITOR_NODE_CHILD_CPP(CLASS_NAME)										\
FEEditorNode* CLASS_NAME::constructorFromJson(Json::Value json)					\
{																				\
	CLASS_NAME* newNode = new CLASS_NAME();										\
	newNode->fromJson(json);													\
	return newNode;																\
}																				\
																				\
FEEditorNode* CLASS_NAME::copyConstructor(FEEditorNode& src)					\
{																				\
	if (src.getType() != #CLASS_NAME)											\
		return nullptr;															\
																				\
	CLASS_NAME* newNode = new CLASS_NAME(*reinterpret_cast<CLASS_NAME*>(&src));	\
	return newNode;																\
}																				\
																				\
FEEditorNodeChildFunc CLASS_NAME::childFunctions;								\
CLASS_NAME##Registration* CLASS_NAME::registration = new CLASS_NAME##Registration();

class FEEditorNode
{
protected:
	friend FEEditorNodeSystem;
	friend FEEditorNodeArea;

	virtual ~FEEditorNode();

	FEEditorNodeArea* parentArea = nullptr;
	std::string ID;
	ImVec2 position;
	ImVec2 size;

	ImVec2 clientRegionMin;
	ImVec2 clientRegionMax;

	std::string name;
	std::string type;
	bool shouldBeDestroyed = false;
	bool couldBeDestroyed = true;

	std::vector<FEEditorNodeSocket*> input;
	std::vector<FEEditorNodeSocket*> output;

	ImVec2 leftTop;
	ImVec2 rightBottom;

	ImColor titleBackgroundColor = ImColor(120, 150, 25);
	ImColor titleBackgroundColorHovered = ImColor(140, 190, 35);

	bool hovered = false;
	void setIsHovered(bool newValue);

	FE_EDITOR_NODE_VISUAL_STYLE style = FE_EDITOR_NODE_VISUAL_STYLE_DEFAULT;

	virtual void draw();
	virtual bool canConnect(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* candidateSocket, char** msgToUser = nullptr);
	virtual void socketEvent(FEEditorNodeSocket* ownSocket, FEEditorNodeSocket* connectedSocket, FE_EDITOR_NODE_SOCKET_EVENT eventType);
	virtual bool openContextMenu();

	void updateClientRegion();

	static std::unordered_map<std::string, FEEditorNodeChildFunc> childClasses;
	static FEEditorNode* constructChild(std::string childClassName, Json::Value data);
	static FEEditorNode* copyChild(std::string childClassName, FEEditorNode* child);
public:
	FEEditorNode(std::string ID = "");
	FEEditorNode(const FEEditorNode& src);

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

	void addInputSocket(FEEditorNodeSocket* socket);
	void addOutputSocket(FEEditorNodeSocket* socket);

	static bool isSocketTypeIn(FEEditorNodeSocketType type);

	virtual Json::Value toJson();
	virtual void fromJson(Json::Value json);

	bool getForcedOutSocketColor(ImColor& color, size_t socketIndex);
	void setForcedOutSocketColor(ImColor* newValue, size_t socketIndex);

	size_t inputSocketCount();
	size_t outSocketCount();

	std::vector<FEEditorNode*> getConnectedNodes();
	virtual FEEditorNode* getLogicallyNextNode();

	static void registerChildNodeClass(FEEditorNodeChildFunc functions, std::string className);

	FE_EDITOR_NODE_VISUAL_STYLE getStyle();
	void setStyle(FE_EDITOR_NODE_VISUAL_STYLE newValue);

	bool isHovered();
};