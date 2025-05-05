#include "ArithmeticMultiplyNode.h"
using namespace VisNodeSys;

ArithmeticMultiplyNode::ArithmeticMultiplyNode() : BaseArithmeticOperatorNode()
{
	Type = "ArithmeticMultiplyNode";
	SetName("Multiply");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	OperatorType = ArithmeticOperationType::MULTIPLY;
}