#include "LogicalNOTOperatorNode.h"
using namespace VisNodeSys;

LogicalNOTOperatorNode::LogicalNOTOperatorNode() : BaseLogicalOperatorNode(false)
{
	Type = "LogicalNOTOperatorNode";
	SetName("NOT");

	OperatorType = LogicalNodeOperatorType::NOT;
}