#include "NotEqualNode.h"
#include "../Arithmetic/ArithmeticAddNode.h"
using namespace VisNodeSys;

NotEqualNode::NotEqualNode() : BaseComparisonOperatorNode()
{
	Type = "NotEqualNode";
	OperatorType = ComparisonNodeOperatorType::NOT_EQUAL;
}