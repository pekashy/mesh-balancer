#include <gtest/gtest.h>
#include "Balancer/Redirector.h"

using namespace balancer;

class RedirectorStub : public Redirector {
 public:
  RedirectorStub(const std::unordered_map<std::string, long long int>& initialETAsForHosts,
                 const std::vector<std::string>& hosts,
                 int lastRequestsWindow)
      : Redirector(initialETAsForHosts, hosts, lastRequestsWindow) {
  }
  void setMeanTimesForEndpoints(const std::unordered_map<Endpoint, long long int>& endps) {
    for (auto& endp_meantime_pair: endps) {
      meanTimesForEndpoints.insert(endp_meantime_pair);
    }
  }

  void assertETA(const std::string& host, long long int targetETA) {
    ASSERT_EQ(hostsETAs[host], targetETA);
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
                            {"host1", "host2", "host3", "host4"}, 10);

  auto url = redirector.getNextRedirectURL(requestID, path);
  ASSERT_EQ(url, "host2/path1");
}

TEST_F(RedirectorTests, TestGetHostWithMinETASecondReuestOtherHost) {
  std::string requestID1 = "requestID1";
  std::string requestID2 = "requestID2";
  std::string path = "/path1";

  RedirectorStub redirector({
                                {"host1", 10}, {"host2", 1}, {"host3", 20}, {"host4", 30},},
                            {"host1", "host2", "host3", "host4"}, 10);

  Endpoint e{"host2", path};
  std::unordered_map<Endpoint, long long int> endpointMeanTimes{{e, 100}};
  redirector.setMeanTimesForEndpoints(endpointMeanTimes);
  auto url1 = redirector.getNextRedirectURL(requestID1, path);
  auto url2 = redirector.getNextRedirectURL(requestID2, path);

  ASSERT_EQ(url2, "host1/path1");
}

TEST_F(RedirectorTests, TestGetHostWithMinETASecondReuestSameHost) {
  std::string requestID1 = "requestID1";
  std::string requestID2 = "requestID2";
  std::string path = "/path1";

  RedirectorStub redirector({
                                {"host1", 10}, {"host2", 1}, {"host3", 20}, {"host4", 30},},
                            {"host1", "host2", "host3", "host4"}, 10);

  Endpoint e{"host2", path};
  std::unordered_map<Endpoint, long long int> endpointMeanTimes{{e, 1}};
  redirector.setMeanTimesForEndpoints(endpointMeanTimes);
  auto url1 = redirector.getNextRedirectURL(requestID1, path);
  auto url2 = redirector.getNextRedirectURL(requestID2, path);

  ASSERT_EQ(url2, "host2/path1");
}

TEST_F(RedirectorTests, TestFinishRequest) {
  std::string requestID1 = "requestID1";
  std::string requestID2 = "requestID2";
  std::string path = "/path1";

  RedirectorStub redirector({
                                {"host1", 10}, {"host2", 1}, {"host3", 20}, {"host4", 300},},
                            {"host1", "host2", "host3", "host4"}, 10);

  Endpoint e{"host2", path};
  std::unordered_map<Endpoint, long long int> endpointMeanTimes{{e, 1000}};
  redirector.setMeanTimesForEndpoints(endpointMeanTimes);
  auto url1 = redirector.getNextRedirectURL(requestID1, path);
  redirector.finishRequest(requestID1);
  auto url2 = redirector.getNextRedirectURL(requestID2, path);

  ASSERT_EQ(url2, "host2/path1");
}

TEST_F(RedirectorTests, TestFinish4RequestsConsequentlyAllAssignedToTheLeastLoaded) {
  std::string requestID1 = "requestID1";
  std::string requestID2 = "requestID2";
  std::string requestID3 = "requestID3";
  std::string requestID4 = "requestID4";
  std::string path = "/path1";

  RedirectorStub redirector({
                                {"host1", 10000}, {"host2", 1}, {"host3", 20}, {"host4", 300},},
                            {"host1", "host2", "host3", "host4"}, 100);

  Endpoint e2{"host2", path};
  std::unordered_map<Endpoint, long long int> endpointMeanTimes{{e2, 1000}};
  redirector.setMeanTimesForEndpoints(endpointMeanTimes);
  auto url1 = redirector.getNextRedirectURL(requestID1, path);
  usleep(1000);
  redirector.finishRequest(requestID1);
  auto url2 = redirector.getNextRedirectURL(requestID2, path);
  usleep(1000);
  redirector.finishRequest(requestID2);
  auto url3 = redirector.getNextRedirectURL(requestID3, path);
  usleep(1000);
  redirector.finishRequest(requestID3);
  auto url4 = redirector.getNextRedirectURL(requestID4, path);
  redirector.finishRequest(requestID4);

  ASSERT_EQ(url1, "host2/path1");
  ASSERT_EQ(url2, "host2/path1");
  ASSERT_EQ(url3, "host2/path1");
  ASSERT_EQ(url4, "host2/path1");
  redirector.assertETA("host1", 10000);
  redirector.assertETA("host2", 1);
  redirector.assertETA("host3", 20);
  redirector.assertETA("host4", 300);
}

TEST_F(RedirectorTests, TestFinish4RequestsInParallel) {
  std::string requestID1 = "requestID1";
  std::string requestID2 = "requestID2";
  std::string requestID3 = "requestID3";
  std::string requestID4 = "requestID4";
  std::string path = "/path1";

  RedirectorStub redirector({
                                {"host1", 10000}, {"host2", 1}, {"host3", 20}, {"host4", 300},},
                            {"host1", "host2", "host3", "host4"}, 100);

  Endpoint e1{"host1", path};
  Endpoint e2{"host2", path};
  Endpoint e3{"host3", path};
  Endpoint e4{"host4", path};

  std::unordered_map<Endpoint, long long int> endpointMeanTimes{{e1, 1000}, {e2, 1000}, {e3, 1000}, {e4, 1000}};
  redirector.setMeanTimesForEndpoints(endpointMeanTimes);
  auto url1 = redirector.getNextRedirectURL(requestID1, path);
  usleep(1000);
  auto url2 = redirector.getNextRedirectURL(requestID2, path);
  usleep(1000);
  auto url3 = redirector.getNextRedirectURL(requestID3, path);
  usleep(1000);
  auto url4 = redirector.getNextRedirectURL(requestID4, path);
  redirector.finishRequest(requestID4);
  redirector.finishRequest(requestID1);
  redirector.finishRequest(requestID3);
  redirector.finishRequest(requestID2);

  ASSERT_EQ(url4,
            "host2/path1"); // not to host1 as too busy, so we assign as round robin skipping host 1 and start the second lap with host2
  redirector.assertETA("host1", 10000);
  redirector.assertETA("host2", 1);
  redirector.assertETA("host3", 20);
  redirector.assertETA("host4", 300);
}

TEST_F(RedirectorTests, TestPopOutLastRequest) {
  std::string requestID1 = "requestID1";
  std::string requestID2 = "requestID2";
  std::string requestID3 = "requestID3";
  std::string requestID4 = "requestID4";
  std::string path = "/path1";

  RedirectorStub redirector({
                                {"host1", 0}},
                            {"host1"}, 1);

  Endpoint e{"host1", path};
  std::unordered_map<Endpoint, long long int> endpointMeanTimes{{e, 0}};
  redirector.setMeanTimesForEndpoints(endpointMeanTimes);
  auto url1 = redirector.getNextRedirectURL(requestID1, path);
  redirector.finishRequest(requestID1);
  auto url2 = redirector.getNextRedirectURL(requestID2, path);
  redirector.finishRequest(requestID2);
  auto url3 = redirector.getNextRedirectURL(requestID3, path);
  redirector.finishRequest(requestID3);
  auto url4 = redirector.getNextRedirectURL(requestID4, path);
  redirector.finishRequest(requestID4);

  ASSERT_EQ(url4, "host1/path1");
  redirector.assertETA("host1", 0);
}
