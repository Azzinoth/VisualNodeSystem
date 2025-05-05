#include "ArithmeticPowerNode.h"
using namespace VisNodeSys;

ArithmeticPowerNode::ArithmeticPowerNode() : BaseArithmeticOperatorNode({ "INT", "FLOAT" })
{
	Type = "ArithmeticPowerNode";
	SetName("Power");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	OperatorType = ArithmeticOperationType::POWER;
}