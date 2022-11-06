#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Result.hpp"

using ::testing::Return;

struct FrameError : ErrorBase
{
	std::string describe_what() override
	{
		return "Frame Error occured";
	}
};

struct Frame
{
	int frameData{12};
};

struct CommInterface
{
public:
	virtual Result<Frame, FrameError> read_frame() = 0;
};

struct CommInterfaceMock : public CommInterface
{
public:
	MOCK_METHOD((Result<Frame, FrameError>), read_frame, (), (override));
};

TEST(Result, CanSetError)
{
	Result<Frame, FrameError> res;
	res.set_error(FrameError{});
	EXPECT_TRUE(res.is_error());
	EXPECT_FALSE(res.is_value());
}

TEST(Result, CanSetValue)
{
	Result<Frame, FrameError> res;
	res.set_value(Frame{});
	EXPECT_TRUE(res.is_value());
	EXPECT_FALSE(res.is_error());
}

TEST(Result, SwitchBetweenErrorAndValue)
{
	Result<Frame, FrameError> res;
	res.set_error(FrameError{});
	EXPECT_TRUE(res.is_error());
	EXPECT_FALSE(res.is_value());
	res.set_value(Frame{});
	EXPECT_TRUE(res.is_value());
	EXPECT_FALSE(res.is_error());
}

TEST(Result, mockedInterface)
{
	CommInterfaceMock comm;

	EXPECT_CALL(comm, read_frame())
		.WillOnce(Return(Result<Frame, FrameError>(Frame{})))
		.WillOnce(Return(Result<Frame, FrameError>(FrameError{})));

	auto res = comm.read_frame();

	EXPECT_TRUE(res.is_value());
	EXPECT_FALSE(res.is_error());

	res = comm.read_frame();

	EXPECT_TRUE(res.is_error());
	EXPECT_FALSE(res.is_value());
}

TEST(Result, CanGetValue)
{
	CommInterfaceMock comm;
	EXPECT_CALL(comm, read_frame)
		.WillOnce(Return(Result<Frame, FrameError>(Frame{54})));

	auto frameRes = comm.read_frame();
	EXPECT_TRUE(frameRes.is_value());
	EXPECT_FALSE(frameRes.is_error());

	Option<Frame> frame = frameRes.get_value();
	EXPECT_TRUE(frame);
	EXPECT_EQ(frame.unwrap().frameData, 54);
}

TEST(Result, CanGetError)
{
	CommInterfaceMock comm;
	EXPECT_CALL(comm, read_frame)
		.WillOnce(Return(Result<Frame, FrameError>(FrameError{})));

	auto frameRes = comm.read_frame();
	EXPECT_TRUE(frameRes.is_error());
	EXPECT_FALSE(frameRes.is_value());

	Option<FrameError> error = frameRes.get_error();
	EXPECT_TRUE(error);
	EXPECT_TRUE(error.unwrap().describe_what() == std::string("Frame Error occured"));
}