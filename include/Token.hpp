#pragma once

#include <shared_mutex>
#include <atomic>
#include "Option.hpp"

template <typename return_type, typename mutex_type = std::shared_mutex, typename guard_type = std::lock_guard<mutex_type>>
struct Token
{
	using token_pointer = std::shared_ptr<Token>;

	explicit Token()
	{
		_state.store(false);
	}
	~Token() {}

	void set(return_type r)
	{
		guard_type guard(mut);
		_ret.set_some(r);
		_state.store(true);
	}

	void reset()
	{
		guard_type guard(mut);
		_ret.reset();
		_state.store(false);
	}

	Option<return_type> get()
	{
		guard_type guard(mut);
		if (_state.load())
		{
			return _ret;
		}
		else
		{
			return Option<return_type>();
		}
	}

	bool is_set()
	{
		guard_type guard(mut);
		return _state.load();
	}

	void wait()
	{
		while (!_state.load())
		{
		}
	}

	operator bool()
	{
		return _state.load();
	}

private:
	std::atomic<bool> _state;
	Option<return_type> _ret;
	mutable mutex_type mut;
};
