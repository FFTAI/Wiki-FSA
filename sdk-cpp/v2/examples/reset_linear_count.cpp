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
  Fourier::GroupFeedback feedback(group->size());

  for (int i = 0; i < 10; ++i) {
    group->sendFeedbackRequest(FourierFeedbackCVP);
    group->getNextFeedback(feedback, 2);
    for (size_t mod_idx = 0; mod_idx < feedback.size(); ++mod_idx) {
      if (feedback[mod_idx]->position !=
          std::numeric_limits<float>::quiet_NaN()) {
        std::cout << "pos:" << feedback[mod_idx]->position << "  "
                  << "vel:" << feedback[mod_idx]->velocity << "  "
                  << "cur:" << feedback[mod_idx]->current << "  ";
      } else {
        std::cout << "feedback error!" << std::endl;
      }
      std::cout << std::endl;
    }
  }

  GroupCommand group_command(group->size());
  group_command.enable(std::vector<float>(group->size(), 1));
  group->sendCommand(group_command);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::vector<PosPtInfo> pos_pt_infos;
  PosPtInfo pos;
  for (int c = 0; c < 400; ++c) {
    pos.pos = c * 0.005;
    for (int i = 0; i < 2; ++i) {
      pos_pt_infos.push_back(pos);
    }
    group_command.setInputPositionPt(pos_pt_infos);
    group->sendCommand(group_command);
    pos_pt_infos.clear();
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));

  for (int i = 0; i < 10; ++i) {
    group->sendFeedbackRequest(FourierFeedbackCVP);
    group->getNextFeedback(feedback, 2);
    for (size_t mod_idx = 0; mod_idx < feedback.size(); ++mod_idx) {
      if (feedback[mod_idx]->position !=
          std::numeric_limits<float>::quiet_NaN()) {
        std::cout << "pos:" << feedback[mod_idx]->position << "  "
                  << "vel:" << feedback[mod_idx]->velocity << "  "
                  << "cur:" << feedback[mod_idx]->current << "  ";
      } else {
        std::cout << "feedback error!" << std::endl;
      }
      std::cout << std::endl;
    }
  }

  /******* Make sure FSA disable when exec resetLinearCount *******/
  group_command.enable(std::vector<float>(group->size(), 0));
  group->sendCommand(group_command);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  group_command.resetLinearCount(std::vector<float>(group->size(), 0));
  group->sendCommand(group_command);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  for (int i = 0; i < 10; ++i) {
    group->sendFeedbackRequest(FourierFeedbackCVP);
    group->getNextFeedback(feedback, 2);
    for (size_t mod_idx = 0; mod_idx < feedback.size(); ++mod_idx) {
      if (feedback[mod_idx]->position !=
          std::numeric_limits<float>::quiet_NaN()) {
        std::cout << "pos:" << feedback[mod_idx]->position << "  "
                  << "vel:" << feedback[mod_idx]->velocity << "  "
                  << "cur:" << feedback[mod_idx]->current << "  ";
      } else {
        std::cout << "feedback error!" << std::endl;
      }
      std::cout << std::endl;
    }
  }

  return 0;
}