#include <Balancer/Redirector.h>
#include <stdexcept>
#include "Logger.h"

namespace balancer {
Redirector::Redirector(const std::unordered_map<std::string, long long int>& initialETAsForHosts,
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
  std::lock_guard<std::mutex> lock(statsMutex);
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
  logger.debug("Adding to ETA of host " + host + " " + std::to_string(meanTimesForEndpoints[endpoint]) + " (endpoint "
                   + endpoint.to_string() + ")" + " (" + requestID + ")");
  hostsETAs[host] += meanTimesForEndpoints[endpoint]; // add request proposed time to host
  proposedETAsForRequests[requestID] = meanTimesForEndpoints[endpoint];
  hostsQueue->push(host); // return host back to queue
}

void Redirector::finishRequest(const std::string& requestID) {
  std::lock_guard<std::mutex> lock(statsMutex);
  long long int duration =
      std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::steady_clock::now()
          - requestTimers[requestID])).count(); // count current request duration
  logger.debug("Finishing request `" + requestID + "` with duration " + std::to_string(duration));
  auto endpoint = requestData[requestID]; // get request context
  auto meanDuration = meanTimesForEndpoints[endpoint]; // get current mean duration for last requests
  logger.debug("Mean duration for endpoint `" + endpoint.to_string() + "` is " + std::to_string(meanDuration));
  auto& durationsList = requestsTimes[endpoint]; // get last requests durations for the endpoint
  int lastRequestsNumber = durationsList.size();
  if (lastRequestsNumber >= lastRequestsMeanWindow) { // update mean duration when window is full
    long long int lastRequestDuration = 0;
    if (lastRequestsNumber != 0) {
      lastRequestDuration = durationsList.back();
      durationsList.pop_back();
    }
    durationsList.push_front(duration);
    meanDuration = (meanDuration * lastRequestsMeanWindow - lastRequestDuration + duration) / lastRequestsMeanWindow;
    meanDuration += duration;
  } else {
    durationsList.push_front(duration);
    if (lastRequestsNumber == 0) {
      meanDuration = duration;
    } else {
      meanDuration = (meanDuration * lastRequestsNumber + duration) / (lastRequestsNumber + 1);
    }
  }
  logger.debug("Updated mean duration for endpoint " + endpoint.to_string() + " is " + std::to_string(meanDuration));
  meanTimesForEndpoints[endpoint] = meanDuration; // restore structures states
  requestTimers.erase(requestID);
  requestData.erase(requestID);
  auto proposedETA = proposedETAsForRequests[requestID];
  proposedETAsForRequests.erase(requestID);
  logger.debug("Proposed ETA for request was " + std::to_string(proposedETA));
  logger.debug("Substracting ETA " + std::to_string(proposedETA) + " (" + requestID + ")");
  hostsETAs[endpoint.host] -= proposedETA;
  logger.debug(
      "Updated ETA for host `" + endpoint.host + "` is " + std::to_string(hostsETAs[endpoint.host]) + " (" + requestID
          + ")");

  auto host = hostsQueue->top(); // update the hosts queue
  hostsQueue->pop();
  hostsQueue->push(host);
}
}
