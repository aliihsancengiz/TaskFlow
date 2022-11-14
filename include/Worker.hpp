#pragma once
#include <iostream>
#include <atomic>
#include <functional>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "Option.hpp"
#include "Token.hpp"

using Work = std::function<void(std::size_t)>;
using TokenType = Token<bool>;
using TokenPtr = std::shared_ptr<TokenType>;

struct Worker
{

	Worker()
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

	TokenPtr dispatch(Work &&w)
	{
		TokenPtr token = std::make_shared<TokenType>();
		work_queue.emplace(std::make_pair(token, w));
		work_queue_notifier.notify_one();
		return token;
	}

	template <typename Func, typename... Args>
	TokenPtr dispatch(Func &&work, Args &&...args)
	{
		TokenPtr token = std::make_shared<TokenType>();
		work_queue.emplace(std::make_pair(token,std::bind(std::forward<Func>(work), std::forward<Args>(args)...)));
		work_queue_notifier.notify_one();
		return token;
	}

	std::size_t get_id() const { return _id; }
	void stop() { is_stop.store(true); }

private:
	void dispatch_loop()
	{
		while (!is_stop)
		{
			Option<std::pair<TokenPtr,Work>> w;
			{
				std::unique_lock<std::mutex> lock(work_queue_mutex);
				work_queue_notifier.wait(lock, [this]()
										 { return !work_queue.empty() || is_stop; });
				if (!is_stop)
				{

					w.set_some(work_queue.front());
					work_queue.pop();
				}
			}
			if (!is_stop && w.is_some())
			{
				w.unwrap().second(_id);
				w.unwrap().first->set(true);
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
