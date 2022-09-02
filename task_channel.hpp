#pragma once

#include "task_dispatcher.hpp"

struct TaskChannel
{
	TaskChannel()
	{
	}
	template <typename type_of_task>
	void register_task_handler(std::function<void(std::unique_ptr<type_of_task> &)> handler)
	{
		dispatcher.register_task<type_of_task>(handler);
	}
	template <typename type_of_task, typename object_type>
	void register_task(void (object_type::*handler)(std::unique_ptr<type_of_task> &), object_type *object)
	{
		dispatcher.register_task<type_of_task, object_type>(handler, object);
	}
	template <typename type_of_task>
	void push(std::unique_ptr<type_of_task> &&task)
	{
		dispatcher.dispatch<type_of_task>(std::forward<std::unique_ptr<type_of_task>>(task));
	}

	TaskDispatcher dispatcher;
};
