#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <chrono>
#include <list>

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
class Redirector {
 public:
	std::string getNextRedirectURL();
	void registerEvent(const RequestID& requestID);
	void finishRequest(const RequestID& requestID);
 private:
	std::unordered_map<Endpoint, int> totalActive;  // total active requests for endpoints
	std::unordered_map<RequestID, std::chrono::steady_clock::time_point> requestTimers;  // timers for active requests
	std::unordered_map<Endpoint, std::list<double>> requestsTimes;  // last requests times history to calculate mean time
	std::unordered_map<Endpoint, double> meanTimesForEndpoints;
	std::unordered_map<std::string, double> hostsETAs;  // Sum_reqType(meanTime(requestType)*reqNumber(requestType))
	int activeRequestsWindow = 100;

	std::vector<std::string> locations = {
			{"http://0.0.0.0:30001/bench"},
			{"http://0.0.0.0:30002/bench"},
			{"http://0.0.0.0:30003/bench"},
			{"http://0.0.0.0:30004/bench"},
	};
	int nextPointer;
};
}
