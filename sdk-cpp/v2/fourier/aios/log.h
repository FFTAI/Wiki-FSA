#ifndef FOURIER_AIOS_LOG_H
#define FOURIER_AIOS_LOG_H

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

class LOG {
 public:
  // 配置日志文件、写入模式
  LOG();
  void setLevel(spdlog::level::level_enum level);

  ~LOG();
};

template <typename T, typename... Args>
constexpr auto DEBUG(T x, Args &&...args) {
  return spdlog::get("multi_sink")->debug(x, std::forward<Args>(args)...);
}
template <typename T, typename... Args>
constexpr auto INFO(T x, Args &&...args) {
  return spdlog::get("multi_sink")->info(x, std::forward<Args>(args)...);
}
template <typename T, typename... Args>
constexpr auto WARN(T x, Args &&...args) {
  return spdlog::get("multi_sink")->warn(x, std::forward<Args>(args)...);
}
template <typename T, typename... Args>
constexpr auto SPDERROR(T x, Args &&...args) {
  return spdlog::get("multi_sink")->error(x, std::forward<Args>(args)...);
}

#endif