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
	Redirector();
	std::string getNextRedirectURL();
	std::string assignRequest(const RequestID& requestID, const std::string& path);
	void finishRequest(const RequestID& requestID);
 private:
	void registerRequestToPathOnHost(const RequestID& requestID, const std::string& path, const std::string& host);

	std::unordered_map<Endpoint, int> totalActive;  // total active requests for endpoints
	std::unordered_map<RequestID, std::chrono::steady_clock::time_point> requestTimers;  // timers for active requests
	std::unordered_map<Endpoint, std::deque<long>> requestsTimes;  // last requests times history to calculate mean time
	std::unordered_map<Endpoint, long> meanTimesForEndpoints;
	std::unordered_map<std::string, long> hostsETAs;  // Sum_reqType(meanTime(requestType)*reqNumber(requestType))
	std::unordered_map<std::string, Endpoint> requestData;
	int lastRequestsMeanWindow;

	std::unique_ptr<queueType> hostsQueue;

	std::vector<std::string> locations = {
			{"http://0.0.0.0:30001/bench"},
			{"http://0.0.0.0:30002/bench"},
			{"http://0.0.0.0:30003/bench"},
			{"http://0.0.0.0:30004/bench"},
	};
	int nextPointer;
};
}
