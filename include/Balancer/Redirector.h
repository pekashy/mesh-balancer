#pragma once

#include <unordered_map>
#include <string>
#include <vector>


namespace balancer {
class Redirector {
 public:
	Redirector() = default;
	std::string GetNextRedirectURL();
 private:
	std::vector<std::string> locations = {
			{"http://0.0.0.0:30001/bench"},
			{"http://0.0.0.0:30002/bench"},
			{"http://0.0.0.0:30003/bench"},
			{"http://0.0.0.0:30004/bench"},
	};
	int nextPointer;
};
}
