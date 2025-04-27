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
	std::string JsonString = R"({ "renderOffset": {"x": 100.5, "y": -50.0} })";
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
	std::string JsonString = R"({ "nodes": "invalid" })";
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
		"nodes": { 
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"position": {"x":0, "y":0}, 
				"size": {"x":100, "y":50}, 
				"name":"Test" 
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
		"nodes": { 
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"nodeType": 123, 
				"position": {"x":0, "y":0}, 
				"size": {"x":100, "y":50}, 
				"name":"Test" 
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
		"nodes": { 
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"nodeType": "NonExistentNode", 
				"position": {"x":0, "y":0}, 
				"size": {"x":100, "y":50}, 
				"name":"Test" 
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
		"nodes": {
			"0": {
				"ID": "6621620F42545B420C103443",
				"nodeType": "CustomNode2",
				"position": {"x": 10, "y": 10},
				"size": {"x": 150, "y": 80},
				"name": "Mismatch Node",
				"input": {
					"0": {"ID": "231D2D4F1D033A39393A157C", "name": "Input 1", "type": "FLOAT"},
					"1": {"ID": "777D5D4709303379786D3B34", "name": "Input 2", "type": "FLOAT"}
				},
				"output": {
					"0": {"ID": "6E58750D6D46781643536C0F", "name": "Output 1", "type": "FLOAT"}
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
		"nodes": { 
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"nodeType": "CustomNode", 
				"position": {"x":0,"y":0},
				"size":{"x":1,"y":1},
				"name":"N1"
			}
		},
		"connections": { 
			"0": "invalid_connection" 
		}
	})";

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	ASSERT_EQ(NodeArea->LoadFromJson(JsonString), false);
	ASSERT_EQ(NodeArea->GetNodeCount(), 0);
	ASSERT_EQ(NodeArea->GetGroupCommentCount(), 0);
	ASSERT_EQ(NodeArea->GetConnectionCount(), 0);
	ASSERT_EQ(NodeArea->GetSelected().size(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaLoadTest, LoadConnectionMissingInOrOut)
{
	std::string JsonString = R"({
		"nodes": {
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"nodeType": "CustomNode5", 
				"position": {"x":0,"y":0},
				"size":{"x":1,"y":1},
				"name":"N1", 
				"input": {
					"0": {
						"ID": "567D5D4709303379786D3B34", 
						"name":"In", 
						"type":"FLOAT"
					}
				},
				"output": {
					"0": {
						"ID": "777D5D4709303379786D3B34", 
						"name":"Out", 
						"type":"FLOAT"
					}
				}
			},
			"1": { 
				"ID": "6E58750D6D46781643536C0F", 
				"nodeType": "CustomNode5", 
				"position": {"x":200,"y":0},
				"size":{"x":1,"y":1},
				"name":"N2", 
				"input": {
					"0": {
						"ID": "167D5D4709303379786D3B34", 
						"name":"In", 
						"type":"FLOAT" 
					}
				},
				"output": {
					"0": {
						"ID": "134D5D4709303379786D3B34", 
						"name":"Out", 
						"type":"FLOAT"
					}
				}
			}
		},
		"connections": {
			"0": {
				"out": {"socket_ID": "777D5D4709303379786D3B34", "node_ID": "6621620F42545B420C103443"}
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
		"nodes": {
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"nodeType": "CustomNode5", 
				"position": {"x":0,"y":0},
				"size":{"x":1,"y":1},
				"name":"N1", 
				"input": {
					"0": {
						"ID": "567D5D4709303379786D3B34", 
						"name":"In", 
						"type":"FLOAT"
					}
				},
				"output": {
					"0": {
						"ID": "777D5D4709303379786D3B34", 
						"name":"Out", 
						"type":"FLOAT"
					}
				}
			},
			"1": { 
				"ID": "6E58750D6D46781643536C0F", 
				"nodeType": "CustomNode5", 
				"position": {"x":200,"y":0},
				"size":{"x":1,"y":1},
				"name":"N2", 
				"input": {
					"0": {
						"ID": "167D5D4709303379786D3B34", 
						"name":"In", 
						"type":"FLOAT" 
					}
				},
				"output": {
					"0": {
						"ID": "134D5D4709303379786D3B34", 
						"name":"Out", 
						"type":"FLOAT"
					}
				}
			}
		},
		"connections": {
			"0": {
				"in": {"socket_ID": "567D5D4709303379786D3B34"},
				"out": {"node_ID": "6621620F42545B420C103443"}
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
		"nodes": {
			"0": { 
				"ID": "6621620F42545B420C103443", 
				"nodeType": "CustomNode5", 
				"position": {"x":0,"y":0},
				"size":{"x":1,"y":1},
				"name":"N1", 
				"input": {
					"0": {
						"ID": "567D5D4709303379786D3B34", 
						"name":"In", 
						"type":"FLOAT"
					}
				},
				"output": {
					"0": {
						"ID": "777D5D4709303379786D3B34", 
						"name":"Out", 
						"type":"FLOAT"
					}
				}
			},
			"1": { 
				"ID": "6E58750D6D46781643536C0F", 
				"nodeType": "CustomNode5", 
				"position": {"x":200,"y":0},
				"size":{"x":1,"y":1},
				"name":"N2", 
				"input": {
					"0": {
						"ID": "167D5D4709303379786D3B34", 
						"name":"In", 
						"type":"FLOAT" 
					}
				},
				"output": {
					"0": {
						"ID": "134D5D4709303379786D3B34", 
						"name":"Out", 
						"type":"FLOAT"
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
		"nodes": {
			"0": {
				"ID": "0E341B791D445B0B2E5B7534", "nodeType": "CustomNode5", "nodeStyle": 0,
				"position": {"x": 0, "y": 0}, "size": {"x": 100, "y": 80}, "name": "Node 1",
				"input": {"0": {"ID": "7B63360C3D3E410D671A0A26", "name": "In", "type": "FLOAT"}},
				"output": {"0": {"ID": "33763545003D3B0C2B557301", "name": "Out", "type": "FLOAT"}}
			},
			"1": {
				"ID": "6D61442D3E48292F126B7E07", "nodeType": "CustomNode5", "nodeStyle": 0,
				"position": {"x": 200, "y": 0}, "size": {"x": 100, "y": 80}, "name": "Node 2",
				"input": {"0": {"ID": "4500261C5F10075178584777", "name": "In", "type": "FLOAT"}},
				"output": {"0": {"ID": "214D5382056A20645E3C2504", "name": "Out", "type": "FLOAT"}}
			},
			"2": {
				"ID": "72387012000A79220E2B0601", "nodeType": "CustomNode5", "nodeStyle": 0,
				"position": {"x": 400, "y": 0}, "size": {"x": 100, "y": 80}, "name": "Node 3",
				"input": {"0": {"ID": "30108938125F62440D440F76", "name": "In", "type": "FLOAT"}},
				"output": {"0": {"ID": "317C1F777F3B51427B081613", "name": "Out", "type": "FLOAT"}}
			}
		},
		"connections": {
			"0": {
				"in": {"socket_ID": "4500261C5F10075178584777", "node_ID": "6D61442D3E48292F126B7E07"},
				"out": {"socket_ID": "33763545003D3B0C2B557301", "node_ID": "0E341B791D445B0B2E5B7534"}
			},
			"1": {
				"in": {"socket_ID": "317C1F777F3B51427B081613", "node_ID": "72387012000A79220E2B0601"},
				"out": {"socket_ID": "214D5382056A20645E3C2504", "node_ID": "6D61442D3E48292F126B7E07"}
			}
		},
        "renderOffset": {"x": 0, "y": 0}
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

TEST(NodeAreaLoadTest, LoadWhenRerouteconnectionsInfoIsNull)
{
	std::string JsonString = R"({
		"connections" : 
		{
			"0" : 
			{
				"in" : 
				{
					"node_ID" : "6451100C0778214351766B7E",
					"socket_ID" : "1C191657360B7903691A5B66",
					"socket_index" : 0
				},
				"out" : 
				{
					"node_ID" : "5C0E2E1D5D005C3E70421A67",
					"socket_ID" : "5D7E2A574B7D3C3069521C3F",
					"socket_index" : 0
				},
				"reroute_connections" : 
				{
					
				}
			}
		},
		"nodes" : 
		{
			"0" : 
			{
				"ID" : "6451100C0778214351766B7E",
				"input" : 
				{
					"0" : 
					{
						"ID" : "1C191657360B7903691A5B66",
						"name" : "in",
						"type" : "EXEC"
					}
				},
				"name" : "Some node",
				"nodeStyle" : 0,
				"nodeType" : "VisualNode",
				"position" : 
				{
					"x" : 350.0,
					"y" : 490.0
				},
				"size" : 
				{
					"x" : 120.0,
					"y" : 80.0
				}
			},
			"1" : 
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"name" : "Some node",
				"nodeStyle" : 0,
				"nodeType" : "VisualNode",
				"output" : 
				{
					"0" : 
					{
						"ID" : "5D7E2A574B7D3C3069521C3F",
						"name" : "out",
						"type" : "EXEC"
					}
				},
				"position" : 
				{
					"x" : 10.0,
					"y" : 490.0
				},
				"size" : 
				{
					"x" : 120.0,
					"y" : 80.0
				}
			},
		},
		"renderOffset" : 
		{
			"x" : 0.0,
			"y" : 0.0
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

TEST(NodeAreaLoadTest, LoadWhenRerouteconnectionsInfoIsDamaged)
{
	std::string JsonString = R"({
		"connections" : 
		{
			"0" : 
			{
				"in" : 
				{
					"node_ID" : "6451100C0778214351766B7E",
					"socket_ID" : "1C191657360B7903691A5B66",
					"socket_index" : 0
				},
				"out" : 
				{
					"node_ID" : "5C0E2E1D5D005C3E70421A67",
					"socket_ID" : "5D7E2A574B7D3C3069521C3F",
					"socket_index" : 0
				},
				"reroute_connections" : 
				{
					"0" : 
					{
						"begin_reroute_ID" : "",
						"begin_socket_ID" : "NONE",
						"end_reroute_ID" : "5E697F0B007F313E4F38577B",
						"end_socket_ID" : "",
						"position_x" : 190.0,
						"position_y" : 470.0,
						"reroute_ID" : "6E3F063E430C12373C68654C"
					},
					"1" : 
					{
						"begin_reroute_ID" : "6E3F063E430C12373C68654C",
						"begin_socket_ID" : "",
						"end_reroute_ID" : "",
						"end_socket_ID" : "1C191657360B7903691A5B66",
						"position_x" : 312.0,
						"position_y" : 470.0,
						"reroute_ID" : "5E697F0B007F313E4F38577B"
					}
				}
			}
		},
		"nodes" : 
		{
			"0" : 
			{
				"ID" : "6451100C0778214351766B7E",
				"input" : 
				{
					"0" : 
					{
						"ID" : "1C191657360B7903691A5B66",
						"name" : "in",
						"type" : "EXEC"
					}
				},
				"name" : "Some node",
				"nodeStyle" : 0,
				"nodeType" : "VisualNode",
				"position" : 
				{
					"x" : 350.0,
					"y" : 490.0
				},
				"size" : 
				{
					"x" : 120.0,
					"y" : 80.0
				}
			},
			"1" : 
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"name" : "Some node",
				"nodeStyle" : 0,
				"nodeType" : "VisualNode",
				"output" : 
				{
					"0" : 
					{
						"ID" : "5D7E2A574B7D3C3069521C3F",
						"name" : "out",
						"type" : "EXEC"
					}
				},
				"position" : 
				{
					"x" : 10.0,
					"y" : 490.0
				},
				"size" : 
				{
					"x" : 120.0,
					"y" : 80.0
				}
			},
		},
		"renderOffset" : 
		{
			"x" : 0.0,
			"y" : 0.0
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

TEST(NodeAreaLoadTest, LoadWhenRerouteconnectionsInfoIsDamaged_2)
{
	std::string JsonString = R"({
		"connections" : 
		{
			"0" : 
			{
				"in" : 
				{
					"node_ID" : "6451100C0778214351766B7E",
					"socket_ID" : "1C191657360B7903691A5B66",
					"socket_index" : 0
				},
				"out" : 
				{
					"node_ID" : "5C0E2E1D5D005C3E70421A67",
					"socket_ID" : "5D7E2A574B7D3C3069521C3F",
					"socket_index" : 0
				},
				"reroute_connections" : 
				{
					"0" : 
					{
						"begin_reroute_ID" : "",
						"begin_socket_ID" : "5D7E2A574B7D3C3069521C3F",
						"end_reroute_ID" : "5E697F0B007F313E4F38577B",
						"end_socket_ID" : "",
						"position_x" : 190.0,
						"position_y" : 470.0,
						"reroute_ID" : "6E3F063E430C12373C68654C"
					},
					"1" : 
					{
						"begin_reroute_ID" : "6E3F063E430C12373C68654C",
						"begin_socket_ID" : "",
						"end_reroute_ID" : "",
						"end_socket_ID" : 4587,
						"position_x" : 312.0,
						"position_y" : 470.0,
						"reroute_ID" : "5E697F0B007F313E4F38577B"
					}
				}
			}
		},
		"nodes" : 
		{
			"0" : 
			{
				"ID" : "6451100C0778214351766B7E",
				"input" : 
				{
					"0" : 
					{
						"ID" : "1C191657360B7903691A5B66",
						"name" : "in",
						"type" : "EXEC"
					}
				},
				"name" : "Some node",
				"nodeStyle" : 0,
				"nodeType" : "VisualNode",
				"position" : 
				{
					"x" : 350.0,
					"y" : 490.0
				},
				"size" : 
				{
					"x" : 120.0,
					"y" : 80.0
				}
			},
			"1" : 
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"name" : "Some node",
				"nodeStyle" : 0,
				"nodeType" : "VisualNode",
				"output" : 
				{
					"0" : 
					{
						"ID" : "5D7E2A574B7D3C3069521C3F",
						"name" : "out",
						"type" : "EXEC"
					}
				},
				"position" : 
				{
					"x" : 10.0,
					"y" : 490.0
				},
				"size" : 
				{
					"x" : 120.0,
					"y" : 80.0
				}
			},
		},
		"renderOffset" : 
		{
			"x" : 0.0,
			"y" : 0.0
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

TEST(NodeAreaLoadTest, LoadWhenRerouteconnectionsInfoIsDamaged_3)
{
	std::string JsonString = R"({
		"connections" : 
		{
			"0" : 
			{
				"in" : 
				{
					"node_ID" : "6451100C0778214351766B7E",
					"socket_ID" : "1C191657360B7903691A5B66",
					"socket_index" : 0
				},
				"out" : 
				{
					"node_ID" : "5C0E2E1D5D005C3E70421A67",
					"socket_ID" : "5D7E2A574B7D3C3069521C3F",
					"socket_index" : 0
				},
				"reroute_connections" : 
				{
					"0" : 
					{
						"begin_reroute_ID" : "",
						"begin_socket_ID" : "5D7E2A574B7D3C3069521C3F",
						"end_reroute_ID" : "5E697F0B007F313E4F38577B",
						"end_socket_ID" : "",
						"position_x" : 190.0,
						"position_y" : 470.0,
						"reroute_ID" : "6E3F063E430C12373C68654C"
					},
					"1" : 
					{
						"begin_reroute_ID" : "6E3F063E430C12373C68654C",
						"begin_socket_ID" : "",
						"end_reroute_ID" : "5E697F0B007F313E4F38577B",
						"end_socket_ID" : "1C191657360B7903691A5B66",
						"position_x" : 312.0,
						"position_y" : 470.0,
						"reroute_ID" : "5E697F0B007F313E4F38577B"
					}
				}
			}
		},
		"nodes" : 
		{
			"0" : 
			{
				"ID" : "6451100C0778214351766B7E",
				"input" : 
				{
					"0" : 
					{
						"ID" : "1C191657360B7903691A5B66",
						"name" : "in",
						"type" : "EXEC"
					}
				},
				"name" : "Some node",
				"nodeStyle" : 0,
				"nodeType" : "VisualNode",
				"position" : 
				{
					"x" : 350.0,
					"y" : 490.0
				},
				"size" : 
				{
					"x" : 120.0,
					"y" : 80.0
				}
			},
			"1" : 
			{
				"ID" : "5C0E2E1D5D005C3E70421A67",
				"name" : "Some node",
				"nodeStyle" : 0,
				"nodeType" : "VisualNode",
				"output" : 
				{
					"0" : 
					{
						"ID" : "5D7E2A574B7D3C3069521C3F",
						"name" : "out",
						"type" : "EXEC"
					}
				},
				"position" : 
				{
					"x" : 10.0,
					"y" : 490.0
				},
				"size" : 
				{
					"x" : 120.0,
					"y" : 80.0
				}
			},
		},
		"renderOffset" : 
		{
			"x" : 0.0,
			"y" : 0.0
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