#pragma once

#include "../../Core.h"

// Copy constructor intentionally does NOT reproduce source sockets.
class DivergentCopyNode : public VisNodeSys::Node
{
	friend class VisNodeSys::NodeFactory;
	static bool bIsRegistered;

public:
	DivergentCopyNode();
	DivergentCopyNode(const DivergentCopyNode& Src);
};