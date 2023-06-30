
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "groupCommand.hpp"
#include "groupFeedback.hpp"
#include "lookup.hpp"

using namespace Fourier;

int main() {
  std::string str("10.10.10.255");
  // After construction,start the background thread lookup actuator
  Lookup lookup(&str);
  // Wait 1 seconds for the module list to populate, and then print out its
  // contents
  std::this_thread::sleep_for(std::chrono::seconds(1));
  lookup.setLookupFrequencyHz(0);

  auto group = lookup.getGroupFromFamily("Default");

  if (!group) {
    std::cout << "Group not found! Check that the family and name of a module "
                 "on the network"
              << std::endl
              << "matches what is given in the source file." << std::endl;
    return -1;
  }

  //// Open-loop controller (position)

  // The command struct has fields for various commands and settings; for the
  // actuator, we will primarily use position, velocity, and current.
  //
  // Fields that are not filled in will be ignored when sending.
  GroupCommand group_command(group->size());
  // GroupCommand uses Eigen types for data interchange
  std::vector<float> positions = {0, 0};
  std::vector<float> velocitys = {50000};
  std::vector<float> currents = {1, 1};
  std::vector<float> currents2 = {5, 5};
  // Allocate feedback
  GroupFeedback group_feedback(group->size());

  std::vector<float> enable_status(group->size(),
                                   std::numeric_limits<float>::quiet_NaN());
  for (int i = 0; i < group->size(); ++i) {
    enable_status[i] = 1;
  }
  group_command.enable(enable_status);
  group->sendCommand(group_command);

  //   for (int i = 0; i < 2000; i++) {
  //     // Even though we don't use the feedback, getting feedback conveniently
  //     // limits the loop rate to the feedback frequency
  //     // group->getNextFeedback(group_feedback);

  //     // Update position set point
  //     positions[0] = sin(i * 0.004 * M_PI) * 2000;
  //     positions[1] = sin(i * 0.004 * M_PI) * 2000;
  //     group_command.setPosition(positions);
  //     std::cout << positions[0] << std::endl;
  //     std::cout << positions[1] << std::endl;

  //     group->sendCommand(group_command);
  //     std::this_thread::sleep_for(std::chrono::milliseconds(10));
  //   }

  std::vector<float> zero = {0};
  // group_command.setVelocityRamp(zero);
  // group->sendCommand(group_command);

  group_command.setInputVelocityPt(velocitys);
  // StartTimeChrono(setVelocity);
  group->sendCommand(group_command);
  // EndTimeChrono(setVelocity);
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  group_command.setInputVelocityPt(zero);
  group->sendCommand(group_command);

  // group_command.setCurrent(currents);
  // group->sendCommand(group_command);
  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  // group_command.setCurrent(currents2);
  // group->sendCommand(group_command);
  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  // group_command.setCurrent(zero);
  // group->sendCommand(group_command);

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  for (int i = 0; i < group->size(); ++i) {
    enable_status[i] = 0;
  }
  group_command.enable(enable_status);
  group->sendCommand(group_command);

  return 0;
}
