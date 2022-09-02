#pragma once

#include "task.hpp"
#include <functional>
#include <memory>

template <typename task_type>
using task_handle = std::function<void(std::unique_ptr<task_type> &)>;

struct task_mapping_base
{
	virtual bool dispatch(std::unique_ptr<task_base> &task_ptr) = 0;
};

template <typename stored_task_type>
struct task_mapping_imp : task_mapping_base
{
	task_mapping_imp(task_handle<stored_task_type> handle) : _handle(handle)
	{
	}
	bool dispatch(std::unique_ptr<task_base> &task_ptr) override
	{
		stored_task_type *ptr = static_cast<stored_task_type *>(task_ptr.release());
		if (ptr)
		{
			std::unique_ptr<stored_task_type> casted_ptr(ptr);

			_handle(casted_ptr);

			if (casted_ptr.get())
			{
				task_ptr.reset(static_cast<task_base *>(casted_ptr.release()));
				return true;
			}
			else
			{
				return false;
			}
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
