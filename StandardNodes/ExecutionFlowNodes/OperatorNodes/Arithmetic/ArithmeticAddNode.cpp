#include "ArithmeticAddNode.h"
using namespace VisNodeSys;

ArithmeticAddNode::ArithmeticAddNode() : BaseArithmeticOperatorNode()
{
	Type = "ArithmeticAddNode";
	SetName("Add");

	TitleBackgroundColor = ImColor(31, 117, 208);
	TitleBackgroundColorHovered = ImColor(35, 145, 255);
}