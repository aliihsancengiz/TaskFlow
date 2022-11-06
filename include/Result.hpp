#pragma once
#include <variant>
#include <type_traits>
#include <string>
#include "Option.hpp"

struct ErrorBase
{
	virtual std::string describe_what() = 0;
};

template <typename Ok, typename Error>
struct Result
{
public:
	Result() = default;
	Result(Ok ok)
	{
		set_value(ok);
	}

	Result(Error err)
	{
		set_error(err);
	}

	bool is_value() { return mRes.index() == 0; }
	bool is_error() { return mRes.index() == 1; }

	void set_value(Ok ok)
	{
		mRes = ok;
	}

	std::enable_if_t<std::is_base_of<ErrorBase, Error>::value>
	set_error(Error err)
	{
		mRes = err;
	}

	Option<Ok> get_value()
	{
		if (is_value())
		{
			return std::get<Ok>(mRes);
		}
		return {};
	}

	Option<Error> get_error()
	{
		if (is_error())
		{
			return std::get<Error>(mRes);
		}
		return {};
	}

private:
	std::variant<Ok, Error> mRes;
};
