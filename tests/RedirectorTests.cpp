#include <gtest/gtest.h>
#include "Balancer/Redirector.h"

using namespace balancer;

class RedirectorStub : public Redirector {
 public:
	RedirectorStub(const std::unordered_map<std::string, long>& initialETAsForHosts,
								 const std::vector<std::string>& hosts)
			: Redirector(initialETAsForHosts, hosts) {
	}
	void setMeanTimesForEndpoints(const std::unordered_map<Endpoint, long>& endps) {
		meanTimesForEndpoints = endps;
	}
};

class RedirectorTests : public testing::Test {
 public:
};

TEST_F(RedirectorTests, TestGetHostWithMinETA) {
	std::string requestID = "requestID1";
	std::string path = "/path1";

	RedirectorStub redirector({
																{"host1", 10}, {"host2", 1}, {"host3", 2}, {"host4", 3},},
														{"host1", "host2", "host3", "host4"});

	auto url = redirector.getNextRedirectURL("requestID", path);
	ASSERT_EQ(url, "host2/path1");
}
