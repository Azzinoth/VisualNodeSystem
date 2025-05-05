#include "ArithmeticDivideNode.h"
using namespace VisNodeSys;

ArithmeticDivideNode::ArithmeticDivideNode() : BaseArithmeticOperatorNode()
{
	Type = "ArithmeticDivideNode";
	SetName("Divide");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	OperatorType = ArithmeticOperationType::DIVIDE;
}