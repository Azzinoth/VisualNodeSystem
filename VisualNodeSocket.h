#pragma once

#include "VisualNodeCore.h"

namespace VisNodeSys
{
#define NODE_SOCKET_SIZE 5.0f
#define DEFAULT_NODE_SOCKET_COLOR ImColor(150, 150, 150)
#define DEFAULT_CONNECTION_COLOR ImColor(200, 200, 200)
#define DEFAULT_NODE_SOCKET_MOUSE_HOVERED_CONNECTION_COLOR ImColor(220, 220, 220)

	class Node;

	class VISUAL_NODE_SYSTEM_API NodeSocket
	{
		friend class NodeSystem;
		friend class NodeArea;
		friend Node;

		~NodeSocket() = default;

		std::string ID;
		bool bOutput = false;
		std::vector<std::string> AllowedTypes;
		std::string Name;
		std::vector<NodeSocket*> ConnectedSockets;

		static std::unordered_map<std::string, ImColor> SocketTypeToColorAssociations;

		std::function<void* ()> OutputData = []() { return nullptr; };
	protected:
		Node* Parent = nullptr;
	public:
		enum class Direction : uint8_t
		{
			Input = 0,
			Output = 1
		};

		NodeSocket(Node* Parent, std::string Type, std::string Name, bool bOutput = false, std::function<void* ()> OutputDataFunction = []() { return nullptr; });
		NodeSocket(Node* Parent, std::vector<std::string> Types, std::string Name, bool bOutput = false, std::function<void* ()> OutputDataFunction = []() { return nullptr; });
		
		Node* GetParent() const;
		std::vector<NodeSocket*> GetConnectedSockets() const;

		std::string GetID() const;

		std::string GetName() const;
		void SetName(std::string NewValue);

		bool IsOutput() const;
		bool IsInput() const;

		std::vector<std::string> GetAllowedTypes() const;
		// Returns true if change did not trigger disconnection of already connected sockets.
		bool SetAllowedTypes(std::vector<std::string> NewTypes);

		void SetFunctionToOutputData(std::function<void* ()> NewFunction);
		void* GetData();
	};

	inline NodeSocket::Direction operator!(NodeSocket::Direction Direction)
	{
		return Direction == NodeSocket::Direction::Input ? NodeSocket::Direction::Output : NodeSocket::Direction::Input;
	}

	struct ConnectionStyle
	{
		ImColor ForceColor = DEFAULT_CONNECTION_COLOR;

		bool bMarchingAntsEffect = false;
		float MarchingAntsSpeed = 1.0f;
		bool bMarchingAntsReverseDirection = false;

		bool bPulseEffect = false;
		float PulseSpeed = 1.0f;
		float PulseMin = 0.1f;
		float PulseMax = 1.0f;
	};

	class NodeArea;
	class Connection;

	class RerouteNode
	{
		friend class NodeSystem;
		friend class NodeArea;
		friend class Connection;
		friend Node;

		std::string ID;
		Connection* Parent = nullptr;
		ImVec2 Position;

		NodeSocket* BeginSocket = nullptr;
		RerouteNode* BeginReroute = nullptr;

		NodeSocket* EndSocket = nullptr;
		RerouteNode* EndReroute = nullptr;

		bool bHovered = false;
		bool bSelected = false;

		RerouteNode() {};
	};

	struct ConnectionSegment
	{
		ImVec2 Begin;
		ImVec2 End;

		NodeSocket* BeginSocket = nullptr;
		RerouteNode* BeginReroute = nullptr;

		NodeSocket* EndSocket = nullptr;
		RerouteNode* EndReroute = nullptr;
	};

	class Connection
	{
		friend class NodeSystem;
		friend class NodeArea;
		friend Node;

		NodeSocket* Out = nullptr;
		NodeSocket* In = nullptr;

		bool bHovered = false;
		bool bSelected = false;
		ConnectionStyle Style;

		std::vector<RerouteNode*> RerouteNodes;

		Connection(NodeSocket* Out, NodeSocket* In);
		~Connection();
	};
}