#pragma once

#include "TaskChannel.hpp"

struct TaskListenerBase
{
	TaskListenerBase(TaskChannel &ch) : _ch(ch) {}

	TaskChannel &get_channel()
	{
		return _ch;
	}

private:
	TaskChannel &_ch;
};