#include <Logger.h>

#include <proxygen/httpserver/HTTPServer.h>
#include "spdlog/sinks/ansicolor_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <memory>
#include <spdlog/logger.h>

spdlog::logger SetUpLogging(spdlog::level::level_enum loglevel) {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(loglevel);
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "log/balance.log", true);
  file_sink->set_level(loglevel);
  spdlog::logger logger("balancer", {console_sink, file_sink});
  logger.set_level(loglevel);
  logger.flush_on(loglevel);
  return logger;
}

spdlog::logger& LoggerContainer::Get(int loglevel) {
  static spdlog::logger logger = SetUpLogging(static_cast<spdlog::level::level_enum>(loglevel));
  return logger;
}
