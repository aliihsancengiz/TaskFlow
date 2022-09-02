#pragma once

#include "task_mapping.hpp"

#include <typeindex>
#include <map>

struct TaskDispatcher
{
	template <typename type_of_task>
	void register_task(std::function<void(std::unique_ptr<type_of_task> &)> handler)
	{
		const auto type_idx = std::type_index(typeid(type_of_task));
		functionMap.emplace(type_idx, std::make_unique<task_mapping_imp<type_of_task>>(std::bind(handler, std::placeholders::_1)));
	}

	template <typename type_of_task, typename object_type>
	void register_task(void (object_type::*handler)(std::unique_ptr<type_of_task> &), object_type *object)
	{
		const auto type_idx = std::type_index(typeid(type_of_task));
		functionMap.emplace(type_idx, std::make_unique<task_mapping_imp<type_of_task>>(std::bind(handler, object, std::placeholders::_1)));
	}

	template <typename type_of_task>
	void dispatch(std::unique_ptr<type_of_task> &&task)
	{
		std::unique_ptr<task_base> tmp(std::move(task));
		const auto type_idx = std::type_index(typeid(type_of_task));
		functionMap[type_idx]->dispatch(tmp);
	}

private:
	std::map<std::type_index, std::unique_ptr<task_mapping_base>> functionMap;
};
