#include "DivergentCopyNode.h"
using namespace VisNodeSys;

bool DivergentCopyNode::bIsRegistered = []()
{
	NODE_FACTORY.RegisterNodeType("DivergentCopyNode",
		[]() -> Node* {
			return new DivergentCopyNode();
		},

		[](const Node& CurrentNode) -> Node* {
			const DivergentCopyNode& NodeToCopy = static_cast<const DivergentCopyNode&>(CurrentNode);
			return new DivergentCopyNode(NodeToCopy);
		}
	);

	return true;
}();

DivergentCopyNode::DivergentCopyNode() : Node()
{
	Type = "DivergentCopyNode";
	SetName("DivergentCopyNode");

	// Default instance has two input sockets.
	AddSocket(new NodeSocket(this, "INT", "A", NodeSocket::SocketFlow::Input));
	AddSocket(new NodeSocket(this, "INT", "B", NodeSocket::SocketFlow::Input));
}

DivergentCopyNode::DivergentCopyNode(const DivergentCopyNode& Src) : Node()
{
	Type = "DivergentCopyNode";
	SetName("DivergentCopyNode");

	// Intentionally recreates only one socket, regardless of the source.
	AddSocket(new NodeSocket(this, "INT", "A", NodeSocket::SocketFlow::Input));
}