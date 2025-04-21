#pragma once

#include "VisualNodeFactory.h"

namespace VisNodeSys
{
#define GROUP_COMMENT_CAPTION_MAX_LENGTH 1024
#define GROUP_COMMENT_TITLE_HEIGHT 30.0f

	class VISUAL_NODE_SYSTEM_API GroupComment
	{
	protected:
		friend class NodeSystem;
		friend class NodeArea;

		NodeArea* ParentArea = nullptr;
		std::string ID;
		ImVec2 Position;
		ImVec2 Size;

		bool bHovered = false;
		bool bCaptionHovered = false;
		bool bSelected = false;

		virtual void Draw();

		std::string Caption;

		bool bIsRenamingActive = false;
		bool bLastFrameRenameEditWasVisible = false;

		bool bVerticalResizeActive = false;
		bool bHorizontalResizeActive = false;

		static char GroupCommentRename[GROUP_COMMENT_CAPTION_MAX_LENGTH];

		float GetCaptionHeight(float Zoom) const;
		ImVec2 GetCaptionSize(float Zoom) const;

		// Temporary storage for moving.
		std::vector<Node*> AttachedNodes;
		std::vector<RerouteNode*> AttachedRerouteNodes;
		std::vector<GroupComment*> AttachedGroupComments;

		ImVec4 BackgroundColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

		bool bMoveElementsWithComment = true;
	public:
		GroupComment(std::string ID = "");
		GroupComment(const GroupComment& Src);

		std::string GetID();

		ImVec2 GetPosition() const;
		void SetPosition(ImVec2 NewValue);

		ImVec2 GetSize() const;
		void SetSize(ImVec2 NewValue);

		NodeArea* GetParentArea() const;

		Json::Value ToJson();
		void FromJson(Json::Value Json);

		bool IsHovered() const;
		bool IsSelected() const;

		void SetCaption(std::string NewValue);
		std::string GetCaption() const;

		bool MoveElementsWithComment() const;
		void SetMoveElementsWithComment(bool NewValue);
	};
}