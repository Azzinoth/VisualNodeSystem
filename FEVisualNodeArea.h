#pragma once

#include "FEVisualNode.h"

#define FE_VISUAL_NODE_GRID_STEP 15.0f

class FEVisualNodeSystem;

// Type of events for general callbacks.
enum FE_VISUAL_NODE_EVENT
{
	FE_VISUAL_NODE_REMOVED = 0,
	FE_VISUAL_NODE_DESTROYED = 1,
	FE_VISUAL_NODE_BEFORE_CONNECTED = 2,
	FE_VISUAL_NODE_AFTER_CONNECTED = 3,
	FE_VISUAL_NODE_BEFORE_DISCONNECTED = 4,
	FE_VISUAL_NODE_AFTER_DISCONNECTED = 5
};

class FEVisualNodeArea
{
	friend FEVisualNodeSystem;

	FEVisualNodeArea();
	~FEVisualNodeArea();

	bool clearing = false;
	ImDrawList* currentDrawList = nullptr;
	ImGuiWindow* nodeAreaWindow = nullptr;

	std::vector<FEVisualNode*> nodes;
	FEVisualNode* hovered;
	bool mouseHovered = false;

	std::vector<FEVisualNode*> selected;
	bool addSelected(FEVisualNode* newNode);
	bool isSelected(FEVisualNode* node);

	ImVec2 mouseCursorPosition;
	ImVec2 mouseCursorSize = ImVec2(1, 1);
	ImVec2 mouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
	ImVec2 mouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);
	ImGuiWindow* mouseDownIn = nullptr;

	void inputUpdateNode(FEVisualNode* node);
	void inputUpdateSocket(FEVisualNodeSocket* socket);

	void renderNode(FEVisualNode* node);
	void renderNodeSockets(FEVisualNode* node);
	void renderNodeSocket(FEVisualNodeSocket* socket);
	bool openMainContextMenu = false;

	void drawHermiteLine(ImVec2 p1, ImVec2 p2, int steps, ImColor color);
	void renderConnection(FEVisualNodeConnection* connection);
	ImVec2 socketToPosition(FEVisualNodeSocket* socket);

	std::vector<FEVisualNodeConnection*> connections;
	std::vector<FEVisualNodeConnection*> getAllConnections(FEVisualNodeSocket* socket);
	void disconnect(FEVisualNodeConnection*& connection);

	FEVisualNodeSocket* socketLookingForConnection = nullptr;
	FEVisualNodeSocket* socketHovered = nullptr;

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

	std::vector<void(*)(FEVisualNode*, FE_VISUAL_NODE_EVENT)> nodeEventsCallbacks;
	void propagateNodeEventsCallbacks(FEVisualNode* node, FE_VISUAL_NODE_EVENT eventToPropagate);

	static bool isNodeIDInList(std::string ID, std::vector<FEVisualNode*> list);

	static bool emptyOrFilledByNulls(std::vector<FEVisualNode*> vector)
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

	void addNode(FEVisualNode* newNode);
	void deleteNode(FEVisualNode* node);

	void setMainContextMenuFunc(void(*func)());

	void propagateUpdateToConnectedNodes(FEVisualNode* callerNode);

	bool tryToConnect(FEVisualNode* outNode, size_t outNodeSocketIndex, FEVisualNode* inNode, size_t inNodeSocketIndex);
	bool tryToConnect(FEVisualNode* outNode, std::string outSocketID, FEVisualNode* inNode, std::string inSocketID);
	void setNodeEventCallback(void(*func)(FEVisualNode*, FE_VISUAL_NODE_EVENT));

	std::string toJson();
	void saveToFile(const char* fileName);
	void loadFromFile(const char* fileName);
	void saveNodesToFile(const char* fileName, std::vector<FEVisualNode*> nodes);

	void runOnEachNode(void(*func)(FEVisualNode*));
	void runOnEachConnectedNode(FEVisualNode* startNode, void(*func)(FEVisualNode*));

	FEVisualNode* getHovered();
	std::vector<FEVisualNode*> getSelected();
	void clearSelection();

	void getAllNodesAABB(ImVec2& min, ImVec2& max);
	ImVec2 getAllNodesAABBCenter();
	ImVec2 getRenderedViewCenter();

	std::vector<FEVisualNode*> getNodesByName(std::string nodeName);
	std::vector<FEVisualNode*> getNodesByType(std::string nodeType);

	static FEVisualNodeArea* createNodeArea(std::vector<FEVisualNode*> nodes);
	static FEVisualNodeArea* fromJson(std::string jsonText);
	static void copyNodesTo(FEVisualNodeArea* sourceNodeArea, FEVisualNodeArea* targetNodeArea);

	int getNodeCount();

	bool isMouseHovered();
};