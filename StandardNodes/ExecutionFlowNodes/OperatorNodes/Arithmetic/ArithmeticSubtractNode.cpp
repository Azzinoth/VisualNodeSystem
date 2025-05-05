#include "ArithmeticSubtractNode.h"
using namespace VisNodeSys;

ArithmeticSubtractNode::ArithmeticSubtractNode() : BaseArithmeticOperatorNode()
{
	Type = "ArithmeticSubtractNode";
	SetName("Subtract");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	OperatorType = ArithmeticOperationType::SUBTRACT;
}