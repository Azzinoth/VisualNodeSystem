#pragma once

#include "../../../VisualNodeSystem.h"

class BoolVec3LiteralNode : public VisNodeSys::Node
{
	glm::bvec3 Data = glm::bvec3(false);

	std::function<void* ()> DataGetter = [this]() -> void* {
		return &Data;
	};

public:
	BoolVec3LiteralNode();
	BoolVec3LiteralNode(const BoolVec3LiteralNode& Other);

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	glm::bvec3 GetData() const;
	void SetData(glm::bvec3 NewValue);

	void Draw();
};