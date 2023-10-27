#pragma once

#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include "ExecutorInterface.hpp"
#include "SchedulePolicy.hpp"

namespace detail
{

struct Worker
{
	explicit Worker()
	{
		static std::atomic<size_t> counter(0);
		_id = counter++;
		is_stop.store(false);
		worker_thread = std::thread(&Worker::dispatch_loop, this);
	}
	~Worker()
	{
		stop();
		work_queue_notifier.notify_one();
		worker_thread.join();
	}

	TokenPtr dispatch(Work&& w)
	{
		TokenPtr token = std::make_shared<TokenType>();
		work_queue.emplace(std::make_pair(token, w));
		work_queue_notifier.notify_one();
		return token;
	}

	template <typename Func, typename... Args>
	TokenPtr dispatch(Func&& work, Args&&... args)
	{
		TokenPtr token = std::make_shared<TokenType>();
		work_queue.emplace(std::make_pair(token, std::bind(std::forward<Func>(work), std::forward<Args>(args)...)));
		work_queue_notifier.notify_one();
		return token;
	}

	std::size_t get_id() const
	{
		return _id;
	}
	void stop()
	{
		is_stop.store(true);
	}

private:
	void dispatch_loop()
	{
		while (!is_stop)
		{
			Option<std::pair<TokenPtr, Work>> w;
			{
				std::unique_lock<std::mutex> lock(work_queue_mutex);
				work_queue_notifier.wait(lock, [this]() { return !work_queue.empty() || is_stop; });
				if (!is_stop)
				{

					w.set_some(work_queue.front());
					work_queue.pop();
				}
			}
			if (!is_stop && w.is_some())
			{
				w.unwrap().second(_id);
				w.unwrap().first->set();
			}
		}
	}
	std::queue<std::pair<TokenPtr, Work>> work_queue;
	std::size_t _id;
	std::atomic<bool> is_stop;
	std::mutex work_queue_mutex;
	std::condition_variable work_queue_notifier;
	std::thread worker_thread;
};

} // namespace detail

struct ThreadPoolExecutor final : public executor::detail::ExecutorInterface
{
	ThreadPoolExecutor(SchedulePolicy& policy, std::size_t worker_size = 4) : number_of_workers(worker_size), _policy(policy)
	{
		for (size_t i = 0; i < number_of_workers; i++)
		{
			auto temp = std::make_unique<detail::Worker>();
			std::size_t wid = temp->get_id();
			workerMap.emplace(wid, std::move(temp));
		}
		std::vector<std::size_t> wids;
		for (auto& it : workerMap)
		{
			wids.push_back(it.first);
		}
		_policy.set_worker_id_list(wids);
	}
	virtual ~ThreadPoolExecutor() override
	{
	}

	virtual TokenPtr dispatch(Work&& work) override
	{
		auto wid = _policy.get_next_id();

		if (workerMap.find(wid) != workerMap.end())
		{
			return workerMap[wid]->dispatch(std::forward<Work>(work));
		}
		return {};
	}

	template <typename Func, typename... Args>
	TokenPtr dispatch_with_args(Func&& work, Args&&... args)
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
	std::map<std::size_t, std::shared_ptr<detail::Worker>> workerMap;
	SchedulePolicy& _policy;
};
