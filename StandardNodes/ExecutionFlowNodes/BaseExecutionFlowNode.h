#pragma once

#include "../../VisualNode.h"

#define NODE_HEIGHT_PER_SOCKET 40

class BaseExecutionFlowNode : public VisNodeSys::Node
{
public:
	BaseExecutionFlowNode(bool bIsInputExecuteSocketNeeded = true);
	virtual ~BaseExecutionFlowNode() = default;
};