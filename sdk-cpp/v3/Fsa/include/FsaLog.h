#ifndef FSA_LOG_H
#define FSA_LOG_H

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace FSA_CONNECT {
namespace LOG {

class LOG {
  public:
    LOG();
    void setLevel(spdlog::level::level_enum level);
    ~LOG();
};

template <typename T, typename... Args>
constexpr auto DEBUG(T x, Args &&...args) {
    return spdlog::get("fsa_cpp_sdk")->debug(x, std::forward<Args>(args)...);
}
template <typename T, typename... Args>
constexpr auto INFO(T x, Args &&...args) {
    return spdlog::get("fsa_cpp_sdk")->info(x, std::forward<Args>(args)...);
}
template <typename T, typename... Args>
constexpr auto WARN(T x, Args &&...args) {
    return spdlog::get("fsa_cpp_sdk")->warn(x, std::forward<Args>(args)...);
}
template <typename T, typename... Args>
constexpr auto SPDERROR(T x, Args &&...args) {
    return spdlog::get("fsa_cpp_sdk")->error(x, std::forward<Args>(args)...);
}

#endif
}
}
