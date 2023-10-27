#pragma once

#include "TaskDispatcher.hpp"

struct TaskChannel
{
	explicit TaskChannel(std::shared_ptr<executor::detail::ExecutorInterface> wg)
	{
		dispatcher = std::make_unique<TaskDispatcher>(wg);
	}
	template <typename type_of_task>
	void registerTaskHandler(std::function<void(std::shared_ptr<type_of_task>&)> handler)
	{
		dispatcher->registerTask<type_of_task>(handler);
	}
	template <typename type_of_task, typename object_type>
	void registerTask(void (object_type::*handler)(std::shared_ptr<type_of_task>&), object_type* object)
	{
		dispatcher->registerTask<type_of_task, object_type>(handler, object);
	}
	template <typename type_of_task>
	TokenPtr push(std::shared_ptr<type_of_task>&& task)
	{
		return dispatcher->dispatch<type_of_task>(std::forward<std::shared_ptr<type_of_task>>(task));
	}

private:
	std::unique_ptr<TaskDispatcher> dispatcher;
};
