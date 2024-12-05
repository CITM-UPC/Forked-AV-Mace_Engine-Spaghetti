#pragma once
#include <list>
#include <memory>
template <typename TParent>
class PolymorphList
{
	std::list<std::unique_ptr<TParent>> _list;
public:
	template <typename TChild>
	TChild& front() {
		for (auto& item : _list) if (TChild* child = dynamic_cast<TChild*>(item.get()))	return *child;
		throw std::runtime_error("Item not found");
	}
	template <typename TChild, typename ...Args>
	TChild* emplace(Args&&... args) {
		TParent& item = _list.emplace(std::make_unique<TChild>(std::forward<Args>(args)...));
	}

};