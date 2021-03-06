#pragma once

#include <spdlog/logger.h>

class LoggerContainer {
 public:
  static spdlog::logger& Get(int loglevel = 3);
};
