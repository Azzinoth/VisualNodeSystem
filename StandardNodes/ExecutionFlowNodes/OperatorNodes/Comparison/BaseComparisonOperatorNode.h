#pragma once

#include "../../BaseExecutionFlowNode.h"

enum class ComparisonNodeOperatorType
{
	EQUAL,
	NOT_EQUAL,
	GREATER_THAN_OR_EQUAL,
	GREATER_THAN,
	LESS_THAN_OR_EQUAL,
	LESS_THAN
};

class BaseComparisonOperatorNode : public BaseExecutionFlowNode
{
	bool CanConnect(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* CandidateSocket, char** MsgToUser);
	void SocketEvent(VisNodeSys::NodeSocket* OwnSocket, VisNodeSys::NodeSocket* ConnectedSocket, VisNodeSys::NODE_SOCKET_EVENT EventType);

protected:
	ComparisonNodeOperatorType OperatorType = ComparisonNodeOperatorType::EQUAL;
private:
	// TO-DO: use std::optional to save memory.
	bool LocalBoolData = false;
	glm::bvec2 LocalBVec2Data = glm::bvec2(false);
	glm::bvec3 LocalBVec3Data = glm::bvec3(false);
	glm::bvec4 LocalBVec4Data = glm::bvec4(false);

	// Generic comparison function for basic types (int, float, bool)
	template <typename T>
	bool CompareValues(const T& A, const T& B)
	{
		switch (OperatorType)
		{
			case ComparisonNodeOperatorType::EQUAL:
				return A == B;
			case ComparisonNodeOperatorType::NOT_EQUAL:
				return A != B;
			case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
				return A >= B;
			case ComparisonNodeOperatorType::GREATER_THAN:
				return A > B;
			case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
				return A <= B;
			case ComparisonNodeOperatorType::LESS_THAN:
				return A < B;
			default:
				return false;
		}
	}

	// Template specialization for glm vector types (vec2, vec3, vec4)
	template <typename T, glm::length_t L, typename = typename std::enable_if<
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, int>::value>::type>
		glm::vec<L, bool> CompareValues(const glm::vec<L, T>& A, const glm::vec<L, T>& B)
	{
		switch (OperatorType)
		{
			case ComparisonNodeOperatorType::EQUAL:
				return glm::equal(A, B);
			case ComparisonNodeOperatorType::NOT_EQUAL:
				return glm::notEqual(A, B);
			case ComparisonNodeOperatorType::GREATER_THAN_OR_EQUAL:
				return glm::greaterThanEqual(A, B);
			case ComparisonNodeOperatorType::GREATER_THAN:
				return glm::greaterThan(A, B);
			case ComparisonNodeOperatorType::LESS_THAN_OR_EQUAL:
				return glm::lessThanEqual(A, B);
			case ComparisonNodeOperatorType::LESS_THAN:
				return glm::lessThan(A, B);
			default:
				return glm::vec<L, bool>(false);
		}
	}

	void Execute()
	{
		std::string CurrentMode = GetActiveINDataType();
		if (CurrentMode.empty())
			return;

		// If we don't have both A and B inputs connected, we can't do anything.
		if (Input[1]->GetConnectedSockets().empty() ||
			Input[2]->GetConnectedSockets().empty())
			return;

		void* AData = Input[1]->GetConnectedSockets()[0]->GetData();
		void* BData = Input[2]->GetConnectedSockets()[0]->GetData();

		if (AData == nullptr || BData == nullptr)
			return;

		if (CurrentMode == "BOOL")
		{
			bool A = *reinterpret_cast<bool*>(AData);
			bool B = *reinterpret_cast<bool*>(BData);

			LocalBoolData = CompareValues(A, B);
		}
		else if (CurrentMode == "INT")
		{
			int A = *reinterpret_cast<int*>(AData);
			int B = *reinterpret_cast<int*>(BData);

			LocalBoolData = CompareValues(A, B);
		}
		else if (CurrentMode == "FLOAT")
		{
			float A = *reinterpret_cast<float*>(AData);
			float B = *reinterpret_cast<float*>(BData);

			LocalBoolData = CompareValues(A, B);
		}
		else if (CurrentMode == "VEC2")
		{
			glm::vec2 A = *reinterpret_cast<glm::vec2*>(AData);
			glm::vec2 B = *reinterpret_cast<glm::vec2*>(BData);

			LocalBVec2Data = CompareValues(A, B);
		}
		else if (CurrentMode == "VEC3")
		{
			glm::vec3 A = *reinterpret_cast<glm::vec3*>(AData);
			glm::vec3 B = *reinterpret_cast<glm::vec3*>(BData);

			LocalBVec3Data = CompareValues(A, B);
		}
		else if (CurrentMode == "VEC4")
		{
			glm::vec4 A = *reinterpret_cast<glm::vec4*>(AData);
			glm::vec4 B = *reinterpret_cast<glm::vec4*>(BData);

			LocalBVec4Data = CompareValues(A, B);
		}
	}

	std::function<void* ()> ComparisonGetter = [this]() -> void* {
		std::string CurrentMode = GetActiveINDataType();
		if (CurrentMode.empty())
			return nullptr;

		if (CurrentMode == "BOOL")
		{
			return &LocalBoolData;
		}
		else if (CurrentMode == "INT")
		{
			return &LocalBoolData;
		}
		else if (CurrentMode == "FLOAT")
		{
			return &LocalBoolData;
		}
		else if (CurrentMode == "VEC2")
		{
			return &LocalBVec2Data;
		}
		else if (CurrentMode == "VEC3")
		{
			return &LocalBVec3Data;
		}
		else if (CurrentMode == "VEC4")
		{
			return &LocalBVec4Data;
		}

		return nullptr;
	};

public:
	BaseComparisonOperatorNode();
	BaseComparisonOperatorNode(const BaseComparisonOperatorNode& Other);
	virtual ~BaseComparisonOperatorNode() = default;

	Json::Value ToJson();
	bool FromJson(Json::Value Json);

	void Draw();

	std::string GetActiveINDataType();
	std::string GetActiveOUTDataType();
};