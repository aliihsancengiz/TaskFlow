#include <gtest/gtest.h>

#include "Option.hpp"

struct Foo
{
	int k;
};

TEST(Option, CanConstructWithSome)
{
	Option<Foo> op;
	op.set_some(Foo {12});
	EXPECT_TRUE(op.is_some());
	EXPECT_FALSE(op.is_none());
}

TEST(Option, CanConstructWithNone)
{
	Option<Foo> op;
	op.reset();
	EXPECT_TRUE(op.is_none());
	EXPECT_FALSE(op.is_some());
}

TEST(Option, CanUnrapWithValue)
{
	Option<Foo> op;
	op.set_some(Foo {23});
	auto res = op.unwrap();
	EXPECT_EQ(res.k, 23);
}

TEST(Option, CanThrowWithNone)
{
	Option<Foo> op;
	op.reset();
	EXPECT_THROW(op.unwrap(), std::exception);
}

TEST(Option, DefaultInCaseOfNone)
{
	Option<Foo> op;
	op.reset();
	auto res = op.unwrap_or(Foo {2022});
	EXPECT_FALSE(op);
	EXPECT_TRUE(op.is_none());
	EXPECT_EQ(res.k, 2022);
}