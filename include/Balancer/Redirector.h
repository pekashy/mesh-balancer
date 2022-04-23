#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include <list>
#include <queue>
#include <functional>
#include <memory>
#include <spdlog/logger.h>

struct Endpoint {
	std::string host;
	std::string path;

	std::string to_string() const {
		return host + path;
	}

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
	Redirector(const std::unordered_map<std::string, long>& initialETAsForHosts,
											const std::vector<std::string>& hosts,
											int lastRequestsWindow);
	std::string getNextRedirectURL(const std::string& requestID, const std::string& path);
	void finishRequest(const std::string& requestID);
 protected:
	void registerRequestToPathOnHost(const std::string& requestID, const std::string& path, const std::string& host);
	std::string assignRequest(const std::string& requestID, const std::string& path);

	spdlog::logger& logger;
	std::unordered_map<std::string, std::chrono::steady_clock::time_point> requestTimers;  // timers for active requests
	std::unordered_map<Endpoint, std::deque<long>> requestsTimes;  // last requests times history to calculate mean time
	std::unordered_map<Endpoint, long> meanTimesForEndpoints;
	std::unordered_map<std::string, long> recordedETAsForRequests;
	std::unordered_map<std::string, long> hostsETAs;  // Sum_reqType(meanTime(requestType)*reqNumber(requestType))
	std::unordered_map<std::string, Endpoint> requestData;
	int lastRequestsMeanWindow;

	std::unique_ptr<queueType> hostsQueue;
};
}
