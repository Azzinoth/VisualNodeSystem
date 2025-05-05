#pragma once
#include "../../Core.h"
#include "CustomNodes/BeginNode.h"

enum class NodeVariableSupportedType
{
	BOOL,
	INT,
	FLOAT,
	VEC2,
	BVEC2,
	VEC3,
	BVEC3,
	VEC4,
	BVEC4
};

class StandardNodeTest : public ::testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

	NodeVariableSupportedType GetRandomNodeVariableType();

	bool GetRandomBoolValue();
	BoolLiteralNode* BoolToBoolLiteralNode(bool bValue);
	BoolVariableNode* BoolToBoolVariableNode(bool bValue);

	int GetRandomIntValue();
	IntegerLiteralNode* IntToIntegerLiteralNode(int Value);
	IntegerVariableNode* IntToIntegerVariableNode(int Value);

	float GetRandomFloatValue();
	FloatLiteralNode* FloatToFloatLiteralNode(float Value);
	FloatVariableNode* FloatToFloatVariableNode(float Value);

	glm::vec2 GetRandomVec2Value();
	Vec2LiteralNode* Vec2ToVec2LiteralNode(glm::vec2 Value);
	Vec2VariableNode* Vec2ToVec2VariableNode(glm::vec2 Value);

	glm::bvec2 GetRandomBVec2Value();
	BoolVec2LiteralNode* BVec2ToBVec2LiteralNode(glm::bvec2 Value);
	BoolVec2VariableNode* BVec2ToBVec2VariableNode(glm::bvec2 Value);

	LogicalNodeOperatorType GetRandomLogicalOperatorType();
	BaseLogicalOperatorNode* CreateBaseLogicalOperatorNode(LogicalNodeOperatorType Type);
	bool GetResultFromLogicalOperator(LogicalNodeOperatorType Type, bool A, bool B);

	ComparisonNodeOperatorType GetRandomComparisonOperatorType();
	BaseComparisonOperatorNode* CreateBaseComparisonOperatorNode(ComparisonNodeOperatorType Type);
	bool GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, int A, int B);
	bool GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, float A, float B);
	glm::bvec2 GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, glm::vec2 A, glm::vec2 B);

	ArithmeticOperationType GetRandomArithmeticOperatorType();
	BaseArithmeticOperatorNode* CreateBaseArithmeticOperatorNode(ArithmeticOperationType Type);
	int GetResultFromArithmeticOperator(ArithmeticOperationType Type, int A, int B);
	float GetResultFromArithmeticOperator(ArithmeticOperationType Type, float A, float B);
	glm::vec2 GetResultFromArithmeticOperator(ArithmeticOperationType Type, glm::vec2 A, glm::vec2 B);
};