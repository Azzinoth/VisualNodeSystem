#pragma once

#include "../VisualNodeCore.h"

namespace VisNodeSys
{
	class VISUAL_NODE_SYSTEM_API TextInputPopup
	{
		SINGLETON_PRIVATE_PART(TextInputPopup)

		std::string Caption;
		std::string PromptText;
		std::string InputBuffer;
		std::function<bool(const std::string& Input, std::string& OutErrorMessage)> Validator;
		std::function<void(const std::string& Input)> OnAccept;
		bool bShouldOpen = false;
	public:
		SINGLETON_PUBLIC_PART(TextInputPopup)

		void Show(const std::string& Caption, const std::string& PromptText, const std::string& InitialText,
				  std::function<bool(const std::string& Input, std::string& OutErrorMessage)> Validator,
				  std::function<void(const std::string& Input)> OnAccept);

		void Render();
	};

#ifdef VISUAL_NODE_SYSTEM_SHARED
	extern "C" __declspec(dllexport) void* GetTextInputPopup();
#define TEXT_INPUT_POPUP (*static_cast<VisNodeSys::TextInputPopup*>(VisNodeSys::GetTextInputPopup()))
#else
#define TEXT_INPUT_POPUP VisNodeSys::TextInputPopup::GetInstance()
#endif
}
