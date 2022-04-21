#include <Balancer/Redirector.h>

namespace balancer {
std::string Redirector::getNextRedirectURL() {
	return locations[nextPointer++ % locations.size()];
};

void Redirector::registerEvent(const std::string& requestID) {

}

void Redirector::finishRequest(const std::string& requestID) {

}
}
