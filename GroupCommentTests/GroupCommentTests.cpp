#include "GroupCommentTests.h"
using namespace VisNodeSys;

TEST(GroupComment, CreateAndDelete)
{
	NODE_SYSTEM.Clear();

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	GroupComment* Comment = new GroupComment();
	EXPECT_NE(Comment, nullptr);

	EXPECT_EQ(NodeArea->GetGroupCommentCount(), 0);
	NodeArea->AddGroupComment(Comment);
	EXPECT_EQ(NodeArea->GetGroupCommentCount(), 1);

	NodeArea->Delete(Comment);
	EXPECT_EQ(NodeArea->GetGroupCommentCount(), 0);

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGroupComment, GetNodesInComment)
{
	NODE_SYSTEM.Clear();

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	GroupComment* Comment = new GroupComment();
	EXPECT_NE(Comment, nullptr);
	Comment->SetPosition(ImVec2(0.0f, 0.0f));
	Comment->SetSize(ImVec2(200.0f, 200.0f));
	Comment->SetCaption("Group of some nodes");
	NodeArea->AddGroupComment(Comment);
	std::vector<Node*> NodesInGroupComment = NodeArea->GetNodesInGroupComment(Comment);
	EXPECT_EQ(NodesInGroupComment.size(), 0);

	std::vector<Node*> NodesOutsideComment;
	std::vector<Node*> NodesInsideComment;
	std::vector<Node*> NodesPartiallyInsideComment;

	// That node is fully outside the comment.
	Node* NewNode = new Node();
	NewNode->SetPosition(ImVec2(300.0f, 250.0f));
	NewNode->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddNode(NewNode);
	NodesOutsideComment.push_back(NewNode);

	NewNode = new Node();
	NewNode->SetPosition(ImVec2(-300.0f, -250.0f));
	NewNode->SetSize(ImVec2(150.0f, 80.0f));
	NodeArea->AddNode(NewNode);
	NodesOutsideComment.push_back(NewNode);

	// That node is fully inside the comment.
	NewNode = new Node();
	NewNode->SetPosition(ImVec2(10.0f, 10.0f));
	NewNode->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddNode(NewNode);
	NodesInsideComment.push_back(NewNode);

	NewNode = new Node();
	NewNode->SetPosition(ImVec2(150.0f, 150.0f));
	NewNode->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddNode(NewNode);
	NodesInsideComment.push_back(NewNode);

	// That node is partially inside the comment.
	NewNode = new Node();
	NewNode->SetPosition(ImVec2(150.0f, 150.0f));
	NewNode->SetSize(ImVec2(60.0f, 60.0f));
	NodeArea->AddNode(NewNode);
	NodesPartiallyInsideComment.push_back(NewNode);

	NewNode = new Node();
	NewNode->SetPosition(ImVec2(-150.0f, -150.0f));
	NewNode->SetSize(ImVec2(70.0f, 160.0f));
	NodeArea->AddNode(NewNode);
	NodesPartiallyInsideComment.push_back(NewNode);


	NodesInGroupComment = NodeArea->GetNodesInGroupComment(Comment);
	EXPECT_EQ(NodesInGroupComment.size(), NodesInsideComment.size());

	for (size_t i = 0; i < NodesOutsideComment.size(); i++)
	{
		for (size_t j = 0; j < NodesInGroupComment.size(); j++)
		{
			EXPECT_NE(NodesOutsideComment[i]->GetID(), NodesInGroupComment[j]->GetID());
		}
	}

	for (size_t i = 0; i < NodesInsideComment.size(); i++)
	{
		bool bFound = false;
		for (size_t j = 0; j < NodesInGroupComment.size(); j++)
		{
			if (NodesInsideComment[i]->GetID() == NodesInGroupComment[j]->GetID())
			{
				bFound = true;
				break;
			}
		}

		EXPECT_TRUE(bFound);
	}

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGroupComment, GetCommentsInComment)
{
	NODE_SYSTEM.Clear();

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	GroupComment* Comment = new GroupComment();
	EXPECT_NE(Comment, nullptr);
	Comment->SetPosition(ImVec2(0.0f, 0.0f));
	Comment->SetSize(ImVec2(200.0f, 200.0f));
	Comment->SetCaption("Group of some nodes");
	NodeArea->AddGroupComment(Comment);
	std::vector<GroupComment*> CommentsInGroupComment = NodeArea->GetGroupCommentsInGroupComment(Comment);
	EXPECT_EQ(CommentsInGroupComment.size(), 0);

	std::vector<GroupComment*> CommentsOutsideComment;
	std::vector<GroupComment*> CommentsInsideComment;
	std::vector<GroupComment*> CommentsPartiallyInsideComment;

	// That comment is fully outside the comment.
	GroupComment* NewComment = new GroupComment();
	NewComment->SetPosition(ImVec2(300.0f, 250.0f));
	NewComment->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddGroupComment(NewComment);
	CommentsOutsideComment.push_back(NewComment);

	NewComment = new GroupComment();
	NewComment->SetPosition(ImVec2(-300.0f, -250.0f));
	NewComment->SetSize(ImVec2(150.0f, 80.0f));
	NodeArea->AddGroupComment(NewComment);
	CommentsOutsideComment.push_back(NewComment);

	// That comment is fully inside the comment.
	NewComment = new GroupComment();
	NewComment->SetPosition(ImVec2(10.0f, 10.0f));
	NewComment->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddGroupComment(NewComment);
	CommentsInsideComment.push_back(NewComment);

	NewComment = new GroupComment();
	NewComment->SetPosition(ImVec2(150.0f, 150.0f));
	NewComment->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddGroupComment(NewComment);
	CommentsInsideComment.push_back(NewComment);

	// That comment is partially inside the comment.
	NewComment = new GroupComment();
	NewComment->SetPosition(ImVec2(150.0f, 150.0f));
	NewComment->SetSize(ImVec2(60.0f, 60.0f));
	NodeArea->AddGroupComment(NewComment);
	CommentsPartiallyInsideComment.push_back(NewComment);

	NewComment = new GroupComment();
	NewComment->SetPosition(ImVec2(-150.0f, -150.0f));
	NewComment->SetSize(ImVec2(70.0f, 160.0f));
	NodeArea->AddGroupComment(NewComment);
	CommentsPartiallyInsideComment.push_back(NewComment);


	CommentsInGroupComment = NodeArea->GetGroupCommentsInGroupComment(Comment);
	EXPECT_EQ(CommentsInGroupComment.size(), CommentsInsideComment.size());

	for (size_t i = 0; i < CommentsOutsideComment.size(); i++)
	{
		for (size_t j = 0; j < CommentsInGroupComment.size(); j++)
		{
			EXPECT_NE(CommentsOutsideComment[i]->GetID(), CommentsInGroupComment[j]->GetID());
		}
	}

	for (size_t i = 0; i < CommentsInsideComment.size(); i++)
	{
		bool bFound = false;
		for (size_t j = 0; j < CommentsInGroupComment.size(); j++)
		{
			if (CommentsInsideComment[i]->GetID() == CommentsInGroupComment[j]->GetID())
			{
				bFound = true;
				break;
			}
		}

		EXPECT_TRUE(bFound);
	}

	NODE_SYSTEM.Clear();
}

TEST(NodeAreaGroupComment, MoveWithElements)
{
	NODE_SYSTEM.Clear();

	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	GroupComment* Comment = new GroupComment();
	EXPECT_NE(Comment, nullptr);
	Comment->SetPosition(ImVec2(0.0f, 0.0f));
	Comment->SetSize(ImVec2(200.0f, 200.0f));
	Comment->SetCaption("Group of some nodes");
	Comment->SetMoveElementsWithComment(true);
	NodeArea->AddGroupComment(Comment);
	std::vector<Node*> NodesInGroupComment = NodeArea->GetNodesInGroupComment(Comment);
	EXPECT_EQ(NodesInGroupComment.size(), 0);

	std::vector<Node*> NodesOutsideComment;
	std::vector<Node*> NodesInsideComment;
	std::vector<Node*> NodesPartiallyInsideComment;

	// That node is fully outside the comment.
	Node* NewNode = new Node();
	NewNode->SetPosition(ImVec2(300.0f, 250.0f));
	NewNode->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddNode(NewNode);
	NodesOutsideComment.push_back(NewNode);

	NewNode = new Node();
	NewNode->SetPosition(ImVec2(-300.0f, -250.0f));
	NewNode->SetSize(ImVec2(150.0f, 80.0f));
	NodeArea->AddNode(NewNode);
	NodesOutsideComment.push_back(NewNode);

	// That node is fully inside the comment.
	NewNode = new Node();
	NewNode->SetPosition(ImVec2(10.0f, 10.0f));
	NewNode->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddNode(NewNode);
	NodesInsideComment.push_back(NewNode);

	NewNode = new Node();
	NewNode->SetPosition(ImVec2(150.0f, 150.0f));
	NewNode->SetSize(ImVec2(50.0f, 50.0f));
	NodeArea->AddNode(NewNode);
	NodesInsideComment.push_back(NewNode);

	// That node is partially inside the comment.
	NewNode = new Node();
	NewNode->SetPosition(ImVec2(150.0f, 150.0f));
	NewNode->SetSize(ImVec2(60.0f, 60.0f));
	NodeArea->AddNode(NewNode);
	NodesPartiallyInsideComment.push_back(NewNode);

	NewNode = new Node();
	NewNode->SetPosition(ImVec2(-150.0f, -150.0f));
	NewNode->SetSize(ImVec2(70.0f, 160.0f));
	NodeArea->AddNode(NewNode);
	NodesPartiallyInsideComment.push_back(NewNode);


	NodesInGroupComment = NodeArea->GetNodesInGroupComment(Comment);
	EXPECT_EQ(NodesInGroupComment.size(), NodesInsideComment.size());

	for (size_t i = 0; i < NodesOutsideComment.size(); i++)
	{
		for (size_t j = 0; j < NodesInGroupComment.size(); j++)
		{
			EXPECT_NE(NodesOutsideComment[i]->GetID(), NodesInGroupComment[j]->GetID());
		}
	}

	for (size_t i = 0; i < NodesInsideComment.size(); i++)
	{
		bool bFound = false;
		for (size_t j = 0; j < NodesInGroupComment.size(); j++)
		{
			if (NodesInsideComment[i]->GetID() == NodesInGroupComment[j]->GetID())
			{
				bFound = true;
				break;
			}
		}

		EXPECT_TRUE(bFound);
	}

	std::vector<ImVec2> NodesInsideCommentPositionsBeforeMove;
	for (size_t i = 0; i < NodesInsideComment.size(); i++)
	{
		NodesInsideCommentPositionsBeforeMove.push_back(NodesInsideComment[i]->GetPosition());
	}

	std::vector<ImVec2> NodesPartiallyInsideCommentPositionsBeforeMove;
	for (size_t i = 0; i < NodesPartiallyInsideComment.size(); i++)
	{
		NodesPartiallyInsideCommentPositionsBeforeMove.push_back(NodesPartiallyInsideComment[i]->GetPosition());
	}

	std::vector<ImVec2> NodesOutsideCommentPositionsBeforeMove;
	for (size_t i = 0; i < NodesOutsideComment.size(); i++)
	{
		NodesOutsideCommentPositionsBeforeMove.push_back(NodesOutsideComment[i]->GetPosition());
	}

	// Move comment.
	ImVec2 PositionShift = ImVec2(100.0f, 100.0f);
	Comment->SetPosition(Comment->GetPosition() + PositionShift);

	for (size_t i = 0; i < NodesInsideComment.size(); i++)
	{
		EXPECT_EQ(NodesInsideComment[i]->GetPosition(), NodesInsideCommentPositionsBeforeMove[i] + PositionShift);
		NodesInsideCommentPositionsBeforeMove[i] = NodesInsideComment[i]->GetPosition();
	}

	for (size_t i = 0; i < NodesPartiallyInsideComment.size(); i++)
	{
		EXPECT_EQ(NodesPartiallyInsideComment[i]->GetPosition(), NodesPartiallyInsideCommentPositionsBeforeMove[i]);
	}

	for (size_t i = 0; i < NodesOutsideComment.size(); i++)
	{
		EXPECT_EQ(NodesOutsideComment[i]->GetPosition(), NodesOutsideCommentPositionsBeforeMove[i]);
	}

	// Disable move elements with comment.
	Comment->SetMoveElementsWithComment(false);

	// Move comment again.
	PositionShift = ImVec2(200.0f, 200.0f);
	Comment->SetPosition(Comment->GetPosition() + PositionShift);

	for (size_t i = 0; i < NodesInsideComment.size(); i++)
	{
		EXPECT_EQ(NodesInsideComment[i]->GetPosition(), NodesInsideCommentPositionsBeforeMove[i]);
	}

	for (size_t i = 0; i < NodesPartiallyInsideComment.size(); i++)
	{
		EXPECT_EQ(NodesPartiallyInsideComment[i]->GetPosition(), NodesPartiallyInsideCommentPositionsBeforeMove[i]);
	}

	for (size_t i = 0; i < NodesOutsideComment.size(); i++)
	{
		EXPECT_EQ(NodesOutsideComment[i]->GetPosition(), NodesOutsideCommentPositionsBeforeMove[i]);
	}

	NODE_SYSTEM.Clear();
}

TEST(GroupComment, SetCaption_RejectsStringLongerThanMaxLength)
{
	NODE_SYSTEM.Clear();

	// Build a string that is exactly one character over the limit.
	const std::string OversizedCaption(GROUP_COMMENT_CAPTION_MAX_LENGTH + 1, 'X');

	GroupComment* NewGroupComment = new GroupComment();
	NewGroupComment->SetCaption(OversizedCaption);
	EXPECT_LE(NewGroupComment->GetCaption().size(), static_cast<size_t>(GROUP_COMMENT_CAPTION_MAX_LENGTH));

	delete NewGroupComment;
}

TEST(GroupComment, SetCaption_LeavesRoomForNullTerminator)
{
	NODE_SYSTEM.Clear();

	GroupComment* NewGroupComment = new GroupComment();

	// A caption of exactly the buffer size must be truncated by one character.
	NewGroupComment->SetCaption(std::string(GROUP_COMMENT_CAPTION_MAX_LENGTH, 'X'));
	EXPECT_EQ(NewGroupComment->GetCaption().size(), static_cast<size_t>(GROUP_COMMENT_CAPTION_MAX_LENGTH - 1));

	// A much longer caption is clamped the same way.
	NewGroupComment->SetCaption(std::string(GROUP_COMMENT_CAPTION_MAX_LENGTH * 2, 'X'));
	EXPECT_EQ(NewGroupComment->GetCaption().size(), static_cast<size_t>(GROUP_COMMENT_CAPTION_MAX_LENGTH - 1));

	// A caption that fits together with the null terminator is preserved unchanged.
	const std::string MaxFittingCaption(GROUP_COMMENT_CAPTION_MAX_LENGTH - 1, 'X');
	NewGroupComment->SetCaption(MaxFittingCaption);
	EXPECT_EQ(NewGroupComment->GetCaption(), MaxFittingCaption);

	delete NewGroupComment;
}

TEST(GroupComment, FromJson_EmptyObject_Returns_False)
{
	NODE_SYSTEM.Clear();

	GroupComment* Comment = new GroupComment();

	Json::Value EmptyJson(Json::objectValue);
	EXPECT_FALSE(Comment->FromJson(EmptyJson));

	delete Comment;
}

TEST(GroupComment, FromJson_MissingPositionField_Returns_False)
{
	NODE_SYSTEM.Clear();

	GroupComment* Comment = new GroupComment();

	Json::Value PartialJson(Json::objectValue);
	PartialJson["ID"] = "some_id";
	EXPECT_FALSE(Comment->FromJson(PartialJson));

	delete Comment;
}

TEST(GroupComment, FromJson_PositionFieldWrongType_Returns_False)
{
	NODE_SYSTEM.Clear();

	GroupComment* Comment = new GroupComment();

	Json::Value JsonWithBadPosition(Json::objectValue);
	JsonWithBadPosition["ID"] = "some_id";
	JsonWithBadPosition["Position"] = "not_an_object";
	EXPECT_FALSE(Comment->FromJson(JsonWithBadPosition));

	delete Comment;
}

TEST(NodeAreaGroupComment, GetNodesInGroupComment_ForeignGroupComment_ReturnsEmpty)
{
	NODE_SYSTEM.Clear();

	NodeArea* HostingArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(HostingArea, nullptr);

	NodeArea* QueriedArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(QueriedArea, nullptr);

	GroupComment* HostedComment = new GroupComment();
	HostedComment->SetPosition(ImVec2(0.0f, 0.0f));
	HostedComment->SetSize(ImVec2(200.0f, 200.0f));
	ASSERT_TRUE(HostingArea->AddGroupComment(HostedComment));

	// QueriedArea has a node whose bounding box sits inside HostedComment's rectangle.
	Node* NodeInQueriedArea = new Node();
	NodeInQueriedArea->SetPosition(ImVec2(50.0f, 50.0f));
	NodeInQueriedArea->SetSize(ImVec2(50.0f, 50.0f));
	ASSERT_TRUE(QueriedArea->AddNode(NodeInQueriedArea));

	std::vector<Node*> Result = QueriedArea->GetNodesInGroupComment(HostedComment);
	EXPECT_EQ(Result.size(), 0);

	NODE_SYSTEM.Clear();
}

TEST(GroupComment, MoveGroupComment_AfterAttachedNodeDeleted_DropsStaleAndMovesSurvivor)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	GroupComment* Comment = new GroupComment();
	Comment->SetSize(ImVec2(500, 500));
	ASSERT_TRUE(LocalNodeArea->AddGroupComment(Comment));

	// Two nodes fully inside the comment's rectangle.
	Node* DoomedNode = new Node();
	DoomedNode->SetPosition(ImVec2(50, 50));
	DoomedNode->SetSize(ImVec2(50, 50));
	ASSERT_TRUE(LocalNodeArea->AddNode(DoomedNode));

	Node* SurvivorNode = new Node();
	SurvivorNode->SetPosition(ImVec2(200, 200));
	SurvivorNode->SetSize(ImVec2(50, 50));
	ASSERT_TRUE(LocalNodeArea->AddNode(SurvivorNode));

	// Drag start: selecting the comment snapshots the IDs of the elements inside it.
	ASSERT_TRUE(LocalNodeArea->AddSelected(Comment));
	ASSERT_EQ(LocalNodeArea->GetGroupCommentAttachedNodeCount(Comment), 2);

	// An attached node is deleted before the move, leaving a stale cached ID.
	ASSERT_TRUE(LocalNodeArea->Delete(DoomedNode));

	// The move resolves each cached ID, drops the stale one, and moves the survivor.
	const ImVec2 SurvivorBefore = SurvivorNode->GetPosition();
	LocalNodeArea->MoveGroupComment(Comment, ImVec2(10, 10));
	EXPECT_EQ(LocalNodeArea->GetGroupCommentAttachedNodeCount(Comment), 1);
	EXPECT_EQ(SurvivorNode->GetPosition(), SurvivorBefore + ImVec2(10, 10));

	NODE_SYSTEM.Clear();
}

TEST(GroupComment, MoveGroupComment_NullGroupComment_IsIgnored)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	// Must not crash.
	LocalNodeArea->MoveGroupComment(nullptr, ImVec2(10, 10));

	NODE_SYSTEM.Clear();
}

TEST(GroupComment, MoveGroupComment_NestedGroupComment_MovesSharedNodeOnce)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	GroupComment* OuterComment = new GroupComment();
	OuterComment->SetPosition(ImVec2(0.0f, 0.0f));
	OuterComment->SetSize(ImVec2(500.0f, 500.0f));
	ASSERT_TRUE(LocalNodeArea->AddGroupComment(OuterComment));

	GroupComment* InnerComment = new GroupComment();
	InnerComment->SetPosition(ImVec2(50.0f, 50.0f));
	InnerComment->SetSize(ImVec2(300.0f, 300.0f));
	ASSERT_TRUE(LocalNodeArea->AddGroupComment(InnerComment));

	// The node is inside the inner comment, and therefore also inside the outer one.
	Node* SharedNode = new Node();
	SharedNode->SetPosition(ImVec2(100.0f, 100.0f));
	SharedNode->SetSize(ImVec2(50.0f, 50.0f));
	ASSERT_TRUE(LocalNodeArea->AddNode(SharedNode));

	const ImVec2 OuterBefore = OuterComment->GetPosition();
	const ImVec2 NodeBefore = SharedNode->GetPosition();
	const ImVec2 InnerBefore = InnerComment->GetPosition();
	const ImVec2 Delta = ImVec2(10.0f, 20.0f);

	OuterComment->SetPosition(OuterComment->GetPosition() + Delta);

	// Each element moves exactly once.
	EXPECT_EQ(OuterComment->GetPosition(), OuterBefore + Delta);
	EXPECT_EQ(InnerComment->GetPosition(), InnerBefore + Delta);
	EXPECT_EQ(SharedNode->GetPosition(), NodeBefore + Delta);

	// A second move accumulates correctly, the inner comment's fresh snapshot must not double-move either.
	OuterComment->SetPosition(OuterComment->GetPosition() + Delta);
	EXPECT_EQ(OuterComment->GetPosition(), OuterBefore + Delta + Delta);
	EXPECT_EQ(InnerComment->GetPosition(), InnerBefore + Delta + Delta);
	EXPECT_EQ(SharedNode->GetPosition(), NodeBefore + Delta + Delta);

	NODE_SYSTEM.Clear();
}

TEST(GroupComment, MoveGroupComment_IdenticalRectComments_ZeroDeltaMove_Terminates)
{
	NODE_SYSTEM.Clear();

	NodeArea* LocalNodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(LocalNodeArea, nullptr);

	GroupComment* FirstComment = new GroupComment();
	FirstComment->SetPosition(ImVec2(0.0f, 0.0f));
	FirstComment->SetSize(ImVec2(200.0f, 200.0f));
	ASSERT_TRUE(LocalNodeArea->AddGroupComment(FirstComment));

	GroupComment* SecondComment = new GroupComment();
	SecondComment->SetPosition(ImVec2(0.0f, 0.0f));
	SecondComment->SetSize(ImVec2(200.0f, 200.0f));
	ASSERT_TRUE(LocalNodeArea->AddGroupComment(SecondComment));

	// Completing the call without exhausting the stack is the pass criterion.
	FirstComment->SetPosition(FirstComment->GetPosition());

	EXPECT_EQ(FirstComment->GetPosition(), ImVec2(0.0f, 0.0f));
	EXPECT_EQ(SecondComment->GetPosition(), ImVec2(0.0f, 0.0f));

	NODE_SYSTEM.Clear();
}