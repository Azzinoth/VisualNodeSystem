#include "NodeAreaLoadTests.h"
using namespace VisNodeSys;

TEST(NodeAreaLoadTest, LoadEmptyJson)
{
	std::string JsonString = "{}";
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), false);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadJsonWithOnlyOffset)
{
	std::string JsonString = R"({ "RenderOffset": {"X": 100.5, "Y": -50.0} })";
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), false);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadJsonWithIncorrectNodes)
{
	std::string JsonString = R"({ "Nodes": "invalid" })";
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), false);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadNodeMissingType)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadNodeTypeNotString)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadNodeUnregisteredType)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadNodeFromJsonFails_SocketCountMismatch)
{
	std::string JsonString = R"({
		"Nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"NodeType": "CustomNode2",
				"Position": {"X": 10, "Y": 10},
				"Size": {"X": 150, "Y": 80},
				"Name": "Mismatch Node",
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
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadConnectionEntryNotObject)
{
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
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadConnectionMissingInOrOut)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadConnectionMissingIDs)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadIncorrectGroupCommentFromJson)
{
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

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), false);
	ASSERT_EQ(NodeArea->GetNodeCount(), 2);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadConnectionFailedTryToConnect_IncompatibleSockets)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadWhenRerouteConnectionsInfoIsNull)
{
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
						"AllowedTypes":["EXEC"]
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
						"AllowedTypes":["EXEC"]
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadWhenRerouteConnectionsInfoIsDamaged)
{
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
						"AllowedTypes":["EXEC"]
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
						"AllowedTypes":["EXEC"]
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadWhenRerouteConnectionsInfoIsDamaged_2)
{
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
						"AllowedTypes":["EXEC"]
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
						"AllowedTypes":["EXEC"]
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadWhenRerouteConnectionsInfoIsDamaged_3)
{
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
						"AllowedTypes":["EXEC"]
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
						"AllowedTypes":["EXEC"]
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadMediumNodeArea)
{
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromFile("Resources/MediumNodeArea.json"), true);

	ASSERT_EQ(NodeArea->GetNodeCount(), 170);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 205);
	ASSERT_EQ(NodeArea->GetRerouteConnectionCount(), 18);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 1);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, ConnectionOrderingTest)
{
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