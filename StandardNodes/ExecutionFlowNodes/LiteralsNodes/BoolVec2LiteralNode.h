#pragma once

#include "../../../VisualNodeSystem.h"

class BoolVec2LiteralNode : public VisNodeSys::Node
{
	glm::bvec2 Data = glm::bvec2(false);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	BoolVec2LiteralNode();
	BoolVec2LiteralNode(const BoolVec2LiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::bvec2 GetData() const;
	void SetData(glm::bvec2 NewValue);

	void Draw();
};