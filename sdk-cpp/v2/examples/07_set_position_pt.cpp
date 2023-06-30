
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "groupCommand.hpp"
#include "groupFeedback.hpp"
#include "lookup.hpp"

using namespace Fourier;

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

int main() {
  std::string str("10.10.10.255");
  // After construction,start the background thread lookup actuator
  Lookup lookup(&str);
  // Wait 1 seconds for the module list to populate, and then print out its
  // contents
  std::this_thread::sleep_for(std::chrono::seconds(2));
  lookup.setLookupFrequencyHz(0);

  auto group = lookup.getGroupFromFamily("Default");

  if (!group) {
    std::cout << "Group not found! Check that the family and name of a module "
                 "on the network"
              << std::endl
              << "matches what is given in the source file." << std::endl;
    return -1;
  }

  std::cout << group->size() << std::endl;
  GroupCommand group_command(group->size());

  GroupFeedback group_feedback(group->size());

  group->sendFeedbackRequest();
  std::this_thread::sleep_for(std::chrono::seconds(3));

  std::vector<float> enable_status(group->size(),
                                   std::numeric_limits<float>::quiet_NaN());
  for (int i = 0; i < group->size(); ++i) {
    enable_status[i] = 1;
  }
  group_command.enable(enable_status);
  group->sendCommand(group_command);
  std::this_thread::sleep_for(std::chrono::seconds(2));

  std::vector<PosPtInfo> pos_pt_infos;
  PosPtInfo pos;
  int timeout_num = 0;

  for (int i = 0; i < 10000; ++i) {
    StartTimeChrono(all);
    auto t = intervalTimeStart();
    // StartTimeChrono(feedback);
    auto ret = group->getNextFeedback(group_feedback, 10);
    // EndTimeChrono(feedback);
    if (ret) {
      for (int i = 0; i < group_feedback.size(); ++i) {
        if (group_feedback[i]->position ==
            std::numeric_limits<float>::quiet_NaN()) {
          timeout_num++;
          if (timeout_num > 3) return 0;
        }
        std::cout << group_feedback[i]->position << " ";
        pos.pos = group_feedback[i]->position + 0.1;
        pos.vel = 0.0;
        pos.torque = 0.0;
        pos_pt_infos.push_back(pos);
      }
      std::cout << std::endl;
      StartTimeChrono(command);
      group_command.setInputPositionPt(pos_pt_infos);
      group->sendCommand(group_command);
      EndTimeChrono(command);
      pos_pt_infos.clear();
    }
    intervalTimeEnd(t, 400);
    EndTimeChrono(all);
  }

  for (int i = 0; i < group->size(); ++i) {
    enable_status[i] = 0;
  }
  group_command.enable(enable_status);
  group->sendCommand(group_command);
  std::cout << "end!!!" << std::endl;

  return 0;
}
