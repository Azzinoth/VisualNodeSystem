#include "GroupComment.h"
#include "SubSystems/VisualNodeArea/VisualNodeArea.h"
using namespace VisNodeSys;

char GroupComment::GroupCommentRename[GROUP_COMMENT_CAPTION_MAX_LENGTH] = "";

GroupComment::GroupComment(const std::string ID)
{
	this->ID = ID;
	if (ID.empty())
		this->ID = NODE_CORE.GetUniqueHexID();

	SetSize(ImVec2(200, 200));
}

GroupComment::GroupComment(const GroupComment& Other)
{
	ParentArea = Other.ParentArea;
	ID = NODE_CORE.GetUniqueHexID();
	Position = Other.Position;
	Size = Other.Size;
	Caption = Other.Caption;
	bMoveElementsWithComment = Other.bMoveElementsWithComment;
	BackgroundColor = Other.BackgroundColor;
}

std::string GroupComment::GetID()
{
	return ID;
}

ImVec2 GroupComment::GetPosition() const
{
	return Position;
}

void GroupComment::SetPosition(const ImVec2 NewValue)
{
	if (ParentArea == nullptr)
	{
		Position = NewValue;
		return;
	}

	ImVec2 Delta = NewValue - Position;
	ParentArea->AttachElementsToGroupComment(this);
	ParentArea->MoveGroupComment(this, Delta);
}

ImVec2 GroupComment::GetSize() const
{
	return Size;
}

void GroupComment::SetSize(const ImVec2 NewValue)
{
	Size = NewValue;
}

void GroupComment::Draw() {}

Json::Value GroupComment::ToJson()
{
	Json::Value Result;

	Result["ID"] = ID;
	Result["Position"]["X"] = Position.x;
	Result["Position"]["Y"] = Position.y;
	Result["Size"]["X"] = Size.x;
	Result["Size"]["Y"] = Size.y;
	Result["Caption"] = Caption;
	Result["bMoveElementsWithComment"] = bMoveElementsWithComment;
	Result["BackgroundColor"]["X"] = BackgroundColor.x;
	Result["BackgroundColor"]["Y"] = BackgroundColor.y;
	Result["BackgroundColor"]["Z"] = BackgroundColor.z;
	Result["BackgroundColor"]["W"] = BackgroundColor.w;

	return Result;
}

void GroupComment::FromJson(Json::Value Json)
{
	ID = Json["ID"].asCString();
	Position.x = Json["Position"]["X"].asFloat();
	Position.y = Json["Position"]["Y"].asFloat();
	Size.x = Json["Size"]["X"].asFloat();
	Size.y = Json["Size"]["Y"].asFloat();
	Caption = Json["Caption"].asCString();
	bMoveElementsWithComment = Json["bMoveElementsWithComment"].asBool();
	BackgroundColor.x = Json["BackgroundColor"]["X"].asFloat();
	BackgroundColor.y = Json["BackgroundColor"]["Y"].asFloat();
	BackgroundColor.z = Json["BackgroundColor"]["Z"].asFloat();
	BackgroundColor.w = Json["BackgroundColor"]["W"].asFloat();
}

bool GroupComment::IsHovered() const
{
	return bHovered;
}

bool GroupComment::IsSelected() const
{
	return bSelected;
}

NodeArea* GroupComment::GetParentArea() const
{
	return ParentArea;
}

void GroupComment::SetCaption(std::string NewValue)
{
	Caption = NewValue;
}

std::string GroupComment::GetCaption() const
{
	return Caption;
}

float GroupComment::GetCaptionHeight(float Zoom) const
{
	return 50.0f * Zoom;
}

ImVec2 GroupComment::GetCaptionSize(float Zoom) const
{
	return ImVec2((GetSize() * Zoom).x - 8.0f * Zoom, GetCaptionHeight(Zoom));
}

bool GroupComment::MoveElementsWithComment() const
{
	return bMoveElementsWithComment;
}

void GroupComment::SetMoveElementsWithComment(bool NewValue)
{
	bMoveElementsWithComment = NewValue;
}