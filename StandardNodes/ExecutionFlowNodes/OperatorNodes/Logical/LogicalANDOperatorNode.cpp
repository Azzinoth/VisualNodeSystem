#include "LogicalANDOperatorNode.h"
using namespace VisNodeSys;

LogicalANDOperatorNode::LogicalANDOperatorNode() : BaseLogicalOperatorNode()
{
	Type = "LogicalANDOperatorNode";
	SetName("AND");
}