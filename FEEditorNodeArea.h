#pragma once

#include "FEEditorNode.h"

#define FEEDITOR_NODE_GRID_STEP 15.0f

class FEEditorNodeSystem;

// Type of events for general callbacks.
enum FE_EDITOR_NODE_EVENT
{
	FE_EDITOR_NODE_REMOVED = 0,
	FE_EDITOR_NODE_DESTROYED = 1,
	FE_EDITOR_NODE_BEFORE_CONNECTED = 2,
	FE_EDITOR_NODE_AFTER_CONNECTED = 3,
	FE_EDITOR_NODE_BEFORE_DISCONNECTED = 4,
	FE_EDITOR_NODE_AFTER_DISCONNECTED = 5
};

class FEEditorNodeArea
{
	friend FEEditorNodeSystem;

	FEEditorNodeArea();
	~FEEditorNodeArea();

	bool clearing = false;
	ImDrawList* currentDrawList = nullptr;
	ImGuiWindow* nodeAreaWindow = nullptr;

	std::vector<FEEditorNode*> nodes;
	FEEditorNode* hovered;
	bool mouseHovered = false;

	std::vector<FEEditorNode*> selected;
	bool addSelected(FEEditorNode* newNode);
	bool isSelected(FEEditorNode* node);

	ImVec2 mouseCursorPosition;
	ImVec2 mouseCursorSize = ImVec2(1, 1);
	ImVec2 mouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	ImVec2 mouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);
	ImGuiWindow* mouseDownIn = nullptr;

	void inputUpdateNode(FEEditorNode* node);
	void inputUpdateSocket(FEEditorNodeSocket* socket);

	void renderNode(FEEditorNode* node);
	void renderNodeSockets(FEEditorNode* node);
	void renderNodeSocket(FEEditorNodeSocket* socket);
	bool openMainContextMenu = false;

	void drawHermiteLine(ImVec2 p1, ImVec2 p2, int steps, ImColor color);
	void renderConnection(FEEditorNodeConnection* connection);
	ImVec2 socketToPosition(FEEditorNodeSocket* socket);

	std::vector<FEEditorNodeConnection*> connections;
	std::vector<FEEditorNodeConnection*> getAllConnections(FEEditorNodeSocket* socket);
	void disconnect(FEEditorNodeConnection*& connection);

	FEEditorNodeSocket* socketLookingForConnection = nullptr;
	FEEditorNodeSocket* socketHovered = nullptr;

	ImVec2 areaPosition;
	ImVec2 areaSize;
	ImU32 gridBackgroundColor = IM_COL32(32, 32, 32, 255);
	ImVec4 gridLinesColor = ImVec4(53.0f / 255.0f, 53.0f / 255.0f, 53.0f / 255.0f, 0.5f);
	ImVec4 gridBoldLinesColor = ImVec4(27.0f / 255.0f, 27.0f / 255.0f, 27.0f / 255.0f, 1.0f);
	ImVec2 renderOffset = ImVec2(0.0, 0.0);

	void(*mainContextMenuFunc)() = nullptr;
	void inputUpdate();
	void render();
	void renderGrid(ImVec2 currentPosition);

	std::vector<void(*)(FEEditorNode*, FE_EDITOR_NODE_EVENT)> nodeEventsCallbacks;
	void propagateNodeEventsCallbacks(FEEditorNode* node, FE_EDITOR_NODE_EVENT eventToPropagate);

	static bool isNodeIDInList(std::string ID, std::vector<FEEditorNode*> list);

	static bool emptyOrFilledByNulls(std::vector<FEEditorNode*> vector)
	{
		for (size_t i = 0; i < vector.size(); i++)
		{
			if (vector[i] != nullptr)
				return false;
		}

		return true;
	}

	static ImVec2 neededShift;
public:
	void update();
	void clear();
	void reset();

	ImVec2 getAreaPosition();
	void setAreaPosition(ImVec2 newValue);

	ImVec2 getAreaSize();
	void setAreaSize(ImVec2 newValue);

	ImVec2 getAreaRenderOffset();
	void setAreaRenderOffset(ImVec2 offset);

	void addNode(FEEditorNode* newNode);
	void deleteNode(FEEditorNode* node);

	void setMainContextMenuFunc(void(*func)());

	void propagateUpdateToConnectedNodes(FEEditorNode* callerNode);

	bool tryToConnect(FEEditorNode* outNode, size_t outNodeSocketIndex, FEEditorNode* inNode, size_t inNodeSocketIndex);
	bool tryToConnect(FEEditorNode* outNode, std::string outSocketID, FEEditorNode* inNode, std::string inSocketID);
	void setNodeEventCallback(void(*func)(FEEditorNode*, FE_EDITOR_NODE_EVENT));

	std::string toJson();
	void saveToFile(const char* fileName);
	void loadFromFile(const char* fileName);
	void saveNodesToFile(const char* fileName, std::vector<FEEditorNode*> nodes);

	void runOnEachNode(void(*func)(FEEditorNode*));
	void runOnEachConnectedNode(FEEditorNode* startNode, void(*func)(FEEditorNode*));

	FEEditorNode* getHovered();
	std::vector<FEEditorNode*> getSelected();
	void clearSelection();

	void getAllNodesAABB(ImVec2& min, ImVec2& max);
	ImVec2 getAllNodesAABBCenter();
	ImVec2 getRenderedViewCenter();

	std::vector<FEEditorNode*> getNodesByName(std::string nodeName);
	std::vector<FEEditorNode*> getNodesByType(std::string nodeType);

	static FEEditorNodeArea* createNodeArea(std::vector<FEEditorNode*> nodes);
	static FEEditorNodeArea* fromJson(std::string jsonText);
	static void copyNodesTo(FEEditorNodeArea* sourceNodeArea, FEEditorNodeArea* targetNodeArea);

	int getNodeCount();

	bool isMouseHovered();
};