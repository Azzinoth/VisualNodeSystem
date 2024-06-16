#pragma once

#include "VisualNodeSocket.h"

namespace VisNodeSys
{
	enum NODE_STYLE
	{
		DEFAULT = 0,
		CIRCLE = 1
	};

#define NODE_NAME_MAX_LENGHT 1024
#define NODE_TITLE_HEIGHT 30.0f
#define NODE_DIAMETER 72.0f

	enum NODE_SOCKET_EVENT
	{
		CONNECTED = 0,
		DISCONNECTED = 1,
		DESTRUCTION = 2,
		UPDATE = 3,
		EXECUTE = 4
	};

	class Node
	{
	protected:
		friend class NodeSystem;
		friend class NodeArea;

		virtual ~Node();

		NodeArea* ParentArea = nullptr;
		std::string ID;
		ImVec2 Position;
		ImVec2 Size;

		ImVec2 ClientRegionMin;
		ImVec2 ClientRegionMax;

		std::string Name;
		std::string Type;
		bool bShouldBeDestroyed = false;
		bool bCouldBeDestroyed = true;
		bool bCouldBeMoved = true;

		std::vector<NodeSocket*> Input;
		std::vector<NodeSocket*> Output;

		ImVec2 LeftTop;
		ImVec2 RightBottom;

		ImColor TitleBackgroundColor = ImColor(120, 150, 25);
		ImColor TitleBackgroundColorHovered = ImColor(140, 190, 35);

		bool bHovered = false;
		void SetIsHovered(bool NewValue);

		NODE_STYLE Style = DEFAULT;

		virtual void Draw();
		virtual bool CanConnect(NodeSocket* OwnSocket, NodeSocket* CandidateSocket, char** MsgToUser = nullptr);
		virtual void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType);
		virtual bool OpenContextMenu();

		void UpdateClientRegion();

		static bool IsNodeWithIDInList(std::string ID, std::vector<Node*> List);
	public:
		Node(std::string ID = "");
		Node(const Node& Src);

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

		size_t GetInputSocketCount() const;
		size_t GetOutputSocketCount() const;

		std::vector<Node*> GetNodesConnectedToInput() const;
		std::vector<Node*> GetNodesConnectedToOutput() const;

		NODE_STYLE GetStyle() const;
		void SetStyle(NODE_STYLE NewValue);

		bool IsHovered() const;

		bool CouldBeMoved() const;
		void SetCouldBeMoved(bool NewValue);

		bool CouldBeDestroyed() const;

		NodeArea* GetParentArea() const;
	};
}