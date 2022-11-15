#include "Token.hpp"
#include <gtest/gtest.h>
#include <thread>

TEST(Token, canSetToken)
{
	Token<> token;
	std::thread t([&token]() mutable
	{
		std::this_thread::sleep_for(std::chrono::seconds(3));
		token.set();
	});
	t.detach();

	for (size_t i = 0; i < 2; i++)
	{
		EXPECT_FALSE(token);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));
	EXPECT_TRUE(token == true);
}