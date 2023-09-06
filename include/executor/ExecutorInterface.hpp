#pragma once
#include <functional>
#include "Token.hpp"

namespace
{
using Work = std::function<void(std::size_t)>;
using TokenType = Token<>;
using TokenPtr = std::shared_ptr<TokenType>;

} // namespace

namespace executor
{
namespace detail
{
struct ExecutorInterface
{

	virtual TokenPtr dispatch(Work&& work) = 0;

	virtual ~ExecutorInterface() = default;
};

} // namespace detail

} // namespace executor
