#include "StandardNodeTests.h"
using namespace VisNodeSys;

// Using a fixed seed to make it easier to debug.
#define RANDOM_SEED 42
#define RANDOM_ACTIONS_ITERATIONS 1000

void StandardNodeTest::SetUp() {}
void StandardNodeTest::TearDown() {}

TEST_F(StandardNodeTest, LogicalOperators)
{
	const std::unordered_map<std::string, bool> ExpectedNodeValues = {
		// AND part.
		{"5818753A73201D630F6C7E1D", false},
		{"36633B7533453E39795B2D78", false},
		{"5D3A4252263A1006263B4250", false},
		{"52624C1D085C3F23266E7A83", true},
		// OR part.
		{"0F4B5D7C0B3A2B3B5733303A", false},
		{"5B4D1E0B1E3D6F4508453969", true},
		{"497D1C1542325A5230066666", true},
		{"12776C6B3F7D433E31153628", true},
		// XOR part.
		{"1801335819175E0E2559684D", false},
		{"7D19783863112B750F667457", true},
		{"292671376D5016491E760148", true},
		{"31197565090F29186676650F", false},
		// NOT part.
		{"1D4251425D2255311F074037", true},
		{"373B76526B19645617464F3A", false},
	};

	NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestNodeArea, nullptr);

	ASSERT_EQ(TestNodeArea->LoadFromFile("Resources/StandardNodesLogicalOperatorsTestData.json"), true);
	TestNodeArea->SetSaveExecutedNodes(true);

	ASSERT_EQ(TestNodeArea->GetNodeCount(), 56);
	ASSERT_EQ(TestNodeArea->GetConnectionCount(), 69);

	// First set all the nodes to check to false.
	for (const auto& NodeToModify : ExpectedNodeValues)
	{
		Node* CurrentNodeToCheck = TestNodeArea->GetNodeByID(NodeToModify.first);
		ASSERT_NE(CurrentNodeToCheck, nullptr);
		ASSERT_EQ(CurrentNodeToCheck->GetType(), "BoolVariableNode");
		BoolVariableNode* CurrentBoolVariableNode = static_cast<BoolVariableNode*>(CurrentNodeToCheck);
		CurrentBoolVariableNode->SetData(false);
		ASSERT_EQ(CurrentBoolVariableNode->GetData(), false);
	}

	ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
	ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 30);

	for (const auto& NodeToCheck : ExpectedNodeValues)
	{
		Node* CurrentNodeToCheck = TestNodeArea->GetNodeByID(NodeToCheck.first);
		ASSERT_NE(CurrentNodeToCheck, nullptr);
		ASSERT_EQ(CurrentNodeToCheck->GetType(), "BoolVariableNode");
		BoolVariableNode* CurrentBoolVariableNode = static_cast<BoolVariableNode*>(CurrentNodeToCheck);
		bool BoolVariableValue = CurrentBoolVariableNode->GetData();
		ASSERT_EQ(BoolVariableValue, NodeToCheck.second);
	}

	// Now set all the nodes to check to true.
	for (const auto& NodeToModify : ExpectedNodeValues)
	{
		Node* CurrentNodeToCheck = TestNodeArea->GetNodeByID(NodeToModify.first);
		ASSERT_NE(CurrentNodeToCheck, nullptr);
		ASSERT_EQ(CurrentNodeToCheck->GetType(), "BoolVariableNode");
		BoolVariableNode* CurrentBoolVariableNode = static_cast<BoolVariableNode*>(CurrentNodeToCheck);
		CurrentBoolVariableNode->SetData(true);
		ASSERT_EQ(CurrentBoolVariableNode->GetData(), true);
	}

	// Re-execute the node network, result should be the same.
	ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
	ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 30);

	for (const auto& NodeToCheck : ExpectedNodeValues)
	{
		Node* CurrentNodeToCheck = TestNodeArea->GetNodeByID(NodeToCheck.first);
		ASSERT_NE(CurrentNodeToCheck, nullptr);
		ASSERT_EQ(CurrentNodeToCheck->GetType(), "BoolVariableNode");
		BoolVariableNode* CurrentBoolVariableNode = static_cast<BoolVariableNode*>(CurrentNodeToCheck);
		bool BoolVariableValue = CurrentBoolVariableNode->GetData();
		ASSERT_EQ(BoolVariableValue, NodeToCheck.second);
	}

	NODE_SYSTEM.DeleteNodeArea(TestNodeArea);
}

bool StandardNodeTest::GetRandomBoolValue()
{
	return rand() % 2 == 0;
}

LogicalNodeOperatorType StandardNodeTest::GetRandomLogicalOperatorType()
{
	int RandomValue = rand() % 4;
	switch (RandomValue)
	{
		case 0:
			return LogicalNodeOperatorType::AND;
		case 1:
			return LogicalNodeOperatorType::OR;
		case 2:
			return LogicalNodeOperatorType::XOR;
		case 3:
			return LogicalNodeOperatorType::NOT;
	}

	return LogicalNodeOperatorType::AND;
}

BoolLiteralNode* StandardNodeTest::BoolToBoolLiteralNode(bool bValue)
{
	BoolLiteralNode* NewNode = new BoolLiteralNode();
	NewNode->SetData(bValue);
	return NewNode;
}

BoolVariableNode* StandardNodeTest::BoolToBoolVariableNode(bool bValue)
{
	BoolVariableNode* NewNode = new BoolVariableNode();
	NewNode->SetData(bValue);
	return NewNode;
}

BaseLogicalOperatorNode* StandardNodeTest::CreateBaseLogicalOperatorNode(LogicalNodeOperatorType Type)
{
	BaseLogicalOperatorNode* NewNode = nullptr;

	switch (Type)
	{
		case LogicalNodeOperatorType::AND:
			NewNode = new LogicalANDOperatorNode();
			break;

		case LogicalNodeOperatorType::OR:
			NewNode = new LogicalOROperatorNode();
			break;

		case LogicalNodeOperatorType::XOR:
			NewNode = new LogicalXOROperatorNode();
			break;

		case LogicalNodeOperatorType::NOT:
			NewNode = new LogicalNOTOperatorNode();
			break;
	}

	return NewNode;
}

bool StandardNodeTest::GetResultFromLogicalOperator(LogicalNodeOperatorType Type, bool A, bool B)
{
	switch (Type)
	{
		case LogicalNodeOperatorType::AND:
			return A && B;
		case LogicalNodeOperatorType::OR:
			return A || B;
		case LogicalNodeOperatorType::XOR:
			return A != B;
		case LogicalNodeOperatorType::NOT:
			return !A;
	}

	return false;
}

TEST_F(StandardNodeTest, LogicalOperatorsRandom)
{
	NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestNodeArea, nullptr);
	TestNodeArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	ASSERT_NE(BeginNode, nullptr);
	TestNodeArea->AddNode(BeginNode);
	ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
	TestNodeArea->SetExecutionEntryNode(BeginNode);

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		bool bFirst = GetRandomBoolValue();
		bool bSecond = GetRandomBoolValue();
		LogicalNodeOperatorType Type = GetRandomLogicalOperatorType();
		bool Result = GetResultFromLogicalOperator(Type, bFirst, bSecond);

		BoolLiteralNode* FirstBoolNode = BoolToBoolLiteralNode(bFirst);
		ASSERT_NE(FirstBoolNode, nullptr);
		BoolLiteralNode* SecondBoolNode = BoolToBoolLiteralNode(bSecond);
		ASSERT_NE(SecondBoolNode, nullptr);

		BaseLogicalOperatorNode* LogicalOperatorNode = CreateBaseLogicalOperatorNode(Type);
		ASSERT_NE(LogicalOperatorNode, nullptr);

		BoolVariableNode* ResultNode = new BoolVariableNode();
		ASSERT_NE(ResultNode, nullptr);

		TestNodeArea->AddNode(FirstBoolNode);
		TestNodeArea->AddNode(SecondBoolNode);
		TestNodeArea->AddNode(LogicalOperatorNode);
		TestNodeArea->AddNode(ResultNode);
		ASSERT_EQ(TestNodeArea->GetNodeCount(), 5);

		ASSERT_EQ(TestNodeArea->TryToConnect(FirstBoolNode, 0, LogicalOperatorNode, 1), true);
		if (Type != LogicalNodeOperatorType::NOT)
			ASSERT_EQ(TestNodeArea->TryToConnect(SecondBoolNode, 0, LogicalOperatorNode, 2), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(LogicalOperatorNode, 0, ResultNode, 0), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(LogicalOperatorNode, 1, ResultNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(BeginNode, 0, LogicalOperatorNode, 0), true);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), Type == LogicalNodeOperatorType::NOT ? 4:5);

		ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
		ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 3);

		//ASSERT_EQ(LogicalOperatorNode->GetData(), Result);
		ASSERT_EQ(ResultNode->GetData(), Result);

		TestNodeArea->DeleteNode(FirstBoolNode);
		TestNodeArea->DeleteNode(SecondBoolNode);
		TestNodeArea->DeleteNode(LogicalOperatorNode);
		TestNodeArea->DeleteNode(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}
}

NodeVariableSupportedType StandardNodeTest::GetRandomNodeVariableType()
{
	int RandomValue = rand() % 9;
	switch (RandomValue)
	{
		case 0:
			return NodeVariableSupportedType::BOOL;
		case 1:
			return NodeVariableSupportedType::INT;
		case 2:
			return NodeVariableSupportedType::FLOAT;
		case 3:
			return NodeVariableSupportedType::VEC2;
		case 4:
			return NodeVariableSupportedType::BVEC2;
		case 5:
			return NodeVariableSupportedType::VEC3;
		case 6:
			return NodeVariableSupportedType::BVEC3;
		case 7:
			return NodeVariableSupportedType::VEC4;
		case 8:
			return NodeVariableSupportedType::BVEC4;
	}

	return NodeVariableSupportedType::BOOL;
}

int StandardNodeTest::GetRandomIntValue()
{
	return rand() % 10;
}

IntegerLiteralNode* StandardNodeTest::IntToIntegerLiteralNode(int Value)
{
	IntegerLiteralNode* NewNode = new IntegerLiteralNode();
	NewNode->SetData(Value);
	return NewNode;
}

IntegerVariableNode* StandardNodeTest::IntToIntegerVariableNode(int Value)
{
	IntegerVariableNode* NewNode = new IntegerVariableNode();
	NewNode->SetData(Value);
	return NewNode;
}

ComparisonNodeOperatorType StandardNodeTest::GetRandomComparisonOperatorType()
{
	int RandomValue = rand() % 6;
	switch (RandomValue)
	{
		case 0:
			return ComparisonNodeOperatorType::EQUAL;
		case 1:
			return ComparisonNodeOperatorType::NOT_EQUAL;
		case 2:
			return ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL;
		case 3:
			return ComparisonNodeOperatorType::GREATER_THAN;
		case 4:
			return ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL;
		case 5:
			return ComparisonNodeOperatorType::LESS_THAN;
	}

	return ComparisonNodeOperatorType::EQUAL;
}

BaseComparisonOperatorNode* StandardNodeTest::CreateBaseComparisonOperatorNode(ComparisonNodeOperatorType Type)
{
	BaseComparisonOperatorNode* NewNode = nullptr;
	switch (Type)
	{
		case ComparisonNodeOperatorType::EQUAL:
			NewNode = new EqualNode();
			break;
		case ComparisonNodeOperatorType::NOT_EQUAL:
			NewNode = new NotEqualNode();
			break;
		case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
			NewNode = new GreaterThanOrEqualNode();
			break;
		case ComparisonNodeOperatorType::GREATER_THAN:
			NewNode = new GreaterThanNode();
			break;
		case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
			NewNode = new LessThanOrEqualNode();
			break;
		case ComparisonNodeOperatorType::LESS_THAN:
			NewNode = new LessThanNode();
			break;
	}
	return NewNode;
}

bool StandardNodeTest::GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, int A, int B)
{
	switch (Type)
	{
		case ComparisonNodeOperatorType::EQUAL:
			return A == B;
		case ComparisonNodeOperatorType::NOT_EQUAL:
			return A != B;
		case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
			return A >= B;
		case ComparisonNodeOperatorType::GREATER_THAN:
			return A > B;
		case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
			return A <= B;
		case ComparisonNodeOperatorType::LESS_THAN:
			return A < B;
	}

	return false;
}

TEST_F(StandardNodeTest, CompareOperatorsRandom)
{
	NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestNodeArea, nullptr);
	TestNodeArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	ASSERT_NE(BeginNode, nullptr);
	TestNodeArea->AddNode(BeginNode);
	ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
	TestNodeArea->SetExecutionEntryNode(BeginNode);

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		int First = GetRandomIntValue();
		int Second = GetRandomIntValue();
		ComparisonNodeOperatorType Type = GetRandomComparisonOperatorType();
		bool Result = GetResultFromComparisonOperator(Type, First, Second);

		IntegerLiteralNode* FirstNode = IntToIntegerLiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		IntegerLiteralNode* SecondNode = IntToIntegerLiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseComparisonOperatorNode* ComparisonOperatorNode = CreateBaseComparisonOperatorNode(Type);
		ASSERT_NE(ComparisonOperatorNode, nullptr);

		BoolVariableNode* ResultNode = BoolToBoolVariableNode(false);
		ASSERT_NE(ResultNode, nullptr);

		TestNodeArea->AddNode(FirstNode);
		TestNodeArea->AddNode(SecondNode);
		TestNodeArea->AddNode(ComparisonOperatorNode);
		TestNodeArea->AddNode(ResultNode);
		ASSERT_EQ(TestNodeArea->GetNodeCount(), 5);

		ASSERT_EQ(TestNodeArea->TryToConnect(FirstNode, 0, ComparisonOperatorNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(SecondNode, 0, ComparisonOperatorNode, 2), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ComparisonOperatorNode, 0, ResultNode, 0), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ComparisonOperatorNode, 1, ResultNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(BeginNode, 0, ComparisonOperatorNode, 0), true);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 5);

		ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
		ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 3);

		//ASSERT_EQ(LogicalOperatorNode->GetData(), Result);
		ASSERT_EQ(ResultNode->GetData(), Result);

		TestNodeArea->DeleteNode(FirstNode);
		TestNodeArea->DeleteNode(SecondNode);
		TestNodeArea->DeleteNode(ComparisonOperatorNode);
		TestNodeArea->DeleteNode(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		float First = GetRandomFloatValue();
		float Second = GetRandomFloatValue();
		ComparisonNodeOperatorType Type = GetRandomComparisonOperatorType();
		bool Result = GetResultFromComparisonOperator(Type, First, Second);

		FloatLiteralNode* FirstNode = FloatToFloatLiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		FloatLiteralNode* SecondNode = FloatToFloatLiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseComparisonOperatorNode* ComparisonOperatorNode = CreateBaseComparisonOperatorNode(Type);
		ASSERT_NE(ComparisonOperatorNode, nullptr);

		BoolVariableNode* ResultNode = BoolToBoolVariableNode(false);
		ASSERT_NE(ResultNode, nullptr);

		TestNodeArea->AddNode(FirstNode);
		TestNodeArea->AddNode(SecondNode);
		TestNodeArea->AddNode(ComparisonOperatorNode);
		TestNodeArea->AddNode(ResultNode);
		ASSERT_EQ(TestNodeArea->GetNodeCount(), 5);

		ASSERT_EQ(TestNodeArea->TryToConnect(FirstNode, 0, ComparisonOperatorNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(SecondNode, 0, ComparisonOperatorNode, 2), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ComparisonOperatorNode, 0, ResultNode, 0), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ComparisonOperatorNode, 1, ResultNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(BeginNode, 0, ComparisonOperatorNode, 0), true);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 5);

		ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
		ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 3);

		//ASSERT_EQ(LogicalOperatorNode->GetData(), Result);
		ASSERT_EQ(ResultNode->GetData(), Result);

		TestNodeArea->DeleteNode(FirstNode);
		TestNodeArea->DeleteNode(SecondNode);
		TestNodeArea->DeleteNode(ComparisonOperatorNode);
		TestNodeArea->DeleteNode(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		glm::vec2 First = GetRandomVec2Value();
		glm::vec2 Second = GetRandomVec2Value();
		ComparisonNodeOperatorType Type = GetRandomComparisonOperatorType();
		glm::bvec2 Result = GetResultFromComparisonOperator(Type, First, Second);

		Vec2LiteralNode* FirstNode = Vec2ToVec2LiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		Vec2LiteralNode* SecondNode = Vec2ToVec2LiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseComparisonOperatorNode* ComparisonOperatorNode = CreateBaseComparisonOperatorNode(Type);
		ASSERT_NE(ComparisonOperatorNode, nullptr);

		BoolVec2VariableNode* ResultNode = BVec2ToBVec2VariableNode(glm::bvec2(false));
		ASSERT_NE(ResultNode, nullptr);

		TestNodeArea->AddNode(FirstNode);
		TestNodeArea->AddNode(SecondNode);
		TestNodeArea->AddNode(ComparisonOperatorNode);
		TestNodeArea->AddNode(ResultNode);
		ASSERT_EQ(TestNodeArea->GetNodeCount(), 5);

		ASSERT_EQ(TestNodeArea->TryToConnect(FirstNode, 0, ComparisonOperatorNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(SecondNode, 0, ComparisonOperatorNode, 2), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ComparisonOperatorNode, 0, ResultNode, 0), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ComparisonOperatorNode, 1, ResultNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(BeginNode, 0, ComparisonOperatorNode, 0), true);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 5);

		ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
		ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 3);

		//ASSERT_EQ(LogicalOperatorNode->GetData(), Result);
		ASSERT_EQ(ResultNode->GetData(), Result);

		TestNodeArea->DeleteNode(FirstNode);
		TestNodeArea->DeleteNode(SecondNode);
		TestNodeArea->DeleteNode(ComparisonOperatorNode);
		TestNodeArea->DeleteNode(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}
}

ArithmeticOperationType StandardNodeTest::GetRandomArithmeticOperatorType()
{
	int RandomValue = rand() % 6;
	switch (RandomValue)
	{
		case 0:
			return ArithmeticOperationType::ADD;
		case 1:
			return ArithmeticOperationType::SUBTRACT;
		case 2:
			return ArithmeticOperationType::MULTIPLY;
		case 3:
			return ArithmeticOperationType::DIVIDE;
		case 4:
			return ArithmeticOperationType::MODULUS;
		case 5:
			return ArithmeticOperationType::POWER;

	}

	return ArithmeticOperationType::ADD;
}

BaseArithmeticOperatorNode* StandardNodeTest::CreateBaseArithmeticOperatorNode(ArithmeticOperationType Type)
{
	BaseArithmeticOperatorNode* NewNode = nullptr;
	switch (Type)
	{
		case ArithmeticOperationType::ADD:
			NewNode = new ArithmeticAddNode();
			break;
		case ArithmeticOperationType::SUBTRACT:
			NewNode = new ArithmeticSubtractNode();
			break;
		case ArithmeticOperationType::MULTIPLY:
			NewNode = new ArithmeticMultiplyNode();
			break;
		case ArithmeticOperationType::DIVIDE:
			NewNode = new ArithmeticDivideNode();
			break;
		case ArithmeticOperationType::MODULUS:
			NewNode = new ArithmeticModulusNode();
			break;
		case ArithmeticOperationType::POWER:
			NewNode = new ArithmeticPowerNode();
			break;
	}

	return NewNode;
}

int StandardNodeTest::GetResultFromArithmeticOperator(ArithmeticOperationType Type, int A, int B)
{
	switch (Type)
	{
		case ArithmeticOperationType::ADD:
			return A + B;
		case ArithmeticOperationType::SUBTRACT:
			return A - B;
		case ArithmeticOperationType::MULTIPLY:
			return A * B;
		case ArithmeticOperationType::DIVIDE:
			if (B == 0)
				return A;
			return A / B;
		case ArithmeticOperationType::MODULUS:
			if (B == 0)
				return A;
			return A % B;
		case ArithmeticOperationType::POWER:
			return static_cast<int>(std::pow(static_cast<double>(A), static_cast<double>(B)));
	}

	return 0;
}

TEST_F(StandardNodeTest, ArithmeticOperatorsRandom)
{
	NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestNodeArea, nullptr);
	TestNodeArea->SetSaveExecutedNodes(true);

	Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
	ASSERT_NE(BeginNode, nullptr);
	TestNodeArea->AddNode(BeginNode);
	ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
	TestNodeArea->SetExecutionEntryNode(BeginNode);

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		int First = GetRandomIntValue();
		int Second = GetRandomIntValue();
		ArithmeticOperationType Type = GetRandomArithmeticOperatorType();
		int Result = GetResultFromArithmeticOperator(Type, First, Second);

		IntegerLiteralNode* FirstNode = IntToIntegerLiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		IntegerLiteralNode* SecondNode = IntToIntegerLiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseArithmeticOperatorNode* ArithmeticOperatorNode = CreateBaseArithmeticOperatorNode(Type);
		ASSERT_NE(ArithmeticOperatorNode, nullptr);

		IntegerVariableNode* ResultNode = IntToIntegerVariableNode(0);
		ASSERT_NE(ResultNode, nullptr);

		TestNodeArea->AddNode(FirstNode);
		TestNodeArea->AddNode(SecondNode);
		TestNodeArea->AddNode(ArithmeticOperatorNode);
		TestNodeArea->AddNode(ResultNode);
		ASSERT_EQ(TestNodeArea->GetNodeCount(), 5);

		ASSERT_EQ(TestNodeArea->TryToConnect(FirstNode, 0, ArithmeticOperatorNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(SecondNode, 0, ArithmeticOperatorNode, 2), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ArithmeticOperatorNode, 0, ResultNode, 0), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ArithmeticOperatorNode, 1, ResultNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(BeginNode, 0, ArithmeticOperatorNode, 0), true);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 5);

		ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
		ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 3);

		//ASSERT_EQ(LogicalOperatorNode->GetData(), Result);
		ASSERT_EQ(ResultNode->GetData(), Result);

		TestNodeArea->DeleteNode(FirstNode);
		TestNodeArea->DeleteNode(SecondNode);
		TestNodeArea->DeleteNode(ArithmeticOperatorNode);
		TestNodeArea->DeleteNode(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		float First = GetRandomFloatValue();
		float Second = GetRandomFloatValue();
		ArithmeticOperationType Type = GetRandomArithmeticOperatorType();
		float Result = GetResultFromArithmeticOperator(Type, First, Second);

		FloatLiteralNode* FirstNode = FloatToFloatLiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		FloatLiteralNode* SecondNode = FloatToFloatLiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseArithmeticOperatorNode* ArithmeticOperatorNode = CreateBaseArithmeticOperatorNode(Type);
		ASSERT_NE(ArithmeticOperatorNode, nullptr);

		FloatVariableNode* ResultNode = FloatToFloatVariableNode(0);
		ASSERT_NE(ResultNode, nullptr);

		TestNodeArea->AddNode(FirstNode);
		TestNodeArea->AddNode(SecondNode);
		TestNodeArea->AddNode(ArithmeticOperatorNode);
		TestNodeArea->AddNode(ResultNode);
		ASSERT_EQ(TestNodeArea->GetNodeCount(), 5);

		ASSERT_EQ(TestNodeArea->TryToConnect(FirstNode, 0, ArithmeticOperatorNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(SecondNode, 0, ArithmeticOperatorNode, 2), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ArithmeticOperatorNode, 0, ResultNode, 0), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ArithmeticOperatorNode, 1, ResultNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(BeginNode, 0, ArithmeticOperatorNode, 0), true);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 5);

		ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
		ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 3);

		//ASSERT_EQ(LogicalOperatorNode->GetData(), Result);
		ASSERT_EQ(ResultNode->GetData(), Result);

		TestNodeArea->DeleteNode(FirstNode);
		TestNodeArea->DeleteNode(SecondNode);
		TestNodeArea->DeleteNode(ArithmeticOperatorNode);
		TestNodeArea->DeleteNode(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		glm::vec2 First = GetRandomVec2Value();
		glm::vec2 Second = GetRandomVec2Value();
		ArithmeticOperationType Type = GetRandomArithmeticOperatorType();
		if (Type == ArithmeticOperationType::POWER || Type == ArithmeticOperationType::MODULUS)
		{
			// Modus and Power operators currently are not supported for vec2.
			continue;
		}
		glm::vec2 Result = GetResultFromArithmeticOperator(Type, First, Second);

		Vec2LiteralNode* FirstNode = Vec2ToVec2LiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		Vec2LiteralNode* SecondNode = Vec2ToVec2LiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseArithmeticOperatorNode* ArithmeticOperatorNode = CreateBaseArithmeticOperatorNode(Type);
		ASSERT_NE(ArithmeticOperatorNode, nullptr);

		Vec2VariableNode* ResultNode = Vec2ToVec2VariableNode(glm::vec2(0.0f));
		ASSERT_NE(ResultNode, nullptr);

		TestNodeArea->AddNode(FirstNode);
		TestNodeArea->AddNode(SecondNode);
		TestNodeArea->AddNode(ArithmeticOperatorNode);
		TestNodeArea->AddNode(ResultNode);
		ASSERT_EQ(TestNodeArea->GetNodeCount(), 5);

		ASSERT_EQ(TestNodeArea->TryToConnect(FirstNode, 0, ArithmeticOperatorNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(SecondNode, 0, ArithmeticOperatorNode, 2), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ArithmeticOperatorNode, 0, ResultNode, 0), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(ArithmeticOperatorNode, 1, ResultNode, 1), true);
		ASSERT_EQ(TestNodeArea->TryToConnect(BeginNode, 0, ArithmeticOperatorNode, 0), true);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 5);

		ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
		ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 3);

		//ASSERT_EQ(LogicalOperatorNode->GetData(), Result);
		ASSERT_EQ(ResultNode->GetData(), Result);

		TestNodeArea->DeleteNode(FirstNode);
		TestNodeArea->DeleteNode(SecondNode);
		TestNodeArea->DeleteNode(ArithmeticOperatorNode);
		TestNodeArea->DeleteNode(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}
}

float StandardNodeTest::GetRandomFloatValue()
{
	return static_cast<float>(rand() % 1000) / 10.0f;
}

FloatLiteralNode* StandardNodeTest::FloatToFloatLiteralNode(float Value)
{
	FloatLiteralNode* NewNode = new FloatLiteralNode();
	NewNode->SetData(Value);
	return NewNode;
}

FloatVariableNode* StandardNodeTest::FloatToFloatVariableNode(float Value)
{
	FloatVariableNode* NewNode = new FloatVariableNode();
	NewNode->SetData(Value);
	return NewNode;
}

bool StandardNodeTest::GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, float A, float B)
{
	switch (Type)
	{
		case ComparisonNodeOperatorType::EQUAL:
			return A == B;
		case ComparisonNodeOperatorType::NOT_EQUAL:
			return A != B;
		case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
			return A >= B;
		case ComparisonNodeOperatorType::GREATER_THAN:
			return A > B;
		case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
			return A <= B;
		case ComparisonNodeOperatorType::LESS_THAN:
			return A < B;
	}

	return false;
}

float StandardNodeTest::GetResultFromArithmeticOperator(ArithmeticOperationType Type, float A, float B)
{
	switch (Type)
	{
		case ArithmeticOperationType::ADD:
			return A + B;
		case ArithmeticOperationType::SUBTRACT:
			return A - B;
		case ArithmeticOperationType::MULTIPLY:
			return A * B;
		case ArithmeticOperationType::DIVIDE:
			if (B == 0.0f)
				return A;
			return A / B;
		case ArithmeticOperationType::MODULUS:
			if (B == 0.0f)
				return A;
			return std::fmod(A, B);
		case ArithmeticOperationType::POWER:
			return static_cast<float>(std::pow(static_cast<double>(A), static_cast<double>(B)));
	}

	return 0.0f;
}

glm::vec2 StandardNodeTest::GetRandomVec2Value()
{
	glm::vec2 RandomValue;
	RandomValue.x = GetRandomFloatValue();
	RandomValue.y = GetRandomFloatValue();
	return RandomValue;
}

Vec2LiteralNode* StandardNodeTest::Vec2ToVec2LiteralNode(glm::vec2 Value)
{
	Vec2LiteralNode* NewNode = new Vec2LiteralNode();
	NewNode->SetData(Value);
	return NewNode;
}

Vec2VariableNode* StandardNodeTest::Vec2ToVec2VariableNode(glm::vec2 Value)
{
	Vec2VariableNode* NewNode = new Vec2VariableNode();
	NewNode->SetData(Value);
	return NewNode;
}

glm::bvec2 StandardNodeTest::GetResultFromComparisonOperator(ComparisonNodeOperatorType Type, glm::vec2 A, glm::vec2 B)
{
	switch (Type)
	{
		case ComparisonNodeOperatorType::EQUAL:
			return glm::equal(A, B);
		case ComparisonNodeOperatorType::NOT_EQUAL:
			return glm::notEqual(A, B);
		case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
			return glm::greaterThanEqual(A, B);
		case ComparisonNodeOperatorType::GREATER_THAN:
			return glm::greaterThan(A, B);
		case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
			return glm::lessThanEqual(A, B);
		case ComparisonNodeOperatorType::LESS_THAN:
			return glm::lessThan(A, B);
	}

	return glm::bvec2(false);
}

glm::vec2 StandardNodeTest::GetResultFromArithmeticOperator(ArithmeticOperationType Type, glm::vec2 A, glm::vec2 B)
{
	switch (Type)
	{
		case ArithmeticOperationType::ADD:
			return A + B;
		case ArithmeticOperationType::SUBTRACT:
			return A - B;
		case ArithmeticOperationType::MULTIPLY:
			return A * B;
		case ArithmeticOperationType::DIVIDE:
			if (B == glm::vec2(0.0f))
				return A;
			return A / B;
		case ArithmeticOperationType::MODULUS:
			if (B == glm::vec2(0.0f))
				return A;
			return glm::mod(A, B);
		case ArithmeticOperationType::POWER:
			return glm::pow(A, B);
	}

	return glm::vec2(0.0f);
}

glm::bvec2 StandardNodeTest::GetRandomBVec2Value()
{
	glm::bvec2 RandomValue;
	RandomValue.x = GetRandomBoolValue();
	RandomValue.y = GetRandomBoolValue();
	return RandomValue;
}

BoolVec2LiteralNode* StandardNodeTest::BVec2ToBVec2LiteralNode(glm::bvec2 Value)
{
	BoolVec2LiteralNode* NewNode = new BoolVec2LiteralNode();
	NewNode->SetData(Value);
	return NewNode;
}

BoolVec2VariableNode* StandardNodeTest::BVec2ToBVec2VariableNode(glm::bvec2 Value)
{
	BoolVec2VariableNode* NewNode = new BoolVec2VariableNode();
	NewNode->SetData(Value);
	return NewNode;
}

TEST_F(StandardNodeTest, ControlFlow)
{
	NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestNodeArea, nullptr);

	ASSERT_EQ(TestNodeArea->LoadFromFile("Resources/StandardNodesControlFlowTestData.json"), true);
	TestNodeArea->SetSaveExecutedNodes(true);

	ASSERT_EQ(TestNodeArea->GetNodeCount(), 25);
	ASSERT_EQ(TestNodeArea->GetConnectionCount(), 38);
	ASSERT_EQ(TestNodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(TestNodeArea->GetRerouteConnectionCount(), 7);

	// Checking all values before execution.
	Node* CurrentNodeToCheck = TestNodeArea->GetNodeByID("01346F1E415A11087D3C2B54");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "IntegerVariableNode");
	IntegerVariableNode* CurrentIntegerVariableNode = static_cast<IntegerVariableNode*>(CurrentNodeToCheck);
	int IntegerVariableValue = CurrentIntegerVariableNode->GetData();
	ASSERT_EQ(IntegerVariableValue, 0);

	CurrentNodeToCheck = TestNodeArea->GetNodeByID("42342D5A04082E7864505104");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "IntegerVariableNode");
	CurrentIntegerVariableNode = static_cast<IntegerVariableNode*>(CurrentNodeToCheck);
	IntegerVariableValue = CurrentIntegerVariableNode->GetData();
	ASSERT_EQ(IntegerVariableValue, 0);

	CurrentNodeToCheck = TestNodeArea->GetNodeByID("146417590D0A1B7C12085A1B");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "FloatVariableNode");
	FloatVariableNode* CurrentFloatVariableNode = static_cast<FloatVariableNode*>(CurrentNodeToCheck);
	float FloatVariableValue = CurrentFloatVariableNode->GetData();
	ASSERT_EQ(FloatVariableValue, 1000.0f);

	CurrentNodeToCheck = TestNodeArea->GetNodeByID("7D07343C414B192B543A575D");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "IntegerVariableNode");
	CurrentIntegerVariableNode = static_cast<IntegerVariableNode*>(CurrentNodeToCheck);
	IntegerVariableValue = CurrentIntegerVariableNode->GetData();
	ASSERT_EQ(IntegerVariableValue, 0);

	CurrentNodeToCheck = TestNodeArea->GetNodeByID("563A457101403D1361667231");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "BoolVariableNode");
	BoolVariableNode* CurrentBoolVariableNode = static_cast<BoolVariableNode*>(CurrentNodeToCheck);
	bool bVariableValue = CurrentBoolVariableNode->GetData();
	ASSERT_EQ(bVariableValue, false);

	ASSERT_EQ(TestNodeArea->ExecuteNodeNetwork(), true);
	ASSERT_EQ(TestNodeArea->GetLastExecutedNodes().size(), 68);

	// Checking all values after execution.
	CurrentNodeToCheck = TestNodeArea->GetNodeByID("01346F1E415A11087D3C2B54");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "IntegerVariableNode");
	CurrentIntegerVariableNode = static_cast<IntegerVariableNode*>(CurrentNodeToCheck);
	IntegerVariableValue = CurrentIntegerVariableNode->GetData();
	ASSERT_EQ(IntegerVariableValue, 9);

	CurrentNodeToCheck = TestNodeArea->GetNodeByID("42342D5A04082E7864505104");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "IntegerVariableNode");
	CurrentIntegerVariableNode = static_cast<IntegerVariableNode*>(CurrentNodeToCheck);
	IntegerVariableValue = CurrentIntegerVariableNode->GetData();
	ASSERT_EQ(IntegerVariableValue, 45);

	CurrentNodeToCheck = TestNodeArea->GetNodeByID("146417590D0A1B7C12085A1B");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "FloatVariableNode");
	CurrentFloatVariableNode = static_cast<FloatVariableNode*>(CurrentNodeToCheck);
	FloatVariableValue = CurrentFloatVariableNode->GetData();
	ASSERT_EQ(FloatVariableValue, 1.0f);

	CurrentNodeToCheck = TestNodeArea->GetNodeByID("7D07343C414B192B543A575D");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "IntegerVariableNode");
	CurrentIntegerVariableNode = static_cast<IntegerVariableNode*>(CurrentNodeToCheck);
	IntegerVariableValue = CurrentIntegerVariableNode->GetData();
	ASSERT_EQ(IntegerVariableValue, 3);

	CurrentNodeToCheck = TestNodeArea->GetNodeByID("563A457101403D1361667231");
	ASSERT_NE(CurrentNodeToCheck, nullptr);
	ASSERT_EQ(CurrentNodeToCheck->GetType(), "BoolVariableNode");
	CurrentBoolVariableNode = static_cast<BoolVariableNode*>(CurrentNodeToCheck);
	bVariableValue = CurrentBoolVariableNode->GetData();
	ASSERT_EQ(bVariableValue, true);

	NODE_SYSTEM.DeleteNodeArea(TestNodeArea);
}