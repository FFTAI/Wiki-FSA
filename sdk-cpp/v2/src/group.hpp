#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "aios.h"

namespace Fourier {

class GroupFeedback;
class GroupCommand;

using GroupFeedbackHandler = std::function<void(const GroupFeedback &)>;

/**
 * @brief Represents a group of physical Fourier actuator, and allows Command,
 * Feedback, and Info objects to be sent to and recieved from the actuator.
 */
class Group final {
 public:
  /**
   * Creates a group from the underlying C-style group object. This should
   * only be called to create groups from the lookup class, not from user
   * code!
   */
  Group(FourierGroupPtr group, float initial_feedback_frequency = 0.0f,
        int32_t initial_command_lifetime = 0);

  /**
   * @brief Destructor cleans up group.
   */
  ~Group() noexcept;

  /**
   * @brief Returns the number of modules in the group
   */
  int size();

  /**
   * @brief Sets the command lifetime for the modules in this group.
   */
  bool setCommandLifetimeMs(int32_t ms);

  /**
   * @brief Send a command to the given group, requesting an acknowledgement
   * of transmission to be sent back.
   *
   * @param group_command The GroupCommand object containing information to be
   * sent to the group.
   *
   * @returns true if an acknowledgement was successfully received
   * (guaranteeing the group received this command), or a negative number for
   * an error otherwise.
   */
  bool sendCommand(const GroupCommand &group_command);

  /**
   * @brief Requests feedback from the group.
   *
   * Sends a background request to the modules in the group; if/when all
   * modules return feedback, any associated handler functions are called.
   * This returned feedback is also stored to be returned by the next call to
   * getNextFeedback (any previously returned data is discarded).
   *
   * @returns @c true if feedback was request was successfully sent, otherwise
   *  @c false on failure (i.e., connection error).
   */
  bool sendFeedbackRequest(
      FourierFeedbackCode feedbackCode = FourierFeedbackAll);

  /**
   * @brief Returns the most recently stored feedback from a sent feedback
   * request, or returns the next one received (up to the requested timeout).
   *
   * Note that a feedback request can be sent either with the
   * sendFeedbackRequest function.
   *
   * Warning: other data in the provided 'Feedback' object is erased!
   *
   * @param feedback On success, the group feedback read from the group are
   * written into this structure.
   *
   * @returns @c true if feedback was returned, otherwise @c false on failure
   * (i.e., connection error or timeout waiting for response).
   */
  bool getNextFeedback(GroupFeedback &feedback,
                       int32_t timeout_ms = DEFAULT_TIMEOUT_MS);

  /**
   * TODO:
   * @brief Sets the frequency of the internal feedback request + callback
   * thread.
   *
   * @returns @c true if the frequency successfully was set, or @c false
   * if the parameter was outside the accepted range (less than zero or faster
   * than supported maximum).
   */
  bool setFeedbackFrequencyHz(float frequency);

  /**
   * TODO:
   */
  float getFeedbackFrequencyHz();

  /**
   * TODO:
   */
  void addFeedbackHandler(GroupFeedbackHandler handler);

  /**
   * TODO:
   */
  void clearFeedbackHandlers();

  /**
   * @brief Gets the actuator error message
   */
  FourierFeedbackErrorPtr getError(int idx);

 private:
  /**
   * C-style group object
   */
  FourierGroupPtr internal_;

  /**
   * The number of modules in this group.
   */
  const int number_of_modules_;

  /**
   * Protects access to the group feedback handler vector.
   */
  std::mutex handler_lock_;

  /**
   * TODO:
   */
  std::vector<GroupFeedbackHandler> handlers_;

  /**
   * TODO:
   */
  friend void callbackWrapper(FourierGroupFeedbackPtr group_feedback,
                              void *user_data);

  /**
   * TODO:
   */
  void callAttachedHandlers(FourierGroupFeedbackPtr group_feedback);

 public:
  static const int32_t DEFAULT_TIMEOUT_MS = 500;
};

}  // namespace Fourier
