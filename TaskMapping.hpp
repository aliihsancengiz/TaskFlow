#pragma once

#include "Task.hpp"
#include <functional>
#include <memory>

template <typename task_type>
using task_handle = std::function<void(std::shared_ptr<task_type> &)>;

struct taskMappingBase
{
	virtual bool dispatch(std::shared_ptr<taskBase> &task_ptr) = 0;
};

template <typename stored_task_type>
struct taskMapping : taskMappingBase
{
	taskMapping(task_handle<stored_task_type> handle) : _handle(handle)
	{
	}
	bool dispatch(std::shared_ptr<taskBase> &task_ptr) override
	{
		std::shared_ptr<stored_task_type> ptr = std::static_pointer_cast<stored_task_type>(task_ptr);
		if (ptr)
		{
			_handle(ptr);
			return true;
		}
		else
		{
			std::cout << "Could not dispatch a task . It is a nullptr." << std::endl;
			return false;
		}
	}

private:
	task_handle<stored_task_type> _handle;
};
