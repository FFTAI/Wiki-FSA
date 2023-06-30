
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "groupCommand.hpp"
#include "groupFeedback.hpp"
#include "lookup.hpp"

// for test
#ifdef WIN32
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define DllExport __declspec(dllexport)
#else
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <time.h>
const timeval intervalTimeStart() {
  timeval tv_last;
  gettimeofday(&tv_last, NULL);
  return tv_last;
}

double intervalTimeEnd(const timeval &tv_last, const float &frequency) {
  float interval_time;
  if (frequency == 0) {
    interval_time = 0;
  } else {
    interval_time = 1.0 / frequency;
  }
  long double delay_time = interval_time * 1000000;
  long double time = 0;
  timeval now_time;
  while (true) {
    gettimeofday(&now_time, NULL);
    time = ((now_time.tv_sec - tv_last.tv_sec) * 1000000 +
            (now_time.tv_usec - tv_last.tv_usec));
    if (time >= delay_time) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
  return time / 1000;
}

#define StartTimeChrono(funName)                                \
  std::chrono::microseconds ms##funName =                       \
      std::chrono::duration_cast<std::chrono::microseconds>(    \
          std::chrono::system_clock::now().time_since_epoch()); \
  long start##funName = ms##funName.count();

#define EndTimeChrono(funName)                                                \
  ms##funName = std::chrono::duration_cast<std::chrono::microseconds>(        \
      std::chrono::system_clock::now().time_since_epoch());                   \
  long end##funName = ms##funName.count();                                    \
  double ms_time = (end##funName - start##funName) / 1000.0;                  \
  std::cout << "The function " << #funName << " runs for " << ms_time << "ms" \
            << std::endl;
// for test

int main(int argc, char *argv[]) {
  // Try and get the requested group.
  std::shared_ptr<Fourier::Group> group;
  {
    std::string str("10.10.10.255");
    Fourier::Lookup lookup(&str);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    lookup.setLookupFrequencyHz(0);
    group = lookup.getGroupFromFamily("Default");
    if (!group) {
      std::cout << "No group found!" << std::endl;
      return -1;
    }
  }

  fourierSetLogLevel("ERROR");

  Fourier::GroupFeedback feedback(group->size());

  Fourier::GroupCommand group_command(group->size());

  std::vector<float> count(group->size(),
                           std::numeric_limits<float>::quiet_NaN());
  for (int i = 0; i < group->size(); ++i) {
    count[i] = 0;
  }
  group_command.resetLinearCount(count);
  group->sendCommand(group_command);

  std::vector<float> enable_status(group->size(),
                                   std::numeric_limits<float>::quiet_NaN());
  for (int i = 0; i < group->size(); ++i) {
    enable_status[i] = 1;
  }
  group_command.enable(enable_status);
  group->sendCommand(group_command);
  // std::this_thread::sleep_for(std::chrono::milliseconds(20));

  auto start = std::chrono::system_clock::now();
  std::chrono::duration<double> t(std::chrono::system_clock::now() - start);
  double duration = 10000;
  std::vector<float> v_pos;
  std::vector<PosPtInfo> pos_pt_infos;
  while (t.count() < duration) {
    t = std::chrono::system_clock::now() - start;
    // StartTimeChrono(all);

    // StartTimeChrono(getNextFeedback);
    group->getNextFeedback(feedback, 2);
    // EndTimeChrono(getNextFeedback);
    for (size_t mod_idx = 0; mod_idx < feedback.size(); ++mod_idx) {
      if (feedback[mod_idx]->position !=
          std::numeric_limits<float>::quiet_NaN()) {
        PosPtInfo info = {0};
        info.pos = feedback[mod_idx]->position + .1;
        pos_pt_infos.push_back(info);
        std::cout << "pos:" << feedback[mod_idx]->position << "  "
                  << "vel:" << feedback[mod_idx]->velocity << "  "
                  << "cur:" << feedback[mod_idx]->current << "  ";
      }
      std::cout << std::endl;
    }

    StartTimeChrono(sendCommand);
    group_command.setInputPositionPt(pos_pt_infos);
    group->sendCommand(group_command);
    EndTimeChrono(sendCommand);

    //  std::this_thread::sleep_for(std::chrono::milliseconds(10));
    pos_pt_infos.clear();
    // EndTimeChrono(all);
  }

  for (int i = 0; i < group->size(); ++i) {
    enable_status[i] = 0;
  }
  group_command.enable(enable_status);
  group->sendCommand(group_command);

  return 0;
}
