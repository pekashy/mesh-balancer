#include <Balancer/Redirector.h>

namespace balancer {
std::string Redirector::GetNextRedirectURL() {
	return locations[nextPointer++ % locations.size()];
};
}
