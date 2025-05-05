#include "LogicalOROperatorNode.h"
using namespace VisNodeSys;

LogicalOROperatorNode::LogicalOROperatorNode() : BaseLogicalOperatorNode()
{
	Type = "LogicalOROperatorNode";
	SetName("OR");

	OperatorType = LogicalNodeOperatorType::OR;
}