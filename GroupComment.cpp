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

GroupComment::GroupComment(const GroupComment& Src)
{
	ParentArea = Src.ParentArea;
	ID = NODE_CORE.GetUniqueHexID();
	Position = Src.Position;
	Size = Src.Size;
	Caption = Src.Caption;
	bMoveElementsWithComment = Src.bMoveElementsWithComment;
	BackgroundColor = Src.BackgroundColor;
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
	Result["position"]["x"] = Position.x;
	Result["position"]["y"] = Position.y;
	Result["size"]["x"] = Size.x;
	Result["size"]["y"] = Size.y;
	Result["caption"] = Caption;
	Result["bMoveElementsWithComment"] = bMoveElementsWithComment;
	Result["BackgroundColor"]["x"] = BackgroundColor.x;
	Result["BackgroundColor"]["y"] = BackgroundColor.y;
	Result["BackgroundColor"]["z"] = BackgroundColor.z;
	Result["BackgroundColor"]["w"] = BackgroundColor.w;

	return Result;
}

void GroupComment::FromJson(Json::Value Json)
{
	ID = Json["ID"].asCString();
	Position.x = Json["position"]["x"].asFloat();
	Position.y = Json["position"]["y"].asFloat();
	Size.x = Json["size"]["x"].asFloat();
	Size.y = Json["size"]["y"].asFloat();
	Caption = Json["caption"].asCString();
	bMoveElementsWithComment = Json["bMoveElementsWithComment"].asBool();
	BackgroundColor.x = Json["BackgroundColor"]["x"].asFloat();
	BackgroundColor.y = Json["BackgroundColor"]["y"].asFloat();
	BackgroundColor.z = Json["BackgroundColor"]["z"].asFloat();
	BackgroundColor.w = Json["BackgroundColor"]["w"].asFloat();
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