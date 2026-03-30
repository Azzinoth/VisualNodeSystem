#include "ExecutionFlowNodesTests.h"
using namespace VisNodeSys;

// Using a fixed seed to make it easier to debug.
#define RANDOM_SEED 42
#define RANDOM_ACTIONS_ITERATIONS 1000

TEST(ExecutionFlowNodesTests, LogicalOperators)
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

TEST(ExecutionFlowNodesTests, LogicalOperatorsRandom)
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
		bool bFirst = TEST_TOOLS.GetRandomBoolValue();
		bool bSecond = TEST_TOOLS.GetRandomBoolValue();
		LogicalNodeOperatorType Type = TEST_TOOLS.GetRandomLogicalOperatorType();
		bool Result = TEST_TOOLS.GetResultFromLogicalOperator(Type, bFirst, bSecond);

		BoolLiteralNode* FirstBoolNode = TEST_TOOLS.CreateBoolLiteralNode(bFirst);
		ASSERT_NE(FirstBoolNode, nullptr);
		BoolLiteralNode* SecondBoolNode = TEST_TOOLS.CreateBoolLiteralNode(bSecond);
		ASSERT_NE(SecondBoolNode, nullptr);

		BaseLogicalOperatorNode* LogicalOperatorNode = TEST_TOOLS.CreateBaseLogicalOperatorNode(Type);
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

		TestNodeArea->Delete(FirstBoolNode);
		TestNodeArea->Delete(SecondBoolNode);
		TestNodeArea->Delete(LogicalOperatorNode);
		TestNodeArea->Delete(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}
}

TEST(ExecutionFlowNodesTests, CompareOperatorsRandom)
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
		int First = TEST_TOOLS.GetRandomIntValue();
		int Second = TEST_TOOLS.GetRandomIntValue();
		ComparisonNodeOperatorType Type = TEST_TOOLS.GetRandomComparisonOperatorType();
		bool Result = TEST_TOOLS.GetResultFromComparisonOperator(Type, First, Second);

		IntegerLiteralNode* FirstNode = TEST_TOOLS.CreateIntegerLiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		IntegerLiteralNode* SecondNode = TEST_TOOLS.CreateIntegerLiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseComparisonOperatorNode* ComparisonOperatorNode = TEST_TOOLS.CreateBaseComparisonOperatorNode(Type);
		ASSERT_NE(ComparisonOperatorNode, nullptr);

		BoolVariableNode* ResultNode = TEST_TOOLS.CreateBoolVariableNode(false);
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

		TestNodeArea->Delete(FirstNode);
		TestNodeArea->Delete(SecondNode);
		TestNodeArea->Delete(ComparisonOperatorNode);
		TestNodeArea->Delete(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		float First = TEST_TOOLS.GetRandomFloatValue();
		float Second = TEST_TOOLS.GetRandomFloatValue();
		ComparisonNodeOperatorType Type = TEST_TOOLS.GetRandomComparisonOperatorType();
		bool Result = TEST_TOOLS.GetResultFromComparisonOperator(Type, First, Second);

		FloatLiteralNode* FirstNode = TEST_TOOLS.CreateFloatLiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		FloatLiteralNode* SecondNode = TEST_TOOLS.CreateFloatLiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseComparisonOperatorNode* ComparisonOperatorNode = TEST_TOOLS.CreateBaseComparisonOperatorNode(Type);
		ASSERT_NE(ComparisonOperatorNode, nullptr);

		BoolVariableNode* ResultNode = TEST_TOOLS.CreateBoolVariableNode(false);
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

		TestNodeArea->Delete(FirstNode);
		TestNodeArea->Delete(SecondNode);
		TestNodeArea->Delete(ComparisonOperatorNode);
		TestNodeArea->Delete(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		glm::vec2 First = TEST_TOOLS.GetRandomVec2Value();
		glm::vec2 Second = TEST_TOOLS.GetRandomVec2Value();
		ComparisonNodeOperatorType Type = TEST_TOOLS.GetRandomComparisonOperatorType();
		glm::bvec2 Result = TEST_TOOLS.GetResultFromComparisonOperator(Type, First, Second);

		Vec2LiteralNode* FirstNode = TEST_TOOLS.CreateVec2LiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		Vec2LiteralNode* SecondNode = TEST_TOOLS.CreateVec2LiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseComparisonOperatorNode* ComparisonOperatorNode = TEST_TOOLS.CreateBaseComparisonOperatorNode(Type);
		ASSERT_NE(ComparisonOperatorNode, nullptr);

		BoolVec2VariableNode* ResultNode = TEST_TOOLS.CreateBVec2VariableNode(glm::bvec2(false));
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

		TestNodeArea->Delete(FirstNode);
		TestNodeArea->Delete(SecondNode);
		TestNodeArea->Delete(ComparisonOperatorNode);
		TestNodeArea->Delete(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}
}

TEST(ExecutionFlowNodesTests, ArithmeticOperatorsRandom)
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
		int First = TEST_TOOLS.GetRandomIntValue();
		int Second = TEST_TOOLS.GetRandomIntValue();
		ArithmeticOperationType Type = TEST_TOOLS.GetRandomArithmeticOperatorType();
		int Result = TEST_TOOLS.GetResultFromArithmeticOperator(Type, First, Second);

		IntegerLiteralNode* FirstNode = TEST_TOOLS.CreateIntegerLiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		IntegerLiteralNode* SecondNode = TEST_TOOLS.CreateIntegerLiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseArithmeticOperatorNode* ArithmeticOperatorNode = TEST_TOOLS.CreateBaseArithmeticOperatorNode(Type);
		ASSERT_NE(ArithmeticOperatorNode, nullptr);

		IntegerVariableNode* ResultNode = TEST_TOOLS.CreateIntegerVariableNode(0);
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

		TestNodeArea->Delete(FirstNode);
		TestNodeArea->Delete(SecondNode);
		TestNodeArea->Delete(ArithmeticOperatorNode);
		TestNodeArea->Delete(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		float First = TEST_TOOLS.GetRandomFloatValue();
		float Second = TEST_TOOLS.GetRandomFloatValue();
		ArithmeticOperationType Type = TEST_TOOLS.GetRandomArithmeticOperatorType();
		float Result = TEST_TOOLS.GetResultFromArithmeticOperator(Type, First, Second);

		FloatLiteralNode* FirstNode = TEST_TOOLS.CreateFloatLiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		FloatLiteralNode* SecondNode = TEST_TOOLS.CreateFloatLiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseArithmeticOperatorNode* ArithmeticOperatorNode = TEST_TOOLS.CreateBaseArithmeticOperatorNode(Type);
		ASSERT_NE(ArithmeticOperatorNode, nullptr);

		FloatVariableNode* ResultNode = TEST_TOOLS.CreateFloatVariableNode(0);
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

		TestNodeArea->Delete(FirstNode);
		TestNodeArea->Delete(SecondNode);
		TestNodeArea->Delete(ArithmeticOperatorNode);
		TestNodeArea->Delete(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}

	srand(RANDOM_SEED);
	for (size_t i = 0; i < RANDOM_ACTIONS_ITERATIONS; i++)
	{
		glm::vec2 First = TEST_TOOLS.GetRandomVec2Value();
		glm::vec2 Second = TEST_TOOLS.GetRandomVec2Value();
		ArithmeticOperationType Type = TEST_TOOLS.GetRandomArithmeticOperatorType();
		if (Type == ArithmeticOperationType::POWER || Type == ArithmeticOperationType::MODULUS)
		{
			// Modus and Power operators currently are not supported for vec2.
			continue;
		}
		glm::vec2 Result = TEST_TOOLS.GetResultFromArithmeticOperator(Type, First, Second);

		Vec2LiteralNode* FirstNode = TEST_TOOLS.CreateVec2LiteralNode(First);
		ASSERT_NE(FirstNode, nullptr);
		Vec2LiteralNode* SecondNode = TEST_TOOLS.CreateVec2LiteralNode(Second);
		ASSERT_NE(SecondNode, nullptr);

		BaseArithmeticOperatorNode* ArithmeticOperatorNode = TEST_TOOLS.CreateBaseArithmeticOperatorNode(Type);
		ASSERT_NE(ArithmeticOperatorNode, nullptr);

		Vec2VariableNode* ResultNode = TEST_TOOLS.CreateVec2VariableNode(glm::vec2(0.0f));
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

		TestNodeArea->Delete(FirstNode);
		TestNodeArea->Delete(SecondNode);
		TestNodeArea->Delete(ArithmeticOperatorNode);
		TestNodeArea->Delete(ResultNode);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 1);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 0);
	}
}

TEST(ExecutionFlowNodesTests, ControlFlow)
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

TEST(ExecutionFlowNodesTests, SavingLoading_RandomArithmetic_Calculation)
{
	srand(RANDOM_SEED);

	for (int i = 0; i < RANDOM_ACTIONS_ITERATIONS / 20; i++)
	{
		NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
		ASSERT_NE(TestNodeArea, nullptr);
		std::string NodeAreaID = TestNodeArea->GetID();
		TestNodeArea->SetSaveExecutedNodes(true);

		Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
		ASSERT_NE(BeginNode, nullptr);
		TestNodeArea->AddNode(BeginNode);
		TestNodeArea->SetExecutionEntryNode(BeginNode);

		// Create first two int literal nodes with random values.
		int FirstValue = TEST_TOOLS.GetRandomIntValue();
		IntegerLiteralNode* FirstIntegerLiteralNode = TEST_TOOLS.CreateIntegerLiteralNode(FirstValue);
		TestNodeArea->AddNode(FirstIntegerLiteralNode);

		int SecondValue = TEST_TOOLS.GetRandomIntValue();
		IntegerLiteralNode* SecondIntegerLiteralNode = TEST_TOOLS.CreateIntegerLiteralNode(SecondValue);
		TestNodeArea->AddNode(SecondIntegerLiteralNode);

		Node* CurrentFirstInputNode = nullptr;
		Node* CurrentSecondInputNode = nullptr;

		BaseArithmeticOperatorNode* PreviousArithmeticOperatorNode = nullptr;
		BaseArithmeticOperatorNode* CurrentArithmeticOperatorNode = nullptr;

		IntegerVariableNode* PreviousResultNode = nullptr;
		IntegerVariableNode* CurrentResultNode = nullptr;

		int IterationCount = 0;
		while (true)
		{
			// We should have at least one operation node.
			if (IterationCount > 0)
			{
				// With 5% chance we will stop generating.
				if (rand() % 100 > 95)
					break;
			}

			ArithmeticOperationType Type = TEST_TOOLS.GetRandomArithmeticOperatorType();
			// Skipping these operators for int.
			if (Type == ArithmeticOperationType::POWER || Type == ArithmeticOperationType::MODULUS)
				continue;

			BaseArithmeticOperatorNode* ArithmeticOperatorNode = TEST_TOOLS.CreateBaseArithmeticOperatorNode(Type);
			TestNodeArea->AddNode(ArithmeticOperatorNode);
			PreviousArithmeticOperatorNode = CurrentArithmeticOperatorNode;
			CurrentArithmeticOperatorNode = ArithmeticOperatorNode;

			if (IterationCount == 0)
			{
				CurrentFirstInputNode = FirstIntegerLiteralNode;
				CurrentSecondInputNode = SecondIntegerLiteralNode;
			}
			else
			{
				CurrentFirstInputNode = CurrentResultNode;

				int RandomValue = TEST_TOOLS.GetRandomIntValue();
				CurrentSecondInputNode = TEST_TOOLS.CreateIntegerLiteralNode(SecondValue);
				TestNodeArea->AddNode(CurrentSecondInputNode);
			}

			IntegerVariableNode* ResultNode = TEST_TOOLS.CreateIntegerVariableNode(0);
			TestNodeArea->AddNode(ResultNode);
			PreviousResultNode = CurrentResultNode;
			CurrentResultNode = ResultNode;

			if (IterationCount == 0)
			{
				ASSERT_EQ(TestNodeArea->TryToConnect(CurrentFirstInputNode, 0, ArithmeticOperatorNode, 1), true);
			}
			else
			{
				ASSERT_EQ(TestNodeArea->TryToConnect(CurrentFirstInputNode, 1, ArithmeticOperatorNode, 1), true);
			}

			ASSERT_EQ(TestNodeArea->TryToConnect(CurrentSecondInputNode, 0, ArithmeticOperatorNode, 2), true);
			ASSERT_EQ(TestNodeArea->TryToConnect(ArithmeticOperatorNode, 0, ResultNode, 0), true);
			ASSERT_EQ(TestNodeArea->TryToConnect(ArithmeticOperatorNode, 1, ResultNode, 1), true);

			if (IterationCount == 0)
			{
				ASSERT_EQ(TestNodeArea->TryToConnect(BeginNode, 0, ArithmeticOperatorNode, 0), true);
			}
			else
			{
				ASSERT_EQ(TestNodeArea->TryToConnect(PreviousResultNode, 0, ArithmeticOperatorNode, 0), true);
			}

			IterationCount++;
		}

		ASSERT_EQ(TestNodeArea->TryToConnect(CurrentArithmeticOperatorNode, 0, CurrentResultNode, 0), true);
		TestNodeArea->ExecuteNodeNetwork();
		std::string FinalResultNodeID = CurrentResultNode->GetID();
		int FinalResultValue = CurrentResultNode->GetData();
		size_t ExecutedNodesCount = TestNodeArea->GetLastExecutedNodes().size();

		// Now we will save the node area and load it again to check if we get the same result.
		// We will start with bacis checks.
		EXPECT_TRUE(TEST_TOOLS.VerifyNodeAreaSaveLoadCycle_BasicChecks(TestNodeArea));

		// VerifyNodeAreaSaveLoadCycle_BasicChecks already saved node area to "TemporaryNodeArea.json".
		TestNodeArea = NODE_SYSTEM.GetNodeAreaByID(NodeAreaID);
		NODE_SYSTEM.DeleteNodeArea(TestNodeArea);

		TestNodeArea = NODE_SYSTEM.CreateNodeArea();
		EXPECT_TRUE(TestNodeArea->LoadFromFile("TemporaryNodeArea.json"));
		TestNodeArea->SetSaveExecutedNodes(true);

		Node* LoadedFinalResultNode = TestNodeArea->GetNodeByID(FinalResultNodeID);
		ASSERT_NE(LoadedFinalResultNode, nullptr);
		IntegerVariableNode* CastedLoadedFinalResultNode = reinterpret_cast<IntegerVariableNode*>(LoadedFinalResultNode);
		CastedLoadedFinalResultNode->SetData(-INT_MAX);

		TestNodeArea->ExecuteNodeNetwork();
		int LoadedFinalResultValue = CastedLoadedFinalResultNode->GetData();
		ASSERT_EQ(FinalResultValue, LoadedFinalResultValue);

		size_t LoadedExecutedNodesCount = TestNodeArea->GetLastExecutedNodes().size();
		ASSERT_EQ(ExecutedNodesCount, LoadedExecutedNodesCount);
	}
}