#pragma once

#include "../../../VisualNodeSystem.h"

class Vec3LiteralNode : public VisNodeSys::Node
{
	glm::vec3 Data = glm::vec3(0.0f);

	std::function<void* ()> Vec3DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	Vec3LiteralNode();
	Vec3LiteralNode(const Vec3LiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::vec3 GetData() const;
	void SetData(glm::vec3 NewValue);

	void Draw();
};