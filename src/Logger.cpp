#include <Logger.h>

#include <proxygen/httpserver/HTTPServer.h>
#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <memory>
#include <spdlog/logger.h>

spdlog::logger SetUpLogging() {
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_level(spdlog::level::err);
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
			"log/balance.log", true);
	file_sink->set_level(spdlog::level::err);
	spdlog::logger logger("balancer", {console_sink, file_sink});
	logger.set_level(spdlog::level::err);
	logger.flush_on(spdlog::level::err);
	return logger;
}

spdlog::logger& LoggerContainer::Get() {
	static spdlog::logger logger = SetUpLogging();
	return logger;
}
