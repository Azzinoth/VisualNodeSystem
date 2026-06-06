#include "NodeSystemLoadTests.h"
using namespace VisNodeSys;

TEST(NodeSystemLoadTest, BasicSaveLoad_EmptySystem)
{
	NODE_SYSTEM.Clear();

	const std::string FilePath = "NodeSystemLoadTest_Empty.json";
	ASSERT_TRUE(NODE_SYSTEM.SaveToFile(FilePath));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	ASSERT_TRUE(NODE_SYSTEM.LoadFromFile(FilePath));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, BasicSaveLoad_SingleEmptyArea) 
{
	NODE_SYSTEM.Clear();

	NodeArea* Area = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(Area, nullptr);
	const std::string AreaID = Area->GetID();
	Area->SetName("OnlyArea");

	const std::string JsonString = NODE_SYSTEM.ToJson();
	NODE_SYSTEM.Clear();
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	ASSERT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 1);

	NodeArea* Loaded = NODE_SYSTEM.GetNodeAreaByID(AreaID);
	ASSERT_NE(Loaded, nullptr);
	EXPECT_EQ(Loaded->GetName(), "OnlyArea");

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_EmptyString_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(""));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_GarbageText_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson("this is not json at all"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_EmptyJsonObject_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson("{}"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_Missing_NodeAreasKey_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(R"({"SocketTypeToColorAssociations":{}})"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_Missing_SocketAssociationsKey_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(R"({"NodeAreas":{}})"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, LoadJsonNodeAreasIsString_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(R"({"SocketTypeToColorAssociations":{},"NodeAreas":"not_an_object"})"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, LoadJsonNodeAreasIsArray_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(R"({"SocketTypeToColorAssociations":{},"NodeAreas":[]})"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, LoadJsonNodeAreasIsNumber_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(R"({"SocketTypeToColorAssociations":{},"NodeAreas":123})"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, LoadJsonEmptyNodeAreas_Succeeds) 
{
	NODE_SYSTEM.Clear();
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(TEST_TOOLS.MakeMinimalSystemJson()));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, LoadFrom_NonExistentFile_Fail_Gracefully) 
{
	NODE_SYSTEM.Clear();
	EXPECT_FALSE(NODE_SYSTEM.LoadFromFile("this_file_really_should_not_exist_12345.json"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_Clears_PreviousLinkRecords)
{
	NODE_SYSTEM.Clear();

	NodeArea* AreaA = NODE_SYSTEM.CreateNodeArea();
	NodeArea* AreaB = NODE_SYSTEM.CreateNodeArea();
	const std::string AreaAID = AreaA->GetID();
	const std::string AreaBID = AreaB->GetID();

	ASSERT_TRUE(NODE_SYSTEM.LinkNodeAreas(AreaAID, AreaBID));
	ASSERT_TRUE(NODE_SYSTEM.IsLinked(AreaAID, AreaBID));

	ASSERT_TRUE(NODE_SYSTEM.LoadFromJson(TEST_TOOLS.MakeMinimalSystemJson()));

	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);
	EXPECT_FALSE(NODE_SYSTEM.IsLinked(AreaAID, AreaBID));
	EXPECT_EQ(NODE_SYSTEM.GetDanglingLinkNodes().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_SocketAssociationEntry_NotObject_Is_Not_Crashing)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({
		"SocketTypeToColorAssociations":{"FOO":"not_an_object"},
		"NodeAreas":{}
	})";
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_SocketAssociationColorMissingComponent_Is_Not_Crashing)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({
		"SocketTypeToColorAssociations":{"FOO":{"R":0.5}},
		"NodeAreas":{}
	})";
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_AreaWithEmptyEmbeddedBody_Do_Not_Add_Area)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({"SocketTypeToColorAssociations":{},"NodeAreas":{"some_key":""}})";
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_AreaWithGarbageEmbeddedBody_Do_Not_Add_Area)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString =
		R"({"SocketTypeToColorAssociations":{},"NodeAreas":{"X":"absolutely not json"}})";

	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_AreaValueIsObject_Do_Not_Add_Area)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({
		"SocketTypeToColorAssociations":{},
		"NodeAreas":{"X":{"ID":"X","Nodes":{}}}
	})";
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_AreaValueIsNumber_Do_Not_Add_Area)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({
		"SocketTypeToColorAssociations":{},
		"NodeAreas":{"X":42}
	})";
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_AreaValueIsNull_Do_Not_Add_Area)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({
		"SocketTypeToColorAssociations":{},
		"NodeAreas":{"X":null}
	})";
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_SocketAssociations_IsString_Fails)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({
		"SocketTypeToColorAssociations":"oops",
		"NodeAreas":{}
	})";
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_SocketAssociations_IsArray_Fails)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({
		"SocketTypeToColorAssociations":["BAD"],
		"NodeAreas":{}
	})";
	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_SocketAssociationColorComponent_NonNumeric_Is_Not_Crashing)
{
	NODE_SYSTEM.Clear();

	const std::string JsonString = R"({
		"SocketTypeToColorAssociations":{"FOO":{"R":"abc","G":0.0,"B":0.0,"A":1.0}},
		"NodeAreas":{}
	})";
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(JsonString));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_SubAreaNode_TransitiveOwnershipCycle_LoadsConsistentAndAcyclic)
{
	NODE_SYSTEM.Clear();

	// Build legitimate nesting:
	//   OuterArea -> OuterSubAreaNode owns MiddleArea -> MiddleSubAreaNode owns InnerArea.
	NodeArea* OuterArea = NODE_SYSTEM.CreateNodeArea();
	const std::string OuterAreaID = OuterArea->GetID();

	SubAreaNode* OuterSubAreaNode = NODE_SYSTEM.CreateSubAreaNode(OuterAreaID);
	ASSERT_NE(OuterSubAreaNode, nullptr);
	NodeArea* MiddleArea = OuterSubAreaNode->GetOwnedArea();
	ASSERT_NE(MiddleArea, nullptr);
	const std::string MiddleAreaID = MiddleArea->GetID();

	SubAreaNode* MiddleSubAreaNode = NODE_SYSTEM.CreateSubAreaNode(MiddleAreaID);
	ASSERT_NE(MiddleSubAreaNode, nullptr);
	NodeArea* InnerArea = MiddleSubAreaNode->GetOwnedArea();
	ASSERT_NE(InnerArea, nullptr);
	const std::string InnerAreaID = InnerArea->GetID();

	std::string Serialized = NODE_SYSTEM.ToJson();

	// Rewrite MiddleSubAreaNode's OwnedAreaID from InnerAreaID to OuterAreaID: an inconsistent
	// save whose ownership points back up the tree (Outer -> Middle -> Outer).
	auto ReplaceAll = [](std::string& Text, const std::string& Needle, const std::string& Replacement)
	{
		size_t Position = 0;
		while ((Position = Text.find(Needle, Position)) != std::string::npos)
		{
			Text.replace(Position, Needle.size(), Replacement);
			Position += Replacement.size();
		}
	};

	const std::string SingleEscapeNeedle = "\\\"OwnedAreaID\\\":\\\"" + InnerAreaID + "\\\"";
	const std::string SingleEscapeReplacement = "\\\"OwnedAreaID\\\":\\\"" + OuterAreaID + "\\\"";
	ReplaceAll(Serialized, SingleEscapeNeedle, SingleEscapeReplacement);

	const std::string DoubleEscapeNeedle = "\\\\\\\"OwnedAreaID\\\\\\\":\\\\\\\"" + InnerAreaID + "\\\\\\\"";
	const std::string DoubleEscapeReplacement = "\\\\\\\"OwnedAreaID\\\\\\\":\\\\\\\"" + OuterAreaID + "\\\\\\\"";
	ReplaceAll(Serialized, DoubleEscapeNeedle, DoubleEscapeReplacement);

	NODE_SYSTEM.Clear();

	// Loading the inconsistent/cyclic save must complete, no crash and no infinite recursion through the ownership graph.
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(Serialized));

	// However the loader resolves the inconsistency, the result must be a consistent, acyclic
	// ownership graph: every surviving SubAreaNode owns a real area, is not dangling, its own
	// area does not descend from the area it owns (no cycle), and no area is owned twice.
	std::vector<std::string> OwnedAreaIDs;
	std::vector<std::string> AreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	for (const std::string& AreaID : AreaIDs)
	{
		NodeArea* Area = NODE_SYSTEM.GetNodeAreaByID(AreaID);
		ASSERT_NE(Area, nullptr);

		for (SubAreaNode* SubArea : Area->GetNodesByType<SubAreaNode>())
		{
			NodeArea* OwnedArea = SubArea->GetOwnedArea();
			ASSERT_NE(OwnedArea, nullptr);
			EXPECT_FALSE(SubArea->IsDangling());
			EXPECT_FALSE(Area->IsChildOf(OwnedArea));

			for (const std::string& SeenID : OwnedAreaIDs)
				EXPECT_NE(SeenID, OwnedArea->GetID());
			OwnedAreaIDs.push_back(OwnedArea->GetID());
		}
	}

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, Load_SubAreaNode_OwnsItsOwnParentArea_Do_Not_Add_Node)
{
	NODE_SYSTEM.Clear();

	// Build a normal SubAreaNode, then rewrite its OwnedAreaID to point at the parent area.
	NodeArea* ParentArea = NODE_SYSTEM.CreateNodeArea();
	const std::string ParentID = ParentArea->GetID();
	SubAreaNode* SubArea = NODE_SYSTEM.CreateSubAreaNode(ParentID);
	ASSERT_NE(SubArea, nullptr);

	std::string Serialized = NODE_SYSTEM.ToJson();

	// Replace every quoted OwnedAreaID payload with the parent area ID. There is only one SubAreaNode in this system, so this is unambiguous.
	const std::string OwnedAreaIDKey = "\\\"OwnedAreaID\\\":\\\"";
	size_t Cursor = 0;
	while (true)
	{
		size_t FoundAt = Serialized.find(OwnedAreaIDKey, Cursor);
		if (FoundAt == std::string::npos)
			break;
		size_t ValueStart = FoundAt + OwnedAreaIDKey.size();
		size_t ValueEnd = Serialized.find("\\\"", ValueStart);
		ASSERT_NE(ValueEnd, std::string::npos);
		Serialized.replace(ValueStart, ValueEnd - ValueStart, ParentID);
		Cursor = ValueStart + ParentID.size();
	}

	NODE_SYSTEM.Clear();

	// Load itself must succeed; incorrect SubAreaNode is dropped so the dangerous self-ownership never enters the system.
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(Serialized));

	NodeArea* ReloadedParent = NODE_SYSTEM.GetNodeAreaByID(ParentID);
	ASSERT_NE(ReloadedParent, nullptr);
	EXPECT_EQ(ReloadedParent->GetNodesByType<SubAreaNode>().size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, LoadRootIsArray)
{
	NODE_SYSTEM.Clear();

	EXPECT_FALSE(NODE_SYSTEM.LoadFromJson(R"([{"a":"b"}])"));
	EXPECT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 0);

	NODE_SYSTEM.Clear();
}