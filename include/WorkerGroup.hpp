#pragma once

#include "Worker.hpp"
#include "SchedulePolicy.hpp"
#include <map>

struct WorkerGroup
{
	WorkerGroup(SchedulePolicy &policy, std::size_t worker_size = 4) : number_of_workers(worker_size),
																	   _policy(policy)
	{
		for (size_t i = 0; i < number_of_workers; i++)
		{
			auto temp = std::make_unique<Worker>();
			std::size_t wid = temp->get_id();
			workerMap.emplace(wid, std::move(temp));
		}
		std::vector<std::size_t> wids;
		for (auto &it : workerMap)
		{
			wids.push_back(it.first);
		}
		_policy.set_worker_id_list(wids);
	}
	~WorkerGroup()
	{
	}

	TokenPtr dispatch(Work &&work)
	{
		auto wid = _policy.get_next_id();

		if (workerMap.find(wid) != workerMap.end())
		{
			return workerMap[wid]->dispatch(std::forward<Work>(work));
		}
		return {};
	}

	template <typename Func, typename... Args>
	TokenPtr dispatch_with_args(Func &&work, Args &&...args)
	{
		auto wid = _policy.get_next_id();

		if (workerMap.find(wid) != workerMap.end())
		{
			return workerMap[wid]->dispatch(std::forward<Func>(work), std::forward<Args>(args)...);
		}
		return {};
	}

private:
	std::size_t number_of_workers;
	std::map<std::size_t, std::shared_ptr<Worker>> workerMap;
	SchedulePolicy &_policy;
};
