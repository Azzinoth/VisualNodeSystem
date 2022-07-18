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
	std::function<FEVisualNode* (Json::Value Data)> JsonToObj;
	std::function<FEVisualNode* (FEVisualNode& Src)> CopyConstructor;
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

	FEVisualNodeArea* ParentArea = nullptr;
	std::string ID;
	ImVec2 Position;
	ImVec2 Size;

	ImVec2 ClientRegionMin;
	ImVec2 ClientRegionMax;

	std::string Name;
	std::string Type;
	bool bShouldBeDestroyed = false;
	bool bCouldBeDestroyed = true;

	std::vector<FEVisualNodeSocket*> Input;
	std::vector<FEVisualNodeSocket*> Output;

	ImVec2 LeftTop;
	ImVec2 RightBottom;

	ImColor TitleBackgroundColor = ImColor(120, 150, 25);
	ImColor TitleBackgroundColorHovered = ImColor(140, 190, 35);

	bool bHovered = false;
	void SetIsHovered(bool NewValue);

	FE_VISUAL_NODE_STYLE Style = FE_VISUAL_NODE_STYLE_DEFAULT;

	virtual void Draw();
	virtual bool CanConnect(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* CandidateSocket, char** MsgToUser = nullptr);
	virtual void SocketEvent(FEVisualNodeSocket* OwnSocket, FEVisualNodeSocket* ConnectedSocket, FE_VISUAL_NODE_SOCKET_EVENT EventType);
	virtual bool OpenContextMenu();

	void UpdateClientRegion();

	static std::unordered_map<std::string, FEVisualNodeChildFunc> ChildClasses;
	static FEVisualNode* ConstructChild(std::string ChildClassName, Json::Value Data);
	static FEVisualNode* CopyChild(std::string ChildClassName, FEVisualNode* Child);
public:
	FEVisualNode(std::string ID = "");
	FEVisualNode(const FEVisualNode& Src);

	std::string GetID();

	ImVec2 GetPosition() const;
	void SetPosition(ImVec2 NewValue);

	ImVec2 GetSize() const;
	void SetSize(ImVec2 NewValue);

	ImVec2 GetClientRegionSize();
	ImVec2 GetClientRegionPosition();

	std::string GetName();
	void SetName(std::string NewValue);

	std::string GetType() const;

	void AddInputSocket(FEVisualNodeSocket* Socket);
	void AddOutputSocket(FEVisualNodeSocket* Socket);

	static bool IsSocketTypeIn(FE_VISUAL_NODE_SOCKET_TYPE Type);

	virtual Json::Value ToJson();
	virtual void FromJson(Json::Value Json);

	bool GetForcedOutSocketColor(ImColor& Color, size_t SocketIndex) const;
	void SetForcedOutSocketColor(ImColor* NewValue, size_t SocketIndex) const;

	size_t InputSocketCount() const;
	size_t OutSocketCount() const;

	std::vector<FEVisualNode*> GetConnectedNodes() const;
	virtual FEVisualNode* GetLogicallyNextNode();

	static void RegisterChildNodeClass(FEVisualNodeChildFunc Functions, std::string ClassName);

	FE_VISUAL_NODE_STYLE GetStyle() const;
	void SetStyle(FE_VISUAL_NODE_STYLE NewValue);

	bool IsHovered() const;
};