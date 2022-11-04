#pragma once

#include "TaskMapping.hpp"
#include <typeindex>
#include <map>

struct TaskDispatcher
{
	template <typename type_of_task>
	void registerTask(std::function<void(std::shared_ptr<type_of_task> &)> handler)
	{
		const auto type_idx = std::type_index(typeid(type_of_task));
		functionMap.emplace(type_idx, std::make_unique<taskMapping<type_of_task>>(std::bind(handler, std::placeholders::_1)));
	}

	template <typename type_of_task, typename object_type>
	void registerTask(void (object_type::*handler)(std::shared_ptr<type_of_task> &), object_type *object)
	{
		const auto type_idx = std::type_index(typeid(type_of_task));
		functionMap.emplace(type_idx, std::make_unique<taskMapping<type_of_task>>(std::bind(handler, object, std::placeholders::_1)));
	}

	template <typename type_of_task>
	void dispatch(std::shared_ptr<type_of_task> &&task)
	{
		std::shared_ptr<taskBase> tmp(std::move(task));
		const auto type_idx = std::type_index(typeid(type_of_task));
		functionMap[type_idx]->dispatch(tmp);
	}

private:
	std::map<std::type_index, std::shared_ptr<taskMappingBase>> functionMap;
};
