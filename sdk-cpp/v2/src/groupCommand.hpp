#pragma once

#include <memory>
#include <vector>

#include "aios.h"

namespace Fourier {

/**
 * @brief A list of Command objects appropriate for sending to a Group of
 * modules; the size() must match the number of modules in the group.
 */
class GroupCommand final {
 public:
  /**
   * @brief Create a group command with the specified number of modules.
   */
  GroupCommand(size_t number_of_modules);

  /**
   * @brief Destructor cleans up group command object as necessary.
   */
  ~GroupCommand() noexcept;

  /**
   * @brief Returns the number of module commands in this group command.
   */
  size_t size() const;

  /**
   * @brief Access the command for an individual module.
   */
  FourierCommandPtr operator[](size_t index);

  /**
   * @brief Access the command for an individual module.
   */
  const FourierCommandPtr operator[](size_t index) const;

  /**
   * @brief Clears all data in this GroupCommand object; this returns to the
   * state the GroupCommand was at time of creation.
   */
  void clear();

  /**
   * @brief Convenience function for setting position commands from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setPosition(const std::vector<float> &position);

  /**
   * @brief Convenience function for setting TrapezoidalMove commands from
   * vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setTrapezoidalMove(const std::vector<float> &val);

  /**
   * @brief Convenience function for setting velocity commands from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setVelocity(const std::vector<float> &velocity);

  /**
   * @brief Convenience function for setting velocity ramp commands from
   * vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setVelocityRamp(const std::vector<float> &velocity);

  /**
   * @brief Convenience function for setting current commands from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setCurrent(const std::vector<float> &effort);

  /**
   * @brief Convenience function for setting enable commands from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   * enable is (nan continue， 0 disable， 1 enable),so choose float type.
   */
  void enable(const std::vector<float> &status);

  /**
   * @brief Convenience function for setting reboot commands from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void reboot(const std::vector<bool> &flag);

  /**
   * @brief Convenience function for setting getError commands from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void getError(const std::vector<bool> &flag);

  /**
   * @brief Convenience function for setting clearError commands from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void clearError(const std::vector<bool> &flag);

  /**
   * @brief Convenience function for setting resetLinearCount commands from
   * vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void resetLinearCount(const std::vector<float> &linearCount);

  /**
   * @brief Convenience function for setting setMotionCtrlConfig commands
   * from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setMotionCtrlConfig(const std::vector<MotionControllerConfig *> &config);

  /**
   * @brief Convenience function for setting setMotorConfig commands from
   * vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setMotorConfig(const std::vector<MotorConfig *> &config);

  /**
   * @brief Convenience function for setting setTrapTraj commands from
   * vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setTrapTraj(const std::vector<TrapTraj *> &config);

  /**
   * @brief Convenience function for setting saveConfig commands from vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void saveConfig(const std::vector<bool> &flag);

  /**
   * @brief Convenience function for setting setNetworkSetting commands from
   * vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void setNetworkSetting(const std::vector<NetworkSetting *> &config);

  /**
   * @brief Convenience function for setting ctrlBoxEnable commands from
   * vectors.
   *
   * Note that if the vector is not the correct link, no action is taken.
   */
  void ctrlBoxEnable(const std::vector<float> &status);

  // For latency test
  void setLatencyTest(const std::vector<bool> &flag);

  /******* fw2 .0 *******/
  void setInputPositionPt(const std::vector<PosPtInfo> &param);

  void setInputVelocityPt(const std::vector<float> &param);

  void setInputTorquePt(const std::vector<float> &param);
  /******* fw2 .0 *******/

 public:
  /**
   * C-style group command object.
   * NOTE: this should not be used except by library functions!
   */
  FourierGroupCommandPtr internal_;

  /**
   * The number of modules in this group command.
   */
  const size_t number_of_modules_;

 private:
  /**
   * The list of Command subobjects
   */
  std::vector<FourierCommandPtr> commands_;
};

}  // namespace Fourier
