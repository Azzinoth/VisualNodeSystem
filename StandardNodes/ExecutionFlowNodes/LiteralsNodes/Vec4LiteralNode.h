#pragma once

#include "../../../VisualNodeSystem.h"

class Vec4LiteralNode : public VisNodeSys::Node
{
	glm::vec4 Data = glm::vec4(0.0f);

	std::function<void* ()> Vec4DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	Vec4LiteralNode();
	Vec4LiteralNode(const Vec4LiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::vec4 GetData() const;
	void SetData(glm::vec4 NewValue);

	void Draw();
};