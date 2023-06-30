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
  lookup.setLookupFrequencyHz(0);  // set lookup stop

  std::shared_ptr<Group> group = lookup.getGroupFromFamily("Default");
  std::cout << std::endl << "group size: " << group->size() << std::endl;
  GroupCommand group_command(group->size());

  group_command.enable(std::vector<float>(group->size(), 1));
  group->sendCommand(group_command);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  for (int i = 0; i < 10; ++i) {
    group_command.setVelocity(std::vector<float>(group->size(), i));
    group->sendCommand(group_command);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));

  group_command.setVelocity(std::vector<float>(group->size(), 0));
  group->sendCommand(group_command);

  group_command.enable(std::vector<float>(group->size(), 0));
  group->sendCommand(group_command);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  return 0;
}