#include "EqualNode.h"
using namespace VisNodeSys;

EqualNode::EqualNode() : BaseComparisonOperatorNode()
{
	Type = "EqualNode";
	OperatorType = ComparisonNodeOperatorType::EQUAL;
}