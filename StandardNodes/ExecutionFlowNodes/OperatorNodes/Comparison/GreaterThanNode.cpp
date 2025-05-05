#include "GreaterThanNode.h"
using namespace VisNodeSys;

GreaterThanNode::GreaterThanNode() : BaseComparisonOperatorNode()
{
	Type = "GreaterThanNode";
	OperatorType = ComparisonNodeOperatorType::GREATER_THAN;
}