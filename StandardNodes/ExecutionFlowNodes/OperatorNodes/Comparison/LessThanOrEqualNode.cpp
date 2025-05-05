#include "LessThanOrEqualNode.h"
using namespace VisNodeSys;

LessThanOrEqualNode::LessThanOrEqualNode() : BaseComparisonOperatorNode()
{
	Type = "LessThanOrEqualNode";
	OperatorType = ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL;
}