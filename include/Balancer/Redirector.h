#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include <list>
#include <queue>
#include <functional>
#include <memory>

using RequestID = std::string;

struct Endpoint {
	std::string host;
	std::string path;

	bool operator==(const Endpoint& e) const {
		return host == e.host && path == e.path;
	}
};

namespace std {
template<>
struct hash<Endpoint> {
	inline size_t operator()(const Endpoint& endpoint) const {
		return std::hash<std::string>()(endpoint.host) ^ std::hash<std::string>()(endpoint.path);
	}
};
}

namespace balancer {
using queueType = std::priority_queue<std::string, std::vector<std::string>,
																			std::function<bool(const std::string&, const std::string&)>>;

class Redirector {
 public:
	explicit Redirector(const std::unordered_map<std::string, long>& initialETAsForHosts,
											const std::vector<std::string>& hosts);
	std::string getNextRedirectURL(const RequestID& requestID, const std::string& path);
	std::string assignRequest(const RequestID& requestID, const std::string& path);
	void finishRequest(const RequestID& requestID);
 protected:
	void registerRequestToPathOnHost(const RequestID& requestID, const std::string& path, const std::string& host);

	std::unordered_map<RequestID, std::chrono::steady_clock::time_point> requestTimers;  // timers for active requests
	std::unordered_map<Endpoint, std::deque<long>> requestsTimes;  // last requests times history to calculate mean time
	std::unordered_map<Endpoint, long> meanTimesForEndpoints;
	std::unordered_map<std::string, long> hostsETAs;  // Sum_reqType(meanTime(requestType)*reqNumber(requestType))
	std::unordered_map<std::string, Endpoint> requestData;
	int lastRequestsMeanWindow;

	std::unique_ptr<queueType> hostsQueue;
};
}
