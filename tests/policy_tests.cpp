#include <gtest/gtest.h>
#include "Task/SchedulePolicy.hpp"

TEST(SchedulePolicy, RoundRobinTest)
{
	DefaultSchedulerPolicy policy;
	std::vector<size_t> mListOfWorkerIds;
	const int wIdSize = 4;
	for (size_t i = 0; i < wIdSize; i++)
	{
		mListOfWorkerIds.push_back(i);
	}
	policy.set_worker_id_list(mListOfWorkerIds);

	for (size_t i = 0; i < wIdSize * 12; i++)
	{
		EXPECT_EQ(policy.get_next_id(), i % wIdSize);
	}
}

TEST(SchedulePolicy, BoundOne)
{
	const int boundedTaskId = 3;
	BoundOnePolicy policy(boundedTaskId);

	for (size_t i = 0; i < 5; i++)
	{
		EXPECT_EQ(policy.get_next_id(), boundedTaskId);
	}
}