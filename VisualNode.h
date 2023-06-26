#pragma once

#include "VisualNodeSocket.h"

enum VISUAL_NODE_STYLE
{
	VISUAL_NODE_STYLE_DEFAULT = 0,
	VISUAL_NODE_STYLE_CIRCLE = 1
};

#define VISUAL_NODE_NAME_MAX_LENGHT 1024
#define NODE_TITLE_HEIGHT 30.0f
#define NODE_DIAMETER 72.0f

enum VISUAL_NODE_SOCKET_EVENT
{
	VISUAL_NODE_SOCKET_CONNECTED = 0,
	VISUAL_NODE_SOCKET_DISCONNECTED = 1,
	VISUAL_NODE_SOCKET_DESTRUCTION = 2,
	VISUAL_NODE_SOCKET_UPDATE = 3
};

class VisualNodeSystem;
class VisualNodeArea;
class NodeSocket;

struct VisualNodeChildFunc
{
	std::function<VisualNode* (Json::Value Data)> JsonToObj;
	std::function<VisualNode* (VisualNode& Src)> CopyConstructor;
};

#define VISUAL_NODE_CHILD_PRIVATE_PART(CLASS_NAME)			\
class CLASS_NAME##Registration;								\
class CLASS_NAME : public VisualNode						\
{															\
friend CLASS_NAME##Registration;							\
static VisualNode* ConstructorFromJson(Json::Value Json);	\
static VisualNode* CopyConstructor(VisualNode& Src);	    \
static CLASS_NAME##Registration* Registration;				\
static VisualNodeChildFunc ChildFunctions;				

#define VISUAL_NODE_CHILD_AFTER_CLASS(CLASS_NAME)										\
class CLASS_NAME##Registration															\
{																						\
public:																					\
	CLASS_NAME##Registration()															\
	{																					\
		CLASS_NAME::ChildFunctions.JsonToObj = CLASS_NAME::ConstructorFromJson;			\
		CLASS_NAME::ChildFunctions.CopyConstructor = CLASS_NAME::CopyConstructor;		\
		VisualNode::RegisterChildNodeClass(CLASS_NAME::ChildFunctions, #CLASS_NAME);	\
	}																					\
};


#define VISUAL_NODE_CHILD_CPP(CLASS_NAME)										\
VisualNode* CLASS_NAME::ConstructorFromJson(Json::Value Json)					\
{																				\
	CLASS_NAME* NewNode = new CLASS_NAME();										\
	NewNode->FromJson(Json);													\
	return NewNode;																\
}																				\
																				\
VisualNode* CLASS_NAME::CopyConstructor(VisualNode& Src)					    \
{																				\
	if (Src.GetType() != #CLASS_NAME)											\
		return nullptr;															\
																				\
	CLASS_NAME* NewNode = new CLASS_NAME(*reinterpret_cast<CLASS_NAME*>(&Src));	\
	return NewNode;																\
}																				\
																				\
VisualNodeChildFunc CLASS_NAME::ChildFunctions;								    \
CLASS_NAME##Registration* CLASS_NAME::Registration = new CLASS_NAME##Registration();

class VisualNode
{
protected:
	friend VisualNodeSystem;
	friend VisualNodeArea;

	virtual ~VisualNode();

	VisualNodeArea* ParentArea = nullptr;
	std::string ID;
	ImVec2 Position;
	ImVec2 Size;

	ImVec2 ClientRegionMin;
	ImVec2 ClientRegionMax;

	std::string Name;
	std::string Type;
	bool bShouldBeDestroyed = false;
	bool bCouldBeDestroyed = true;

	std::vector<NodeSocket*> Input;
	std::vector<NodeSocket*> Output;

	ImVec2 LeftTop;
	ImVec2 RightBottom;

	ImColor TitleBackgroundColor = ImColor(120, 150, 25);
	ImColor TitleBackgroundColorHovered = ImColor(140, 190, 35);

	bool bHovered = false;
	void SetIsHovered(bool NewValue);

	VISUAL_NODE_STYLE Style = VISUAL_NODE_STYLE_DEFAULT;

	virtual void Draw();
	virtual bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser = nullptr);
	virtual void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, VISUAL_NODE_SOCKET_EVENT EventType);
	virtual bool OpenContextMenu();

	void UpdateClientRegion();

	// To overcome rare cases of the static initialization order fiasco
	static std::unordered_map<std::string, VisualNodeChildFunc>& GetChildClasses();

	static VisualNode* ConstructChild(std::string ChildClassName, Json::Value Data);
	static VisualNode* CopyChild(std::string ChildClassName, VisualNode* Child);
public:
	VisualNode(std::string ID = "");
	VisualNode(const VisualNode& Src);

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

	void AddSocket(NodeSocket* Socket);

	virtual Json::Value ToJson();
	virtual void FromJson(Json::Value Json);

	bool GetForcedOutSocketColor(ImColor& Color, size_t SocketIndex) const;
	void SetForcedOutSocketColor(ImColor* NewValue, size_t SocketIndex) const;

	size_t InputSocketCount() const;
	size_t OutSocketCount() const;

	std::vector<VisualNode*> GetConnectedNodes() const;
	virtual VisualNode* GetLogicallyNextNode();

	static void RegisterChildNodeClass(VisualNodeChildFunc Functions, std::string ClassName);

	VISUAL_NODE_STYLE GetStyle() const;
	void SetStyle(VISUAL_NODE_STYLE NewValue);

	bool IsHovered() const;
};