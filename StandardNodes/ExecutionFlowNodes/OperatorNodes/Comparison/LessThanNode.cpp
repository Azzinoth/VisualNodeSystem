#include "LessThanNode.h"
using namespace VisNodeSys;

LessThanNode::LessThanNode() : BaseComparisonOperatorNode()
{
	Type = "LessThanNode";
	OperatorType = ComparisonNodeOperatorType::LESS_THAN;
}