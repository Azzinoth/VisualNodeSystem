#include "LogicalXOROperatorNode.h"
using namespace VisNodeSys;

LogicalXOROperatorNode::LogicalXOROperatorNode() : BaseLogicalOperatorNode()
{
	Type = "LogicalXOROperatorNode";
	SetName("XOR");

	OperatorType = LogicalNodeOperatorType::XOR;
}