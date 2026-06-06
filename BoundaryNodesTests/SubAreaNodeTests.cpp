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
	NodeSocket* InputSocket = NewNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	ASSERT_NE(InputSocket, nullptr);
	EXPECT_EQ(InputSocket->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});
	EXPECT_EQ(NewNode->GetOutputSocketCount(), 1);
	NodeSocket* OutputSocket = NewNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	ASSERT_NE(OutputSocket, nullptr);
	EXPECT_EQ(OutputSocket->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});

	SubAreaInputNode* InputNode = NewNode->GetSubAreaInputNode();
	ASSERT_NE(InputNode, nullptr);
	EXPECT_EQ(InputNode->GetInputSocketCount(), 0);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 1);
	InputSocket = InputNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	ASSERT_NE(InputSocket, nullptr);
	EXPECT_EQ(InputSocket->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});
	EXPECT_EQ(NewNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Output)->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});

	SubAreaOutputNode* OutputNode = NewNode->GetSubAreaOutputNode();
	ASSERT_NE(OutputNode, nullptr);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 1);
	EXPECT_EQ(OutputNode->GetOutputSocketCount(), 0);
	OutputSocket = OutputNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
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
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::SocketFlow::Input));
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
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool OUT", NodeSocket::SocketFlow::Output));
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

	// Reverify execution traces.
	EXPECT_EQ(ParentArea->GetLastExecutedNodes().size(), 4); // BeginNode, SubAreaNode, SubAreaNode, ParentBoolResult
	EXPECT_EQ(OwnedArea->GetLastExecutedNodes().size(), 3);  // SubAreaInputNode, InnerBoolNode, SubAreaOutputNode

	// Adding incompatible socket should fail.
	EXPECT_FALSE(InputNode->AddSocket(new NodeSocket(SubArea, { std::string("FLOAT") }, "IncompatibleSocket", NodeSocket::SocketFlow::Input)));
	EXPECT_FALSE(OutputNode->AddSocket(new NodeSocket(SubArea, { std::string("FLOAT") }, "IncompatibleSocket", NodeSocket::SocketFlow::Output)));

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, DeleteSocket_OnlyRemoves_CorrespondingDirection_From_PartnerMirrorNode)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	// Start: 1 execute socket each.
	EXPECT_EQ(SubArea->GetInputSocketCount(), 1);
	EXPECT_EQ(SubArea->GetOutputSocketCount(), 1);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 1);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 1);

	// Add a BOOL input socket.
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::SocketFlow::Input));
	EXPECT_EQ(SubArea->GetInputSocketCount(), 2);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 2);

	// Add a BOOL output socket.
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool OUT", NodeSocket::SocketFlow::Output));
	EXPECT_EQ(SubArea->GetOutputSocketCount(), 2);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 2);

	// Get the ID of the BOOL input socket on the SubAreaNode (index 1).
	std::string BoolInputSocketID = SubArea->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	ASSERT_NE(BoolInputSocketID, "");

	// Delete it via SubAreaNode.
	EXPECT_TRUE(SubArea->DeleteSocket(BoolInputSocketID));

	// SubAreaNode and InputNode should both be back to 1.
	EXPECT_EQ(SubArea->GetInputSocketCount(), 1);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 1);

	// Output side unchanged.
	EXPECT_EQ(SubArea->GetOutputSocketCount(), 2);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 2);

	// Delete the BOOL output socket (now at index 1).
	std::string BoolOutputSocketID = SubArea->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Output);
	ASSERT_NE(BoolOutputSocketID, "");
	EXPECT_TRUE(SubArea->DeleteSocket(BoolOutputSocketID));

	EXPECT_EQ(SubArea->GetOutputSocketCount(), 1);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 1);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, RenameSocket_PropagatesPartnerName)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	// Add a BOOL input socket and a BOOL output socket to the SubAreaNode.
	ASSERT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::SocketFlow::Input));
	ASSERT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool OUT", NodeSocket::SocketFlow::Output));

	// Grab all sockets in play so we can verify unrelated ones are untouched by renames.
	NodeSocket* SubAreaExecIn = SubArea->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	NodeSocket* SubAreaExecOut = SubArea->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	NodeSocket* InputNodeExec = InputNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	NodeSocket* OutputNodeExec = OutputNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	ASSERT_NE(SubAreaExecIn, nullptr);
	ASSERT_NE(SubAreaExecOut, nullptr);
	ASSERT_NE(InputNodeExec, nullptr);
	ASSERT_NE(OutputNodeExec, nullptr);

	std::string SubAreaExecInOriginalName = SubAreaExecIn->GetName();
	std::string SubAreaExecOutOriginalName = SubAreaExecOut->GetName();
	std::string InputNodeExecOriginalName = InputNodeExec->GetName();
	std::string OutputNodeExecOriginalName = OutputNodeExec->GetName();

	// Input side: SubAreaNode input socket <=> SubAreaInputNode output socket.
	std::string SubAreaInputSocketID = SubArea->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	NodeSocket* SubAreaInputSocket = SubArea->GetSocketByID(SubAreaInputSocketID);
	ASSERT_NE(SubAreaInputSocket, nullptr);

	std::string InputNodeMirrorSocketID = InputNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Output);
	NodeSocket* InputNodeMirrorSocket = InputNode->GetSocketByID(InputNodeMirrorSocketID);
	ASSERT_NE(InputNodeMirrorSocket, nullptr);

	// Both partner sockets should start with the same name.
	EXPECT_EQ(SubAreaInputSocket->GetName(), InputNodeMirrorSocket->GetName());

	// Rename from the SubAreaNode side.
	std::string InputNewName = "RenamedBoolInputSocket";
	SubAreaInputSocket->SetName(InputNewName);
	EXPECT_EQ(SubAreaInputSocket->GetName(), InputNewName);
	EXPECT_EQ(InputNodeMirrorSocket->GetName(), InputNewName);

	// Rename from the SubAreaInputNode side to verify bidirectional propagation.
	std::string InputAnotherName = "RenamedAgainFromInputNode";
	InputNodeMirrorSocket->SetName(InputAnotherName);
	EXPECT_EQ(InputNodeMirrorSocket->GetName(), InputAnotherName);
	EXPECT_EQ(SubAreaInputSocket->GetName(), InputAnotherName);

	// Unrelated sockets must NOT have been touched by renaming the input-side pair.
	EXPECT_EQ(SubAreaExecIn->GetName(), SubAreaExecInOriginalName);
	EXPECT_EQ(SubAreaExecOut->GetName(), SubAreaExecOutOriginalName);
	EXPECT_EQ(InputNodeExec->GetName(), InputNodeExecOriginalName);
	EXPECT_EQ(OutputNodeExec->GetName(), OutputNodeExecOriginalName);

	// Output side: SubAreaNode output socket <=> SubAreaOutputNode input socket.
	std::string SubAreaOutputSocketID = SubArea->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Output);
	NodeSocket* SubAreaOutputSocket = SubArea->GetSocketByID(SubAreaOutputSocketID);
	ASSERT_NE(SubAreaOutputSocket, nullptr);

	std::string OutputNodeMirrorSocketID = OutputNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	NodeSocket* OutputNodeMirrorSocket = OutputNode->GetSocketByID(OutputNodeMirrorSocketID);
	ASSERT_NE(OutputNodeMirrorSocket, nullptr);

	// Capture the input-side pair's current names so we can confirm they're not disturbed
	// when we rename the output-side pair.
	std::string SubAreaInputSocketNameBefore = SubAreaInputSocket->GetName();
	std::string InputNodeMirrorSocketNameBefore = InputNodeMirrorSocket->GetName();

	// Both partner sockets should start with the same name.
	EXPECT_EQ(SubAreaOutputSocket->GetName(), OutputNodeMirrorSocket->GetName());

	// Rename from the SubAreaNode side.
	std::string OutputNewName = "RenamedBoolOutputSocket";
	SubAreaOutputSocket->SetName(OutputNewName);
	EXPECT_EQ(SubAreaOutputSocket->GetName(), OutputNewName);
	EXPECT_EQ(OutputNodeMirrorSocket->GetName(), OutputNewName);

	// Rename from the SubAreaOutputNode side to verify bidirectional propagation.
	std::string OutputAnotherName = "RenamedAgainFromOutputNode";
	OutputNodeMirrorSocket->SetName(OutputAnotherName);
	EXPECT_EQ(OutputNodeMirrorSocket->GetName(), OutputAnotherName);
	EXPECT_EQ(SubAreaOutputSocket->GetName(), OutputAnotherName);

	// Unrelated sockets must NOT have been touched by renaming the output-side pair.
	EXPECT_EQ(SubAreaExecIn->GetName(), SubAreaExecInOriginalName);
	EXPECT_EQ(SubAreaExecOut->GetName(), SubAreaExecOutOriginalName);
	EXPECT_EQ(InputNodeExec->GetName(), InputNodeExecOriginalName);
	EXPECT_EQ(OutputNodeExec->GetName(), OutputNodeExecOriginalName);
	EXPECT_EQ(SubAreaInputSocket->GetName(), SubAreaInputSocketNameBefore);
	EXPECT_EQ(InputNodeMirrorSocket->GetName(), InputNodeMirrorSocketNameBefore);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, CopyPaste_CreatesIndependentOwnedArea)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::SocketFlow::Input));

	NodeArea* OriginalOwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OriginalOwnedArea, nullptr);
	std::string OriginalOwnedAreaID = OriginalOwnedArea->GetID();

	// Simulating copy-paste.
	TEST_TOOLS.SimulateCopyPasteNodes({ SubArea }, ParentArea);

	// Find the pasted SubAreaNode.
	SubAreaNode* PastedSubArea = nullptr;
	std::vector<SubAreaNode*> SubAreaNodes = ParentArea->GetNodesByType<SubAreaNode>();
	for (auto* Node : SubAreaNodes)
	{
		if (Node->GetID() != SubArea->GetID())
		{
			PastedSubArea = Node;
			break;
		}
	}
	ASSERT_NE(PastedSubArea, nullptr);
	EXPECT_NE(PastedSubArea->GetID(), SubArea->GetID());

	// Sockets should match.
	EXPECT_EQ(PastedSubArea->GetInputSocketCount(), SubArea->GetInputSocketCount());
	EXPECT_EQ(PastedSubArea->GetOutputSocketCount(), SubArea->GetOutputSocketCount());

	// Pasted node should have its own node area.
	NodeArea* PastedOwnedArea = PastedSubArea->GetOwnedArea();
	ASSERT_NE(PastedOwnedArea, nullptr);
	EXPECT_NE(PastedOwnedArea->GetID(), OriginalOwnedAreaID);

	// Both owned areas should be children of the parent.
	EXPECT_TRUE(OriginalOwnedArea->IsChildOf(ParentArea));
	EXPECT_TRUE(PastedOwnedArea->IsChildOf(ParentArea));

	// Pasted owned area should have its own input/output nodes.
	EXPECT_EQ(PastedOwnedArea->GetNodesByType<SubAreaInputNode>().size(), 1);
	EXPECT_EQ(PastedOwnedArea->GetNodesByType<SubAreaOutputNode>().size(), 1);
	EXPECT_NE(PastedSubArea->GetSubAreaInputNode()->GetID(), SubArea->GetSubAreaInputNode()->GetID());
	EXPECT_NE(PastedSubArea->GetSubAreaOutputNode()->GetID(), SubArea->GetSubAreaOutputNode()->GetID());

	// Input/output socket counts should match the originals.
	EXPECT_EQ(PastedSubArea->GetSubAreaInputNode()->GetOutputSocketCount(), SubArea->GetSubAreaInputNode()->GetOutputSocketCount());
	EXPECT_EQ(PastedSubArea->GetSubAreaOutputNode()->GetInputSocketCount(), SubArea->GetSubAreaOutputNode()->GetInputSocketCount());

	// Cleanup should remove both owned areas.
	std::string PastedOwnedAreaID = PastedOwnedArea->GetID();
	NODE_SYSTEM.DeleteNodeArea(ParentArea);

	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(OriginalOwnedAreaID), nullptr);
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(PastedOwnedAreaID), nullptr);
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
}

TEST(SubAreaNodeTests, CopyPaste_CreatesIndependentCopy)
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
	ParentBoolLiteral->SetData(true);
	ParentArea->AddNode(ParentBoolLiteral);

	// Bool variable that will receive data coming back out of the child area.
	BoolVariableNode* ParentBoolResult = new BoolVariableNode();
	ParentBoolResult->SetName("ParentBoolResult");
	ParentArea->AddNode(ParentBoolResult);

	// Create the SubAreaNode and add data sockets.
	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool OUT", NodeSocket::SocketFlow::Output));

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	// Parent area wiring.
	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 0, ParentBoolResult, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(ParentBoolLiteral, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, ParentBoolResult, 1));

	// Set up the child area with an inner node that passes data through.
	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	OwnedArea->SetSaveExecutedNodes(true);
	std::string OriginalOwnedAreaID = OwnedArea->GetID();

	BoolVariableNode* InnerBoolNode = new BoolVariableNode();
	InnerBoolNode->SetName("InnerBoolNode");
	OwnedArea->AddNode(InnerBoolNode);

	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, InnerBoolNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 0, OutputNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 1, InnerBoolNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 1, OutputNode, 1));

	// Verify original works before copy.
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_TRUE(ParentBoolResult->GetData());

	// Record counts before copy.
	size_t NodeCountBefore = ParentArea->GetNodeCount();

	// Simulating copy-paste.
	TEST_TOOLS.SimulateCopyPasteNodes({ SubArea }, ParentArea);

	// There should be one more node in the parent area.
	EXPECT_EQ(ParentArea->GetNodeCount(), NodeCountBefore + 1);

	// Find the pasted SubAreaNode.
	SubAreaNode* PastedSubArea = nullptr;
	std::vector<SubAreaNode*> SubAreaNodes = ParentArea->GetNodesByType<SubAreaNode>();
	for (auto* Node : SubAreaNodes)
	{
		if (Node->GetID() != SubArea->GetID())
		{
			PastedSubArea = Node;
			break;
		}
	}
	ASSERT_NE(PastedSubArea, nullptr);

	// Pasted node should have a different ID.
	EXPECT_NE(PastedSubArea->GetID(), SubArea->GetID());

	// Pasted node should have the same socket counts as the original.
	EXPECT_EQ(PastedSubArea->GetInputSocketCount(), SubArea->GetInputSocketCount());
	EXPECT_EQ(PastedSubArea->GetOutputSocketCount(), SubArea->GetOutputSocketCount());

	// Verify socket types match.
	for (size_t i = 0; i < SubArea->GetInputSocketCount(); i++)
	{
		NodeSocket* OriginalSocket = SubArea->GetSocketByIndex(i, NodeSocket::SocketFlow::Input);
		NodeSocket* PastedSocket = PastedSubArea->GetSocketByIndex(i, NodeSocket::SocketFlow::Input);
		ASSERT_NE(OriginalSocket, nullptr);
		ASSERT_NE(PastedSocket, nullptr);
		EXPECT_EQ(OriginalSocket->GetAllowedTypes(), PastedSocket->GetAllowedTypes());
	}

	for (size_t i = 0; i < SubArea->GetOutputSocketCount(); i++)
	{
		NodeSocket* OriginalSocket = SubArea->GetSocketByIndex(i, NodeSocket::SocketFlow::Output);
		NodeSocket* PastedSocket = PastedSubArea->GetSocketByIndex(i, NodeSocket::SocketFlow::Output);
		ASSERT_NE(OriginalSocket, nullptr);
		ASSERT_NE(PastedSocket, nullptr);
		EXPECT_EQ(OriginalSocket->GetAllowedTypes(), PastedSocket->GetAllowedTypes());
	}

	// Pasted node should have no connections in the parent area.
	EXPECT_EQ(PastedSubArea->GetNodesConnectedToInput().size(), 0);
	EXPECT_EQ(PastedSubArea->GetNodesConnectedToOutput().size(), 0);

	// Verify the pasted node has its own independent owned area
	NodeArea* PastedOwnedArea = PastedSubArea->GetOwnedArea();
	ASSERT_NE(PastedOwnedArea, nullptr);

	// Owned area should be a different instance from the original.
	EXPECT_NE(PastedOwnedArea->GetID(), OriginalOwnedAreaID);

	// Parent-child relationship should be correct.
	EXPECT_TRUE(PastedOwnedArea->IsChildOf(ParentArea));
	EXPECT_TRUE(ParentArea->IsParentOf(PastedOwnedArea));

	// Pasted owned area should contain its own SubAreaInputNode and SubAreaOutputNode.
	std::vector<SubAreaInputNode*> PastedInputNodes = PastedOwnedArea->GetNodesByType<SubAreaInputNode>();
	EXPECT_EQ(PastedInputNodes.size(), 1);
	std::vector<SubAreaOutputNode*> PastedOutputNodes = PastedOwnedArea->GetNodesByType<SubAreaOutputNode>();
	EXPECT_EQ(PastedOutputNodes.size(), 1);

	SubAreaInputNode* PastedInputNode = PastedSubArea->GetSubAreaInputNode();
	SubAreaOutputNode* PastedOutputNode = PastedSubArea->GetSubAreaOutputNode();
	ASSERT_NE(PastedInputNode, nullptr);
	ASSERT_NE(PastedOutputNode, nullptr);

	// They should be different instances from the originals.
	EXPECT_NE(PastedInputNode->GetID(), InputNode->GetID());
	EXPECT_NE(PastedOutputNode->GetID(), OutputNode->GetID());

	// Pasted input/output nodes should have the same socket counts as originals.
	EXPECT_EQ(PastedInputNode->GetOutputSocketCount(), InputNode->GetOutputSocketCount());
	EXPECT_EQ(PastedOutputNode->GetInputSocketCount(), OutputNode->GetInputSocketCount());

	// The internal nodes should have been copied too.
	// Original owned area has: SubAreaInputNode, InnerBoolNode, SubAreaOutputNode = 3 nodes.
	EXPECT_EQ(PastedOwnedArea->GetNodeCount(), OwnedArea->GetNodeCount());

	// There should be a BoolVariableNode inside the pasted owned area.
	std::vector<BoolVariableNode*> PastedInnerBools = PastedOwnedArea->GetNodesByType<BoolVariableNode>();
	EXPECT_EQ(PastedInnerBools.size(), 1);

	// Wire the pasted SubAreaNode into the parent execution and data flow.
	ASSERT_TRUE(ParentArea->TryToDisconnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, PastedSubArea, 0));
	ASSERT_TRUE(ParentArea->TryToDisconnect(SubArea, 0, ParentBoolResult, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(PastedSubArea, 0, ParentBoolResult, 0));
	ASSERT_TRUE(ParentArea->TryToDisconnect(ParentBoolLiteral, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(ParentBoolLiteral, 0, PastedSubArea, 1));
	ASSERT_TRUE(ParentArea->TryToDisconnect(SubArea, 1, ParentBoolResult, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(PastedSubArea, 1, ParentBoolResult, 1));

	// Execute through the pasted SubAreaNode and data should flow correctly only if internal connections were also copied.
	ParentBoolResult->SetData(false);
	PastedOwnedArea->SetSaveExecutedNodes(true);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_TRUE(ParentBoolResult->GetData());

	// Verify execution traces in the pasted owned area.
	EXPECT_EQ(PastedOwnedArea->GetLastExecutedNodes().size(), 3); // SubAreaInputNode, BoolVariableNode, SubAreaOutputNode

	// Re-wire the original back in and confirm it still works.
	ASSERT_TRUE(ParentArea->TryToDisconnect(ExecutionBeginNode, 0, PastedSubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToDisconnect(PastedSubArea, 0, ParentBoolResult, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 0, ParentBoolResult, 0));
	ASSERT_TRUE(ParentArea->TryToDisconnect(ParentBoolLiteral, 0, PastedSubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(ParentBoolLiteral, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToDisconnect(PastedSubArea, 1, ParentBoolResult, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, ParentBoolResult, 1));

	ParentBoolResult->SetData(false);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_TRUE(ParentBoolResult->GetData());

	// Test with false to confirm it is not a default.
	ParentBoolLiteral->SetData(false);
	ParentBoolResult->SetData(true);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_FALSE(ParentBoolResult->GetData());

	std::string PastedOwnedAreaID = PastedOwnedArea->GetID();
	NODE_SYSTEM.DeleteNodeArea(ParentArea);

	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(OriginalOwnedAreaID), nullptr);
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(PastedOwnedAreaID), nullptr);
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
}

TEST(SubAreaNodeTests, MultipleSockets_ExecutionAndData)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	Node* ExecutionBeginNode = new BeginNode();
	ParentArea->AddNode(ExecutionBeginNode);
	ParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	BoolLiteralNode* BoolInput = new BoolLiteralNode();
	BoolInput->SetData(true);
	ParentArea->AddNode(BoolInput);

	FloatLiteralNode* FloatInput = new FloatLiteralNode();
	FloatInput->SetData(3.14f);
	ParentArea->AddNode(FloatInput);

	BoolVariableNode* BoolOutput = new BoolVariableNode();
	BoolOutput->SetData(false);
	ParentArea->AddNode(BoolOutput);

	FloatVariableNode* FloatOutput = new FloatVariableNode();
	FloatOutput->SetData(0.0f);
	ParentArea->AddNode(FloatOutput);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "FLOAT" }, "Float IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool OUT", NodeSocket::SocketFlow::Output));
	EXPECT_TRUE(SubArea->AddSocket({ "FLOAT" }, "Float OUT", NodeSocket::SocketFlow::Output));

	// SubAreaNode: input[0]=EXEC, input[1]=BOOL, input[2]=FLOAT
	//              output[0]=EXEC, output[1]=BOOL, output[2]=FLOAT
	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	OwnedArea->SetSaveExecutedNodes(true);

	BoolVariableNode* InnerBoolNode = new BoolVariableNode();
	InnerBoolNode->SetData(false);
	OwnedArea->AddNode(InnerBoolNode);

	FloatVariableNode* InnerFloatNode = new FloatVariableNode();
	InnerFloatNode->SetData(0.0f);
	OwnedArea->AddNode(InnerFloatNode);

	// Parent area wiring.
	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(BoolInput, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(FloatInput, 0, SubArea, 2));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 0, BoolOutput, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, BoolOutput, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(BoolOutput, 0, FloatOutput, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 2, FloatOutput, 1));

	// Child area wiring: InputNode[0]=EXEC => InnerBool => InnerFloat => OutputNode.
	// InputNode output[1]=BOOL, output[2]=FLOAT.
	// OutputNode input[1]=BOOL, input[2]=FLOAT.
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, InnerBoolNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 1, InnerBoolNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 0, InnerFloatNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 2, InnerFloatNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerFloatNode, 0, OutputNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 1, OutputNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerFloatNode, 1, OutputNode, 2));

	EXPECT_FALSE(BoolOutput->GetData());
	EXPECT_EQ(FloatOutput->GetData(), 0.0f);

	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());

	EXPECT_TRUE(BoolOutput->GetData());
	EXPECT_EQ(FloatOutput->GetData(), 3.14f);

	// With opposite values to confirm it is not a default result.
	BoolInput->SetData(false);
	FloatInput->SetData(0.0f);
	BoolOutput->SetData(true);
	FloatOutput->SetData(99.0f);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_FALSE(BoolOutput->GetData());
	EXPECT_EQ(FloatOutput->GetData(), 0.0f);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetSocketAllowedTypes_DisconnectsIncompatible)  // FAIL
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	Node* ExecutionBeginNode = new BeginNode();
	ParentArea->AddNode(ExecutionBeginNode);
	ParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	BoolLiteralNode* BoolInput = new BoolLiteralNode();
	BoolInput->SetData(true);
	ParentArea->AddNode(BoolInput);

	BoolVariableNode* BoolOutput = new BoolVariableNode();
	BoolOutput->SetData(false);
	ParentArea->AddNode(BoolOutput);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool OUT", NodeSocket::SocketFlow::Output));

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	OwnedArea->SetSaveExecutedNodes(true);

	BoolVariableNode* InnerBoolNode = new BoolVariableNode();
	OwnedArea->AddNode(InnerBoolNode);

	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(BoolInput, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 0, BoolOutput, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, BoolOutput, 1));

	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, InnerBoolNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 1, InnerBoolNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 0, OutputNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 1, OutputNode, 1));

	// Verify data flows before type change.
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_TRUE(BoolOutput->GetData());

	// Change SubAreaNode input socket type to FLOAT (incompatible with BOOL connections).
	std::string InputSocketID = SubArea->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	NodeSocket* InputSocket = SubArea->GetSocketByID(InputSocketID);
	ASSERT_NE(InputSocket, nullptr);

	bool bNoDisconnections = InputSocket->SetAllowedTypes({ "FLOAT" });
	ASSERT_FALSE(bNoDisconnections);

	// Upstream connection from BoolInput to SubArea should be severed.
	EXPECT_FALSE(ParentArea->IsConnected(BoolInput, 0, SubArea, 1));

	// Mirror socket on InputNode should also have changed type.
	std::string MirrorSocketID = InputNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Output);
	ASSERT_FALSE(MirrorSocketID.empty());
	const NodeSocket* MirrorSocket = InputNode->GetSocketByID(MirrorSocketID);
	ASSERT_NE(MirrorSocket, nullptr);
	EXPECT_EQ(MirrorSocket->GetAllowedTypes().size(), 1);
	EXPECT_EQ(MirrorSocket->GetAllowedTypes()[0], "FLOAT");

	// Inner connection (InputNode => InnerBoolNode data) should also be severed.
	EXPECT_FALSE(OwnedArea->IsConnected(InputNode, 1, InnerBoolNode, 1));
	// Downstream connection from SubArea to BoolOutput should be preserved since output socket type has not changed.
	ASSERT_TRUE(ParentArea->IsConnected(SubArea, 1, BoolOutput, 1));

	// Now test the output side.
	std::string OutputSocketID = SubArea->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Output);
	NodeSocket* OutputSocket = SubArea->GetSocketByID(OutputSocketID);
	ASSERT_NE(OutputSocket, nullptr);

	// Revert input socket back to BOOL to restore compatibility with InnerBoolNode before testing output socket type change.
	bNoDisconnections = InputSocket->SetAllowedTypes({ "BOOL" });
	ASSERT_TRUE(bNoDisconnections);
	ASSERT_TRUE(ParentArea->TryToConnect(BoolInput, 0, SubArea, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 1, InnerBoolNode, 1));
	//ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, BoolOutput, 1));
	
	// Verify data flows again before changing output socket type.
	BoolOutput->SetData(false);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_TRUE(BoolOutput->GetData());

	bNoDisconnections = OutputSocket->SetAllowedTypes({ "INT" });
	ASSERT_FALSE(bNoDisconnections);

	// Downstream connection from SubArea to BoolOutput should be severed.
	EXPECT_FALSE(ParentArea->IsConnected(SubArea, 1, BoolOutput, 1));

	// Mirror socket on OutputNode should also have changed.
	std::string OutputMirrorSocketID = OutputNode->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	ASSERT_FALSE(OutputMirrorSocketID.empty());
	const NodeSocket* OutputMirrorSocket = OutputNode->GetSocketByID(OutputMirrorSocketID);
	ASSERT_NE(OutputMirrorSocket, nullptr);
	EXPECT_EQ(OutputMirrorSocket->GetAllowedTypes().size(), 1);
	EXPECT_EQ(OutputMirrorSocket->GetAllowedTypes()[0], "INT");

	// Inner connection (InnerBoolNode => OutputNode data) should also be severed.
	EXPECT_FALSE(OwnedArea->IsConnected(InnerBoolNode, 1, OutputNode, 1));

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetSocketAllowedTypes_KeepsCompatible)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	Node* ExecutionBeginNode = new BeginNode();
	ParentArea->AddNode(ExecutionBeginNode);
	ParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	IntegerLiteralNode* IntInput = new IntegerLiteralNode();
	IntInput->SetData(42);
	ParentArea->AddNode(IntInput);

	IntegerVariableNode* IntOutput = new IntegerVariableNode();
	IntOutput->SetData(0);
	ParentArea->AddNode(IntOutput);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	EXPECT_TRUE(SubArea->AddSocket({ "INT" }, "Int IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "INT" }, "Int OUT", NodeSocket::SocketFlow::Output));

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	IntegerVariableNode* InnerIntNode = new IntegerVariableNode();
	OwnedArea->AddNode(InnerIntNode);

	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(IntInput, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 0, IntOutput, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, IntOutput, 1));

	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, InnerIntNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 1, InnerIntNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerIntNode, 0, OutputNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerIntNode, 1, OutputNode, 1));

	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntOutput->GetData(), 42);

	// Widen input type to INT + FLOAT; INT connections should survive.
	std::string InputSocketID = SubArea->GetSocketIDByIndex(1, NodeSocket::SocketFlow::Input);
	NodeSocket* InputSocket = SubArea->GetSocketByID(InputSocketID);
	ASSERT_NE(InputSocket, nullptr);

	bool bNoDisconnections = InputSocket->SetAllowedTypes({ "INT", "FLOAT" });
	EXPECT_TRUE(bNoDisconnections);

	EXPECT_TRUE(ParentArea->IsConnected(IntInput, 0, SubArea, 1));
	EXPECT_TRUE(OwnedArea->IsConnected(InputNode, 1, InnerIntNode, 1));
	EXPECT_TRUE(OwnedArea->IsConnected(InnerIntNode, 1, OutputNode, 1));
	EXPECT_TRUE(ParentArea->IsConnected(SubArea, 1, IntOutput, 1));

	IntOutput->SetData(0);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntOutput->GetData(), 42);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SaveLoad_WithExecutionAndData)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	Node* ExecutionBeginNode = new BeginNode();
	ParentArea->AddNode(ExecutionBeginNode);
	ParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	IntegerLiteralNode* IntInput = new IntegerLiteralNode();
	IntInput->SetData(77);
	ParentArea->AddNode(IntInput);

	IntegerVariableNode* IntOutput = new IntegerVariableNode();
	IntOutput->SetData(0);
	ParentArea->AddNode(IntOutput);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	EXPECT_TRUE(SubArea->AddSocket({ "INT" }, "Int IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "INT" }, "Int OUT", NodeSocket::SocketFlow::Output));

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	OwnedArea->SetSaveExecutedNodes(true);

	IntegerVariableNode* InnerIntNode = new IntegerVariableNode();
	OwnedArea->AddNode(InnerIntNode);

	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(IntInput, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 0, IntOutput, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, IntOutput, 1));

	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, InnerIntNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 1, InnerIntNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerIntNode, 0, OutputNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerIntNode, 1, OutputNode, 1));

	size_t ParentConnectionCount = ParentArea->GetConnectionCount();
	size_t OwnedConnectionCount = OwnedArea->GetConnectionCount();

	// Verify before save.
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntOutput->GetData(), 77);

	std::string ParentAreaID = ParentArea->GetID();

	NODE_SYSTEM.SaveToFile("SubAreaNodeTests_SaveLoad.json");
	NODE_SYSTEM.Clear();
	NODE_SYSTEM.LoadFromFile("SubAreaNodeTests_SaveLoad.json");

	NodeArea* ReloadedParent = NODE_SYSTEM.GetNodeAreaByID(ParentAreaID);
	ASSERT_NE(ReloadedParent, nullptr);
	ReloadedParent->SetSaveExecutedNodes(true);

	std::vector<SubAreaNode*> SubAreaNodes = ReloadedParent->GetNodesByType<SubAreaNode>();
	ASSERT_EQ(SubAreaNodes.size(), 1);
	SubAreaNode* ReloadedSubArea = SubAreaNodes[0];

	NodeArea* ReloadedOwnedArea = ReloadedSubArea->GetOwnedArea();
	ASSERT_NE(ReloadedOwnedArea, nullptr);
	ReloadedOwnedArea->SetSaveExecutedNodes(true);

	// Connection counts should be preserved.
	EXPECT_EQ(ReloadedParent->GetConnectionCount(), ParentConnectionCount);
	EXPECT_EQ(ReloadedOwnedArea->GetConnectionCount(), OwnedConnectionCount);

	// GetSubAreaInputNode/OutputNode should still work.
	ASSERT_NE(ReloadedSubArea->GetSubAreaInputNode(), nullptr);
	ASSERT_NE(ReloadedSubArea->GetSubAreaOutputNode(), nullptr);

	// Find the integer output node and verify execution.
	std::vector<IntegerVariableNode*> IntOutputNodes = ReloadedParent->GetNodesByType<IntegerVariableNode>();
	ASSERT_EQ(IntOutputNodes.size(), 1);
	IntegerVariableNode* ReloadedIntOutput = IntOutputNodes[0];
	ReloadedIntOutput->SetData(0);

	ASSERT_TRUE(ReloadedParent->ExecuteNodeNetwork());
	EXPECT_EQ(ReloadedIntOutput->GetData(), 77);

	// Owned area should have been executed.
	EXPECT_EQ(ReloadedOwnedArea->GetLastExecutedNodes().size(), 3); // InputNode, InnerIntNode, OutputNode

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, SaveLoad_RestoresInputOutputNodeRelationship)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "BOOL" }, "Bool OUT", NodeSocket::SocketFlow::Output));

	std::string ParentAreaID = ParentArea->GetID();
	std::string SubAreaNodeID = SubArea->GetID();
	std::string InputNodeID = SubArea->GetSubAreaInputNode()->GetID();
	std::string OutputNodeID = SubArea->GetSubAreaOutputNode()->GetID();

	NODE_SYSTEM.SaveToFile("SubAreaNodeTests_SaveLoad_Relationship.json");
	NODE_SYSTEM.Clear();
	NODE_SYSTEM.LoadFromFile("SubAreaNodeTests_SaveLoad_Relationship.json");

	NodeArea* ReloadedParent = NODE_SYSTEM.GetNodeAreaByID(ParentAreaID);
	ASSERT_NE(ReloadedParent, nullptr);

	std::vector<SubAreaNode*> SubAreaNodes = ReloadedParent->GetNodesByType<SubAreaNode>();
	ASSERT_EQ(SubAreaNodes.size(), 1);
	SubAreaNode* ReloadedSubArea = SubAreaNodes[0];
	EXPECT_EQ(ReloadedSubArea->GetID(), SubAreaNodeID);

	SubAreaInputNode* ReloadedInputNode = ReloadedSubArea->GetSubAreaInputNode();
	ASSERT_NE(ReloadedInputNode, nullptr);
	EXPECT_EQ(ReloadedInputNode->GetID(), InputNodeID);

	SubAreaOutputNode* ReloadedOutputNode = ReloadedSubArea->GetSubAreaOutputNode();
	ASSERT_NE(ReloadedOutputNode, nullptr);
	EXPECT_EQ(ReloadedOutputNode->GetID(), OutputNodeID);

	// GetOwnerSubAreaNode should return the reloaded SubAreaNode.
	EXPECT_EQ(ReloadedInputNode->GetOwnerSubAreaNode(), ReloadedSubArea);
	EXPECT_EQ(ReloadedOutputNode->GetOwnerSubAreaNode(), ReloadedSubArea);

	// GetParentArea on input/output nodes should return the parent area.
	EXPECT_EQ(ReloadedInputNode->GetOwningParentArea(), ReloadedParent);
	EXPECT_EQ(ReloadedOutputNode->GetOwningParentArea(), ReloadedParent);

	// Parent-child relationship should be restored.
	NodeArea* ReloadedOwnedArea = ReloadedSubArea->GetOwnedArea();
	ASSERT_NE(ReloadedOwnedArea, nullptr);
	EXPECT_TRUE(ReloadedOwnedArea->IsChildOf(ReloadedParent));
	EXPECT_TRUE(ReloadedParent->IsParentOf(ReloadedOwnedArea));

	// Socket counts should be preserved.
	EXPECT_EQ(ReloadedSubArea->GetInputSocketCount(), 2);
	EXPECT_EQ(ReloadedSubArea->GetOutputSocketCount(), 2);
	EXPECT_EQ(ReloadedInputNode->GetOutputSocketCount(), 2);
	EXPECT_EQ(ReloadedOutputNode->GetInputSocketCount(), 2);

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, FromJson_MalformedOwnedAreaData_FailsWithoutLeakOrDangling)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	std::string OwnedAreaID = OwnedArea->GetID();

	Json::Value SerializedNode = SubArea->ToJson();
	// Fresh id, not owned and not already loaded.
	SerializedNode["OwnedAreaID"] = "FakeOwnedAreaID";
	// Malformed, so NodeArea::LoadFromJson fails.
	SerializedNode["OwnedAreaData"] = "{ broken";

	size_t AreaCountBefore = NODE_SYSTEM.GetNodeAreaCount();
	bool bResult = SubArea->FromJson(SerializedNode);
	size_t AreaCountAfter = NODE_SYSTEM.GetNodeAreaCount();

	EXPECT_FALSE(bResult);
	EXPECT_EQ(AreaCountAfter, AreaCountBefore);
	EXPECT_EQ(SubArea->GetOwnedArea(), OwnedArea);
	EXPECT_EQ(SubArea->GetOwnedArea()->GetID(), OwnedAreaID);

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, FromJson_OnEstablishedNode_ReleasesPreviousOwnedArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	std::string OldOwnedAreaID = SubArea->GetOwnedArea()->GetID();

	// Build a valid SubAreaNode JSON describing a different owned area, then delete its source
	// so the described area ID is free and unowned at FromJson time.
	NodeArea* OtherParentArea = NODE_SYSTEM.CreateNodeArea();
	SubAreaNode* OtherSubArea = NODE_SYSTEM.CreateSubAreaNode(OtherParentArea->GetID());
	Json::Value OtherJson = OtherSubArea->ToJson();
	NODE_SYSTEM.DeleteNodeArea(OtherParentArea);

	bool bResult = SubArea->FromJson(OtherJson);
	EXPECT_TRUE(bResult);

	// The previously owned area is released, not orphaned.
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(OldOwnedAreaID), nullptr);
	// SubArea now owns the recreated area.
	ASSERT_NE(SubArea->GetOwnedArea(), nullptr);
	EXPECT_NE(SubArea->GetOwnedArea()->GetID(), OldOwnedAreaID);
	EXPECT_EQ(NODE_SYSTEM.FindOwnerSubAreaNode(SubArea->GetOwnedArea()->GetID()), SubArea);

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, MultipleSubAreasInParent_BothExecute)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	Node* ExecutionBeginNode = new BeginNode();
	ParentArea->AddNode(ExecutionBeginNode);
	ParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	IntegerLiteralNode* IntInputA = new IntegerLiteralNode();
	IntInputA->SetData(10);
	ParentArea->AddNode(IntInputA);

	IntegerLiteralNode* IntInputB = new IntegerLiteralNode();
	IntInputB->SetData(20);
	ParentArea->AddNode(IntInputB);

	IntegerVariableNode* IntOutputA = new IntegerVariableNode();
	IntOutputA->SetData(0);
	ParentArea->AddNode(IntOutputA);

	IntegerVariableNode* IntOutputB = new IntegerVariableNode();
	IntOutputB->SetData(0);
	ParentArea->AddNode(IntOutputB);

	SubAreaNode* SubAreaA = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubAreaA, nullptr);
	EXPECT_TRUE(SubAreaA->AddSocket({ "INT" }, "Int IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubAreaA->AddSocket({ "INT" }, "Int OUT", NodeSocket::SocketFlow::Output));

	SubAreaNode* SubAreaB = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubAreaB, nullptr);
	EXPECT_TRUE(SubAreaB->AddSocket({ "INT" }, "Int IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubAreaB->AddSocket({ "INT" }, "Int OUT", NodeSocket::SocketFlow::Output));

	SubAreaInputNode* InputNodeA = SubAreaA->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNodeA = SubAreaA->GetSubAreaOutputNode();
	SubAreaInputNode* InputNodeB = SubAreaB->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNodeB = SubAreaB->GetSubAreaOutputNode();
	ASSERT_NE(InputNodeA, nullptr);
	ASSERT_NE(OutputNodeA, nullptr);
	ASSERT_NE(InputNodeB, nullptr);
	ASSERT_NE(OutputNodeB, nullptr);

	NodeArea* OwnedAreaA = SubAreaA->GetOwnedArea();
	NodeArea* OwnedAreaB = SubAreaB->GetOwnedArea();
	ASSERT_NE(OwnedAreaA, nullptr);
	ASSERT_NE(OwnedAreaB, nullptr);
	EXPECT_NE(OwnedAreaA->GetID(), OwnedAreaB->GetID());
	OwnedAreaA->SetSaveExecutedNodes(true);
	OwnedAreaB->SetSaveExecutedNodes(true);

	IntegerVariableNode* InnerIntNodeA = new IntegerVariableNode();
	OwnedAreaA->AddNode(InnerIntNodeA);

	IntegerVariableNode* InnerIntNodeB = new IntegerVariableNode();
	OwnedAreaB->AddNode(InnerIntNodeB);

	// Chain: BeginNode => SubAreaA => IntOutputA => SubAreaB => IntOutputB.
	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubAreaA, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(IntInputA, 0, SubAreaA, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubAreaA, 0, IntOutputA, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubAreaA, 1, IntOutputA, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(IntOutputA, 0, SubAreaB, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(IntInputB, 0, SubAreaB, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubAreaB, 0, IntOutputB, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubAreaB, 1, IntOutputB, 1));

	ASSERT_TRUE(OwnedAreaA->TryToConnect(InputNodeA, 0, InnerIntNodeA, 0));
	ASSERT_TRUE(OwnedAreaA->TryToConnect(InputNodeA, 1, InnerIntNodeA, 1));
	ASSERT_TRUE(OwnedAreaA->TryToConnect(InnerIntNodeA, 0, OutputNodeA, 0));
	ASSERT_TRUE(OwnedAreaA->TryToConnect(InnerIntNodeA, 1, OutputNodeA, 1));

	ASSERT_TRUE(OwnedAreaB->TryToConnect(InputNodeB, 0, InnerIntNodeB, 0));
	ASSERT_TRUE(OwnedAreaB->TryToConnect(InputNodeB, 1, InnerIntNodeB, 1));
	ASSERT_TRUE(OwnedAreaB->TryToConnect(InnerIntNodeB, 0, OutputNodeB, 0));
	ASSERT_TRUE(OwnedAreaB->TryToConnect(InnerIntNodeB, 1, OutputNodeB, 1));

	EXPECT_EQ(IntOutputA->GetData(), 0);
	EXPECT_EQ(IntOutputB->GetData(), 0);

	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());

	EXPECT_EQ(IntOutputA->GetData(), 10);
	EXPECT_EQ(IntOutputB->GetData(), 20);

	EXPECT_GT(OwnedAreaA->GetLastExecutedNodes().size(), 0);
	EXPECT_GT(OwnedAreaB->GetLastExecutedNodes().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, NestedSubArea_BasicExecution)
{
	NodeArea* OuterParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(OuterParentArea, nullptr);
	OuterParentArea->SetSaveExecutedNodes(true);

	Node* ExecutionBeginNode = new BeginNode();
	OuterParentArea->AddNode(ExecutionBeginNode);
	OuterParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	IntegerLiteralNode* IntInput = new IntegerLiteralNode();
	IntInput->SetData(5);
	OuterParentArea->AddNode(IntInput);

	IntegerVariableNode* IntOutput = new IntegerVariableNode();
	IntOutput->SetData(0);
	OuterParentArea->AddNode(IntOutput);

	// Outer SubAreaNode.
	SubAreaNode* OuterSubArea = NODE_SYSTEM.CreateSubAreaNode(OuterParentArea->GetID());
	ASSERT_NE(OuterSubArea, nullptr);
	EXPECT_TRUE(OuterSubArea->AddSocket({ "INT" }, "Int IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(OuterSubArea->AddSocket({ "INT" }, "Int OUT", NodeSocket::SocketFlow::Output));

	SubAreaInputNode* OuterInputNode = OuterSubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OuterOutputNode = OuterSubArea->GetSubAreaOutputNode();
	ASSERT_NE(OuterInputNode, nullptr);
	ASSERT_NE(OuterOutputNode, nullptr);

	NodeArea* OuterOwnedArea = OuterSubArea->GetOwnedArea();
	ASSERT_NE(OuterOwnedArea, nullptr);
	OuterOwnedArea->SetSaveExecutedNodes(true);

	// Inner SubAreaNode inside the outer owned area.
	SubAreaNode* InnerSubArea = NODE_SYSTEM.CreateSubAreaNode(OuterOwnedArea->GetID());
	ASSERT_NE(InnerSubArea, nullptr);
	EXPECT_TRUE(InnerSubArea->AddSocket({ "INT" }, "Int IN", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(InnerSubArea->AddSocket({ "INT" }, "Int OUT", NodeSocket::SocketFlow::Output));

	SubAreaInputNode* InnerInputNode = InnerSubArea->GetSubAreaInputNode();
	SubAreaOutputNode* InnerOutputNode = InnerSubArea->GetSubAreaOutputNode();
	ASSERT_NE(InnerInputNode, nullptr);
	ASSERT_NE(InnerOutputNode, nullptr);

	NodeArea* InnerOwnedArea = InnerSubArea->GetOwnedArea();
	ASSERT_NE(InnerOwnedArea, nullptr);
	InnerOwnedArea->SetSaveExecutedNodes(true);

	// Parent-child relationships.
	EXPECT_TRUE(OuterOwnedArea->IsChildOf(OuterParentArea));
	EXPECT_TRUE(InnerOwnedArea->IsChildOf(OuterOwnedArea));
	EXPECT_TRUE(InnerOwnedArea->IsChildOf(OuterParentArea));

	IntegerVariableNode* InnermostIntNode = new IntegerVariableNode();
	InnerOwnedArea->AddNode(InnermostIntNode);

	// Outer parent wiring.
	ASSERT_TRUE(OuterParentArea->TryToConnect(ExecutionBeginNode, 0, OuterSubArea, 0));
	ASSERT_TRUE(OuterParentArea->TryToConnect(IntInput, 0, OuterSubArea, 1));
	ASSERT_TRUE(OuterParentArea->TryToConnect(OuterSubArea, 0, IntOutput, 0));
	ASSERT_TRUE(OuterParentArea->TryToConnect(OuterSubArea, 1, IntOutput, 1));

	// Outer owned area wiring: OuterInputNode => InnerSubArea => OuterOutputNode.
	ASSERT_TRUE(OuterOwnedArea->TryToConnect(OuterInputNode, 0, InnerSubArea, 0));
	ASSERT_TRUE(OuterOwnedArea->TryToConnect(OuterInputNode, 1, InnerSubArea, 1));
	ASSERT_TRUE(OuterOwnedArea->TryToConnect(InnerSubArea, 0, OuterOutputNode, 0));
	ASSERT_TRUE(OuterOwnedArea->TryToConnect(InnerSubArea, 1, OuterOutputNode, 1));

	// Inner owned area wiring.
	ASSERT_TRUE(InnerOwnedArea->TryToConnect(InnerInputNode, 0, InnermostIntNode, 0));
	ASSERT_TRUE(InnerOwnedArea->TryToConnect(InnerInputNode, 1, InnermostIntNode, 1));
	ASSERT_TRUE(InnerOwnedArea->TryToConnect(InnermostIntNode, 0, InnerOutputNode, 0));
	ASSERT_TRUE(InnerOwnedArea->TryToConnect(InnermostIntNode, 1, InnerOutputNode, 1));

	EXPECT_EQ(IntOutput->GetData(), 0);
	ASSERT_TRUE(OuterParentArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntOutput->GetData(), 5);

	EXPECT_GT(OuterOwnedArea->GetLastExecutedNodes().size(), 0);
	EXPECT_GT(InnerOwnedArea->GetLastExecutedNodes().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(OuterParentArea);
}

TEST(SubAreaNodeTests, SocketIndexRoutesDataCorrectly)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	Node* ExecutionBeginNode = new BeginNode();
	ParentArea->AddNode(ExecutionBeginNode);
	ParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	IntegerLiteralNode* IntSource = new IntegerLiteralNode();
	IntSource->SetData(99);
	ParentArea->AddNode(IntSource);

	IntegerVariableNode* IntResult = new IntegerVariableNode();
	IntResult->SetData(0);
	ParentArea->AddNode(IntResult);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	EXPECT_TRUE(SubArea->AddSocket({ "INT" }, "Int IN 0", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "INT" }, "Int IN 1", NodeSocket::SocketFlow::Input));
	EXPECT_TRUE(SubArea->AddSocket({ "INT" }, "Int OUT 0", NodeSocket::SocketFlow::Output));
	EXPECT_TRUE(SubArea->AddSocket({ "INT" }, "Int OUT 1", NodeSocket::SocketFlow::Output));

	// SubAreaNode: input[0]=EXEC, input[1]=INT, input[2]=INT
	//              output[0]=EXEC, output[1]=INT, output[2]=INT
	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	IntegerVariableNode* InnerIntNodeA = new IntegerVariableNode();
	InnerIntNodeA->SetData(0);
	OwnedArea->AddNode(InnerIntNodeA);

	IntegerVariableNode* InnerIntNodeB = new IntegerVariableNode();
	InnerIntNodeB->SetData(0);
	OwnedArea->AddNode(InnerIntNodeB);

	// Connect IntSource into SubArea input[1]; read from SubArea output[1].
	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, SubArea, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(IntSource, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 0, IntResult, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 1, IntResult, 1));

	// Inner: forward input[1] => output[1], input[2] => output[2].
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, InnerIntNodeA, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 1, InnerIntNodeA, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerIntNodeA, 0, InnerIntNodeB, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 2, InnerIntNodeB, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerIntNodeB, 0, OutputNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerIntNodeA, 1, OutputNode, 1));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerIntNodeB, 1, OutputNode, 2));

	// IntSource at input[1], result reads output[1]: should get 99.
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntResult->GetData(), 99);

	// Move IntSource to input[2] (mismatch): output[1] gets nothing => 0.
	ASSERT_TRUE(ParentArea->TryToDisconnect(IntSource, 0, SubArea, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(IntSource, 0, SubArea, 2));

	IntResult->SetData(0);
	InnerIntNodeA->SetData(0);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntResult->GetData(), 0);

	// Now read from output[2] instead: should get 99.
	ASSERT_TRUE(ParentArea->TryToDisconnect(SubArea, 1, IntResult, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(SubArea, 2, IntResult, 1));

	IntResult->SetData(0);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_EQ(IntResult->GetData(), 99);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, ExecSocket_CannotBeDeletedOrRetyped)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	// Obtain socket pointers first so retype checks run independently of deletion.
	NodeSocket* SubAreaExecIn = SubArea->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	NodeSocket* SubAreaExecOut = SubArea->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	NodeSocket* InputNodeExecOut = InputNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	NodeSocket* OutputNodeExecIn = OutputNode->GetSocketByIndex(0, NodeSocket::SocketFlow::Input);
	ASSERT_NE(SubAreaExecIn, nullptr);
	ASSERT_NE(SubAreaExecOut, nullptr);
	ASSERT_NE(InputNodeExecOut, nullptr);
	ASSERT_NE(OutputNodeExecIn, nullptr);

	// Changing the exec socket type to a non-EXECUTE type must be blocked.
	// SetAllowedTypes return value indicates disconnections, not success/failure,
	// so we verify the type itself did not change.
	EXPECT_FALSE(SubAreaExecIn->SetAllowedTypes({ "INT" }));
	EXPECT_FALSE(SubAreaExecOut->SetAllowedTypes({ "BOOL" }));
	EXPECT_FALSE(InputNodeExecOut->SetAllowedTypes({ "FLOAT" }));
	EXPECT_FALSE(OutputNodeExecIn->SetAllowedTypes({ "INT" }));

	EXPECT_EQ(SubAreaExecIn->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});
	EXPECT_EQ(SubAreaExecOut->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});
	EXPECT_EQ(InputNodeExecOut->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});
	EXPECT_EQ(OutputNodeExecIn->GetAllowedTypes(), std::vector<std::string>{"EXECUTE"});

	// Deleting the exec socket (index 0) must fail on all three nodes.
	std::string SubAreaExecInID = SubArea->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	std::string SubAreaExecOutID = SubArea->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output);
	std::string InputNodeExecOutID = InputNode->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Output);
	std::string OutputNodeExecInID = OutputNode->GetSocketIDByIndex(0, NodeSocket::SocketFlow::Input);
	ASSERT_NE(SubAreaExecInID, "");
	ASSERT_NE(SubAreaExecOutID, "");
	ASSERT_NE(InputNodeExecOutID, "");
	ASSERT_NE(OutputNodeExecInID, "");

	EXPECT_FALSE(SubArea->DeleteSocket(SubAreaExecInID));
	EXPECT_FALSE(SubArea->DeleteSocket(SubAreaExecOutID));
	EXPECT_FALSE(InputNode->DeleteSocket(InputNodeExecOutID));
	EXPECT_FALSE(OutputNode->DeleteSocket(OutputNodeExecInID));

	// Socket counts must be unchanged after failed deletions.
	EXPECT_EQ(SubArea->GetInputSocketCount(), 1);
	EXPECT_EQ(SubArea->GetOutputSocketCount(), 1);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 1);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 1);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, Small_Graph)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallSubAreaNodeGraph();
	EXPECT_TRUE(TEST_TOOLS.VerifyParentChildInSmallSubAreaNodeGraph());

	// After verification of the graph structure, delete the SubAreaNode that owns Areas[1]
	// and verify the entire Areas[1] subtree is removed while branches 2 and 3 remain intact.
	NodeArea* RootArea = Areas[0];
	NodeArea* Area1 = Areas[1];

	// Find the SubAreaNode in Areas[0] whose owned area is Areas[1].
	SubAreaNode* SubAreaOwningArea1 = nullptr;
	{
		std::vector<SubAreaNode*> SubAreasInRoot = RootArea->GetNodesByType<SubAreaNode>();
		for (SubAreaNode* CurrentNode : SubAreasInRoot)
		{
			if (CurrentNode->GetOwnedArea() == Area1)
			{
				SubAreaOwningArea1 = CurrentNode;
				break;
			}
		}
	}
	ASSERT_NE(SubAreaOwningArea1, nullptr);

	// Remember the IDs of every area that is expected to disappear (Areas[1] and its full subtree).
	std::vector<std::string> DoomedAreaIDs;
	for (int Index : std::vector<int>{ 1, 4, 5, 6, 13, 14, 15, 23, 24 })
		DoomedAreaIDs.push_back(Areas[Index]->GetID());

	// Delete the SubAreaNode. This should destroy Areas[1] and everything underneath it.
	RootArea->Delete(SubAreaOwningArea1);

	// All 9 areas in branch 1 should be gone.
	for (const std::string& DoomedID : DoomedAreaIDs)
		EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(DoomedID), nullptr);

	// The verifier relies on there being exactly 30 areas and should now fail.
	EXPECT_FALSE(TEST_TOOLS.VerifyParentChildInSmallSubAreaNodeGraph());

	// Root should now have only two immediate children (Areas[2] and Areas[3]).
	EXPECT_EQ(RootArea->GetImediateChildrenCount(), 2);
	std::vector<NodeArea*> RemainingImmediate = RootArea->GetImediateChildren();
	EXPECT_EQ(RemainingImmediate.size(), 2);
	EXPECT_NE(std::find(RemainingImmediate.begin(), RemainingImmediate.end(), Areas[2]), RemainingImmediate.end());
	EXPECT_NE(std::find(RemainingImmediate.begin(), RemainingImmediate.end(), Areas[3]), RemainingImmediate.end());
	EXPECT_EQ(std::find(RemainingImmediate.begin(), RemainingImmediate.end(), Areas[1]), RemainingImmediate.end());

	// All remaining descendants of the root should be exactly branches 2 and 3.
	std::vector<NodeArea*> ExpectedRemainingRecursive = {
		Areas[2], Areas[3],
		Areas[7], Areas[8], Areas[9],
		Areas[10], Areas[11], Areas[12],
		Areas[16], Areas[17], Areas[18],
		Areas[19], Areas[20], Areas[21], Areas[22],
		Areas[25], Areas[26],
		Areas[27], Areas[28], Areas[29]
	};
	EXPECT_EQ(RootArea->GetRecursiveChildCount(), ExpectedRemainingRecursive.size());

	std::vector<NodeArea*> ActualRemainingRecursive = RootArea->GetRecursiveChildren();
	std::sort(ActualRemainingRecursive.begin(), ActualRemainingRecursive.end());
	std::sort(ExpectedRemainingRecursive.begin(), ExpectedRemainingRecursive.end());
	EXPECT_EQ(ActualRemainingRecursive, ExpectedRemainingRecursive);

	// Branches 2 and 3 should be completely unaffected.
	// Branch 2.
	EXPECT_EQ(Areas[2]->GetParent(), RootArea);
	EXPECT_TRUE(Areas[2]->IsChildOf(RootArea));
	EXPECT_EQ(Areas[2]->GetImediateChildrenCount(), 3);
	{
		std::vector<NodeArea*> Area2Immediate = Areas[2]->GetImediateChildren();
		std::sort(Area2Immediate.begin(), Area2Immediate.end());
		std::vector<NodeArea*> Expected = { Areas[7], Areas[8], Areas[9] };
		std::sort(Expected.begin(), Expected.end());
		EXPECT_EQ(Area2Immediate, Expected);
	}
	EXPECT_EQ(Areas[2]->GetRecursiveChildCount(), 8); // 7,8,9,16,17,18,25,26

	// Branch 3.
	EXPECT_EQ(Areas[3]->GetParent(), RootArea);
	EXPECT_TRUE(Areas[3]->IsChildOf(RootArea));
	EXPECT_EQ(Areas[3]->GetImediateChildrenCount(), 3);
	{
		std::vector<NodeArea*> Area3Immediate = Areas[3]->GetImediateChildren();
		std::sort(Area3Immediate.begin(), Area3Immediate.end());
		std::vector<NodeArea*> Expected = { Areas[10], Areas[11], Areas[12] };
		std::sort(Expected.begin(), Expected.end());
		EXPECT_EQ(Area3Immediate, Expected);
	}
	EXPECT_EQ(Areas[3]->GetRecursiveChildCount(), 10); // 10,11,12,19,20,21,22,27,28,29

	// Spot-check some deep descendants: their parent chains should still be intact.
	// Area 25 lives under 16 => 7 => 2 => 0.
	EXPECT_EQ(Areas[25]->GetParent(), Areas[16]);
	EXPECT_EQ(Areas[16]->GetParent(), Areas[7]);
	EXPECT_EQ(Areas[7]->GetParent(), Areas[2]);
	EXPECT_TRUE(Areas[25]->IsChildOf(RootArea));
	EXPECT_TRUE(Areas[25]->IsChildOf(Areas[2]));
	EXPECT_TRUE(Areas[25]->IsChildOf(Areas[7]));
	EXPECT_TRUE(Areas[25]->IsChildOf(Areas[16]));

	// Area 29 lives under 22 => 12 => 3 => 0.
	EXPECT_EQ(Areas[29]->GetParent(), Areas[22]);
	EXPECT_EQ(Areas[22]->GetParent(), Areas[12]);
	EXPECT_EQ(Areas[12]->GetParent(), Areas[3]);
	EXPECT_TRUE(Areas[29]->IsChildOf(RootArea));
	EXPECT_TRUE(Areas[29]->IsChildOf(Areas[3]));
	EXPECT_TRUE(Areas[29]->IsChildOf(Areas[12]));
	EXPECT_TRUE(Areas[29]->IsChildOf(Areas[22]));

	// Leaves should report zero children.
	EXPECT_EQ(Areas[25]->GetImediateChildrenCount(), 0);
	EXPECT_EQ(Areas[25]->GetRecursiveChildCount(), 0);
	EXPECT_EQ(Areas[29]->GetImediateChildrenCount(), 0);
	EXPECT_EQ(Areas[29]->GetRecursiveChildCount(), 0);

	NODE_SYSTEM.Clear();
}

// File-scope helper mirroring IntCalculationInSmallLinkedNodeAreaGraph but for SubAreaNode containment.
// For each area, wires an integer arithmetic chain that passes (upstream_int + area_index) down the tree.
// Leaf areas store the final sum in an IntegerVariableNode named "ResultNode_<index>".
static void IntCalculationInSmallSubAreaNodeGraph()
{
	std::vector<NodeArea*> Areas = TEST_TOOLS.GetOrderedAreasFromSmallSubAreaNodeGraph();
	for (size_t i = 0; i < Areas.size(); i++)
	{
		if (Areas[i] == nullptr)
			return;
	}

	if (Areas.size() != 30)
		return;

	// Same topology as CreateSmallSubAreaNodeGraph.
	std::vector<std::vector<int>> ImmediateChildren(30);
	ImmediateChildren[0] = { 1, 2, 3 };
	ImmediateChildren[1] = { 4, 5, 6 };
	ImmediateChildren[2] = { 7, 8, 9 };
	ImmediateChildren[3] = { 10, 11, 12 };
	ImmediateChildren[4] = { 13, 14 };
	ImmediateChildren[5] = { 15 };
	ImmediateChildren[7] = { 16 };
	ImmediateChildren[8] = { 17 };
	ImmediateChildren[9] = { 18 };
	ImmediateChildren[10] = { 19 };
	ImmediateChildren[11] = { 20 };
	ImmediateChildren[12] = { 21, 22 };
	ImmediateChildren[13] = { 23 };
	ImmediateChildren[15] = { 24 };
	ImmediateChildren[16] = { 25 };
	ImmediateChildren[18] = { 26 };
	ImmediateChildren[20] = { 27 };
	ImmediateChildren[21] = { 28 };
	ImmediateChildren[22] = { 29 };

	// Helper: find the SubAreaNode in ParentArea whose owned area is ChildArea.
	auto FindSubAreaNodeForChild = [](NodeArea* ParentArea, NodeArea* ChildArea) -> SubAreaNode*
		{
			std::vector<SubAreaNode*> SubAreaNodes = ParentArea->GetNodesByType<SubAreaNode>();
			for (SubAreaNode* SAN : SubAreaNodes)
			{
				if (SAN->GetOwnedArea() == ChildArea)
					return SAN;
			}
			return nullptr;
		};

	// For each area, wire up:
	//   - In root (area 0): BeginNode as exec source; no upstream int.
	//   - In non-root: SubAreaInputNode as exec source; add INT input socket on SubAreaNode to receive upstream int.
	//   - IndexNode with value = area index.
	//   - If leaf: one AddNode(exec, index, upstream_int) => ResultNode.
	//   - If non-leaf: one AddNode per child. Before wiring to a child SubAreaNode, add an INT data
	//     input socket to that child SubAreaNode so it can receive the upstream int.
	//   - Exec chain completion: for non-root areas, the last child SubAreaNode's exec-out is wired
	//     to SubAreaOutputNode so execution returns to the parent area.
	for (int i = 0; i < 30; i++)
	{
		NodeArea* CurrentArea = Areas[i];
		const std::vector<int>& Children = ImmediateChildren[i];
		bool bIsLeaf = Children.empty();
		bool bIsRoot = (i == 0);

		// Resolve the exec source node and the optional upstream-int data source in this area.
		Node* ExecSource = nullptr;        // whatever supplies exec in this area
		Node* UpstreamIntSource = nullptr; // whatever supplies the upstream int on its socket 1
		int UpstreamIntSourceSocket = 1;

		if (bIsRoot)
		{
			BeginNode* Begin = reinterpret_cast<BeginNode*>(NODE_FACTORY.CreateNode("BeginNode"));
			ASSERT_NE(Begin, nullptr);
			CurrentArea->AddNode(Begin);
			CurrentArea->SetExecutionEntryNode(Begin);
			ExecSource = Begin;
			// No upstream for root; we leave AddNode's upstream-int input unconnected (defaults to 0).
		}
		else
		{
			// Get this area's owning SubAreaNode in the parent area so we can reach SubAreaInputNode.
			NodeArea* ParentArea = CurrentArea->GetParent();
			ASSERT_NE(ParentArea, nullptr);
			SubAreaNode* OwnerSubArea = FindSubAreaNodeForChild(ParentArea, CurrentArea);
			ASSERT_NE(OwnerSubArea, nullptr);

			// Add an INT input socket on the owning SubAreaNode (will create a matching output on SubAreaInputNode).
			ASSERT_TRUE(OwnerSubArea->AddSocket({ "INT" }, "Upstream IN", NodeSocket::SocketFlow::Input));

			SubAreaInputNode* InputNode = OwnerSubArea->GetSubAreaInputNode();
			ASSERT_NE(InputNode, nullptr);
			ExecSource = InputNode;
			UpstreamIntSource = InputNode;
			UpstreamIntSourceSocket = 1; // first non-exec output, matching the socket we just added
		}

		// IndexNode (always present, with this area's index).
		IntegerVariableNode* IndexNode = new IntegerVariableNode();
		IndexNode->SetData(i);
		CurrentArea->AddNode(IndexNode);

		if (bIsLeaf)
		{
			ArithmeticAddNode* AddNode = new ArithmeticAddNode();
			CurrentArea->AddNode(AddNode);

			IntegerVariableNode* ResultNode = new IntegerVariableNode();
			ResultNode->SetName("ResultNode_" + CurrentArea->GetName());
			CurrentArea->AddNode(ResultNode);

			// Exec chain: ExecSource => AddNode => ResultNode.
			ASSERT_TRUE(CurrentArea->TryToConnect(ExecSource, 0, AddNode, 0));
			ASSERT_TRUE(CurrentArea->TryToConnect(AddNode, 0, ResultNode, 0));

			// Data: IndexNode(socket 1) => AddNode(socket 1); upstream(socket 1) => AddNode(socket 2) if present.
			ASSERT_TRUE(CurrentArea->TryToConnect(IndexNode, 1, AddNode, 1));
			if (UpstreamIntSource != nullptr)
				ASSERT_TRUE(CurrentArea->TryToConnect(UpstreamIntSource, UpstreamIntSourceSocket, AddNode, 2));

			// Data result: AddNode(socket 1) => ResultNode(socket 1).
			ASSERT_TRUE(CurrentArea->TryToConnect(AddNode, 1, ResultNode, 1));

			// For non-root leaves, wire ResultNode exec => SubAreaOutputNode so execution returns cleanly.
			if (!bIsRoot)
			{
				NodeArea* ParentArea = CurrentArea->GetParent();
				SubAreaNode* OwnerSubArea = FindSubAreaNodeForChild(ParentArea, CurrentArea);
				SubAreaOutputNode* OutputNode = OwnerSubArea->GetSubAreaOutputNode();
				ASSERT_NE(OutputNode, nullptr);
				ASSERT_TRUE(CurrentArea->TryToConnect(ResultNode, 0, OutputNode, 0));
			}
		}
		else
		{
			// Non-leaf: one AddNode per child, each AddNode wired to the corresponding child SubAreaNode.
			Node* LastChildSubAreaNode = nullptr;
			for (size_t ci = 0; ci < Children.size(); ci++)
			{
				int ChildIndex = Children[ci];
				NodeArea* ChildArea = Areas[ChildIndex];
				SubAreaNode* ChildSubArea = FindSubAreaNodeForChild(CurrentArea, ChildArea);
				ASSERT_NE(ChildSubArea, nullptr);

				// Add an INT input socket on the child SubAreaNode (for the upstream int passed from this area).
				ASSERT_TRUE(ChildSubArea->AddSocket({ "INT" }, "Upstream IN", NodeSocket::SocketFlow::Input));

				ArithmeticAddNode* AddNode = new ArithmeticAddNode();
				CurrentArea->AddNode(AddNode);

				// Exec: ExecSource(0) => AddNode(0); AddNode(0) => ChildSubArea(0).
				ASSERT_TRUE(CurrentArea->TryToConnect(ExecSource, 0, AddNode, 0));
				ASSERT_TRUE(CurrentArea->TryToConnect(AddNode, 0, ChildSubArea, 0));

				// Data: IndexNode(1) => AddNode(1); upstream(1) => AddNode(2) if present.
				ASSERT_TRUE(CurrentArea->TryToConnect(IndexNode, 1, AddNode, 1));
				if (UpstreamIntSource != nullptr)
					ASSERT_TRUE(CurrentArea->TryToConnect(UpstreamIntSource, UpstreamIntSourceSocket, AddNode, 2));

				// Data: AddNode(1) => ChildSubArea INT input (socket 1 on the child SubAreaNode).
				ASSERT_TRUE(CurrentArea->TryToConnect(AddNode, 1, ChildSubArea, 1));

				LastChildSubAreaNode = ChildSubArea;
			}

			// For non-root intermediates, wire the last child's exec-out to SubAreaOutputNode to complete the return path.
			if (!bIsRoot && LastChildSubAreaNode != nullptr)
			{
				NodeArea* ParentArea = CurrentArea->GetParent();
				SubAreaNode* OwnerSubArea = FindSubAreaNodeForChild(ParentArea, CurrentArea);
				SubAreaOutputNode* OutputNode = OwnerSubArea->GetSubAreaOutputNode();
				ASSERT_NE(OutputNode, nullptr);
				ASSERT_TRUE(CurrentArea->TryToConnect(LastChildSubAreaNode, 0, OutputNode, 0));
			}
		}
	}
}

TEST(SubAreaNodeTests, SaveLoad_With_Execute_Connections_Small)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallSubAreaNodeGraph();
	EXPECT_TRUE(TEST_TOOLS.VerifyParentChildInSmallSubAreaNodeGraph());
	IntCalculationInSmallSubAreaNodeGraph();

	size_t TotalConnections = NODE_SYSTEM.GetTotalConnectionCount();
	std::vector<size_t> PerAreaConnectionCounts;
	for (auto Area : Areas)
	{
		PerAreaConnectionCounts.push_back(Area->GetConnectionCount());
		Area->SetSaveExecutedNodes(true);
	}

	Areas[0]->ExecuteNodeNetwork();

	std::unordered_map<std::string, std::vector<Node*>> ExecutedNodes = NODE_SYSTEM.GetLastExecutedNodes(Areas[0]->GetID());
	size_t ExecutedNodeAreaCount = ExecutedNodes.size();
	std::vector<size_t> ExecutedNodesPerArea;
	for (auto Area : Areas)
		ExecutedNodesPerArea.push_back(ExecutedNodes[Area->GetID()].size());

	// Same expected results as the LinkNode version: each area's ResultNode stores
	// the sum of its index plus the integer propagated from its root-bound parent chain.
	std::vector<std::pair<size_t, int>> ExpectedResults = {
		// Depth Level 0.
		{ 0, -1 },
		// Depth Level 1.
		{ 1, -1 }, { 2, -1 }, { 3, -1 },
		// Depth Level 2.
		{ 4, -1 }, { 5, -1 }, { 6, 7 }, { 7, -1 }, { 8, -1 }, { 9, -1 }, { 10, -1 }, { 11, -1 }, { 12, -1 },
		// Depth Level 3.
		{ 13, -1 }, { 14, 19 }, { 15, -1 }, { 16, -1 }, { 17, 27 }, { 18, -1 }, { 19, 32 }, { 20, -1 }, { 21, -1 }, { 22, -1 },
		// Depth Level 4.
		{ 23, 41 }, { 24, 45 }, { 25, 50 }, { 26, 55 }, { 27, 61 }, { 28, 64 }, { 29, 66 }
	};

	for (auto ExpectedResult : ExpectedResults)
	{
		if (ExpectedResult.second == -1)
			continue;

		std::vector<Node*> FoundNodes = Areas[ExpectedResult.first]->GetNodesByName("ResultNode_" + std::to_string(ExpectedResult.first));
		ASSERT_EQ(FoundNodes.size(), 1);
		Node* ResultNode = FoundNodes[0];
		IntegerVariableNode* CastedResultNode = reinterpret_cast<IntegerVariableNode*>(ResultNode);
		int ResultValue = CastedResultNode->GetData();
		int ExpectedValue = ExpectedResult.second;
		ASSERT_EQ(ResultValue, ExpectedValue);
	}

	NODE_SYSTEM.SaveToFile("NodeSystemTests_TestSaveLoad_Small_SubArea.json");
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(TEST_TOOLS.VerifyParentChildInSmallSubAreaNodeGraph());

	NODE_SYSTEM.LoadFromFile("NodeSystemTests_TestSaveLoad_Small_SubArea.json");
	EXPECT_TRUE(TEST_TOOLS.VerifyParentChildInSmallSubAreaNodeGraph());
	Areas = TEST_TOOLS.GetOrderedAreasFromSmallSubAreaNodeGraph();
	for (auto Area : Areas)
	{
		ASSERT_NE(Area, nullptr);
		Area->SetSaveExecutedNodes(true);
	}	

	size_t AfterLoadTotalConnections = NODE_SYSTEM.GetTotalConnectionCount();
	for (int i = 0; i < PerAreaConnectionCounts.size(); i++)
		ASSERT_EQ(PerAreaConnectionCounts[i], Areas[i]->GetConnectionCount());

	ASSERT_EQ(TotalConnections, AfterLoadTotalConnections);

	// Reset every leaf's ResultNode to 0 so the post-load ASSERT_EQ proves that
	// execution actually recomputed the values, not that save/load preserved them.
	for (auto ExpectedResult : ExpectedResults)
	{
		if (ExpectedResult.second == -1)
			continue;

		Node* ResultNode = Areas[ExpectedResult.first]->GetNodesByName("ResultNode_" + std::to_string(ExpectedResult.first))[0];
		IntegerVariableNode* CastedResultNode = reinterpret_cast<IntegerVariableNode*>(ResultNode);
		CastedResultNode->SetData(0);
		ASSERT_EQ(CastedResultNode->GetData(), 0);
	}

	Areas[0]->ExecuteNodeNetwork();

	std::unordered_map<std::string, std::vector<Node*>> AfterLoadExecutedNodes = NODE_SYSTEM.GetLastExecutedNodes(Areas[0]->GetID());
	size_t AfterLoadExecutedNodeAreaCount = AfterLoadExecutedNodes.size();
	ASSERT_EQ(ExecutedNodeAreaCount, AfterLoadExecutedNodeAreaCount);
	for (int i = 0; i < Areas.size(); i++)
		ASSERT_EQ(ExecutedNodesPerArea[i], static_cast<int>(AfterLoadExecutedNodes[Areas[i]->GetID()].size()));

	for (auto ExpectedResult : ExpectedResults)
	{
		if (ExpectedResult.second == -1)
			continue;

		Node* ResultNode = Areas[ExpectedResult.first]->GetNodesByName("ResultNode_" + std::to_string(ExpectedResult.first))[0];
		IntegerVariableNode* CastedResultNode = reinterpret_cast<IntegerVariableNode*>(ResultNode);
		int ResultValue = CastedResultNode->GetData();
		int ExpectedValue = ExpectedResult.second;
		ASSERT_EQ(ResultValue, ExpectedValue);
	}

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, DeleteOwnedArea_Removes_OrphanedSubAreaNode_From_Parent)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* CurrentSubAreaNode = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(CurrentSubAreaNode, nullptr);

	NodeArea* OwnedArea = CurrentSubAreaNode->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	const std::string OwnedAreaID = OwnedArea->GetID();
	const std::string SubNodeID = CurrentSubAreaNode->GetID();

	ASSERT_EQ(ParentArea->GetNodeCount(), 1);

	// Delete the OWNED area directly.
	NODE_SYSTEM.DeleteNodeArea(OwnedArea);

	// The owned area must be gone from the system.
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(OwnedAreaID), nullptr);

	// After the owned area is deleted the SubAreaNode should be also gone since it has no area to point to, and the parent should have no nodes.
	EXPECT_EQ(ParentArea->GetNodeCount(), 0);
	EXPECT_EQ(ParentArea->GetNodeByID(SubNodeID), nullptr);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, DeleteOwnedArea_OnNonDestroyableSubAreaOwner_DeletesArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	std::string Serialized = ParentArea->ToJson();
	NODE_SYSTEM.Clear();

	std::string Needle = "\"bCouldBeDestroyedByUser\":true";
	std::string Replacement = "\"bCouldBeDestroyedByUser\":false";
	size_t NeedlePosition = Serialized.find(Needle);
	ASSERT_NE(NeedlePosition, std::string::npos);
	Serialized.replace(NeedlePosition, Needle.size(), Replacement);

	NodeArea* LoadedParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_TRUE(LoadedParentArea->LoadFromJson(Serialized));

	std::vector<SubAreaNode*> SubAreas = LoadedParentArea->GetNodesByType<SubAreaNode>();
	ASSERT_EQ(SubAreas.size(), 1);
	SubAreaNode* LoadedSubArea = SubAreas[0];
	ASSERT_NE(LoadedSubArea->GetOwnedArea(), nullptr);
	std::string OwnedAreaID = LoadedSubArea->GetOwnedArea()->GetID();
	std::string SubAreaNodeID = LoadedSubArea->GetID();

	size_t AreaCountBefore = NODE_SYSTEM.GetNodeAreaCount();
	NODE_SYSTEM.DeleteNodeArea(LoadedSubArea->GetOwnedArea());

	// The owned area is actually deleted (not silently leaked), and its owner removed too.
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaByID(OwnedAreaID), nullptr);
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), AreaCountBefore - 1);
	EXPECT_EQ(LoadedParentArea->GetNodeByID(SubAreaNodeID), nullptr);

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, AddSocket_FromSubArea_Input_Output_Nodes_PropagatesToSubAreaNode_Correctly)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	// Only the exec sockets on each side.
	ASSERT_EQ(SubArea->GetInputSocketCount(), 1);
	ASSERT_EQ(SubArea->GetOutputSocketCount(), 1);
	ASSERT_EQ(InputNode->GetInputSocketCount(), 0);
	ASSERT_EQ(InputNode->GetOutputSocketCount(), 1);
	ASSERT_EQ(OutputNode->GetInputSocketCount(), 1);
	ASSERT_EQ(OutputNode->GetOutputSocketCount(), 0);

	// Add socket from SubAreaInputNode side.
	EXPECT_TRUE(InputNode->AddSocket({ "BOOL" }, "BOOL in", NodeSocket::SocketFlow::Output));
	EXPECT_EQ(InputNode->GetInputSocketCount(), 0);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 2);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 1);
	EXPECT_EQ(OutputNode->GetOutputSocketCount(), 0);
	EXPECT_EQ(SubArea->GetInputSocketCount(), 2);
	EXPECT_EQ(SubArea->GetOutputSocketCount(), 1);

	// Add socket from SubAreaOutputNode side.
	EXPECT_TRUE(OutputNode->AddSocket({ "BOOL" }, "BOOL out", NodeSocket::SocketFlow::Input));
	EXPECT_EQ(InputNode->GetInputSocketCount(), 0);
	EXPECT_EQ(InputNode->GetOutputSocketCount(), 2);
	EXPECT_EQ(OutputNode->GetInputSocketCount(), 2);
	EXPECT_EQ(OutputNode->GetOutputSocketCount(), 0);
	EXPECT_EQ(SubArea->GetInputSocketCount(), 2);
	EXPECT_EQ(SubArea->GetOutputSocketCount(), 2);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, Looped_SubAreaNode_Accumulates_OwnedArea_ExecutedNodes)
{
	const int IterationCount = 5;

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);
	ParentArea->SetSaveExecutedNodes(true);

	Node* ExecutionBeginNode = new BeginNode();
	ParentArea->AddNode(ExecutionBeginNode);
	ParentArea->SetExecutionEntryNode(ExecutionBeginNode);

	IntegerVariableNode* StartIndex = new IntegerVariableNode();
	StartIndex->SetData(0);
	ParentArea->AddNode(StartIndex);

	IntegerVariableNode* LastIndex = new IntegerVariableNode();
	LastIndex->SetData(IterationCount);
	ParentArea->AddNode(LastIndex);

	LoopNode* Loop = new LoopNode();
	ParentArea->AddNode(Loop);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	OwnedArea->SetSaveExecutedNodes(true);

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	BoolVariableNode* InnerBoolNode = new BoolVariableNode();
	OwnedArea->AddNode(InnerBoolNode);

	ASSERT_TRUE(ParentArea->TryToConnect(ExecutionBeginNode, 0, Loop, 0));
	ASSERT_TRUE(ParentArea->TryToConnect(StartIndex, 1, Loop, 1));
	ASSERT_TRUE(ParentArea->TryToConnect(LastIndex, 1, Loop, 2));
	ASSERT_TRUE(ParentArea->TryToConnect(Loop, 0, SubArea, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InputNode, 0, InnerBoolNode, 0));
	ASSERT_TRUE(OwnedArea->TryToConnect(InnerBoolNode, 0, OutputNode, 0));

	LastIndex->SetData(IterationCount);
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	const size_t OwnedTracePerIteration = 3; // SubAreaInputNode, InnerBoolNode, SubAreaOutputNode
	EXPECT_EQ(OwnedArea->GetLastExecutedNodes().size(), OwnedTracePerIteration * IterationCount);

	// Inner node should appear in the trace IterationCount times.
	size_t InnerBoolOccurrences = 0;
	for (Node* Executed : OwnedArea->GetLastExecutedNodes())
	{
		if (Executed == InnerBoolNode)
			InnerBoolOccurrences++;
	}
	EXPECT_EQ(InnerBoolOccurrences, static_cast<size_t>(IterationCount));

	// Second run should clear the previous trace and accumulate the same number of executions again.
	ASSERT_TRUE(ParentArea->ExecuteNodeNetwork());
	EXPECT_EQ(OwnedArea->GetLastExecutedNodes().size(), OwnedTracePerIteration * IterationCount);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, OwnedAreaCleared_GetDataReturnsNull_SubAreaNode_BecomesDangling)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	// Boundary nodes exist.
	ASSERT_NE(SubArea->GetSubAreaInputNode(), nullptr);
	ASSERT_NE(SubArea->GetSubAreaOutputNode(), nullptr);

	OwnedArea->Clear();

	// Boundary nodes are gone.
	EXPECT_EQ(SubArea->GetSubAreaInputNode(), nullptr);
	EXPECT_EQ(SubArea->GetSubAreaOutputNode(), nullptr);

	// Reading data from the SubAreaNode's output socket should not crash and should return nullptr.
	NodeSocket* OutputExecutionSocket = SubArea->GetSocketByIndex(0, NodeSocket::SocketFlow::Output);
	ASSERT_NE(OutputExecutionSocket, nullptr);
	EXPECT_EQ(OutputExecutionSocket->GetData(), nullptr);

	// SubAreaNode should be dangling now.
	EXPECT_TRUE(SubArea->IsDangling());

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetName_OnSubAreaNode_PropagatesTo_OwnedArea_AndIONodes)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	const std::string NewName = "NewName";
	SubArea->SetName(NewName);

	EXPECT_EQ(SubArea->GetName(), NewName);
	EXPECT_EQ(OwnedArea->GetName(), NewName);
	EXPECT_EQ(InputNode->GetName(), NewName + " Input");
	EXPECT_EQ(OutputNode->GetName(), NewName + " Output");

	// Renaming again should overwrite all four consistently.
	const std::string SecondName = "Renamed";
	SubArea->SetName(SecondName);
	EXPECT_EQ(SubArea->GetName(), SecondName);
	EXPECT_EQ(OwnedArea->GetName(), SecondName);
	EXPECT_EQ(InputNode->GetName(), SecondName + " Input");
	EXPECT_EQ(OutputNode->GetName(), SecondName + " Output");

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetName_OnOwnedArea_PropagatesTo_SubAreaNode_AndIONodes)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);
	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	const std::string NewName = "NewName";
	OwnedArea->SetName(NewName);

	EXPECT_EQ(OwnedArea->GetName(), NewName);
	EXPECT_EQ(SubArea->GetName(), NewName);
	EXPECT_EQ(InputNode->GetName(), NewName + " Input");
	EXPECT_EQ(OutputNode->GetName(), NewName + " Output");

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetName_OnInputOrOutputNode_DoesNotPropagateUpward)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(OwnedArea, nullptr);
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	SubArea->SetName("Anchor");
	const std::string SubAreaNameBefore = SubArea->GetName();
	const std::string OwnedAreaNameBefore = OwnedArea->GetName();

	InputNode->SetName("CustomInputName");
	EXPECT_EQ(InputNode->GetName(), "CustomInputName");
	EXPECT_EQ(SubArea->GetName(), SubAreaNameBefore);
	EXPECT_EQ(OwnedArea->GetName(), OwnedAreaNameBefore);
	// The OutputNode should not have been renamed by touching the InputNode.
	EXPECT_EQ(OutputNode->GetName(), SubAreaNameBefore + " Output");

	OutputNode->SetName("CustomOutputName");
	EXPECT_EQ(OutputNode->GetName(), "CustomOutputName");
	EXPECT_EQ(SubArea->GetName(), SubAreaNameBefore);
	EXPECT_EQ(OwnedArea->GetName(), OwnedAreaNameBefore);
	EXPECT_EQ(InputNode->GetName(), "CustomInputName");

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetName_OnTopLevelArea_DoesNotAffectUnrelatedSubAreaNode)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	SubArea->SetName("KeepThisName");

	const std::string SubAreaNameBefore = SubArea->GetName();
	const std::string InputNameBefore = SubArea->GetSubAreaInputNode()->GetName();
	const std::string OutputNameBefore = SubArea->GetSubAreaOutputNode()->GetName();
	const std::string OwnedNameBefore = SubArea->GetOwnedArea()->GetName();

	// ParentArea is not owned by any SubAreaNode; renaming it should not affect the SubAreaNode or its owned area or I/O nodes.
	ParentArea->SetName("RenamedParent");
	EXPECT_EQ(ParentArea->GetName(), "RenamedParent");
	EXPECT_EQ(SubArea->GetName(), SubAreaNameBefore);
	EXPECT_EQ(SubArea->GetSubAreaInputNode()->GetName(), InputNameBefore);
	EXPECT_EQ(SubArea->GetSubAreaOutputNode()->GetName(), OutputNameBefore);
	EXPECT_EQ(SubArea->GetOwnedArea()->GetName(), OwnedNameBefore);

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetName_MultipleSubAreasInParent_NamesAreIndependent)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubA = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	SubAreaNode* SubB = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubA, nullptr);
	ASSERT_NE(SubB, nullptr);
	ASSERT_NE(SubA->GetID(), SubB->GetID());

	SubA->SetName("Alpha");
	SubB->SetName("Beta");

	EXPECT_EQ(SubA->GetName(), "Alpha");
	EXPECT_EQ(SubA->GetOwnedArea()->GetName(), "Alpha");
	EXPECT_EQ(SubA->GetSubAreaInputNode()->GetName(), "Alpha Input");
	EXPECT_EQ(SubA->GetSubAreaOutputNode()->GetName(), "Alpha Output");

	EXPECT_EQ(SubB->GetName(), "Beta");
	EXPECT_EQ(SubB->GetOwnedArea()->GetName(), "Beta");
	EXPECT_EQ(SubB->GetSubAreaInputNode()->GetName(), "Beta Input");
	EXPECT_EQ(SubB->GetSubAreaOutputNode()->GetName(), "Beta Output");

	// Renaming one through the area side must not touch the other.
	SubA->GetOwnedArea()->SetName("AlphaPrime");
	EXPECT_EQ(SubA->GetName(), "AlphaPrime");
	EXPECT_EQ(SubA->GetSubAreaInputNode()->GetName(), "AlphaPrime Input");
	EXPECT_EQ(SubA->GetSubAreaOutputNode()->GetName(), "AlphaPrime Output");

	EXPECT_EQ(SubB->GetName(), "Beta");
	EXPECT_EQ(SubB->GetOwnedArea()->GetName(), "Beta");
	EXPECT_EQ(SubB->GetSubAreaInputNode()->GetName(), "Beta Input");
	EXPECT_EQ(SubB->GetSubAreaOutputNode()->GetName(), "Beta Output");

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetName_NestedSubAreas_NamesPropagateIndependently)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* OuterSubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(OuterSubArea, nullptr);
	NodeArea* OuterOwned = OuterSubArea->GetOwnedArea();
	ASSERT_NE(OuterOwned, nullptr);

	SubAreaNode* InnerSubArea = NODE_SYSTEM.CreateSubAreaNode(OuterOwned->GetID());
	ASSERT_NE(InnerSubArea, nullptr);
	NodeArea* InnerOwned = InnerSubArea->GetOwnedArea();
	ASSERT_NE(InnerOwned, nullptr);

	OuterSubArea->SetName("Outer");
	InnerSubArea->SetName("Inner");

	// Outer naming.
	EXPECT_EQ(OuterSubArea->GetName(), "Outer");
	EXPECT_EQ(OuterOwned->GetName(), "Outer");
	EXPECT_EQ(OuterSubArea->GetSubAreaInputNode()->GetName(), "Outer Input");
	EXPECT_EQ(OuterSubArea->GetSubAreaOutputNode()->GetName(), "Outer Output");

	// Inner naming, untouched by outer.
	EXPECT_EQ(InnerSubArea->GetName(), "Inner");
	EXPECT_EQ(InnerOwned->GetName(), "Inner");
	EXPECT_EQ(InnerSubArea->GetSubAreaInputNode()->GetName(), "Inner Input");
	EXPECT_EQ(InnerSubArea->GetSubAreaOutputNode()->GetName(), "Inner Output");

	// Renaming the outer must NOT affect the inner.
	OuterSubArea->SetName("OuterRenamed");
	EXPECT_EQ(OuterSubArea->GetName(), "OuterRenamed");
	EXPECT_EQ(OuterOwned->GetName(), "OuterRenamed");
	EXPECT_EQ(OuterSubArea->GetSubAreaInputNode()->GetName(), "OuterRenamed Input");
	EXPECT_EQ(OuterSubArea->GetSubAreaOutputNode()->GetName(), "OuterRenamed Output");

	EXPECT_EQ(InnerSubArea->GetName(), "Inner");
	EXPECT_EQ(InnerOwned->GetName(), "Inner");
	EXPECT_EQ(InnerSubArea->GetSubAreaInputNode()->GetName(), "Inner Input");
	EXPECT_EQ(InnerSubArea->GetSubAreaOutputNode()->GetName(), "Inner Output");

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, CopyPaste_PreservesPropagatedNames)
{
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	SubArea->SetName("Original");

	TEST_TOOLS.SimulateCopyPasteNodes({ SubArea }, ParentArea);

	SubAreaNode* PastedSubArea = nullptr;
	for (SubAreaNode* Node : ParentArea->GetNodesByType<SubAreaNode>())
	{
		if (Node->GetID() != SubArea->GetID())
		{
			PastedSubArea = Node;
			break;
		}
	}
	ASSERT_NE(PastedSubArea, nullptr);

	// The pasted SubAreaNode keeps the source name and the propagated names.
	EXPECT_EQ(PastedSubArea->GetName(), "Original");
	NodeArea* PastedOwned = PastedSubArea->GetOwnedArea();
	ASSERT_NE(PastedOwned, nullptr);
	EXPECT_EQ(PastedOwned->GetName(), "Original");
	ASSERT_NE(PastedSubArea->GetSubAreaInputNode(), nullptr);
	ASSERT_NE(PastedSubArea->GetSubAreaOutputNode(), nullptr);
	EXPECT_EQ(PastedSubArea->GetSubAreaInputNode()->GetName(), "Original Input");
	EXPECT_EQ(PastedSubArea->GetSubAreaOutputNode()->GetName(), "Original Output");

	// Renaming the pasted node must not change the original.
	PastedSubArea->SetName("Clone");
	EXPECT_EQ(PastedSubArea->GetName(), "Clone");
	EXPECT_EQ(PastedOwned->GetName(), "Clone");
	EXPECT_EQ(PastedSubArea->GetSubAreaInputNode()->GetName(), "Clone Input");
	EXPECT_EQ(PastedSubArea->GetSubAreaOutputNode()->GetName(), "Clone Output");

	EXPECT_EQ(SubArea->GetName(), "Original");
	EXPECT_EQ(SubArea->GetOwnedArea()->GetName(), "Original");
	EXPECT_EQ(SubArea->GetSubAreaInputNode()->GetName(), "Original Input");
	EXPECT_EQ(SubArea->GetSubAreaOutputNode()->GetName(), "Original Output");

	NODE_SYSTEM.DeleteNodeArea(ParentArea);
}

TEST(SubAreaNodeTests, SetName_Propagation_SurvivesSaveLoad)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);
	SubArea->SetName("Persisted");

	const std::string ParentAreaID = ParentArea->GetID();
	const std::string SubAreaID = SubArea->GetID();

	NODE_SYSTEM.SaveToFile("SubAreaNodeTests_SetName_Propagation.json");
	NODE_SYSTEM.Clear();
	NODE_SYSTEM.LoadFromFile("SubAreaNodeTests_SetName_Propagation.json");

	NodeArea* ReloadedParent = NODE_SYSTEM.GetNodeAreaByID(ParentAreaID);
	ASSERT_NE(ReloadedParent, nullptr);

	std::vector<SubAreaNode*> SubAreaNodes = ReloadedParent->GetNodesByType<SubAreaNode>();
	ASSERT_EQ(SubAreaNodes.size(), 1);
	SubAreaNode* ReloadedSubArea = SubAreaNodes[0];
	EXPECT_EQ(ReloadedSubArea->GetID(), SubAreaID);

	NodeArea* ReloadedOwned = ReloadedSubArea->GetOwnedArea();
	ASSERT_NE(ReloadedOwned, nullptr);
	SubAreaInputNode* ReloadedInput = ReloadedSubArea->GetSubAreaInputNode();
	SubAreaOutputNode* ReloadedOutput = ReloadedSubArea->GetSubAreaOutputNode();
	ASSERT_NE(ReloadedInput, nullptr);
	ASSERT_NE(ReloadedOutput, nullptr);

	// All four names should have been preserved.
	EXPECT_EQ(ReloadedSubArea->GetName(), "Persisted");
	EXPECT_EQ(ReloadedOwned->GetName(), "Persisted");
	EXPECT_EQ(ReloadedInput->GetName(), "Persisted Input");
	EXPECT_EQ(ReloadedOutput->GetName(), "Persisted Output");

	// Relationship must still be live after load: rename via the SubAreaNode
	// and via the OwnedArea, both must propagate.
	ReloadedSubArea->SetName("AfterLoadFromNode");
	EXPECT_EQ(ReloadedSubArea->GetName(), "AfterLoadFromNode");
	EXPECT_EQ(ReloadedOwned->GetName(), "AfterLoadFromNode");
	EXPECT_EQ(ReloadedInput->GetName(), "AfterLoadFromNode Input");
	EXPECT_EQ(ReloadedOutput->GetName(), "AfterLoadFromNode Output");

	ReloadedOwned->SetName("AfterLoadFromArea");
	EXPECT_EQ(ReloadedSubArea->GetName(), "AfterLoadFromArea");
	EXPECT_EQ(ReloadedOwned->GetName(), "AfterLoadFromArea");
	EXPECT_EQ(ReloadedInput->GetName(), "AfterLoadFromArea Input");
	EXPECT_EQ(ReloadedOutput->GetName(), "AfterLoadFromArea Output");

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, AddNode_DefaultConstructedSubAreaNode_IsRejected)
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);

	// NODE_FACTORY uses the parameter-less SubAreaNode constructor, which leaves OwnedAreaID empty.
	Node* DefaultConstructedSubAreaNode = NODE_FACTORY.CreateNode("SubAreaNode");
	ASSERT_NE(DefaultConstructedSubAreaNode, nullptr);

	// Adding a default constructed SubAreaNode should fail since it has no owned area, and it should not be added to the area.
	EXPECT_FALSE(Area->AddNode(DefaultConstructedSubAreaNode));
	EXPECT_EQ(Area->GetNodesByType<SubAreaNode>().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, CopyNode_DefaultConstructedSubAreaNode_Is_Not_Crashing)
{
	NODE_SYSTEM.Clear();

	Node* DefaultConstructedSubAreaNode = NODE_FACTORY.CreateNode("SubAreaNode");
	ASSERT_NE(DefaultConstructedSubAreaNode, nullptr);

	Node* Copy = NODE_FACTORY.CopyNode("SubAreaNode", *DefaultConstructedSubAreaNode);
	ASSERT_NE(Copy, nullptr);

	// The copy inherits the broken state and should be rejected by AddNode.
	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	EXPECT_FALSE(Area->AddNode(Copy));
	EXPECT_EQ(Area->GetNodesByType<SubAreaNode>().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, MoveNodesTo_OwnedAreaToParentArea_DoesNotCreateSelfParentingArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	NODE_SYSTEM.MoveNodesTo(OwnedArea, ParentArea);

	EXPECT_EQ(ParentArea->GetParent(), nullptr);

	NodeArea* UnrelatedArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(UnrelatedArea, nullptr);
	EXPECT_FALSE(ParentArea->IsChildOf(UnrelatedArea));
	EXPECT_FALSE(UnrelatedArea->IsParentOf(ParentArea));

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, MoveNodesTo_That_WouldCreateOwnershipCycle_IsRejected)
{
	NODE_SYSTEM.Clear();

	NodeArea* Root = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Root, nullptr);

	SubAreaNode* OuterSubArea = NODE_SYSTEM.CreateSubAreaNode(Root->GetID());
	ASSERT_NE(OuterSubArea, nullptr);
	NodeArea* AreaA = OuterSubArea->GetOwnedArea();
	ASSERT_NE(AreaA, nullptr);

	SubAreaNode* InnerSubArea = NODE_SYSTEM.CreateSubAreaNode(AreaA->GetID());
	ASSERT_NE(InnerSubArea, nullptr);
	NodeArea* AreaB = InnerSubArea->GetOwnedArea();
	ASSERT_NE(AreaB, nullptr);

	// Moving OuterSubArea (owner of AreaA) into AreaB (a descendant of AreaA) would form AreaA <=> AreaB.
	EXPECT_FALSE(NODE_SYSTEM.MoveNodesTo(Root, AreaB));

	EXPECT_EQ(OuterSubArea->GetParentArea(), Root);
	EXPECT_EQ(OuterSubArea->GetOwnedArea(), AreaA);
	EXPECT_EQ(Root->GetNodeByID(OuterSubArea->GetID()), OuterSubArea);
	EXPECT_EQ(AreaB->GetNodeByID(OuterSubArea->GetID()), nullptr);

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, MoveNodesTo_IntoOwnOwnedArea_IsRejected_AndPreservesOwnership)
{
	NODE_SYSTEM.Clear();

	NodeArea* Root = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Root, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(Root->GetID());
	ASSERT_NE(SubArea, nullptr);
	NodeArea* Owned = SubArea->GetOwnedArea();
	ASSERT_NE(Owned, nullptr);
	const std::string OwnedID = Owned->GetID();

	// Moving Root's nodes (just SubArea) into the area SubArea itself owns would form a cycle.
	EXPECT_FALSE(NODE_SYSTEM.MoveNodesTo(Root, Owned));

	EXPECT_EQ(SubArea->GetParentArea(), Root);
	EXPECT_EQ(SubArea->GetOwnedArea(), Owned);
	EXPECT_NE(NODE_SYSTEM.GetNodeAreaByID(OwnedID), nullptr);

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, MoveNodesTo_DoesNotMoveSubAreaBoundaryNodes)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	SubAreaInputNode* InputNode = SubArea->GetSubAreaInputNode();
	SubAreaOutputNode* OutputNode = SubArea->GetSubAreaOutputNode();
	ASSERT_NE(InputNode, nullptr);
	ASSERT_NE(OutputNode, nullptr);

	// A regular node in the owned area should still be movable.
	Node* RegularNode = new BeginNode();
	ASSERT_TRUE(OwnedArea->AddNode(RegularNode));
	const std::string RegularNodeID = RegularNode->GetID();

	EXPECT_TRUE(NODE_SYSTEM.MoveNodesTo(OwnedArea, ParentArea));

	// The hidden SubAreaInputNode and SubAreaOutputNode must stay inside the owned area.
	EXPECT_EQ(OwnedArea->GetNodeByID(InputNode->GetID()), InputNode);
	EXPECT_EQ(OwnedArea->GetNodeByID(OutputNode->GetID()), OutputNode);
	EXPECT_EQ(InputNode->GetParentArea(), OwnedArea);
	EXPECT_EQ(OutputNode->GetParentArea(), OwnedArea);
	EXPECT_EQ(ParentArea->GetNodesByType<SubAreaInputNode>().size(), 0);
	EXPECT_EQ(ParentArea->GetNodesByType<SubAreaOutputNode>().size(), 0);

	// The regular node was still moved out into the target area.
	EXPECT_EQ(OwnedArea->GetNodeByID(RegularNodeID), nullptr);
	EXPECT_EQ(ParentArea->GetNodeByID(RegularNodeID), RegularNode);
	EXPECT_EQ(RegularNode->GetParentArea(), ParentArea);

	// The SubAreaNode stays consistent: accessors resolve and IsDangling agrees.
	EXPECT_EQ(SubArea->GetSubAreaInputNode(), InputNode);
	EXPECT_EQ(SubArea->GetSubAreaOutputNode(), OutputNode);
	EXPECT_FALSE(SubArea->IsDangling());

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, CopyNodesTo_OwnedAreaToParentArea_DoesNotCreateSelfParentingArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	NODE_SYSTEM.CopyNodesTo(OwnedArea, ParentArea);

	EXPECT_EQ(ParentArea->GetParent(), nullptr);

	NodeArea* UnrelatedArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(UnrelatedArea, nullptr);
	EXPECT_FALSE(ParentArea->IsChildOf(UnrelatedArea));
	EXPECT_FALSE(UnrelatedArea->IsParentOf(ParentArea));

	NODE_SYSTEM.Clear();
}

TEST(SubAreaNodeTests, CopyNodesTo_OwnedAreaToUnrelatedArea_DoesNotCreatePhantomParent)
{
	NODE_SYSTEM.Clear();

	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(ParentArea, nullptr);

	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentArea->GetID());
	ASSERT_NE(SubArea, nullptr);

	NodeArea* OwnedArea = SubArea->GetOwnedArea();
	ASSERT_NE(OwnedArea, nullptr);

	NodeArea* UnrelatedArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(UnrelatedArea, nullptr);

	NODE_SYSTEM.CopyNodesTo(OwnedArea, UnrelatedArea);

	EXPECT_EQ(UnrelatedArea->GetParent(), nullptr);
	EXPECT_FALSE(UnrelatedArea->IsChildOf(ParentArea));
	EXPECT_FALSE(ParentArea->IsParentOf(UnrelatedArea));

	NODE_SYSTEM.Clear();
}