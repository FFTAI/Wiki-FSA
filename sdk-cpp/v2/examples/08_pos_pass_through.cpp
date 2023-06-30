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
  auto entry_list = lookup.getEntryList();
  for (const auto& entry : *entry_list) {
    std::cout << "Name: " << entry.name_ << std::endl;
    std::cout << "Family: " << entry.family_ << std::endl;
  }

  GroupCommand group_command(group->size());

  // Allocate feedback
  GroupFeedback group_feedback(group->size());

  std::vector<float> enable_status(group->size(),
                                   std::numeric_limits<float>::quiet_NaN());
  for (int i = 0; i < group->size(); ++i) {
    enable_status[i] = 1;
  }
  group_command.enable(enable_status);
  group->sendCommand(group_command);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::vector<PosPtInfo> pos_pt_infos;
  PosPtInfo pos;
  pos.pos = 0;
  pos.vel = 0;
  pos.torque = 1.0;

  for (int i = 0; i < group->size(); ++i) {
    pos_pt_infos.push_back(pos);
  }

  group_command.setInputPositionPt(pos_pt_infos);
  group->sendCommand(group_command);

  Fourier::GroupFeedback feedback(group->size());
  while (true) {
    group->getNextFeedback(feedback, 2);
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
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}