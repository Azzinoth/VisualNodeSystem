#include "GroupCommentTests.h"
using namespace VisNodeSys;

TEST(GroupComment, CreateAndDelete)
{
	NodeArea* NodeArea = NODE_SYSTEM.CreateNodeArea();
	ASSERT_NE(NodeArea, nullptr);

	GroupComment* Comment = new GroupComment();
	EXPECT_NE(Comment, nullptr);

	EXPECT_EQ(NodeArea->GetGroupCommentCount(), 0);
	NodeArea->AddGroupComment(Comment);
	EXPECT_EQ(NodeArea->GetGroupCommentCount(), 1);

	NodeArea->DeleteGroupComment(Comment);
	EXPECT_EQ(NodeArea->GetGroupCommentCount(), 0);

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaGroupComment, GetNodesInComment)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaGroupComment, GetCommentsInComment)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}

TEST(NodeAreaGroupComment, MoveWithElements)
{
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

	NODE_SYSTEM.DeleteNodeArea(NodeArea);
}