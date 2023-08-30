#pragma once

#include "../../GroupComment.h"

namespace VisNodeSys
{
#define NODE_GRID_STEP 15.0f

	class NodeSystem;

	// Type of events for general callbacks.
	enum NODE_EVENT
	{
		REMOVED = 0,
		DESTROYED = 1,
		BEFORE_CONNECTED = 2,
		AFTER_CONNECTED = 3,
		BEFORE_DISCONNECTED = 4,
		AFTER_DISCONNECTED = 5
	};

	struct NodeAreaGeneralConnectionStyle
	{
		int LineSegments = 16;
		float LineXTangentMagnitude = 80.0f * 2.0f;
		float LineYTangentMagnitude = 0.0f;

		ImVec4 SelectionOutlineColor = ImVec4(55.0f / 255.0f, 1.0f, 55.0f / 255.0f, 1.0f);
		ImVec4 HoveredOutlineColor = ImVec4(55.0f / 255.0f, 55.0f / 255.0f, 250.0f / 255.0f, 1.0f);
	};

	struct NodeAreaGridStyle
	{
		float GRID_SIZE = 10000.0f;
		int BOLD_LINE_FREQUENCY = 10;
		float DEFAULT_LINE_WIDTH = 1;
		float BOLD_LINE_WIDTH = 3;

		ImVec4 GridBackgroundColor = ImVec4(32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f, 1.0f);
		ImVec4 GridLinesColor = ImVec4(53.0f / 255.0f, 53.0f / 255.0f, 53.0f / 255.0f, 0.5f);
		ImVec4 GridBoldLinesColor = ImVec4(27.0f / 255.0f, 27.0f / 255.0f, 27.0f / 255.0f, 1.0f);
	};

	struct NodeAreaStyle
	{
		NodeAreaGridStyle Grid;
		NodeAreaGeneralConnectionStyle GeneralConnection;

		ImVec4 NodeBackgroundColor = ImVec4(75.0f / 255.0f, 75.0f / 255.0f, 75.0f / 255.0f, 125.0f / 255.0f);
		ImVec4 HoveredNodeBackgroundColor = ImVec4(60.0f / 255.0f, 60.0f / 255.0f, 60.0f / 255.0f, 125.0f / 255.0f);
		ImVec4 NodeSelectionColor = ImVec4(175.0f / 255.0f, 255.0f / 255.0f, 175.0f / 255.0f, 1.0f);

		ImVec4 MouseSelectRegionColor = ImVec4(175.0f / 255.0f, 175.0f / 255.0f, 1.0f, 125.0f / 255.0f);

		ImVec4 GroupCommentDefaultBackgroundColor = ImVec4(75.0f / 255.0f, 75.0f / 255.0f, 135.0f / 255.0f, 185.0f / 255.0f);
	};

	struct NodeAreaSettings
	{
		NodeAreaStyle Style;
		float ZoomSpeed = 0.15f;
		bool bRequireFullOverlapToSelect = false;
		bool bShowDefaultMainContextMenu = true;
	};

	class NodeArea
	{
		friend NodeSystem;
	public:
		NodeArea();
		~NodeArea();

		ImVec2 GetPosition() const;
		void SetPosition(ImVec2 NewValue);

		ImVec2 GetSize() const;
		void SetSize(ImVec2 NewValue);

		ImVec2 GetRenderOffset() const;
		void SetRenderOffset(ImVec2 Offset);

		float GetZoomFactor() const;
		void SetZoomFactor(float NewValue);

		Node* GetHovered() const;
		std::vector<Node*> GetSelected();

		std::vector<Node*> GetNodesByName(std::string NodeName) const;
		std::vector<Node*> GetNodesByType(std::string NodeType) const;

		bool IsMouseHovered() const;
		bool IsFillingWindow();
		void SetIsFillingWindow(bool NewValue);
		int GetNodeCount() const;

		void Update();
		void Clear();
		void Reset();
		void AddNode(Node* NewNode);
		void DeleteNode(const Node* Node);
		void AddGroupComment(GroupComment* NewGroupComment);
		void DeleteGroupComment(GroupComment* GroupComment);
		void SetMainContextMenuFunc(void(*Func)());
		void PropagateUpdateToConnectedNodes(const Node* CallerNode) const;
		void SetNodeEventCallback(void(*Func)(Node*, NODE_EVENT));
		std::string ToJson() const;
		void SaveToFile(const char* FileName) const;
		void LoadFromFile(const char* FileName);
		void SaveNodesToFile(const char* FileName, std::vector<Node*> Nodes);
		void RunOnEachNode(void(*Func)(Node*));
		void RunOnEachConnectedNode(Node* StartNode, void(*Func)(Node*));
		void UnSelectAll();
		void GetAllElementsAABB(ImVec2& Min, ImVec2& Max) const;
		ImVec2 GetAllElementsAABBCenter() const;
		ImVec2 GetRenderedViewCenter() const;
		bool TryToConnect(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex);
		bool TryToConnect(const Node* OutNode, std::string OutSocketID, const Node* InNode, std::string InSocketID);
		bool TriggerSocketEvent(NodeSocket* CallerNodeSocket, NodeSocket* TriggeredNodeSocket, NODE_SOCKET_EVENT EventType);
		bool TriggerOrphanSocketEvent(Node* Node, NODE_SOCKET_EVENT EventType);

		std::vector<std::pair<ImVec2, ImVec2>> GetConnectionSegments(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex) const;
		std::vector<std::pair<ImVec2, ImVec2>> GetConnectionSegments(const Node* OutNode, std::string OutSocketID, const Node* InNode, std::string InSocketID) const;
		bool AddRerouteNodeToConnection(const Node* OutNode, size_t OutNodeSocketIndex, const Node* InNode, size_t InNodeSocketIndex, size_t SegmentToDivide, ImVec2 Position);
		bool AddRerouteNodeToConnection(const Node* OutNode, std::string OutSocketID, const Node* InNode, std::string InSocketID, size_t SegmentToDivide, ImVec2 Position);

		static bool IsAlreadyConnected(NodeSocket* FirstSocket, NodeSocket* SecondSocket, const std::vector<Connection*>& Connections);
		static void NodeArea::ProcessConnections(const std::vector<NodeSocket*>& Sockets,
												 std::unordered_map<NodeSocket*, NodeSocket*>& OldToNewSocket,
												 NodeArea* TargetArea, size_t NodeShift, const std::vector<Node*>& SourceNodes);
		static void CopyNodesInternal(const std::vector<Node*>& SourceNodes, NodeArea* TargetArea, const size_t NodeShift = 0);

		static NodeArea* CreateNodeArea(std::vector<Node*> Nodes, const std::vector<GroupComment*> GroupComments);
		static NodeArea* FromJson(std::string JsonText);
		static void CopyNodesTo(NodeArea* SourceNodeArea, NodeArea* TargetNodeArea);
		static bool IsNodeIDInList(std::string ID, std::vector<Node*> List);
		static bool EmptyOrFilledByNulls(const std::vector<Node*> Vector);

		bool NodeArea::GetConnectionStyle(Node* Node, bool bOutputSocket, size_t SocketIndex, ConnectionStyle& Style) const;
		void NodeArea::SetConnectionStyle(Node* Node, bool bOutputSocket, size_t SocketIndex, ConnectionStyle NewStyle);
	private:
		struct SocketEvent
		{
			NodeSocket* TriggeredNodeSocket;
			NodeSocket* CallerNodeSocket;
			NODE_SOCKET_EVENT EventType;
		};

		NodeAreaSettings Settings;

		float Zoom = 1.0f;
		void ApplyZoom(float Delta);
		float MAX_ZOOM_LEVEL = 5.0f;  // Max zoom 500%
		float MIN_ZOOM_LEVEL = 0.2f;  // Min zoom 20%
		float GetNodeSocketSize() const { return NODE_SOCKET_SIZE * Zoom; }
		float GetRerouteNodeSize() const { return NODE_SOCKET_SIZE * Zoom * 1.5f; }
		float GetNodeTitleHeight() const { return NODE_TITLE_HEIGHT * Zoom; }
		ImVec2 GetMouseDragDelta() const { return ImGui::GetMouseDragDelta(0) * Zoom; }
		ImVec2 GetMouseDelta() const { return ImGui::GetIO().MouseDelta / Zoom; }
		float GetConnectionThickness() const { return 3.0f * Zoom; }

		static ImVec2 NeededShift;

		bool bClearing = false;
		bool bFillWindow = false;
		bool bMouseHovered = false;
		ImDrawList* CurrentDrawList = nullptr;
		ImGuiWindow* NodeAreaWindow = nullptr;
		std::vector<Node*> Nodes;

		Node* HoveredNode = nullptr;
		NodeSocket* SocketLookingForConnection = nullptr;
		Connection* HoveredConnection = nullptr;
		NodeSocket* SocketHovered = nullptr;
		RerouteNode* RerouteNodeHovered = nullptr;
		GroupComment* GroupCommentHovered = nullptr;

		// Info for context menu
		GroupComment* GroupCommentHoveredWhenContextMenuWasOpened = nullptr;
		ImVec4 ColorPickerStartValue = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		bool bShowGroupCommentColorPicker = false;

		std::vector<GroupComment*> GroupComments;

		std::vector<Node*> SelectedNodes;
		std::vector<Connection*> SelectedConnections;
		std::vector<RerouteNode*> SelectedRerouteNodes;
		std::vector<GroupComment*> SelectedGroupComments;

		ImVec2 MouseCursorPosition;
		ImVec2 MouseCursorSize = ImVec2(1, 1);
		ImVec2 MouseSelectRegionMin = ImVec2(FLT_MAX, FLT_MAX);
		ImVec2 MouseSelectRegionMax = ImVec2(FLT_MAX, FLT_MAX);
		ImGuiWindow* MouseDownIn = nullptr;
		bool bOpenMainContextMenu = false;
		std::vector<Connection*> Connections;

		ImVec2 Position;
		ImVec2 Size;
		ImVec2 RenderOffset = ImVec2(0.0, 0.0);
		void(*MainContextMenuFunc)() = nullptr;
		void RenderDefaultMainContextMenu(ImVec2 LocalMousePosition);
		std::vector<void(*)(Node*, NODE_EVENT)> NodeEventsCallbacks;
		std::queue<SocketEvent> SocketEventQueue;

		void PropagateNodeEventsCallbacks(Node* Node, NODE_EVENT EventToPropagate) const;
		void ProcessSocketEventQueue();
		ImVec2 SocketToPosition(const NodeSocket* Socket) const;
		std::vector<Connection*> GetAllConnections(const NodeSocket* Socket) const;
		Connection* GetConnection(const NodeSocket* FirstSocket, const NodeSocket* SecondSocket) const;

		void Delete(Connection* Connection);
		void Delete(RerouteNode* RerouteNode);
		void Delete(GroupComment* GroupComment);

		void InputUpdate();

		void MouseInputUpdate();
		void MouseInputUpdateNodes();
		void MouseInputUpdateGroupComments();

		void LeftMouseClick();
		void LeftMouseClickNodesUpdate();
		void LeftMouseClickConnectionsUpdate();
		void LeftMouseClickRerouteUpdate();
		void LeftMouseClickGroupCommentsUpdate();

		void RightMouseClick();
		void RightMouseClickNodesUpdate();
		void RightMouseClickConnectionsUpdate();
		void RightMouseClickRerouteUpdate();

		void DoubleMouseClick();

		bool ShouldDragGrid();

		void MouseDragging();
		void MouseDraggingNodesUpdate();
		void MouseDraggingConnectionsUpdate();
		void MouseDraggingRerouteUpdate();
		void MouseDraggingGroupCommentUpdate();

		void LeftMouseDown();
		void LeftMouseReleased();
		void LeftMouseReleasedGroupCommentUpdate();

		void KeyboardInputUpdate();
		void InputUpdateNode(Node* Node);
		void InputUpdateSocket(NodeSocket* Socket);
		void MouseInputUpdateConnections();
		void InputUpdateReroute(RerouteNode* Reroute);

		bool AddSelected(Node* Node);
		bool IsSelected(const Node* Node) const;
		bool AddSelected(Connection* Connection);
		bool IsSelected(const Connection* Connection) const;
		bool UnSelect(const Connection* Connection);
		void UnSelectAllConnections();

		bool AddSelected(RerouteNode* RerouteNode);
		bool IsSelected(const RerouteNode* RerouteNode) const;
		bool UnSelect(const RerouteNode* RerouteNode);
		void UnSelectAllRerouteNodes();

		bool AddSelected(GroupComment* GroupComment);
		bool IsSelected(const GroupComment* GroupComment) const;
		bool UnSelect(GroupComment* GroupComment);
		void UnSelectAllGroupComments();

		bool IsMouseAboveSomethingSelected() const;

		void ConnectionsDoubleMouseClick();
		std::vector<ConnectionSegment> GetConnectionSegments(const Connection* Connection) const;
		bool AddRerouteNode(Connection* Connection, size_t SegmentToDivide, ImVec2 Position);
		bool IsMouseOverConnection(Connection* Connection, const int Steps, const float MaxDistance, ImVec2& CollisionPoint = ImVec2());
		bool IsMouseOverSegment(ImVec2 Begin, ImVec2 End, const int Steps, const float MaxDistance, ImVec2& CollisionPoint = ImVec2());
		bool IsPointInRegion(const ImVec2& Point, const ImVec2& RegionMin, const ImVec2& RegionMax);
		bool IsSegmentInRegion(ImVec2 Begin, ImVec2 End, const int Steps);
		bool IsConnectionInRegion(Connection* Connection, const int Steps);

		bool IsRectsOverlaping(ImVec2 FirstRectMin, ImVec2 FirstRectSize, ImVec2 SecondRectMin, ImVec2 SecondRectSize);
		bool IsSecondRectInsideFirstOne(ImVec2 FirstRectMin, ImVec2 FirstRectSize, ImVec2 SecondRectMin, ImVec2 SecondRectSize);
		bool IsRectInMouseSelectionRegion(ImVec2 RectMin, ImVec2 RectSize);
		bool IsRectUnderMouse(ImVec2 RectMin, ImVec2 RectSize);

		
		bool IsGroupCommentCaptionUnderMouse(GroupComment* GroupComment);
		bool IsGroupCommentRightPartUnderMouse(GroupComment* GroupComment);
		bool IsGroupCommentBottomPartUnderMouse(GroupComment* GroupComment);
		bool IsAnyGroupCommentInResizeMode();

		void SelectFontSettings() const;

		void GroupCommentDoubleMouseClick();
		void AttachElemetnsToGroupComment(GroupComment* GroupComment);
		void MoveGroupComment(GroupComment* GroupComment, ImVec2 Delta);

		void Render();
		void RenderGrid(ImVec2 CurrentPosition) const;
		void RenderNode(Node* Node) const;
		void RenderNodeSockets(const Node* Node) const;
		void RenderNodeSocket(NodeSocket* Socket) const;
		std::vector<ImVec2> GetTangentsForLine(const ImVec2 P1, const ImVec2 P2) const;
		void DrawHermiteLine(ImVec2 P1, ImVec2 P2, int Steps, ImVec4 Color, const ConnectionStyle* Style) const;
		void DrawHermiteLine(const ImVec2 P1, const ImVec2 P2, const int Steps, const ImVec4 Color, const float Thickness) const;
		void RenderConnection(const Connection* Connection) const;
		void RenderReroute(const RerouteNode* RerouteNode) const;
		ConnectionStyle* GetConnectionStyle(const NodeSocket* ParticipantOfConnection) const;

		void RenderGroupComment(GroupComment* GroupComment);

		bool IsMouseRegionSelectionActive() const;

		ImVec2 ScreenToLocal(ImVec2 ScreenPosition) const;
		ImVec2 LocalToScreen(ImVec2 LocalPosition) const;
	};
}