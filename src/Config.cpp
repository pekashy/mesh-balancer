#include <iostream>
#include "Config.h"
#include "Logger.h"

#include "yaml-cpp/yaml.h"

Config::Config(const std::string& fileneame) {
	YAML::Node config;
	auto logger = LoggerContainer::Get();
	try{
		config = YAML::LoadFile(fileneame);
	} catch(YAML::BadFile &e){
		throw std::runtime_error("Failed to load config file");
	}
	if(!config["hosts"]) {
		throw std::runtime_error("No `hosts` specified!");
	}
	for(int i = 0; i < config["hosts"].size(); ++i) {
		auto host = config["hosts"][i].as<std::string>();
		logger.info("Found host in config: " + host);
		hosts.push_back(host);
		initialHostsETAs[host];
	}
	if(!config["window_size"]) {
		throw std::runtime_error("No `window_size` - number of requests to consider while congfiguring mean time specified!");
	}
	windowSize = config["window_size"].as<int>();
	logger.info(config["window_size"].as<int>());
	if(!config["ip"]) {
		throw std::runtime_error("Failed to load value `ip`!");
	}
	ip = config["ip"].as<std::string>();
	logger.info("Using ip: " + ip);
	if(!config["port_record"]) {
		throw std::runtime_error("Failed to load value `port_record`!");
	}
	recorderPort = config["port_record"].as<int>();
	logger.info("Port for recording: " + std::to_string(recorderPort));
	if(!config["port_redirect"]) {
		throw std::runtime_error("Failed to load value `port_redirect`!");
	}
	redirectorPort = config["port_redirect"].as<int>();
	logger.info("Port for redirection: " + std::to_string(redirectorPort));
	if(!config["threads"]) {
		throw std::runtime_error("Failed to load value `threads`!");
	}
	nThreads = config["threads"].as<int>();
	logger.info("Number of threads (0 for number of logical cores): " + std::to_string(nThreads));
}
