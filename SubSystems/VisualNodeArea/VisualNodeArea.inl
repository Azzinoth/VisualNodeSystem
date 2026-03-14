#pragma once

template<typename T>
std::vector<T*> NodeArea::GetNodesByType() const
{
	static_assert(std::is_base_of<Node, T>::value, "T must derive from Node");
	std::vector<T*> Result;
	for (Node* CurrentNode : Nodes)
	{
		T* Casted = dynamic_cast<T*>(CurrentNode);
		if (Casted != nullptr)
			Result.push_back(Casted);
	}

	return Result;
}