#include "SubAreaNodeTests.h"
using namespace VisNodeSys;

TEST(SubAreaNodeTests, Basic_Creation)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	// Creating with invalid area ID should fail.
	EXPECT_EQ(NODE_SYSTEM.CreateSubAreaNode(""), nullptr);
	EXPECT_EQ(NODE_SYSTEM.CreateSubAreaNode("NonExistentID"), nullptr);

	SubAreaNode* NewNode = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(NewNode, nullptr);
	EXPECT_EQ(NewNode->GetType(), "SubAreaNode");
	EXPECT_EQ(NewNode->GetName(), "SubArea");

	// SubAreaNode should already be in the parent area after creation.
	EXPECT_EQ(ParentArea->GetNodeCount(), 1);
	EXPECT_NE(ParentArea->GetNodeByID(NewNode->GetID()), nullptr);

	NodeArea* OwnedArea = NewNode->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	// Parent-child relationship.
	EXPECT_TRUE(OwnedArea->IsChildOf(ParentArea));
	EXPECT_FALSE(ParentArea->IsChildOf(OwnedArea));
	EXPECT_TRUE(ParentArea->IsParentOf(OwnedArea));
	EXPECT_FALSE(OwnedArea->IsParentOf(ParentArea));

	// Should not be able to add the node again to the same area.
	EXPECT_FALSE(ParentArea->AddNode(NewNode));

	// Owned area should not be the same as the parent area.
	EXPECT_NE(OwnedArea->GetID(), ParentArea->GetID());

	EXPECT_EQ(NewNode->GetParentArea(), ParentArea);
	
	// Owned area should contain SubAreaInputNode and SubAreaOutputNode.
	std::vector<SubAreaInputNode*> InputNodes = OwnedArea->GetNodesByType<SubAreaInputNode>();
	EXPECT_EQ(InputNodes.size(), 1);

	std::vector<SubAreaOutputNode*> OutputNodes = OwnedArea->GetNodesByType<SubAreaOutputNode>();
	EXPECT_EQ(OutputNodes.size(), 1);

	std::string OwnedAreaID = OwnedArea->GetID();
	NODE_SYSTEM.DeleteNodeArea(ParentArea);

	// Owned area should be cleaned up along with the parent.
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(OwnedAreaID), nullptr);
}

TEST(SubAreaNodeTests, Basic_Sockets)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* NewNode = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(NewNode, nullptr);

	// SubAreaNode should start with execution in and output.
	EXPECT_EQ(NewNode->GetInputSocketCount(), 1);
	NodeSocket* InputSocket = NewNode->GetSocketByIndex(0, true);
	ASSERT_NE(InputSocket, nullptr);
	EXPECT_EQ(InputSocket->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});
	EXPECT_EQ(NewNode->GetOutputSocketCount(), 1);
	NodeSocket* OutputSocket = NewNode->GetSocketByIndex(0, false);
	ASSERT_NE(OutputSocket, nullptr);
	EXPECT_EQ(OutputSocket->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});

	SubAreaInputNode* InputNode = NewNode->GetSubAreaInputNode();
	ASSERT_NE(InputNode, nullptr);
	EXPECT_EQ(InputNode->GetInputSocketCount(), 0);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 1);
	InputSocket = InputNode->GetSocketByIndex(0, true);
	ASSERT_NE(InputSocket, nullptr);
	EXPECT_EQ(InputSocket->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});
	EXPECT_EQ(NewNode->GetSocketByIndex(0, true)->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});

	SubAreaOutputNode* OutputNode = NewNode->GetSubAreaOutputNode();
	ASSERT_NE(OutputNode, nullptr);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 1);
	EXPECT_EQ(OutputNode->GetOutputSocketCount(), 0);
	OutputSocket = OutputNode->GetSocketByIndex(0, false);
	ASSERT_NE(OutputSocket, nullptr);
	EXPECT_EQ(OutputSocket->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});

	// Neither should be directly destroyable.
	EXPECT_FALSE(InputNode->CouldBeDestroyed());
	EXPECT_FALSE(OutputNode->CouldBeDestroyed());

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, Basic_AddSockets)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	// Set up execution entry in the parent area.
	Node* ExecutionBeginNode = new BeginNode();
	ASSERT_NE(ExecutionBeginNode, nullptr);
	ParentArea->AddNode(ExecutionBeginNode);
	ParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	// Parent area: a bool literal that will feed data into the SubAreaNode.
	BoolLiteralNode* ParentBoolLiteral = new BoolLiteralNode();
	ParentBoolLiteral->SetName("ParentBoolLiteral");
	EXPECT_FALSE(ParentBoolLiteral->GetData());
	ParentBoolLiteral->SetData(true);
	EXPECT_TRUE(ParentBoolLiteral->GetData());
	ParentArea->AddNode(ParentBoolLiteral);

	// bool variable that will receive data coming back out of the child area.
	BoolVariableNode* ParentBoolResult = new BoolVariableNode();
	ParentBoolResult->SetName("ParentBoolResult");
	EXPECT_FALSE(ParentBoolResult->GetData());
	ParentArea->AddNode(ParentBoolResult);

	// Create the SubAreaNode.
	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	// Add input BOOL data socket to the SubAreaNode.
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::Direction::Input));
	EXPECT_EQ(SubArea->GetInputSocketCount(), 2);
	EXPECT_EQ(SubArea->GetOutputSocketCount(), 1);

	// Input node should add BOOL data output.
	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	ASSERT_NE(InputNode, nullptr);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 2);

	// Output node should stay the same.
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(OutputNode, nullptr);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 1);

	// Now add output BOOL data socket to the SubAreaNode.
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool OUT", NodeSocket::Direction::Output));
	EXPECT_EQ(SubArea->GetInputSocketCount(), 2);
	EXPECT_EQ(SubArea->GetOutputSocketCount(), 2);

	// Input node should stay the same.
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 2);

	// Output node should add BOOL data output.
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 2);

	// Parent area wiring.
	// Execution: BeginNode => SubAreaNode => ParentBoolResult.
	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 0, ParentBoolResult, 0));
	// Data in: ParentBoolLiteral => SubAreaNode input socket[1].
	ASSERT_TRUE(ParentArea->TryToConnect(ParentBoolLiteral, 0, SubArea, 1));
	// Data out: SubAreaNode output socket[1] => ParentBoolResult data input.
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, ParentBoolResult, 1));

	// Setting up child area.
	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	OwnedArea->SetSaveExecutedNodes(true);

	BoolVariableNode* InnerBoolNode = new BoolVariableNode();
	InnerBoolNode->SetName("InnerBoolNode");
	EXPECT_FALSE(InnerBoolNode->GetData());
	OwnedArea->AddNode(InnerBoolNode);

	// Child area wiring.
	// Execution: SubAreaInputNode => InnerBoolNode => SubAreaOutputNode.
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, InnerBoolNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 0, OutputNode, 0));
	// Data: SubAreaInputNode output[1] => InnerBoolNode data input, InnerBoolNode data output => SubAreaOutputNode input[1].
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 1, InnerBoolNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 1, OutputNode, 1));

	// Execute and verify data flow.
	EXPECT_FALSE(ParentBoolResult->GetData());
	EXPECT_FALSE(InnerBoolNode->GetData());
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_TRUE(InnerBoolNode->GetData());
	EXPECT_TRUE(ParentBoolResult->GetData());

	// Verify execution traces.
	EXPECT_EQ(ParentArea->GetLastExecutedNodes().size(), 4); // BeginNode, SubAreaNode, SubAreaNode, ParentBoolResult
	EXPECT_EQ(OwnedArea->GetLastExecutedNodes().size(), 3);  // SubAreaInputNode, InnerBoolNode, SubAreaOutputNode

	// Test with a different value to confirm it is not a default result.
	ParentBoolLiteral->SetData(false);
	ParentBoolResult->SetData(true);
	InnerBoolNode->SetData(true);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_FALSE(InnerBoolNode->GetData());
	EXPECT_FALSE(ParentBoolResult->GetData());

	// Adding incompatible socket should fail.
	EXPECT_FALSE(InputNode->AddSocket(new NodeSocket(SubArea, { std::string("FLOAT") }, "IncompatibleSocket", false)));
	EXPECT_FALSE(OutputNode->AddSocket(new NodeSocket(SubArea, { std::string("FLOAT") }, "IncompatibleSocket", true)));

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}