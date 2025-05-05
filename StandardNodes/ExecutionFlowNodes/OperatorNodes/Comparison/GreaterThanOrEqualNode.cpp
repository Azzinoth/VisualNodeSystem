#include "GreaterThanOrEqualNode.h"
using namespace VisNodeSys;

GreaterThanOrEqualNode::GreaterThanOrEqualNode() : BaseComparisonOperatorNode()
{
	Type = "GreaterThanOrEqualNode";
	OperatorType = ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL;
}