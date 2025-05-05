#include "BaseExecutionFlowNode.h"
using namespace VisNodeSys;

BaseExecutionFlowNode::BaseExecutionFlowNode(bool bIsInputExecuteSocketNeeded) : Node()
{
	Type = "BaseExecutionFlowNode";

	if (bIsInputExecuteSocketNeeded)
		AddSocket(new NodeSocket(this, "EXECUTE", "", false));
}