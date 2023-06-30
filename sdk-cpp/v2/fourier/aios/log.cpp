#include "log.h"

#ifdef WIN32
#include "../src/utils/utils_common_win.hpp"
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

LOG::LOG() {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::debug);
  console_sink->set_pattern("[%Y-%m-%d %T.%e] [%n] [%L] [t %t] %v");
  // console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

  // 日志实例名、日志文件最大大小、滚动文件数量（日志太多的时候，当前文件重命名_1,_2,_3.再写新的文件）
#ifdef WIN32
  std::wstring program_data_path = utils::GetAllUsersCacheDirectory();
  std::wstring log_path =
      program_data_path + L"\\FourierDynamics\\FourierDynamicsLog.txt";
#elif __APPLE__
  std::string log_path = "log/FourierDynamicsLog.txt";
#else
  std::string log_path = "/tmp/log/FourierDynamics/";
  if (access(log_path.c_str(), 0) == -1) {  // not exit
    mkdir(log_path.c_str(), 0777);
  }
  log_path += "FourierDynamicsLog.txt";
#endif  //  WIN32

  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(console_sink);
#ifndef __APPLE__
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      log_path.c_str(), 4 * 1024 * 1024, 7);
  //格式设置：[年月日 时分秒毫秒] [logger实例名] [日志等级缩写] [线程ID]
  //日志正文
  file_sink->set_pattern("[%Y-%m-%d %T.%e] [%n] [%L] [thread %t] %v");
  file_sink->set_level(spdlog::level::debug);
  sinks.push_back(file_sink);
#endif
  auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(),
                                                 sinks.end());

  logger->set_level(spdlog::level::debug);

  //遇到info级别，立马将缓存的buffer写到文件中
  spdlog::flush_on(spdlog::level::info);
  // logger->debug("test debug");
  // logger->info("test info");
  // logger->error("test error");
  spdlog::register_logger(logger);
}

void LOG::setLevel(spdlog::level::level_enum level) {
  spdlog::get("multi_sink")->set_level(level);
}

LOG::~LOG() { spdlog::get("multi_sink")->flush(); }
