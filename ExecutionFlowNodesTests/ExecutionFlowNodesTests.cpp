#include "ExecutionFlowNodesTests.h"
using namespace VisNodeSys;

// Using a fixed seed to make it easier to debug.
#define RANDOM_SEED 42
#define RANDOM_ACTIONS_ITERATIONS 1000

TEST(ExecutionFlowNodesTests, LogicalOperators)
{
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(ExecutionFlowNodesTests, LogicalOperatorsRandom)
{
	NODE_SYSTEM.Clear();

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

TEST(ExecutionFlowNodesTests, LogicalOperators_ExecuteWithDisconnectedInputs_IsSafe)
{
	NODE_SYSTEM.Clear();

	const LogicalNodeOperatorType AllOperatorTypes[] = {
		LogicalNodeOperatorType::AND,
		LogicalNodeOperatorType::NOT,
		LogicalNodeOperatorType::OR,
		LogicalNodeOperatorType::XOR,
	};

	for (LogicalNodeOperatorType OperatorType : AllOperatorTypes)
	{
		NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
		ASSERT_NE(TestNodeArea, nullptr);

		Node* BeginNode = NODE_FACTORY.CreateNode("BeginNode");
		ASSERT_NE(BeginNode, nullptr);
		TestNodeArea->AddNode(BeginNode);
		ASSERT_TRUE(TestNodeArea->SetExecutionEntryNode(BeginNode));

		BaseLogicalOperatorNode* OperatorNode = TEST_TOOLS.CreateBaseLogicalOperatorNode(OperatorType);
		ASSERT_NE(OperatorNode, nullptr);
		TestNodeArea->AddNode(OperatorNode);

		// Wire only the execution flow, the A / B data inputs are deliberately left disconnected.
		ASSERT_TRUE(TestNodeArea->TryToConnect(BeginNode, 0, OperatorNode, 0));

		// Executing an operator with no data inputs must not crash or read out of bounds.
		ASSERT_TRUE(TestNodeArea->ExecuteNodeNetwork());

		NODE_SYSTEM.DeleteNodeArea(TestNodeArea);
	}
}

// Creates a standard node of the given type, serializes it, drops every input socket EXCEPT the EXECUTE input (index "0") then deserializes it back and try to execute it.
static void ExpectStandardNodeSurvivesMissingDataInputsOnExecute(const std::string& NodeType)
{
	Node* CurrentNode = NODE_FACTORY.CreateNode(NodeType);
	ASSERT_NE(CurrentNode, nullptr);

	Json::Value NodeJson = CurrentNode->ToJson();
	Json::Value& Inputs = NodeJson["Input"];
	const std::vector<std::string> InputKeys = Inputs.getMemberNames();
	for (size_t i = 0; i < InputKeys.size(); i++)
	{
		if (InputKeys[i] != "0")
			Inputs.removeMember(InputKeys[i]);
	}

	ASSERT_TRUE(CurrentNode->FromJson(NodeJson));

	NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_TRUE(TestNodeArea->AddNode(CurrentNode));
	ASSERT_TRUE(TestNodeArea->SetExecutionEntryNode(CurrentNode));

	TestNodeArea->ExecuteNodeNetwork();

	NODE_SYSTEM.DeleteNodeArea(TestNodeArea);
}

TEST(ExecutionFlowNodesTests, StandardNodes_LoadedWithMissingDataInputs_ExecuteWithoutCrashing)
{
	NODE_SYSTEM.Clear();

	const std::vector<std::string> NodeTypes = {
		"IntegerVariableNode", "FloatVariableNode", "BoolVariableNode",
		"Vec2VariableNode", "Vec3VariableNode", "Vec4VariableNode",
		"ArithmeticAddNode", "ArithmeticDivideNode",
		"EqualNode", "LessThanNode",
		"LogicalANDOperatorNode", "LogicalNOTOperatorNode", "LogicalXOROperatorNode",
		"BranchNode", "WhileLoopNode", "LoopNode", "SequenceNode",
	};

	for (size_t i = 0; i < NodeTypes.size(); i++)
		ExpectStandardNodeSurvivesMissingDataInputsOnExecute(NodeTypes[i]);
}

TEST(ExecutionFlowNodesTests, CompareOperatorsRandom)
{
	NODE_SYSTEM.Clear();

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
	NODE_SYSTEM.Clear();

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
	NODE_SYSTEM.Clear();

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

	NODE_SYSTEM.Clear();
}

TEST(ExecutionFlowNodesTests, SavingLoading_RandomArithmetic_Calculation)
{
	NODE_SYSTEM.Clear();

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

TEST(ExecutionFlowNodesTests, DeleteSocket_OnUndeletableStandardNodeSocket_IsRejected_AndCopyStaysSafe)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();

	// Every structural socket of a standard node must reject user deletion,
	// and the socket vectors must stay intact.
	auto ExpectAllSocketsUndeletable = [](Node* TestNode)
	{
		ASSERT_NE(TestNode, nullptr);
		const size_t InputCountBefore  = TestNode->GetInputSocketCount();
		const size_t OutputCountBefore = TestNode->GetOutputSocketCount();

		for (size_t Index = 0; Index < InputCountBefore; ++Index)
			EXPECT_FALSE(TestNode->DeleteSocket(TestNode->GetSocketByIndex(Index, NodeSocket::SocketFlow::Input)));

		for (size_t Index = 0; Index < OutputCountBefore; ++Index)
			EXPECT_FALSE(TestNode->DeleteSocket(TestNode->GetSocketByIndex(Index, NodeSocket::SocketFlow::Output)));

		EXPECT_EQ(TestNode->GetInputSocketCount(),  InputCountBefore);
		EXPECT_EQ(TestNode->GetOutputSocketCount(), OutputCountBefore);
	};

	// Variable nodes lock their structural sockets.
	FloatVariableNode* Variable = new FloatVariableNode();
	ASSERT_TRUE(Area->AddNode(Variable));
	ASSERT_GE(Variable->GetOutputSocketCount(), 2);
	ExpectAllSocketsUndeletable(Variable);

	// Control-flow nodes.
	LoopNode* Loop = new LoopNode();
	ASSERT_TRUE(Area->AddNode(Loop));
	ExpectAllSocketsUndeletable(Loop);

	WhileLoopNode* While = new WhileLoopNode();
	ASSERT_TRUE(Area->AddNode(While));
	ExpectAllSocketsUndeletable(While);

	BranchNode* Branch = new BranchNode();
	ASSERT_TRUE(Area->AddNode(Branch));
	ExpectAllSocketsUndeletable(Branch);

	// Operator nodes (sockets are locked in the operator base constructors).
	ArithmeticAddNode* Arithmetic = new ArithmeticAddNode();
	ASSERT_TRUE(Area->AddNode(Arithmetic));
	ExpectAllSocketsUndeletable(Arithmetic);

	EqualNode* Comparison = new EqualNode();
	ASSERT_TRUE(Area->AddNode(Comparison));
	ExpectAllSocketsUndeletable(Comparison);

	LogicalANDOperatorNode* Logical = new LogicalANDOperatorNode();
	ASSERT_TRUE(Area->AddNode(Logical));
	ExpectAllSocketsUndeletable(Logical);

	// A copied standard node keeps the same undeletable structural sockets.
	Node* Copy = NODE_FACTORY.CopyNode("FloatVariableNode", *Variable);
	ASSERT_NE(Copy, nullptr);
	EXPECT_TRUE(Area->AddNode(Copy));
	ExpectAllSocketsUndeletable(Copy);

	NODE_SYSTEM.DeleteNodeArea(Area);
	NODE_SYSTEM.Clear();
}

TEST(ExecutionFlowNodesTests, IntegerDivide_IntMinByNegativeOne_SaturatesWithoutCrashing)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	IntegerLiteralNode* IntegerA = new IntegerLiteralNode();
	IntegerA->SetData(INT_MIN);
	IntegerLiteralNode* IntegerB = new IntegerLiteralNode();
	IntegerB->SetData(-1);
	ArithmeticDivideNode* DivideNode = new ArithmeticDivideNode();
	ASSERT_TRUE(LocalNodeArea->AddNode(IntegerA));
	ASSERT_TRUE(LocalNodeArea->AddNode(IntegerB));
	ASSERT_TRUE(LocalNodeArea->AddNode(DivideNode));

	ASSERT_TRUE(LocalNodeArea->TryToConnect(IntegerA, 0, DivideNode, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(IntegerB, 0, DivideNode, 2));
	ASSERT_TRUE(LocalNodeArea->SetExecutionEntryNode(DivideNode));
	LocalNodeArea->ExecuteNodeNetwork();

	NodeSocket* ResultSocket = DivideNode->GetSocketByIndex(1, NodeSocket::SocketFlow::Output);
	ASSERT_NE(ResultSocket, nullptr);
	int Result = *reinterpret_cast<int*>(ResultSocket->GetData());
	EXPECT_EQ(Result, INT_MAX);

	NODE_SYSTEM.Clear();
}

TEST(ExecutionFlowNodesTests, IntegerModulus_IntMinByNegativeOne_ReturnsZeroWithoutCrashing)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	IntegerLiteralNode* IntegerA = new IntegerLiteralNode();
	IntegerA->SetData(INT_MIN);
	IntegerLiteralNode* IntegerB = new IntegerLiteralNode();
	IntegerB->SetData(-1);
	ArithmeticModulusNode* ModulusNode = new ArithmeticModulusNode();
	ASSERT_TRUE(LocalNodeArea->AddNode(IntegerA));
	ASSERT_TRUE(LocalNodeArea->AddNode(IntegerB));
	ASSERT_TRUE(LocalNodeArea->AddNode(ModulusNode));

	ASSERT_TRUE(LocalNodeArea->TryToConnect(IntegerA, 0, ModulusNode, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(IntegerB, 0, ModulusNode, 2));
	ASSERT_TRUE(LocalNodeArea->SetExecutionEntryNode(ModulusNode));
	LocalNodeArea->ExecuteNodeNetwork();

	NodeSocket* ResultSocket = ModulusNode->GetSocketByIndex(1, NodeSocket::SocketFlow::Output);
	ASSERT_NE(ResultSocket, nullptr);
	int Result = *reinterpret_cast<int*>(ResultSocket->GetData());
	EXPECT_EQ(Result, 0);

	NODE_SYSTEM.Clear();
}

TEST(ExecutionFlowNodesTests, ArithmeticChain_DownstreamOperatorInheritsUpstreamActiveType)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	FloatLiteralNode* FloatA = new FloatLiteralNode();
	FloatA->SetData(2.0f);
	FloatLiteralNode* FloatB = new FloatLiteralNode();
	FloatB->SetData(3.0f);
	ArithmeticMultiplyNode* MultiplyNode = new ArithmeticMultiplyNode();
	ArithmeticAddNode* AdditionNode = new ArithmeticAddNode();
	ASSERT_TRUE(LocalNodeArea->AddNode(FloatA));
	ASSERT_TRUE(LocalNodeArea->AddNode(FloatB));
	ASSERT_TRUE(LocalNodeArea->AddNode(MultiplyNode));
	ASSERT_TRUE(LocalNodeArea->AddNode(AdditionNode));

	ASSERT_TRUE(LocalNodeArea->TryToConnect(FloatA, 0, MultiplyNode, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(FloatB, 0, MultiplyNode, 2));
	// Multiply's Result feeds Addition's A input, and its execution drives Addition.
	ASSERT_TRUE(LocalNodeArea->TryToConnect(MultiplyNode, 1, AdditionNode, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(MultiplyNode, 0, AdditionNode, 0));

	// Both operators resolve to FLOAT.
	EXPECT_EQ(MultiplyNode->GetActiveINDataType(), "FLOAT");
	EXPECT_EQ(AdditionNode->GetActiveINDataType(), "FLOAT");

	ASSERT_TRUE(LocalNodeArea->SetExecutionEntryNode(MultiplyNode));
	LocalNodeArea->ExecuteNodeNetwork();

	// Multiply = 2 * 3 = 6; Addition reads that 6.0 as a float (B disconnected = 0) = 6.0.
	NodeSocket* AdditionResultSocket = AdditionNode->GetSocketByIndex(1, NodeSocket::SocketFlow::Output);
	ASSERT_NE(AdditionResultSocket, nullptr);
	float AdditionResult = *reinterpret_cast<float*>(AdditionResultSocket->GetData());
	EXPECT_FLOAT_EQ(AdditionResult, 6.0f);

	NODE_SYSTEM.Clear();
}

TEST(ExecutionFlowNodesTests, VectorModulus_ComputesComponentWiseRemainder)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	Vec2LiteralNode* FirstVector = new Vec2LiteralNode();
	FirstVector->SetData(glm::vec2(7.0f, 5.0f));
	Vec2LiteralNode* SecondVector = new Vec2LiteralNode();
	SecondVector->SetData(glm::vec2(3.0f, 2.0f));
	ArithmeticModulusNode* ModulusNode = new ArithmeticModulusNode();
	ASSERT_TRUE(LocalNodeArea->AddNode(FirstVector));
	ASSERT_TRUE(LocalNodeArea->AddNode(SecondVector));
	ASSERT_TRUE(LocalNodeArea->AddNode(ModulusNode));

	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstVector, 0, ModulusNode, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(SecondVector, 0, ModulusNode, 2));
	ASSERT_EQ(ModulusNode->GetActiveINDataType(), "VEC2");

	ASSERT_TRUE(LocalNodeArea->SetExecutionEntryNode(ModulusNode));
	LocalNodeArea->ExecuteNodeNetwork();

	NodeSocket* ResultSocket = ModulusNode->GetSocketByIndex(1, NodeSocket::SocketFlow::Output);
	ASSERT_NE(ResultSocket, nullptr);
	glm::vec2 Result = *reinterpret_cast<glm::vec2*>(ResultSocket->GetData());

	// 7 mod 3 = 1, 5 mod 2 = 1.
	EXPECT_FLOAT_EQ(Result.x, 1.0f);
	EXPECT_FLOAT_EQ(Result.y, 1.0f);

	NODE_SYSTEM.Clear();
}

TEST(ExecutionFlowNodesTests, BranchNode_ConditionRejectsVectorComparison_AcceptsBool)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	// VEC2-mode comparison: its Result is a bvec, so the bool condition must reject it.
	Vec2LiteralNode* FirstVector = new Vec2LiteralNode();
	FirstVector->SetData(glm::vec2(0.0f, 5.0f));
	Vec2LiteralNode* SecondVector = new Vec2LiteralNode();
	SecondVector->SetData(glm::vec2(1.0f, 1.0f));
	GreaterThanNode* VectorComparison = new GreaterThanNode();
	BranchNode* VectorBranch = new BranchNode();
	ASSERT_TRUE(LocalNodeArea->AddNode(FirstVector));
	ASSERT_TRUE(LocalNodeArea->AddNode(SecondVector));
	ASSERT_TRUE(LocalNodeArea->AddNode(VectorComparison));
	ASSERT_TRUE(LocalNodeArea->AddNode(VectorBranch));

	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstVector, 0, VectorComparison, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(SecondVector, 0, VectorComparison, 2));
	ASSERT_EQ(VectorComparison->GetActiveINDataType(), "VEC2");

	EXPECT_FALSE(LocalNodeArea->TryToConnect(VectorComparison, 1, VectorBranch, 1));

	// INT-mode comparison: its Result is BOOL, a valid branch condition.
	IntegerLiteralNode* FirstInt = new IntegerLiteralNode();
	FirstInt->SetData(5);
	IntegerLiteralNode* SecondInt = new IntegerLiteralNode();
	SecondInt->SetData(1);
	GreaterThanNode* ScalarComparison = new GreaterThanNode();
	BranchNode* ScalarBranch = new BranchNode();
	ASSERT_TRUE(LocalNodeArea->AddNode(FirstInt));
	ASSERT_TRUE(LocalNodeArea->AddNode(SecondInt));
	ASSERT_TRUE(LocalNodeArea->AddNode(ScalarComparison));
	ASSERT_TRUE(LocalNodeArea->AddNode(ScalarBranch));

	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstInt, 0, ScalarComparison, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(SecondInt, 0, ScalarComparison, 2));
	ASSERT_EQ(ScalarComparison->GetActiveINDataType(), "INT");

	EXPECT_TRUE(LocalNodeArea->TryToConnect(ScalarComparison, 1, ScalarBranch, 1));

	NODE_SYSTEM.Clear();
}

TEST(ExecutionFlowNodesTests, ComparisonNode_MultiTypeProducer_ResolvesProducerType)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	// Two arithmetic producers, each outputting a negative FLOAT.
	FloatLiteralNode* FloatA = new FloatLiteralNode();
	FloatA->SetData(-2.0f);
	FloatLiteralNode* FloatB = new FloatLiteralNode();
	FloatB->SetData(-1.0f);
	ArithmeticAddNode* FirstAddNode = new ArithmeticAddNode();  // Result = -2.0 (B disconnected = 0).
	ArithmeticAddNode* SecondAddNode = new ArithmeticAddNode(); // Result = -1.0.
	LessThanNode* CompareNode = new LessThanNode();

	ASSERT_TRUE(LocalNodeArea->AddNode(FloatA));
	ASSERT_TRUE(LocalNodeArea->AddNode(FloatB));
	ASSERT_TRUE(LocalNodeArea->AddNode(FirstAddNode));
	ASSERT_TRUE(LocalNodeArea->AddNode(SecondAddNode));
	ASSERT_TRUE(LocalNodeArea->AddNode(CompareNode));

	// Data: literal => add node A input.
	ASSERT_TRUE(LocalNodeArea->TryToConnect(FloatA, 0, FirstAddNode, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(FloatB, 0, SecondAddNode, 1));
	// Data: add node Result -> comparison A and B inputs.
	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstAddNode, 1, CompareNode, 1));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(SecondAddNode, 1, CompareNode, 2));
	// Execution chain: FirstAddNode => SecondAddNode => CompareNode.
	ASSERT_TRUE(LocalNodeArea->TryToConnect(FirstAddNode, 0, SecondAddNode, 0));
	ASSERT_TRUE(LocalNodeArea->TryToConnect(SecondAddNode, 0, CompareNode, 0));

	// Producers correctly self-identify as FLOAT.
	EXPECT_EQ(FirstAddNode->GetActiveINDataType(), "FLOAT");
	EXPECT_EQ(SecondAddNode->GetActiveINDataType(), "FLOAT");
	EXPECT_EQ(CompareNode->GetActiveINDataType(), "FLOAT");

	ASSERT_TRUE(LocalNodeArea->SetExecutionEntryNode(FirstAddNode));
	LocalNodeArea->ExecuteNodeNetwork();

	NodeSocket* ComparisonResultSocket = CompareNode->GetSocketByIndex(1, NodeSocket::SocketFlow::Output);
	ASSERT_NE(ComparisonResultSocket, nullptr);
	bool ComparisonResult = *reinterpret_cast<bool*>(ComparisonResultSocket->GetData());
	EXPECT_TRUE(ComparisonResult);

	NODE_SYSTEM.Clear();
}