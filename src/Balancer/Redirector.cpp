#include <Balancer/Redirector.h>
#include <stdexcept>
#include "Logger.h"

namespace balancer {
Redirector::Redirector(const std::unordered_map<std::string, long>& initialETAsForHosts,
											 const std::vector<std::string>& hosts,
											 int lastRequestsWindow)
		: lastRequestsMeanWindow(lastRequestsWindow), hostsETAs(initialETAsForHosts), logger(LoggerContainer::Get()) {
	hostsQueue = std::make_unique<queueType>([this](const std::string& lhs, const std::string& rhs) -> bool {
		return this->hostsETAs[lhs] > this->hostsETAs[rhs];
	});
	for (auto& host: hosts) {
		logger.info("Registering host `" + host + "`");
		hostsQueue->push(host);
	}
}

std::string Redirector::getNextRedirectURL(const std::string& requestID, const std::string& path) {
	return assignRequest(requestID, path) + path;
}

std::string Redirector::assignRequest(const std::string& requestID, const std::string& path) {
	// TODO implement locking logic
	if (hostsQueue->empty()) {
		throw std::runtime_error("Locking logic is not implemented; No hosts in queue, cannot assign request");
	}
	logger.debug("Assigning request `" + requestID + "` " + "targeted at endpoint `" + path + "`");
	auto host = hostsQueue->top();
	hostsQueue->pop(); // get host with minimum expected ETA
	logger.info("Request `" + requestID + "` " + "is to be targeted to host `" + host + "`");
	registerRequestToPathOnHost(requestID, path, host); // make state changes related to assignment
	return host;
}

void Redirector::registerRequestToPathOnHost(const std::string& requestID,
																						 const std::string& path,
																						 const std::string& host) {
	Endpoint endpoint{host, path};
	requestData[requestID] = endpoint; // save request context to id
	requestTimers[requestID] = std::chrono::steady_clock::now(); // save request start time
	logger.debug("Request `" + requestID + "` start time is " +
			std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
					requestTimers[requestID].time_since_epoch()).count()));
	logger.trace("Adding to ETA of host " + host + " " + std::to_string(meanTimesForEndpoints[endpoint]) + " (endpoint "
									 + endpoint.to_string() + ")");
	hostsETAs[host] += meanTimesForEndpoints[endpoint]; // add request proposed time to host
	recordedETAsForRequests[requestID] = meanTimesForEndpoints[endpoint];
	hostsQueue->push(host); // return host back to queue
}

void Redirector::finishRequest(const std::string& requestID) {
	auto duration =
			std::chrono::duration_cast<std::chrono::microseconds>((std::chrono::steady_clock::now()
					- requestTimers[requestID])).count(); // count current request duration
	logger.debug("Finishing request `" + requestID + "` with duration " + std::to_string(duration));
	auto endpoint = requestData[requestID]; // get request context
	long meanDuration = meanTimesForEndpoints[endpoint]; // get current mean duration for last requests
	logger.debug("Mean duration for endpoint `" + endpoint.to_string() + "` is " + std::to_string(meanDuration));
	auto& durationsList = requestsTimes[endpoint]; // get last requests durations for the endpoint
	int lastRequestsNumber = durationsList.size();
	if (lastRequestsNumber >= lastRequestsMeanWindow) { // update mean duration when window is full
		long lastRequestDuration = 0;
		if (lastRequestsNumber != 0) {
			lastRequestDuration = durationsList.back();
			durationsList.pop_back();
		}
		durationsList.push_front(duration);
		meanDuration -= lastRequestDuration;
		meanDuration += duration;
	} else {
		durationsList.push_front(duration);
		if (lastRequestsNumber == 0) {
			meanDuration = duration;
		} else {
			meanDuration =
					meanDuration * ((long)(lastRequestsNumber + 1.0) / lastRequestsNumber) + duration / (lastRequestsNumber + 1);
		}
	}
	logger.debug("Updated mean duration for endpoint " + endpoint.to_string() + " is " + std::to_string(meanDuration));
	meanTimesForEndpoints[endpoint] = meanDuration; // restore structures states
	requestTimers.erase(requestID);
	requestData.erase(requestID);
	long recordedETA = recordedETAsForRequests[requestID];
	recordedETAsForRequests.erase(requestID);
	logger.trace("Last mean duration is " + std::to_string(recordedETA));
	hostsETAs[endpoint.host] = std::max(hostsETAs[endpoint.host] - recordedETA, (long)0);
	logger.debug("Updated ETA for host `" + endpoint.host + "` is " + std::to_string(hostsETAs[endpoint.host]));

	auto host = hostsQueue->top(); // update the hosts queue
	hostsQueue->pop();
	hostsQueue->push(host);
}
}
