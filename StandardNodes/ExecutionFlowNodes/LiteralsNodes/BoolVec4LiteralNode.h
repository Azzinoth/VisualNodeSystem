#pragma once

#include "../../../VisualNodeSystem.h"

class BoolVec4LiteralNode : public VisNodeSys::Node
{
	glm::bvec4 Data = glm::bvec4(false);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	BoolVec4LiteralNode();
	BoolVec4LiteralNode(const BoolVec4LiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::bvec4 GetData() const;
	void SetData(glm::bvec4 NewValue);

	void Draw();
};