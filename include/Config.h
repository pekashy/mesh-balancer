#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct Config {
	explicit Config(const std::string& fileneame);
	std::vector<std::string> hosts;
	std::unordered_map<std::string, long long int> initialHostsETAs;
	std::string ip;
	int redirectorPort;
	int recorderPort;
	int nThreads;
	int windowSize;
};