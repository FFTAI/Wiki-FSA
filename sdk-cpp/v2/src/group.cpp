#include "group.hpp"

#include "groupCommand.hpp"
#include "groupFeedback.hpp"

namespace Fourier {

void callbackWrapper(FourierGroupFeedbackPtr group_feedback, void *user_data) {
  reinterpret_cast<Group *>(user_data)->callAttachedHandlers(group_feedback);
}

void Group::callAttachedHandlers(FourierGroupFeedbackPtr group_feedback) {
  // Wrap this:
  GroupFeedback wrapped_fbk(group_feedback);
  // Call handlers:
  std::lock_guard<std::mutex> lock_guard(handler_lock_);
  for (unsigned int i = 0; i < handlers_.size(); i++) {
    GroupFeedbackHandler handler = handlers_[i];
    try {
      handler(wrapped_fbk);
    } catch (...) {
    }
  }
}

Group::Group(FourierGroupPtr group, float initial_feedback_frequency,
             int32_t initial_command_lifetime)
    : internal_(group), number_of_modules_(fourierGroupGetSize(internal_)) {
  if (initial_feedback_frequency != 0)
    fourierGroupSetFeedbackFrequencyHz(internal_, initial_feedback_frequency);
  if (initial_command_lifetime != 0)
    fourierGroupSetCommandLifetime(internal_, initial_command_lifetime);
}

Group::~Group() noexcept {
  if (internal_ != nullptr) {
    fourierGroupRelease(internal_);
  }
}

int Group::size() { return number_of_modules_; }

bool Group::setCommandLifetimeMs(int32_t ms) {
  return (fourierGroupSetCommandLifetime(internal_, ms) ==
          FourierStatusSuccess);
}

bool Group::sendCommand(const GroupCommand &group_command) {
  return (fourierGroupSendCommand(internal_, group_command.internal_) ==
          FourierStatusSuccess);
}

bool Group::sendFeedbackRequest(FourierFeedbackCode feedbackCode) {
  return (fourierGroupSendFeedbackRequest(internal_, feedbackCode) ==
          FourierStatusSuccess);
}

bool Group::getNextFeedback(GroupFeedback &feedback, int32_t timeout_ms) {
  return (fourierGroupGetNextFeedback(internal_, feedback.internal_,
                                      timeout_ms) == FourierStatusSuccess);
}

bool Group::setFeedbackFrequencyHz(float frequency) {
  return (fourierGroupSetFeedbackFrequencyHz(internal_, frequency) ==
          FourierStatusSuccess);
}

float Group::getFeedbackFrequencyHz() {
  return fourierGroupGetFeedbackFrequencyHz(internal_);
}

void Group::addFeedbackHandler(GroupFeedbackHandler handler) {
  std::lock_guard<std::mutex> lock_guard(handler_lock_);
  handlers_.push_back(handler);
  if (handlers_.size() == 1)  // (i.e., this was the first one)
    fourierGroupRegisterFeedbackHandler(internal_, callbackWrapper,
                                        reinterpret_cast<void *>(this));
}

void Group::clearFeedbackHandlers() {
  std::lock_guard<std::mutex> lock_guard(handler_lock_);
  fourierGroupClearFeedbackHandlers(internal_);
  handlers_.clear();
}

FourierFeedbackErrorPtr Group::getError(int idx) {
  return fourierGroupFeedbackError(internal_, idx);
}
}  // namespace Fourier
