#include <gtest/gtest.h>
#include "Balancer/Redirector.h"

using namespace balancer;

class RedirectorTests : public testing::Test, public Redirector {

};

TEST(RedirectorTests, TestNumberOne){
	Redirector redirector;
	EXPECT_NE(2, 1);
}
