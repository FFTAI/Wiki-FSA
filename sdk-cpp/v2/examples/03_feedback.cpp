
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

#include "groupCommand.hpp"
#include "groupFeedback.hpp"
#include "lookup.hpp"

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

  int num_modules = group->size();

  // Create a group feedback object; this will be filled in during the request.
  Fourier::GroupFeedback feedback(num_modules);
  Fourier::GroupCommand group_command(group->size());

  // In a loop, send requests for feedback to the group and wait for responses.
  long timeout_ms = 1000;
  float period_s = 0.25f;

  group_command.resetLinearCount({0});
  group->sendCommand(group_command);
  group->setFeedbackFrequencyHz(500);
  for (int i = 0; i < 20; i++) {
    // group->sendFeedbackRequest(FourierFeedbackCVP);
    // group->sendFeedbackRequest();
    if (group->getNextFeedback(feedback, timeout_ms)) {
      for (size_t mod_idx = 0; mod_idx < feedback.size(); ++mod_idx) {
        if (feedback[mod_idx]->position !=
            std::numeric_limits<float>::quiet_NaN()) {
          std::cout << "i:" << i << "  " << feedback[mod_idx]->position << "  "
                    << feedback[mod_idx]->velocity << "  "
                    << feedback[mod_idx]->current << "  ";
        }
        std::cout << std::endl;
      }
    } else {
      std::cout << "Received no feedback from group!" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
  }

  group->setFeedbackFrequencyHz(0);
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  group->setFeedbackFrequencyHz(100);
  std::this_thread::sleep_for(std::chrono::milliseconds(30));

  // group->setFeedbackFrequencyHz(0);
  // NOTE: destructors automatically clean up remaining objects
  return 0;
}
