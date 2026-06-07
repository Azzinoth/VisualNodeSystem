#include "NodeAreaLoadTests.h"
using namespace VisNodeSys;

TEST(NodeAreaLoadTest, BasicSaveLoad)
{
	NODE_SYSTEM.Clear();

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);
	std::string NodeAreaID = NodeArea->GetID();
	NodeArea->SetName("TestNodeArea");

	EXPECT_TRUE(TEST_TOOLS.VerifyNodeAreaSaveLoadCycle_BasicChecks(NodeArea));
	NodeArea = NODE_SYSTEM.GetNodeAreaByID(NodeAreaID);
	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadEmptyJson)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = "{}";
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), false);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadJsonWithOnlyOffset)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({ "RenderOffset": {"X": 100.5, "Y": -50.0} })";
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), false);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadJsonWithIncorrectNodes)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({ "Nodes": "invalid" })";
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), false);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadShouldClear)
{
	NODE_SYSTEM.Clear();

	std::vector<std::string> NodesIDList;
	std::vector<std::string> GroupCommentsIDList;

	NodeArea* NodeArea = TEST_TOOLS.CreateTinyPopulatedNodeArea(NodesIDList, GroupCommentsIDList);
	ASSERT_NE(NodeArea, nullptr);
	ASSERT_EQ(NodeArea->GetNodeCount(), 11);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 5);
	ASSERT_EQ(NodeArea->GetRerouteConnectionCount(), 2);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 1);

	std::string JsonString = NodeArea->ToJson();

	Node* NewNode = new Node();
	EXPECT_TRUE(NodeArea->AddNode(NewNode));
	ASSERT_EQ(NodeArea->GetNodeCount(), 12);

	NodeArea->LoadFromJson(JsonString);
	ASSERT_EQ(NodeArea->GetNodeCount(), 11);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 5);
	ASSERT_EQ(NodeArea->GetRerouteConnectionCount(), 2);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeMissingType)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({ 
		"Nodes": { 
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"Position": {"X":0, "Y":0}, 
				"Size": {"X":100, "Y":50}, 
				"Name":"Test" 
			} 
		} 
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeTypeNotString)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({ 
		"Nodes": { 
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"NodeType": 123, 
				"Position": {"X":0, "Y":0}, 
				"Size": {"X":100, "Y":50}, 
				"Name":"Test" 
			} 
		} 
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeUnregisteredType)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({ 
		"Nodes": { 
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"NodeType": "NonExistentNode", 
				"Position": {"X":0, "Y":0}, 
				"Size": {"X":100, "Y":50}, 
				"Name":"Test" 
			} 
		} 
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeWithMoreInputsThanClassDefinition)
{
	NODE_SYSTEM.Clear();

	// CustomNode2 normally has 1 input, but JSON provides 2 inputs.
	// This simulates loading an old file after the class lost a socket.
	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"NodeType": "CustomNode2",
				"Position": {"X": 10, "Y": 10},
				"Size": {"X": 150, "Y": 80},
				"Name": "Extra Input Node",
				"Input": {
					"0": {"ID": "231D2D4F1D033A39393A157C", "Name": "Input 1", "AllowedTypes":["FLOAT"]},
					"1": {"ID": "777D5D4709303379786D3B34", "Name": "Input 2", "AllowedTypes":["FLOAT"]}
				},
				"Output": {
					"0": {"ID": "6E58750D6D46781643536C0F", "Name": "Output 1", "AllowedTypes":["FLOAT"]}
				}
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 1);

	const Node* LoadedNode = NodeArea->GetNodeByID("6621620F42545B420C103443");
	ASSERT_NE(LoadedNode, nullptr);
	EXPECT_EQ(LoadedNode->GetInputSocketCount(), 2);
	EXPECT_EQ(LoadedNode->GetOutputSocketCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeWithFewerInputsThanClassDefinition)
{
	NODE_SYSTEM.Clear();

	// CustomNode2 normally has 1 input, but JSON provides 0 inputs.
	// This simulates loading an old file after the class gained a socket.
	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"NodeType": "CustomNode2",
				"Position": {"X": 10, "Y": 10},
				"Size": {"X": 150, "Y": 80},
				"Name": "No Input Node",
				"Input": {},
				"Output": {
					"0": {"ID": "6E58750D6D46781643536C0F", "Name": "Output 1", "AllowedTypes":["FLOAT"]}
				}
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 1);

	const Node* LoadedNode = NodeArea->GetNodeByID("6621620F42545B420C103443");
	ASSERT_NE(LoadedNode, nullptr);
	EXPECT_EQ(LoadedNode->GetInputSocketCount(), 0);
	EXPECT_EQ(LoadedNode->GetOutputSocketCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeWithMoreOutputsThanClassDefinition)
{
	NODE_SYSTEM.Clear();

	// CustomNode2 normally has 1 output, but JSON provides 3 outputs.
	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"NodeType": "CustomNode2",
				"Position": {"X": 10, "Y": 10},
				"Size": {"X": 150, "Y": 80},
				"Name": "Extra Output Node",
				"Input": {
					"0": {"ID": "231D2D4F1D033A39393A157C", "Name": "Input 1", "AllowedTypes":["FLOAT"]}
				},
				"Output": {
					"0": {"ID": "6E58750D6D46781643536C0F", "Name": "Output 1", "AllowedTypes":["FLOAT"]},
					"1": {"ID": "7F69860E7E80424F503947AC", "Name": "Output 2", "AllowedTypes":["FLOAT"]},
					"2": {"ID": "8A7A971F8F91535060504ABC", "Name": "Output 3", "AllowedTypes":["FLOAT"]}
				}
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 1);

	const Node* LoadedNode = NodeArea->GetNodeByID("6621620F42545B420C103443");
	ASSERT_NE(LoadedNode, nullptr);
	EXPECT_EQ(LoadedNode->GetInputSocketCount(), 1);
	EXPECT_EQ(LoadedNode->GetOutputSocketCount(), 3);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeWithMismatchedSocketsAndConnections)
{
	NODE_SYSTEM.Clear();

	// Two CustomNode2 nodes, each with extra sockets beyond class definition.
	// A valid connection exists between them through the "extra" sockets.
	// Verifies the full round-trip: mismatched load + connection restoration.
	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "0E341B791D445B0B2E5B7534",
				"NodeType": "CustomNode2",
				"Position": {"X": 0, "Y": 0},
				"Size": {"X": 150, "Y": 80},
				"Name": "Source",
				"Input": {
					"0": {"ID": "7B63360C3D3E410D671A0A26", "Name": "Input 1", "AllowedTypes":["FLOAT"]}
				},
				"Output": {
					"0": {"ID": "33763545003D3B0C2B557301", "Name": "Output 1", "AllowedTypes":["FLOAT"]},
					"1": {"ID": "44874656114D4C1D782A1B12", "Name": "Output 2", "AllowedTypes":["FLOAT"]}
				}
			},
			"1": {
				"ID": "6D61442D3E48292F126B7E07",
				"NodeType": "CustomNode2",
				"Position": {"X": 200, "Y": 0},
				"Size": {"X": 150, "Y": 80},
				"Name": "Destination",
				"Input": {
					"0": {"ID": "4500261C5F10075178584777", "Name": "Input 1", "AllowedTypes":["FLOAT"]},
					"1": {"ID": "5611372D604121626839287A", "Name": "Input 2", "AllowedTypes":["FLOAT"]}
				},
				"Output": {
					"0": {"ID": "214D5382056A20645E3C2504", "Name": "Output 1", "AllowedTypes":["FLOAT"]}
				}
			}
		},
		"Connections": {
			"0": {
				"In": {"SocketID": "5611372D604121626839287A", "NodeID": "6D61442D3E48292F126B7E07"},
				"Out": {"SocketID": "44874656114D4C1D782A1B12", "NodeID": "0E341B791D445B0B2E5B7534"}
			}
		},
		"RenderOffset": {"X": 0, "Y": 0}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 1);

	const Node* SourceNode = NodeArea->GetNodeByID("0E341B791D445B0B2E5B7534");
	const Node* DestNode = NodeArea->GetNodeByID("6D61442D3E48292F126B7E07");
	ASSERT_NE(SourceNode, nullptr);
	ASSERT_NE(DestNode, nullptr);

	EXPECT_EQ(SourceNode->GetOutputSocketCount(), 2);
	EXPECT_EQ(DestNode->GetInputSocketCount(), 2);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadConnectionEntryNotObject)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": { 
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"NodeType": "CustomNode", 
				"Position": {"X":0,"Y":0},
				"Size":{"X":1,"Y":1},
				"Name":"N1"
			}
		},
		"Connections": { 
			"0": "invalid_connection" 
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 1);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadConnectionMissingInOrOut)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"NodeType": "CustomNode5", 
				"Position": {"X":0,"Y":0},
				"Size":{"X":1,"Y":1},
				"Name":"N1", 
				"Input": {
					"0": {
						"ID": "567D5D4709303379786D3B34", 
						"Name":"In", 
						"AllowedTypes":["FLOAT"]
					}
				},
				"Output": {
					"0": {
						"ID": "777D5D4709303379786D3B34", 
						"Name":"Out", 
						"AllowedTypes":["FLOAT"]
					}
				}
			},
			"1": { 
				"ID": "6E58750D6D46781643536C0F", 
				"NodeType": "CustomNode5", 
				"Position": {"X":200,"Y":0},
				"Size":{"X":1,"Y":1},
				"Name":"N2", 
				"Input": {
					"0": {
						"ID": "167D5D4709303379786D3B34", 
						"Name":"In", 
						"AllowedTypes":["FLOAT"]
					}
				},
				"Output": {
					"0": {
						"ID": "134D5D4709303379786D3B34", 
						"Name":"Out", 
						"AllowedTypes":["FLOAT"]
					}
				}
			}
		},
		"Connections": {
			"0": {
				"Out": {"SocketID": "777D5D4709303379786D3B34", "NodeID": "6621620F42545B420C103443"}
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadConnectionMissingIDs)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"NodeType": "CustomNode5", 
				"Position": {"X":0,"Y":0},
				"Size":{"X":1,"Y":1},
				"Name":"N1", 
				"Input": {
					"0": {
						"ID": "567D5D4709303379786D3B34", 
						"Name":"In", 
						"AllowedTypes":["FLOAT"]
					}
				},
				"Output": {
					"0": {
						"ID": "777D5D4709303379786D3B34", 
						"Name":"Out", 
						"AllowedTypes":["FLOAT"]
					}
				}
			},
			"1": { 
				"ID": "6E58750D6D46781643536C0F", 
				"NodeType": "CustomNode5", 
				"Position": {"X":200,"Y":0},
				"Size":{"X":1,"Y":1},
				"Name":"N2", 
				"Input": {
					"0": {
						"ID": "167D5D4709303379786D3B34", 
						"Name":"In", 
						"AllowedTypes":["FLOAT"] 
					}
				},
				"Output": {
					"0": {
						"ID": "134D5D4709303379786D3B34", 
						"Name":"Out", 
						"AllowedTypes":["FLOAT"]
					}
				}
			}
		},
		"Connections": {
			"0": {
				"In": {"SocketID": "567D5D4709303379786D3B34"},
				"Out": {"NodeID": "6621620F42545B420C103443"}
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadIncorrectGroupCommentFromJson)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"NodeType": "CustomNode5", 
				"Position": {"X":0,"Y":0},
				"Size":{"X":1,"Y":1},
				"Name":"N1", 
				"Input": {
					"0": {
						"ID": "567D5D4709303379786D3B34", 
						"Name":"In", 
						"AllowedTypes":["FLOAT"]
					}
				},
				"Output": {
					"0": {
						"ID": "777D5D4709303379786D3B34", 
						"Name":"Out", 
						"AllowedTypes":["FLOAT"]
					}
				}
			},
			"1": { 
				"ID": "6E58750D6D46781643536C0F", 
				"NodeType": "CustomNode5", 
				"Position": {"X":200,"Y":0},
				"Size":{"X":1,"Y":1},
				"Name":"N2", 
				"Input": {
					"0": {
						"ID": "167D5D4709303379786D3B34", 
						"Name":"In", 
						"AllowedTypes":["FLOAT"] 
					}
				},
				"Output": {
					"0": {
						"ID": "134D5D4709303379786D3B34", 
						"Name":"Out", 
						"AllowedTypes":["FLOAT"]
					}
				}
			}
		},
		"GroupComments": { 
			"0": "invalid_comment" 
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadConnectionFailedTryToConnect_IncompatibleSockets)
{
	NODE_SYSTEM.Clear();

	// Node 0 (n1): FLOAT output
	// Node 1 (n2): FLOAT input, FLOAT output
	// Node 2 (n3): FLOAT input, FLOAT output
	// Connection 0 (Valid): n1 (out) -> n2 (in)  (FLOAT -> FLOAT)
	// Connection 1 (Invalid): n2 (out) -> n3 (out) (FLOAT output -> FLOAT output)
	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "0E341B791D445B0B2E5B7534", "NodeType": "CustomNode5", "NodeStyle": 0,
				"Position": {"X": 0, "Y": 0}, "Size": {"X": 100, "Y": 80}, "Name": "Node 1",
				"Input": {"0": {"ID": "7B63360C3D3E410D671A0A26", "Name": "In", "AllowedTypes":["FLOAT"]}},
				"Output": {"0": {"ID": "33763545003D3B0C2B557301", "Name": "Out", "AllowedTypes":["FLOAT"]}}
			},
			"1": {
				"ID": "6D61442D3E48292F126B7E07", "NodeType": "CustomNode5", "NodeStyle": 0,
				"Position": {"X": 200, "Y": 0}, "Size": {"X": 100, "Y": 80}, "Name": "Node 2",
				"Input": {"0": {"ID": "4500261C5F10075178584777", "Name": "In", "AllowedTypes":["FLOAT"]}},
				"Output": {"0": {"ID": "214D5382056A20645E3C2504", "Name": "Out", "AllowedTypes":["FLOAT"]}}
			},
			"2": {
				"ID": "72387012000A79220E2B0601", "NodeType": "CustomNode5", "NodeStyle": 0,
				"Position": {"X": 400, "Y": 0}, "Size": {"X": 100, "Y": 80}, "Name": "Node 3",
				"Input": {"0": {"ID": "30108938125F62440D440F76", "Name": "In", "AllowedTypes":["FLOAT"]}},
				"Output": {"0": {"ID": "317C1F777F3B51427B081613", "Name": "Out", "AllowedTypes":["FLOAT"]}}
			}
		},
		"Connections": {
			"0": {
				"In": {"SocketID": "4500261C5F10075178584777", "NodeID": "6D61442D3E48292F126B7E07"},
				"Out": {"SocketID": "33763545003D3B0C2B557301", "NodeID": "0E341B791D445B0B2E5B7534"}
			},
			"1": {
				"In": {"SocketID": "317C1F777F3B51427B081613", "NodeID": "72387012000A79220E2B0601"},
				"Out": {"SocketID": "214D5382056A20645E3C2504", "NodeID": "6D61442D3E48292F126B7E07"}
			}
		},
        "RenderOffset": {"X": 0, "Y": 0}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);

	ASSERT_EQ(NodeArea->GetNodeCount(), 3);
	ASSERT_NE(NodeArea->GetNodeByID("0E341B791D445B0B2E5B7534"), nullptr);
	ASSERT_NE(NodeArea->GetNodeByID("6D61442D3E48292F126B7E07"), nullptr);
	ASSERT_NE(NodeArea->GetNodeByID("72387012000A79220E2B0601"), nullptr);

	ASSERT_EQ(NodeArea->GetConnectionCount(), 1);

	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadWhenRerouteConnectionsInfoIsNull)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Connections" : 
		{
			"0" : 
			{
				"In" : 
				{
					"NodeID" : "6451100C0778214351766B7E",
					"SocketID" : "1C191657360B7903691A5B66",
					"SocketIndex" : 0
				},
				"Out" : 
				{
					"NodeID" : "5C0E2E1D5D005C3E70421A67",
					"SocketID" : "5D7E2A574B7D3C3069521C3F",
					"SocketIndex" : 0
				},
				"RerouteConnections" : 
				{
					
				}
			}
		},
		"Nodes" : 
		{
			"0" : 
			{
				"ID" : "6451100C0778214351766B7E",
				"Input" : 
				{
					"0" : 
					{
						"ID" : "1C191657360B7903691A5B66",
						"Name" : "In",
						"AllowedTypes":["EXECUTE"]
					}
				},
				"Name" : "Some node",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Position" : 
				{
					"X" : 350.0,
					"Y" : 490.0
				},
				"Size" : 
				{
					"X" : 120.0,
					"Y" : 80.0
				}
			},
			"1" : 
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"Name" : "Some node",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Output" : 
				{
					"0" : 
					{
						"ID" : "5D7E2A574B7D3C3069521C3F",
						"Name" : "Out",
						"AllowedTypes":["EXECUTE"]
					}
				},
				"Position" : 
				{
					"X" : 10.0,
					"Y" : 490.0
				},
				"Size" : 
				{
					"X" : 120.0,
					"Y" : 80.0
				}
			},
		},
		"RenderOffset" : 
		{
			"X" : 0.0,
			"Y" : 0.0
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);

	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_NE(NodeArea->GetNodeByID("6451100C0778214351766B7E"), nullptr);
	ASSERT_NE(NodeArea->GetNodeByID("5C0E2E1D5D005C3E70421A67"), nullptr);

	ASSERT_EQ(NodeArea->GetConnectionCount(), 1);

	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);
	ASSERT_EQ(NodeArea->GetRerouteConnectionCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadWhenRerouteConnectionsInfoIsDamaged)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Connections" : 
		{
			"0" : 
			{
				"In" : 
				{
					"NodeID" : "6451100C0778214351766B7E",
					"SocketID" : "1C191657360B7903691A5B66",
					"SocketIndex" : 0
				},
				"Out" : 
				{
					"NodeID" : "5C0E2E1D5D005C3E70421A67",
					"SocketID" : "5D7E2A574B7D3C3069521C3F",
					"SocketIndex" : 0
				},
				"RerouteConnections" : 
				{
					"0" : 
					{
						"BeginRerouteID" : "",
						"BeginSocketID" : "NONE",
						"EndRerouteID" : "5E697F0B007F313E4F38577B",
						"EndSocketID" : "",
						"PositionX" : 190.0,
						"PositionY" : 470.0,
						"RerouteID" : "6E3F063E430C12373C68654C"
					},
					"1" : 
					{
						"BeginRerouteID" : "6E3F063E430C12373C68654C",
						"BeginSocketID" : "",
						"EndRerouteID" : "",
						"EndSocketID" : "1C191657360B7903691A5B66",
						"PositionX" : 312.0,
						"PositionY" : 470.0,
						"RerouteID" : "5E697F0B007F313E4F38577B"
					}
				}
			}
		},
		"Nodes" : 
		{
			"0" : 
			{
				"ID" : "6451100C0778214351766B7E",
				"Input" : 
				{
					"0" : 
					{
						"ID" : "1C191657360B7903691A5B66",
						"Name" : "In",
						"AllowedTypes":["EXECUTE"]
					}
				},
				"Name" : "Some node",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Position" : 
				{
					"X" : 350.0,
					"Y" : 490.0
				},
				"Size" : 
				{
					"X" : 120.0,
					"Y" : 80.0
				}
			},
			"1" : 
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"Name" : "Some node",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Output" : 
				{
					"0" : 
					{
						"ID" : "5D7E2A574B7D3C3069521C3F",
						"Name" : "Out",
						"AllowedTypes":["EXECUTE"]
					}
				},
				"Position" : 
				{
					"X" : 10.0,
					"Y" : 490.0
				},
				"Size" : 
				{
					"X" : 120.0,
					"Y" : 80.0
				}
			},
		},
		"RenderOffset" : 
		{
			"X" : 0.0,
			"Y" : 0.0
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);

	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_NE(NodeArea->GetNodeByID("6451100C0778214351766B7E"), nullptr);
	ASSERT_NE(NodeArea->GetNodeByID("5C0E2E1D5D005C3E70421A67"), nullptr);

	ASSERT_EQ(NodeArea->GetConnectionCount(), 1);

	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);
	ASSERT_EQ(NodeArea->GetRerouteConnectionCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadWhenRerouteConnectionsInfoIsDamaged_2)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Connections" : 
		{
			"0" : 
			{
				"In" : 
				{
					"NodeID" : "6451100C0778214351766B7E",
					"SocketID" : "1C191657360B7903691A5B66",
					"SocketIndex" : 0
				},
				"Out" : 
				{
					"NodeID" : "5C0E2E1D5D005C3E70421A67",
					"SocketID" : "5D7E2A574B7D3C3069521C3F",
					"SocketIndex" : 0
				},
				"RerouteConnections" : 
				{
					"0" : 
					{
						"BeginRerouteID" : "",
						"BeginSocketID" : "5D7E2A574B7D3C3069521C3F",
						"EndRerouteID" : "5E697F0B007F313E4F38577B",
						"EndSocketID" : "",
						"PositionX" : 190.0,
						"PositionY" : 470.0,
						"RerouteID" : "6E3F063E430C12373C68654C"
					},
					"1" : 
					{
						"BeginRerouteID" : "6E3F063E430C12373C68654C",
						"BeginSocketID" : "",
						"EndRerouteID" : "",
						"EndSocketID" : 4587,
						"PositionX" : 312.0,
						"PositionY" : 470.0,
						"RerouteID" : "5E697F0B007F313E4F38577B"
					}
				}
			}
		},
		"Nodes" : 
		{
			"0" : 
			{
				"ID" : "6451100C0778214351766B7E",
				"Input" : 
				{
					"0" : 
					{
						"ID" : "1C191657360B7903691A5B66",
						"Name" : "In",
						"AllowedTypes":["EXECUTE"]
					}
				},
				"Name" : "Some node",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Position" : 
				{
					"X" : 350.0,
					"Y" : 490.0
				},
				"Size" : 
				{
					"X" : 120.0,
					"Y" : 80.0
				}
			},
			"1" : 
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"Name" : "Some node",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Output" : 
				{
					"0" : 
					{
						"ID" : "5D7E2A574B7D3C3069521C3F",
						"Name" : "Out",
						"AllowedTypes":["EXECUTE"]
					}
				},
				"Position" : 
				{
					"X" : 10.0,
					"Y" : 490.0
				},
				"Size" : 
				{
					"X" : 120.0,
					"Y" : 80.0
				}
			},
		},
		"RenderOffset" : 
		{
			"X" : 0.0,
			"Y" : 0.0
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);

	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_NE(NodeArea->GetNodeByID("6451100C0778214351766B7E"), nullptr);
	ASSERT_NE(NodeArea->GetNodeByID("5C0E2E1D5D005C3E70421A67"), nullptr);

	ASSERT_EQ(NodeArea->GetConnectionCount(), 1);

	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);
	ASSERT_EQ(NodeArea->GetRerouteConnectionCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadWhenRerouteConnectionsInfoIsDamaged_3)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Connections" : 
		{
			"0" : 
			{
				"In" : 
				{
					"NodeID" : "6451100C0778214351766B7E",
					"SocketID" : "1C191657360B7903691A5B66",
					"SocketIndex" : 0
				},
				"Out" : 
				{
					"NodeID" : "5C0E2E1D5D005C3E70421A67",
					"SocketID" : "5D7E2A574B7D3C3069521C3F",
					"SocketIndex" : 0
				},
				"RerouteConnections" : 
				{
					"0" : 
					{
						"BeginRerouteID" : "",
						"BeginSocketID" : "5D7E2A574B7D3C3069521C3F",
						"EndRerouteID" : "5E697F0B007F313E4F38577B",
						"EndSocketID" : "",
						"PositionX" : 190.0,
						"PositionY" : 470.0,
						"RerouteID" : "6E3F063E430C12373C68654C"
					},
					"1" : 
					{
						"BeginRerouteID" : "6E3F063E430C12373C68654C",
						"BeginSocketID" : "",
						"EndRerouteID" : "5E697F0B007F313E4F38577B",
						"EndSocketID" : "1C191657360B7903691A5B66",
						"PositionX" : 312.0,
						"PositionY" : 470.0,
						"RerouteID" : "5E697F0B007F313E4F38577B"
					}
				}
			}
		},
		"Nodes" : 
		{
			"0" : 
			{
				"ID" : "6451100C0778214351766B7E",
				"Input" : 
				{
					"0" : 
					{
						"ID" : "1C191657360B7903691A5B66",
						"Name" : "In",
						"AllowedTypes":["EXECUTE"]
					}
				},
				"Name" : "Some node",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Position" : 
				{
					"X" : 350.0,
					"Y" : 490.0
				},
				"Size" : 
				{
					"X" : 120.0,
					"Y" : 80.0
				}
			},
			"1" : 
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"Name" : "Some node",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Output" : 
				{
					"0" : 
					{
						"ID" : "5D7E2A574B7D3C3069521C3F",
						"Name" : "Out",
						"AllowedTypes":["EXECUTE"]
					}
				},
				"Position" : 
				{
					"X" : 10.0,
					"Y" : 490.0
				},
				"Size" : 
				{
					"X" : 120.0,
					"Y" : 80.0
				}
			},
		},
		"RenderOffset" : 
		{
			"X" : 0.0,
			"Y" : 0.0
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);

	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_NE(NodeArea->GetNodeByID("6451100C0778214351766B7E"), nullptr);
	ASSERT_NE(NodeArea->GetNodeByID("5C0E2E1D5D005C3E70421A67"), nullptr);

	ASSERT_EQ(NodeArea->GetConnectionCount(), 1);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);
	ASSERT_EQ(NodeArea->GetRerouteConnectionCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadConnection_With_MixedReroute_Validity)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Connections" :
		{
			"0" :
			{
				"In" :
				{
					"NodeID" : "6451100C0778214351766B7E",
					"SocketID" : "1C191657360B7903691A5B66",
					"SocketIndex" : 0
				},
				"Out" :
				{
					"NodeID" : "5C0E2E1D5D005C3E70421A67",
					"SocketID" : "5D7E2A574B7D3C3069521C3F",
					"SocketIndex" : 0
				},
				"RerouteConnections" :
				{
					"0" :
					{
						"BeginRerouteID" : "",
						"BeginSocketID" : "5D7E2A574B7D3C3069521C3F",
						"EndRerouteID" : "AAAA000000000000AAAA0000",
						"EndSocketID" : "",
						"PositionX" : 100.0,
						"PositionY" : 100.0,
						"RerouteID" : "1111000000000000111100000"
					},
					"1" :
					{
						"BeginRerouteID" : "",
						"BeginSocketID" : "",
						"EndRerouteID" : "",
						"EndSocketID" : ""
					},
					"2" :
					{
						"BeginRerouteID" : "AAAA000000000000AAAA0000",
						"BeginSocketID" : "",
						"EndRerouteID" : "",
						"EndSocketID" : "1C191657360B7903691A5B66",
						"PositionX" : 200.0,
						"PositionY" : 200.0,
						"RerouteID" : "AAAA000000000000AAAA0000"
					}
				}
			}
		},
		"Nodes" :
		{
			"0" :
			{
				"ID" : "6451100C0778214351766B7E",
				"Input" :
				{
					"0" : { "ID" : "1C191657360B7903691A5B66", "Name" : "In", "AllowedTypes":["EXECUTE"] }
				},
				"Name" : "Sink",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Position" : { "X" : 350.0, "Y" : 490.0 },
				"Size" : { "X" : 120.0, "Y" : 80.0 }
			},
			"1" :
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"Name" : "Source",
				"NodeStyle" : 0,
				"NodeType" : "VisualNode",
				"Output" :
				{
					"0" : { "ID" : "5D7E2A574B7D3C3069521C3F", "Name" : "Out", "AllowedTypes":["EXECUTE"] }
				},
				"Position" : { "X" : 10.0, "Y" : 490.0 },
				"Size" : { "X" : 120.0, "Y" : 80.0 }
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);

	EXPECT_EQ(NodeArea->GetNodeCount(), 2);
	EXPECT_EQ(NodeArea->GetConnectionCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadMediumNodeArea)
{
	NODE_SYSTEM.Clear();

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromFile("Resources/MediumNodeArea.json"), true);

	ASSERT_EQ(NodeArea->GetNodeCount(), 170);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 205);
	ASSERT_EQ(NodeArea->GetRerouteConnectionCount(), 18);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 1);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, ConnectionOrderingTest)
{
	NODE_SYSTEM.Clear();

	std::ifstream NodesFile;
	NodesFile.open("Resources/ConnectionOrderingTestData.json");

	const std::string FileData((std::istreambuf_iterator<char>(NodesFile)), std::istreambuf_iterator<char>());
	NodesFile.close();

	Json::Value Root;
	JSONCPP_STRING Error;
	Json::CharReaderBuilder Builder;

	const std::unique_ptr<Json::CharReader> Reader(Builder.newCharReader());
	ASSERT_EQ(Reader->parse(FileData.c_str(), FileData.c_str() + FileData.size(), &Root, &Error), true);

	std::vector<Json::String> ConnectionIDList = Root["Connections"].getMemberNames();
	std::map<Json::String, Json::Value> OriginalConnectionMap;
	const Json::Value& ConnectionsObject = Root["Connections"];
	std::vector<Json::String> ConnectionKeys = Root["Connections"].getMemberNames();

	for (const auto& ConnectionID : ConnectionKeys)
	{
		if (ConnectionsObject.isMember(ConnectionID))
			OriginalConnectionMap[ConnectionID] = ConnectionsObject[ConnectionID];
	}

	auto GenerateShuffledConnections = [&](size_t CurrentIndex) {
		std::mt19937 RandomEngine(static_cast<unsigned int>(CurrentIndex));
		std::shuffle(ConnectionIDList.begin(), ConnectionIDList.end(), RandomEngine);

		size_t Index = 0;
		Root["Connections"].clear();
		for (const auto& ShuffledID : ConnectionIDList)
		{
			auto ConnectionEntry = OriginalConnectionMap.find(ShuffledID);
			Root["Connections"][std::to_string(Index++)] = ConnectionEntry->second;
		}

		Json::StreamWriterBuilder WriterBuilder;
		return Json::writeString(WriterBuilder, Root);
	};

	const int TestIterationCount = 100;
	for (size_t i = 0; i < TestIterationCount; i++)
	{
		std::string ShuffledJsonData = GenerateShuffledConnections(i);

		NodeArea* TestNodeArea = NODE_SYSTEM.CreateNodeArea();
		ASSERT_NE(TestNodeArea, nullptr);

		ASSERT_EQ(TestNodeArea->LoadFromJson(ShuffledJsonData), true);

		ASSERT_EQ(TestNodeArea->GetNodeCount(), 32);
		ASSERT_EQ(TestNodeArea->GetConnectionCount(), 36);
		NODE_SYSTEM.DeleteNodeArea(TestNodeArea);
	}
}

TEST(NodeAreaLoadTest, LoadJsonWithNonSequentialKeys)
{
	NODE_SYSTEM.Clear();

	// Build a small area: two nodes and one group comment.
	NodeArea* SourceArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(SourceArea, nullptr);
	ASSERT_TRUE(SourceArea->AddNode(TEST_TOOLS.CreateBoolLiteralNode(true)));
	ASSERT_TRUE(SourceArea->AddNode(TEST_TOOLS.CreateBoolLiteralNode(false)));
	SourceArea->AddGroupComment(new GroupComment());
	ASSERT_EQ(SourceArea->GetNodeCount(), 2);
	ASSERT_EQ(SourceArea->GetGroupCommentCount(), 1);

	// Serialize, then parse it so the member keys can be rewritten.
	const std::string OriginalJson = SourceArea->ToJson();
	Json::Value Root;
	JSONCPP_STRING Error;
	Json::CharReaderBuilder Builder;
	const std::unique_ptr<Json::CharReader> Reader(Builder.newCharReader());
	ASSERT_EQ(Reader->parse(OriginalJson.c_str(), OriginalJson.c_str() + OriginalJson.size(), &Root, &Error), true);

	// Re-key every numeric member of an object to a non-zero-based set (offset +5).
	// Non-numeric members (e.g. the reserved "ExecutionEntryNodeID") are kept as-is.
	auto ShiftNumericKeys = [](Json::Value& Object) {
		Json::Value Shifted(Json::objectValue);
		for (const std::string& Key : Object.getMemberNames())
		{
			const bool bNumeric = !Key.empty() && Key.find_first_not_of("0123456789") == std::string::npos;
			Shifted[bNumeric ? std::to_string(std::stoi(Key) + 5) : Key] = Object[Key];
		}
		Object = Shifted;
	};
	ShiftNumericKeys(Root["Nodes"]);
	ShiftNumericKeys(Root["GroupComments"]);

	Json::StreamWriterBuilder WriterBuilder;
	const std::string EditedJson = Json::writeString(WriterBuilder, Root);

	// Loading the re-keyed JSON must still restore every node and group comment.
	NodeArea* LoadedArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LoadedArea, nullptr);
	ASSERT_EQ(LoadedArea->LoadFromJson(EditedJson), true);
	EXPECT_EQ(LoadedArea->GetNodeCount(), 2);
	EXPECT_EQ(LoadedArea->GetGroupCommentCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeNodeStyleNotNumeric)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"NodeType": "VisualNode",
				"NodeStyle": "not_an_int",
				"Position": {"X": 0, "Y": 0},
				"Size": {"X": 100, "Y": 50},
				"Name": "Test"
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 1);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeInputFieldIsString)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"NodeType": "VisualNode",
				"Position": {"X": 0, "Y": 0},
				"Size": {"X": 100, "Y": 50},
				"Name": "Test",
				"Input": "not_an_object"
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeOutputFieldIsArray)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"NodeType": "VisualNode",
				"Position": {"X": 0, "Y": 0},
				"Size": {"X": 100, "Y": 50},
				"Name": "Test",
				"Output": [1, 2, 3]
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeAllowedTypesContainsNonString)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"NodeType": "VisualNode",
				"Position": {"X": 0, "Y": 0},
				"Size": {"X": 100, "Y": 50},
				"Name": "Test",
				"Input": {
					"0": {
						"ID": "231D2D4F1D033A39393A157C",
						"Name": "In",
						"AllowedTypes": [42, true, null]
					}
				}
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadNodeEntryNotObject)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {
			"0": "this_is_a_string_not_an_object"
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadRenderOffsetNotObject)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {},
		"RenderOffset": "not_an_object"
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadRenderOffsetXNotNumeric)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {},
		"RenderOffset": {"X": "abc", "Y": 0}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, Load_Connection_With_RerouteConnections_NotObject)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Connections": {
			"0": {
				"In":  { "NodeID": "6451100C0778214351766B7E", "SocketID": "1C191657360B7903691A5B66", "SocketIndex": 0 },
				"Out": { "NodeID": "5C0E2E1D5D005C3E70421A67", "SocketID": "5D7E2A574B7D3C3069521C3F", "SocketIndex": 0 },
				"RerouteConnections": "not_an_object"
			}
		},
		"Nodes": {
			"0": {
				"ID": "6451100C0778214351766B7E",
				"NodeType": "VisualNode",
				"NodeStyle": 0,
				"Name": "Sink",
				"Position": {"X": 350.0, "Y": 490.0},
				"Size": {"X": 120.0, "Y": 80.0},
				"Input": { "0": { "ID": "1C191657360B7903691A5B66", "Name": "In", "AllowedTypes":["EXECUTE"] } }
			},
			"1": {
				"ID": "5C0E2E1D5D005C3E70421A67",
				"NodeType": "VisualNode",
				"NodeStyle": 0,
				"Name": "Source",
				"Position": {"X": 10.0, "Y": 490.0},
				"Size": {"X": 120.0, "Y": 80.0},
				"Output": { "0": { "ID": "5D7E2A574B7D3C3069521C3F", "Name": "Out", "AllowedTypes":["EXECUTE"] } }
			}
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	EXPECT_EQ(NodeArea->GetNodeCount(), 2);
	EXPECT_EQ(NodeArea->GetConnectionCount(), 1);
	EXPECT_EQ(NodeArea->GetRerouteConnectionCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadRootIsArray)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"([{"a":"b"}])";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	EXPECT_FALSE(NodeArea->LoadFromJson(JsonString));
	EXPECT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadAreaIDNotString)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {},
		"ID": 42
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	EXPECT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaLoadTest, LoadAreaNameNotString)
{
	NODE_SYSTEM.Clear();

	std::string JsonString = R"({
		"Nodes": {},
		"Name": true
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), true);
	EXPECT_EQ(NodeArea->GetNodeCount(), 0);

	NODE_SYSTEM.Clear();
}