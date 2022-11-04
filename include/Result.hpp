#pragma once
#include <variant>
#include <type_traits>
#include <optional>
#include <string>

struct ErrorBase
{
	virtual std::string describe_what() = 0;
};

template <typename Ok, typename Error>
struct Result
{
	void set_value(Ok ok)
	{
		mRes = ok;
	}

	std::enable_if_t<std::is_base_of<ErrorBase, Error>::value>
	set_error(Error err)
	{
		mRes = err;
	}

	std::optional<Ok> get_result()
	{
		if (is_okey())
		{
			return std::get<Ok>(mRes);
		}
		return std::nullopt;
	}

	std::optional<Error> get_error()
	{
		if (is_error())
		{
			return std::get<Error>(mRes);
		}
		return std::nullopt;
	}

	bool is_okey() { return mRes.index() == 0; }
	bool is_error() { return mRes.index() == 1; }

private:
	std::variant<Ok, Error> mRes;
};
