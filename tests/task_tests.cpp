#include <gtest/gtest.h>

#include "Task/TaskListener.hpp"

std::mutex m1, m2;
struct Task1 : taskBase
{
	void do_task1()
	{
		std::lock_guard<std::mutex> _lock(m1);
		taskCounter++;
	}
	static size_t taskCounter;
};

struct Task2 : taskBase
{
	void do_task2()
	{
		std::lock_guard<std::mutex> _lock(m2);
		taskCounter++;
	}
	static size_t taskCounter;
};

size_t Task1::taskCounter = 0;
size_t Task2::taskCounter = 0;

struct DummyTaskListener : TaskListenerBase
{
	DummyTaskListener(TaskChannel& ch) : TaskListenerBase(ch)
	{
		ch.registerTask<Task1>(&DummyTaskListener::handle_task1, this);
		ch.registerTask<Task2>(&DummyTaskListener::handle_task2, this);
	}
	void handle_task1(std::shared_ptr<Task1>& task)
	{
		task->do_task1();
	}
	void handle_task2(std::shared_ptr<Task2>& task)
	{
		task->do_task2();
	}
};

TEST(TaskTestFixture, canDispatchTasks)
{
	DefaultSchedulerPolicy policy;
	std::shared_ptr<ThreadPoolExecutor> wg = std::make_shared<ThreadPoolExecutor>(policy);
	TaskChannel ch(wg);
	DummyTaskListener listener(ch);

	const int taskSize = 100;
	std::vector<TokenPtr> tokenList;
	for (size_t i = 0; i < taskSize; i++)
	{
		tokenList.push_back(listener.get_channel().push(std::make_shared<Task1>()));
		tokenList.push_back(listener.get_channel().push(std::make_shared<Task2>()));
	}

	for (size_t i = 0; i < tokenList.size(); i++)
	{
		tokenList[i]->wait();
	}

	EXPECT_EQ(Task1::taskCounter, taskSize);
	EXPECT_EQ(Task2::taskCounter, taskSize);
}