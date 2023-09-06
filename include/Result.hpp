#pragma once
#include <string>
#include <type_traits>
#include <variant>
#include "Option.hpp"

struct ErrorBase
{
	virtual std::string describe_what() = 0;
};

template <typename Ok, typename Error>
struct Result
{
public:
	explicit Result() = default;
	explicit Result(Ok ok)
	{
		set_value(ok);
	}

	explicit Result(Error err)
	{
		set_error(err);
	}

	bool is_value() const
	{
		return mRes.index() == 0;
	}
	bool is_error() const
	{
		return mRes.index() == 1;
	}

	void set_value(Ok ok)
	{
		mRes = ok;
	}

	std::enable_if_t<std::is_base_of<ErrorBase, Error>::value> set_error(Error err)
	{
		mRes = err;
	}

	auto get_value() const
	{
		if (is_value())
		{
			return Option<Ok>(std::get<Ok>(mRes));
		}
		return Option<Ok>();
	}

	auto get_error() const
	{
		if (is_error())
		{
			return Option<Error>(std::get<Error>(mRes));
		}
		return Option<Error>();
	}

private:
	std::variant<Ok, Error> mRes;
};
