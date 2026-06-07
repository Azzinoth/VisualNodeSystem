#include "ExecutionEntryPointTests.h"
using namespace VisNodeSys;

TEST(ExecutionEntryPointTests, RejectsNodeWithoutExecuteOutput)
{
	NODE_SYSTEM.Clear();

	NodeArea* TestArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestArea, nullptr);

	IntegerLiteralNode* LiteralNode = new IntegerLiteralNode();
	TestArea->AddNode(LiteralNode);

	EXPECT_FALSE(TestArea->SetExecutionEntryNode(LiteralNode));

	NODE_SYSTEM.Clear();
}

TEST(ExecutionEntryPointTests, RejectsNodeWithNoSockets)
{
	NODE_SYSTEM.Clear();

	NodeArea* TestArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestArea, nullptr);

	EventCountingNode* CountingNode = new EventCountingNode();
	TestArea->AddNode(CountingNode);

	EXPECT_FALSE(TestArea->SetExecutionEntryNode(CountingNode));

	NODE_SYSTEM.Clear();
}

TEST(ExecutionEntryPointTests, SubAreaInputNodeAsEntryDoesNotCrash)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	OwnedArea->SetSaveExecutedNodes(true);

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	ASSERT_NE(InputNode, nullptr);

	EXPECT_TRUE(OwnedArea->SetExecutionEntryNode(InputNode));
	EXPECT_NO_FATAL_FAILURE(OwnedArea->ExecuteNodeNetwork());

	NODE_SYSTEM.Clear();
}

TEST(ExecutionEntryPointTests, SubAreaInputNodeAsEntryPropagatesDownstream)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	OwnedArea->SetSaveExecutedNodes(true);

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	ASSERT_NE(InputNode, nullptr);

	IntegerVariableNode* Downstream = TEST_TOOLS.CreateIntegerVariableNode(0);
	OwnedArea->AddNode(Downstream);
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, Downstream, 0));

	EXPECT_TRUE(OwnedArea->SetExecutionEntryNode(InputNode));
	EXPECT_TRUE(OwnedArea->ExecuteNodeNetwork());

	auto Executed = OwnedArea->GetLastExecutedNodes();
	bool bDownstreamRan = false;
	for (Node* CurrentNode : Executed)
	{
		if (CurrentNode == Downstream)
		{
			bDownstreamRan = true;
			break;
		}
	}
	EXPECT_TRUE(bDownstreamRan);

	NODE_SYSTEM.Clear();
}

// "Run From Here" use case: pick a mid-graph execution flow node as the entry.
// Begin -> A -> B -> C, set B as entry. Expectation: B and C execute, A and Begin do not.
TEST(ExecutionEntryPointTests, MidGraphIntegerVariableNodeAsEntry)
{
	NODE_SYSTEM.Clear();

	NodeArea* TestArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestArea, nullptr);
	TestArea->SetSaveExecutedNodes(true);

	BeginNode* Begin = new BeginNode();
	IntegerVariableNode* A = TEST_TOOLS.CreateIntegerVariableNode(0);
	IntegerVariableNode* B = TEST_TOOLS.CreateIntegerVariableNode(0);
	IntegerVariableNode* C = TEST_TOOLS.CreateIntegerVariableNode(0);

	TestArea->AddNode(Begin);
	TestArea->AddNode(A);
	TestArea->AddNode(B);
	TestArea->AddNode(C);

	ASSERT_TRUE(TestArea->TryToConnect(Begin, 0, A, 0));
	ASSERT_TRUE(TestArea->TryToConnect(A, 0, B, 0));
	ASSERT_TRUE(TestArea->TryToConnect(B, 0, C, 0));

	EXPECT_TRUE(TestArea->SetExecutionEntryNode(B));
	EXPECT_TRUE(TestArea->ExecuteNodeNetwork());

	auto Executed = TestArea->GetLastExecutedNodes();
	auto WasExecuted = [&](Node* CurrentNode) {
		for (Node* Other : Executed)
			if (Other == CurrentNode) return true;

		return false;
	};

	EXPECT_FALSE(WasExecuted(Begin));
	EXPECT_FALSE(WasExecuted(A));
	EXPECT_TRUE(WasExecuted(B));
	EXPECT_TRUE(WasExecuted(C));

	NODE_SYSTEM.Clear();
}

namespace
{
	// Node that records the OwnSocket pointer SocketEvent was last invoked with for an EXECUTE event.
	class OrphanObserverNode : public BaseExecutionFlowNode
	{
	public:
		NodeSocket* LastReceivedOwnSocket = nullptr;
		int ExecuteCallCount = 0;

		OrphanObserverNode() : BaseExecutionFlowNode()
		{
			Type = "OrphanObserverNode";
			AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
		}

		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType) override
		{
			Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);
			if (EventType == EXECUTE)
			{
				LastReceivedOwnSocket = OwnSocket;
				ExecuteCallCount++;
			}
		}
	};
}

TEST(ExecutionEntryPointTests, OrphanTriggerPassesFirstExecuteInputAsOwnSocket)
{
	NODE_SYSTEM.Clear();

	NodeArea* TestArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestArea, nullptr);

	OrphanObserverNode* Entry = new OrphanObserverNode();
	TestArea->AddNode(Entry);

	NodeSocket* ExpectedSocket = Entry->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	ASSERT_NE(ExpectedSocket, nullptr);
	std::vector<std::string> AllowedTypes = ExpectedSocket->GetAllowedTypes();
	bool bIsExecute = false;
	for (size_t i = 0; i < AllowedTypes.size(); i++)
	{
		if (AllowedTypes[i] == "EXECUTE")
		{
			bIsExecute = true;
			break;
		}
	}
	ASSERT_TRUE(bIsExecute);

	ASSERT_TRUE(TestArea->SetExecutionEntryNode(Entry));
	ASSERT_TRUE(TestArea->ExecuteNodeNetwork());

	EXPECT_EQ(Entry->ExecuteCallCount, 1);
	EXPECT_EQ(Entry->LastReceivedOwnSocket, ExpectedSocket);

	NODE_SYSTEM.Clear();
}

namespace
{
	// Entry-only observer — mimics BeginNode / SubAreaInputNode shape: no
	// EXECUTE input (constructed with BaseExecutionFlowNode(false)), only an
	// EXECUTE output. Used to verify the fallback path of Option A: when there
	// is no EXECUTE input, the framework must hand us our first EXECUTE output.
	class EntryOnlyObserverNode : public BaseExecutionFlowNode
	{
	public:
		NodeSocket* LastReceivedOwnSocket = nullptr;
		int ExecuteCallCount = 0;

		EntryOnlyObserverNode() : BaseExecutionFlowNode(false)
		{
			Type = "EntryOnlyObserverNode";
			AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
		}

		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType) override
		{
			Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);
			if (EventType == EXECUTE)
			{
				LastReceivedOwnSocket = OwnSocket;
				ExecuteCallCount++;
			}
		}
	};

	// Node whose first input is INT, not EXECUTE.
	class NonStandardInputOrderNode : public Node
	{
	public:
		NodeSocket* LastReceivedOwnSocket = nullptr;
		int ExecuteCallCount = 0;

		NonStandardInputOrderNode() : Node()
		{
			Type = "NonStandardInputOrderNode";
			AddSocket(new NodeSocket(this, "INT", "Data", NodeSocket::SocketFlow::Input));
			AddSocket(new NodeSocket(this, "EXECUTE", "Go", NodeSocket::SocketFlow::Input));
			AddSocket(new NodeSocket(this, "EXECUTE", "", NodeSocket::SocketFlow::Output));
		}

		void SocketEvent(NodeSocket* OwnSocket, NodeSocket* ConnectedSocket, NODE_SOCKET_EVENT EventType) override
		{
			Node::SocketEvent(OwnSocket, ConnectedSocket, EventType);
			if (EventType == EXECUTE)
			{
				LastReceivedOwnSocket = OwnSocket;
				ExecuteCallCount++;
			}
		}
	};
}

TEST(ExecutionEntryPointTests, OrphanTriggerFallbackToFirstExecuteOutputWhenNoExecuteInput)
{
	NODE_SYSTEM.Clear();

	NodeArea* TestArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestArea, nullptr);

	EntryOnlyObserverNode* Entry = new EntryOnlyObserverNode();
	TestArea->AddNode(Entry);

	NodeSocket* ExpectedSocket = Entry->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	ASSERT_NE(ExpectedSocket, nullptr);

	ASSERT_TRUE(TestArea->SetExecutionEntryNode(Entry));
	ASSERT_TRUE(TestArea->ExecuteNodeNetwork());

	EXPECT_EQ(Entry->ExecuteCallCount, 1);
	EXPECT_EQ(Entry->LastReceivedOwnSocket, ExpectedSocket);

	NODE_SYSTEM.Clear();
}

TEST(ExecutionEntryPointTests, OrphanTriggerSkipsNonExecuteInputsToFindFirstExecuteInput)
{
	NODE_SYSTEM.Clear();

	NodeArea* TestArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestArea, nullptr);

	NonStandardInputOrderNode* Entry = new NonStandardInputOrderNode();
	TestArea->AddNode(Entry);

	NodeSocket* IntInput = Entry->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	NodeSocket* ExecuteInput = Entry->GetSocketByIndex(1, NodeSocket::SocketFlow::Input);
	ASSERT_NE(IntInput, nullptr);
	ASSERT_NE(ExecuteInput, nullptr);

	ASSERT_TRUE(TestArea->SetExecutionEntryNode(Entry));
	ASSERT_TRUE(TestArea->ExecuteNodeNetwork());

	EXPECT_EQ(Entry->ExecuteCallCount, 1);
	EXPECT_NE(Entry->LastReceivedOwnSocket, IntInput);
	EXPECT_EQ(Entry->LastReceivedOwnSocket, ExecuteInput);

	NODE_SYSTEM.Clear();
}

// Successive runs with different entries should each work cleanly.
TEST(ExecutionEntryPointTests, RunFromHereThenFullRun_BothWork)
{
	NODE_SYSTEM.Clear();

	NodeArea* TestArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TestArea, nullptr);
	TestArea->SetSaveExecutedNodes(true);

	BeginNode* Begin = new BeginNode();
	IntegerVariableNode* A = TEST_TOOLS.CreateIntegerVariableNode(0);
	IntegerVariableNode* B = TEST_TOOLS.CreateIntegerVariableNode(0);

	TestArea->AddNode(Begin);
	TestArea->AddNode(A);
	TestArea->AddNode(B);

	ASSERT_TRUE(TestArea->TryToConnect(Begin, 0, A, 0));
	ASSERT_TRUE(TestArea->TryToConnect(A, 0, B, 0));

	// First: partial run from B.
	ASSERT_TRUE(TestArea->SetExecutionEntryNode(B));
	ASSERT_TRUE(TestArea->ExecuteNodeNetwork());

	auto FirstExecuted = TestArea->GetLastExecutedNodes();
	bool bBeginRanFirst = false, bARanFirst = false, bBRanFirst = false;
	for (Node* CurrentNode : FirstExecuted)
	{
		if (CurrentNode == Begin) bBeginRanFirst = true;
		if (CurrentNode == A) bARanFirst = true;
		if (CurrentNode == B) bBRanFirst = true;
	}
	EXPECT_FALSE(bBeginRanFirst);
	EXPECT_FALSE(bARanFirst);
	EXPECT_TRUE(bBRanFirst);

	// Second: full run from Begin. Should re-execute everything from the top.
	ASSERT_TRUE(TestArea->SetExecutionEntryNode(Begin));
	ASSERT_TRUE(TestArea->ExecuteNodeNetwork());

	auto SecondExecuted = TestArea->GetLastExecutedNodes();
	bool bBeginRan = false, bARan = false, bBRan = false;
	for (Node* CurrentNode : SecondExecuted)
	{
		if (CurrentNode == Begin) bBeginRan = true;
		if (CurrentNode == A) bARan = true;
		if (CurrentNode == B) bBRan = true;
	}
	EXPECT_TRUE(bBeginRan);
	EXPECT_TRUE(bARan);
	EXPECT_TRUE(bBRan);

	NODE_SYSTEM.Clear();
}

TEST(ExecutionEntryPointTests, NestedSubAreaInputNodeAsEntryPropagates)
{
	NODE_SYSTEM.Clear();

	NodeArea* TopArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(TopArea, nullptr);

	SubAreaNode* OuterSub = NODE_SYSTEM.CreateSubAreaNode(TopArea->GetID());
	ASSERT_NE(OuterSub, nullptr);
	NodeArea* OuterOwned = OuterSub->GetOwnedArea();
	ASSERT_NE(OuterOwned, nullptr);

	SubAreaNode* InnerSub = NODE_SYSTEM.CreateSubAreaNode(OuterOwned->GetID());
	ASSERT_NE(InnerSub, nullptr);
	NodeArea* InnerOwned = InnerSub->GetOwnedArea();
	ASSERT_NE(InnerOwned, nullptr);
	InnerOwned->SetSaveExecutedNodes(true);

	SubAreaInputNode* InnerInput = InnerSub->GetSubAreaInputNode();
	ASSERT_NE(InnerInput, nullptr);

	IntegerVariableNode* Downstream = TEST_TOOLS.CreateIntegerVariableNode(0);
	InnerOwned->AddNode(Downstream);
	ASSERT_TRUE(InnerOwned->TryToConnect(InnerInput, 0, Downstream, 0));

	ASSERT_TRUE(InnerOwned->SetExecutionEntryNode(InnerInput));
	ASSERT_TRUE(InnerOwned->ExecuteNodeNetwork());

	auto Executed = InnerOwned->GetLastExecutedNodes();
	bool bDownstreamRan = false;
	for (Node* CurrentNode : Executed)
	{
		if (CurrentNode == Downstream)
		{
			bDownstreamRan = true;
			break;
		}
	}
	EXPECT_TRUE(bDownstreamRan);

	NODE_SYSTEM.Clear();
}

TEST(ExecutionEntryPointTests, SubAreaNodeAsEntryPropagatesIntoChildArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	OwnedArea->SetSaveExecutedNodes(true);

	SubAreaInputNode* InnerInput = SubArea->GetSubAreaInputNode();
	ASSERT_NE(InnerInput, nullptr);

	IntegerVariableNode* InnerNode = TEST_TOOLS.CreateIntegerVariableNode(0);
	OwnedArea->AddNode(InnerNode);
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerInput, 0, InnerNode, 0));

	// SubAreaNode is the container in the parent. Set it as the entry.
	ASSERT_TRUE(ParentArea->SetExecutionEntryNode(SubArea));
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());

	// SubAreaNode itself ran in the parent area.
	auto ParentExecuted = ParentArea->GetLastExecutedNodes();
	bool bSubAreaRan = false;
	for (Node* CurrentNode : ParentExecuted)
	{
		if (CurrentNode == SubArea)
		{
			bSubAreaRan = true;
			break;
		}
	}
	EXPECT_TRUE(bSubAreaRan);

	// Mirror dispatched into the child area, and InnerNode ran there.
	auto OwnedExecuted = OwnedArea->GetLastExecutedNodes();
	bool bInnerRan = false;
	for (Node* CurrentNode : OwnedExecuted)
	{
		if (CurrentNode == InnerNode)
		{
			bInnerRan = true;
			break;
		}
	}
	EXPECT_TRUE(bInnerRan);

	NODE_SYSTEM.Clear();
}