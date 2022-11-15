#pragma once

#include <shared_mutex>
#include <atomic>
#include "Option.hpp"

template <typename mutex_type = std::shared_mutex, typename guard_type = std::lock_guard<mutex_type>>
struct Token
{
	using token_pointer = std::shared_ptr<Token>;

	explicit Token()
	{
		_ret.reset();
	}
	~Token() {}

	void set()
	{
		guard_type guard(mut);
		_ret.set_some(true);
	}

	void reset()
	{
		guard_type guard(mut);
		_ret.reset();
	}

	Option<bool> get()
	{
		guard_type guard(mut);
		return _ret;
	}

	bool is_set()
	{
		guard_type guard(mut);
		return _ret.is_some();
	}

	void wait()
	{
		while (!_ret.is_some())
		{
		}
	}

	operator bool()
	{
		guard_type guard(mut);
		return _ret.is_some();
	}

private:
	Option<bool> _ret;
	mutable mutex_type mut;
};
