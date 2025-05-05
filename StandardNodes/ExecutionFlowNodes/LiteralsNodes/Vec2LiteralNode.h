#pragma once

#include "../../../VisualNodeSystem.h"

class Vec2LiteralNode : public VisNodeSys::Node
{
	glm::vec2 Data = glm::vec2(0.0f);

	std::function<void* ()> Vec2DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	Vec2LiteralNode();
	Vec2LiteralNode(const Vec2LiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::vec2 GetData() const;
	void SetData(glm::vec2 NewValue);

	void Draw();
};