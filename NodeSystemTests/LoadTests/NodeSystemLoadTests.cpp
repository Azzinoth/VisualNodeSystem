#include "NodeSystemLoadTests.h"
#include <regex>
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
	const FEUUID AreaID = Area->GetID();
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
	const FEUUID AreaAID = AreaA->GetID();
	const FEUUID AreaBID = AreaB->GetID();

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
	const FEUUID OuterAreaID = OuterArea->GetID();

	SubAreaNode* OuterSubAreaNode = NODE_SYSTEM.CreateSubAreaNode(OuterAreaID);
	ASSERT_NE(OuterSubAreaNode, nullptr);
	NodeArea* MiddleArea = OuterSubAreaNode->GetOwnedArea();
	ASSERT_NE(MiddleArea, nullptr);
	const FEUUID MiddleAreaID = MiddleArea->GetID();

	SubAreaNode* MiddleSubAreaNode = NODE_SYSTEM.CreateSubAreaNode(MiddleAreaID);
	ASSERT_NE(MiddleSubAreaNode, nullptr);
	NodeArea* InnerArea = MiddleSubAreaNode->GetOwnedArea();
	ASSERT_NE(InnerArea, nullptr);
	const FEUUID InnerAreaID = InnerArea->GetID();

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

	const std::string SingleEscapeNeedle = "\\\"OwnedAreaID\\\":\\\"" + ToString(InnerAreaID) + "\\\"";
	const std::string SingleEscapeReplacement = "\\\"OwnedAreaID\\\":\\\"" + ToString(OuterAreaID) + "\\\"";
	ReplaceAll(Serialized, SingleEscapeNeedle, SingleEscapeReplacement);

	const std::string DoubleEscapeNeedle = "\\\\\\\"OwnedAreaID\\\\\\\":\\\\\\\"" + ToString(InnerAreaID) + "\\\\\\\"";
	const std::string DoubleEscapeReplacement = "\\\\\\\"OwnedAreaID\\\\\\\":\\\\\\\"" + ToString(OuterAreaID) + "\\\\\\\"";
	ReplaceAll(Serialized, DoubleEscapeNeedle, DoubleEscapeReplacement);

	NODE_SYSTEM.Clear();

	// Loading the inconsistent/cyclic save must complete, no crash and no infinite recursion through the ownership graph.
	EXPECT_TRUE(NODE_SYSTEM.LoadFromJson(Serialized));

	// However the loader resolves the inconsistency, the result must be a consistent, acyclic
	// ownership graph: every surviving SubAreaNode owns a real area, is not dangling, its own
	// area does not descend from the area it owns (no cycle), and no area is owned twice.
	std::vector<FEUUID> OwnedAreaIDs;
	std::vector<FEUUID> AreaIDs = NODE_SYSTEM.GetNodeAreaIDList();
	for (const FEUUID& AreaID : AreaIDs)
	{
		NodeArea* Area = NODE_SYSTEM.GetNodeAreaByID(AreaID);
		ASSERT_NE(Area, nullptr);

		for (SubAreaNode* SubArea : Area->GetNodesByType<SubAreaNode>())
		{
			NodeArea* OwnedArea = SubArea->GetOwnedArea();
			ASSERT_NE(OwnedArea, nullptr);
			EXPECT_FALSE(SubArea->IsDangling());
			EXPECT_FALSE(Area->IsChildOf(OwnedArea));

			for (const FEUUID& SeenID : OwnedAreaIDs)
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
	const FEUUID ParentID = ParentArea->GetID();
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
		Serialized.replace(ValueStart, ValueEnd - ValueStart, ToString(ParentID));
		Cursor = ValueStart + ToString(ParentID).size();
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

// Rewrites a save to use old 24 character hex string IDs.
static std::string ConvertToLegacyIDSave(const std::string& Json, std::unordered_map<std::string, std::string>& LegacyIDs)
{
	static const std::regex UUIDPattern("[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}");
	std::mt19937 RandomEngine(42);
	std::uniform_int_distribution<int> Distribution(0, 15);

	std::string Result;
	size_t LastPosition = 0;
	for (std::sregex_iterator Iterator(Json.begin(), Json.end(), UUIDPattern), End; Iterator != End; ++Iterator)
	{
		Result.append(Json, LastPosition, static_cast<size_t>(Iterator->position()) - LastPosition);
		LastPosition = static_cast<size_t>(Iterator->position() + Iterator->length());

		const std::string UUIDString = Iterator->str();
		if (IsNull(FromString(UUIDString)))
			continue;

		std::string& LegacyID = LegacyIDs[UUIDString];
		if (LegacyID.empty())
		{
			for (int i = 0; i < 24; i++)
				LegacyID += "0123456789ABCDEF"[Distribution(RandomEngine)];
		}
		Result += LegacyID;
	}
	Result.append(Json, LastPosition, std::string::npos);

	return Result;
}

static std::vector<std::pair<FEUUID, std::vector<FEUUID>>> CaptureAreaAndNodeIDs()
{
	std::vector<std::pair<FEUUID, std::vector<FEUUID>>> Result;
	for (const FEUUID& AreaID : NODE_SYSTEM.GetNodeAreaIDList())
	{
		std::vector<FEUUID> NodeIDs;
		NODE_SYSTEM.GetNodeAreaByID(AreaID)->RunOnEachNode([&](Node* CurrentNode) { NodeIDs.push_back(CurrentNode->GetID()); });
		Result.push_back({ AreaID, NodeIDs });
	}

	return Result;
}

static void ExpectLegacyIDsConverted(const std::vector<std::pair<FEUUID, std::vector<FEUUID>>>& OriginalIDs, const std::unordered_map<std::string, std::string>& LegacyIDs)
{
	for (const auto& [AreaID, NodeIDs] : OriginalIDs)
	{
		NodeArea* LoadedArea = NODE_SYSTEM.GetNodeAreaByID(ConvertLegacyHexID(LegacyIDs.at(ToString(AreaID))));
		ASSERT_NE(LoadedArea, nullptr);
		for (const FEUUID& NodeID : NodeIDs)
			EXPECT_NE(LoadedArea->GetNodeByID(ConvertLegacyHexID(LegacyIDs.at(ToString(NodeID)))), nullptr);
	}

	const std::string Resaved = NODE_SYSTEM.ToJson();
	for (const auto& [UUIDString, LegacyID] : LegacyIDs)
		EXPECT_EQ(Resaved.find(LegacyID), std::string::npos);
}

static std::vector<size_t> ExecuteAndCountPerArea(const std::vector<NodeArea*>& Areas)
{
	for (NodeArea* Area : Areas)
		Area->SetSaveExecutedNodes(true);

	Areas[0]->ExecuteNodeNetwork();
	std::unordered_map<FEUUID, std::vector<Node*>> ExecutedNodes = NODE_SYSTEM.GetLastExecutedNodes(Areas[0]->GetID());

	std::vector<size_t> Result;
	for (NodeArea* Area : Areas)
		Result.push_back(ExecutedNodes[Area->GetID()].size());

	return Result;
}

TEST(NodeSystemLoadTest, LegacySave_LinkedNodeAreaGraph)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallLinkedNodeAreaGraph();
	TEST_TOOLS.ConnectSmallLinkedNodeAreaGraph();
	ASSERT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());

	const size_t NodeCount = NODE_SYSTEM.GetTotalNodeCount();
	const size_t ConnectionCount = NODE_SYSTEM.GetTotalConnectionCount();
	const std::vector<size_t> ExecutedPerArea = ExecuteAndCountPerArea(Areas);
	const auto OriginalIDs = CaptureAreaAndNodeIDs();

	std::unordered_map<std::string, std::string> LegacyIDs;
	const std::string LegacySave = ConvertToLegacyIDSave(NODE_SYSTEM.ToJson(), LegacyIDs);
	ASSERT_FALSE(LegacyIDs.empty());

	NODE_SYSTEM.Clear();
	ASSERT_TRUE(NODE_SYSTEM.LoadFromJson(LegacySave));

	EXPECT_TRUE(TEST_TOOLS.VerifyLinksInSmallNodeAreaGraph());
	EXPECT_EQ(NODE_SYSTEM.GetTotalNodeCount(), NodeCount);
	EXPECT_EQ(NODE_SYSTEM.GetTotalConnectionCount(), ConnectionCount);
	EXPECT_TRUE(NODE_SYSTEM.GetDanglingLinkNodes().empty());

	Areas = TEST_TOOLS.GetOrderedAreasFromSmallLinkedNodeAreaGraph();
	EXPECT_EQ(ExecuteAndCountPerArea(Areas), ExecutedPerArea);

	ExpectLegacyIDsConverted(OriginalIDs, LegacyIDs);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, LegacySave_SubAreaNodeGraph)
{
	NODE_SYSTEM.Clear();

	std::vector<NodeArea*> Areas = TEST_TOOLS.CreateSmallSubAreaNodeGraph();
	TEST_TOOLS.ConnectSmallSubAreaNodeGraph();
	ASSERT_TRUE(TEST_TOOLS.VerifyParentChildInSmallSubAreaNodeGraph());

	const size_t NodeCount = NODE_SYSTEM.GetTotalNodeCount();
	const size_t ConnectionCount = NODE_SYSTEM.GetTotalConnectionCount();
	const std::vector<size_t> ExecutedPerArea = ExecuteAndCountPerArea(Areas);
	const auto OriginalIDs = CaptureAreaAndNodeIDs();

	std::unordered_map<std::string, std::string> LegacyIDs;
	const std::string LegacySave = ConvertToLegacyIDSave(NODE_SYSTEM.ToJson(), LegacyIDs);

	NODE_SYSTEM.Clear();
	ASSERT_TRUE(NODE_SYSTEM.LoadFromJson(LegacySave));

	EXPECT_TRUE(TEST_TOOLS.VerifyParentChildInSmallSubAreaNodeGraph());
	EXPECT_EQ(NODE_SYSTEM.GetTotalNodeCount(), NodeCount);
	EXPECT_EQ(NODE_SYSTEM.GetTotalConnectionCount(), ConnectionCount);

	Areas = TEST_TOOLS.GetOrderedAreasFromSmallSubAreaNodeGraph();
	for (NodeArea* Area : Areas)
	{
		for (SubAreaNode* CurrentSubAreaNode : Area->GetNodesByType<SubAreaNode>())
		{
			EXPECT_FALSE(CurrentSubAreaNode->IsDangling());
			EXPECT_FALSE(CurrentSubAreaNode->GetSubAreaInputNode()->IsDangling());
			EXPECT_FALSE(CurrentSubAreaNode->GetSubAreaOutputNode()->IsDangling());
		}
	}
	EXPECT_EQ(ExecuteAndCountPerArea(Areas), ExecutedPerArea);

	ExpectLegacyIDsConverted(OriginalIDs, LegacyIDs);
	NODE_SYSTEM.Clear();
}

TEST(NodeSystemLoadTest, LegacySave_RerouteNodesGroupCommentsAndEntryNode)
{
	NODE_SYSTEM.Clear();

	std::vector<FEUUID> NodesIDList;
	std::vector<FEUUID> GroupCommentsIDList;
	NodeArea* Area = TEST_TOOLS.CreateTinyPopulatedNodeArea(NodesIDList, GroupCommentsIDList);
	ASSERT_NE(Area, nullptr);

	BeginNode* Begin = new BeginNode();
	ASSERT_TRUE(Area->AddNode(Begin));
	ASSERT_TRUE(Area->SetExecutionEntryNode(Begin));
	const FEUUID BeginID = Begin->GetID();

	Node* RerouteStart = Area->GetNodeByID(NodesIDList[8]);
	Node* RerouteEnd = Area->GetNodeByID(NodesIDList[10]);
	const size_t SegmentCount = Area->GetConnectionSegments(RerouteStart, 0, RerouteEnd, 0).size();
	const size_t RerouteCount = Area->GetRerouteConnectionCount();
	ASSERT_EQ(SegmentCount, 3);

	const size_t NodeCount = Area->GetNodeCount();
	const size_t ConnectionCount = Area->GetConnectionCount();
	const auto OriginalIDs = CaptureAreaAndNodeIDs();

	std::unordered_map<std::string, std::string> LegacyIDs;
	const std::string LegacySave = ConvertToLegacyIDSave(NODE_SYSTEM.ToJson(), LegacyIDs);

	NODE_SYSTEM.Clear();
	ASSERT_TRUE(NODE_SYSTEM.LoadFromJson(LegacySave));
	ASSERT_EQ(NODE_SYSTEM.GetNodeAreaCount(), 1);

	Area = NODE_SYSTEM.GetNodeAreaByID(NODE_SYSTEM.GetNodeAreaIDList()[0]);
	EXPECT_EQ(Area->GetNodeCount(), NodeCount);
	EXPECT_EQ(Area->GetConnectionCount(), ConnectionCount);
	EXPECT_EQ(Area->GetRerouteConnectionCount(), RerouteCount);

	RerouteStart = Area->GetNodeByID(ConvertLegacyHexID(LegacyIDs.at(ToString(NodesIDList[8]))));
	RerouteEnd = Area->GetNodeByID(ConvertLegacyHexID(LegacyIDs.at(ToString(NodesIDList[10]))));
	ASSERT_NE(RerouteStart, nullptr);
	ASSERT_NE(RerouteEnd, nullptr);
	EXPECT_EQ(Area->GetConnectionSegments(RerouteStart, 0, RerouteEnd, 0).size(), SegmentCount);

	EXPECT_NE(Area->GetGroupCommentByID(ConvertLegacyHexID(LegacyIDs.at(ToString(GroupCommentsIDList[0])))), nullptr);

	ASSERT_NE(Area->GetExecutionEntryNode(), nullptr);
	EXPECT_EQ(Area->GetExecutionEntryNode()->GetID(), ConvertLegacyHexID(LegacyIDs.at(ToString(BeginID))));

	ExpectLegacyIDsConverted(OriginalIDs, LegacyIDs);
	NODE_SYSTEM.Clear();
}