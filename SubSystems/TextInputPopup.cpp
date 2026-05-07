#include "TextInputPopup.h"
using namespace VisNodeSys;

#ifdef VISUAL_NODE_SYSTEM_SHARED
extern "C" __declspec(dllexport) void* GetTextInputPopup()
{
	return TextInputPopup::GetInstancePointer();
}
#endif

TextInputPopup::TextInputPopup() {}

void TextInputPopup::Show(const std::string& InCaption, const std::string& InPromptText, const std::string& InitialText,
						  std::function<bool(const std::string& Input, std::string& OutErrorMessage)> InValidator,
						  std::function<void(const std::string& Input)> InOnAccept)
{
	Caption = InCaption;
	PromptText = InPromptText;
	InputBuffer = InitialText;
	Validator = std::move(InValidator);
	OnAccept = std::move(InOnAccept);
	bShouldOpen = true;
}

void TextInputPopup::Render()
{
	std::string FullCaption = Caption + "##TextInputPopup";

	if (bShouldOpen)
	{
		ImGui::OpenPopup(FullCaption.c_str());
		bShouldOpen = false;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
	if (ImGui::BeginPopupModal(FullCaption.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s", PromptText.c_str());
		ImGui::SetNextItemWidth(300.0f);
		if (ImGui::IsWindowAppearing())
			ImGui::SetKeyboardFocusHere();
		bool bEnterPressed = ImGui::InputText("##TextInputPopupField", &InputBuffer, ImGuiInputTextFlags_EnterReturnsTrue);

		std::string ErrorMessage;
		bool bIsValid = true;
		if (Validator != nullptr)
			bIsValid = Validator(InputBuffer, ErrorMessage);

		if (!bIsValid && !ErrorMessage.empty())
			ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", ErrorMessage.c_str());

		ImGui::Spacing();

		const float ButtonWidth = 120.0f;
		const float ButtonsTotalWidth = ButtonWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ButtonsTotalWidth) * 0.5f);

		if (!bIsValid)
			ImGui::BeginDisabled();

		if (ImGui::Button("Apply", ImVec2(ButtonWidth, 0)) || (bEnterPressed && bIsValid))
		{
			if (OnAccept != nullptr)
				OnAccept(InputBuffer);
			ImGui::CloseCurrentPopup();
		}

		if (!bIsValid)
			ImGui::EndDisabled();

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(ButtonWidth, 0)))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}
