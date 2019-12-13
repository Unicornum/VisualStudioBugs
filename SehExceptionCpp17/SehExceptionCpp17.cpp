
#include <iostream>
#include <src/gtest-all.cc>
#include <src/gmock-all.cc>

class Check_test :
  public ::testing::Test
{
};

TEST_F(Check_test, /*DISABLED_*/Test_CheckExpect)
{
  class Proxy
  {
  public:
    MOCK_METHOD0(ExpectFailed, void(void));
  };

  Proxy CheckProxy;

  using namespace ::testing;

  EXPECT_CALL(CheckProxy, ExpectFailed())
    .Times(1);

  CheckProxy.ExpectFailed();
}

int main()
{
  ::testing::InitGoogleMock();

  return RUN_ALL_TESTS();
}
