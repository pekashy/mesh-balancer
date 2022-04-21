#include <Balancer/Redirector.h>
#include <stdexcept>

namespace balancer {
Redirector::Redirector()
	: lastRequestsMeanWindow(100) {
	hostsQueue = std::make_unique<queueType>([this](const std::string& lhs, const std::string& rhs) -> bool {
		return this->hostsETAs[lhs] > this->hostsETAs[rhs];
	});
}

std::string Redirector::getNextRedirectURL() {
	return locations[nextPointer++ % locations.size()];
}

std::string Redirector::assignRequest(const RequestID& requestID, const std::string& path) {
	// TODO implement locking logic
	if(hostsQueue->empty()) {
		throw new std::runtime_error("Locking logic is not implemented; No hosts in queue, cannot assign request");
	}

	auto host = hostsQueue->top();
	hostsQueue->pop(); // get host with minimum expected ETA
	registerRequestToPathOnHost(requestID, path, host); // make state changes related to assignment
	return host;
}

void Redirector::registerRequestToPathOnHost(const RequestID& requestID, const std::string& path, const std::string& host) {
	Endpoint endpoint {host, path};
	requestData[requestID] = endpoint; // save request context to id
	totalActive[endpoint]++; // increase number of total active requests for endpoint
	requestTimers[requestID] = std::chrono::steady_clock::now(); // save request start time
	hostsETAs[host] += meanTimesForEndpoints[endpoint]; // add request proposed time to host
	hostsQueue->push(host); // return host back to queue
}

void Redirector::finishRequest(const std::string& requestID) {
	auto endpoint = requestData[requestID]; // get request context
	auto& durationsList = requestsTimes[endpoint]; // get last requests durations for the endpoint
	int lastRequestsNumber = durationsList.size();
	auto duration = (std::chrono::steady_clock::now() - requestTimers[requestID]).count(); // count current request duration
	long lastMeanDuration, meanDuration = meanTimesForEndpoints[endpoint]; // get current mean duration for last requests

	if(lastRequestsNumber >= lastRequestsMeanWindow) { // update mean duration when window is full
		auto lastRequestDuration = durationsList.back();
		durationsList.pop_back();
		durationsList.push_front(duration);
		meanDuration -= lastRequestDuration;
		meanDuration += duration;
	} else {
		durationsList.push_front(duration);
		meanDuration = meanDuration*((long) (lastRequestsNumber + 1.0)/lastRequestsNumber) + duration/(lastRequestsNumber + 1);
	}
	meanTimesForEndpoints[endpoint] = meanDuration; // restore structures states
	totalActive[endpoint]--;
	requestTimers.erase(requestID);
	requestData.erase(requestID);
	hostsETAs[endpoint.host] -= lastMeanDuration;
}
}
