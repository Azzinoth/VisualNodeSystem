#include "ArithmeticModulusNode.h"
using namespace VisNodeSys;

ArithmeticModulusNode::ArithmeticModulusNode() : BaseArithmeticOperatorNode()
{
	Type = "ArithmeticModulusNode";
	SetName("Modulus");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);

	OperatorType = ArithmeticOperationType::MODULUS;
}