#include "groupCommand.hpp"

namespace Fourier {

GroupCommand::GroupCommand(size_t number_of_modules)
    : internal_(fourierGroupCommandCreate(number_of_modules)),
      number_of_modules_(number_of_modules) {
  for (size_t i = 0; i < number_of_modules_; i++)
    commands_.emplace_back(fourierGroupCommandGetModuleCommand(internal_, i));
}

GroupCommand::~GroupCommand() noexcept {
  if (internal_ != nullptr) fourierGroupCommandRelease(internal_);
}

size_t GroupCommand::size() const { return number_of_modules_; }

FourierCommandPtr GroupCommand::operator[](size_t index) {
  return commands_[index];
}

const FourierCommandPtr GroupCommand::operator[](size_t index) const {
  return commands_[index];
}

void GroupCommand::clear() { fourierGroupCommandClear(internal_); }

void GroupCommand::setPosition(const std::vector<float> &position) {
  if (position.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandPosition);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = position[i];
  }
}

void GroupCommand::setTrapezoidalMove(const std::vector<float> &val) {
  if (val.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandTrapezoidal);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = val[i];
  }
}
void GroupCommand::setVelocity(const std::vector<float> &velocity) {
  if (velocity.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandVelocity);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = velocity[i];
  }
}
void GroupCommand::setVelocityRamp(const std::vector<float> &velocity) {
  if (velocity.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandVelocityRamp);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = velocity[i];
  }
}
void GroupCommand::setCurrent(const std::vector<float> &current) {
  if (current.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandCurrent);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = current[i];
  }
}

void GroupCommand::enable(const std::vector<float> &status) {
  if (status.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandEnable);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = status[i];
  }
}

void GroupCommand::reboot(const std::vector<bool> &flag) {
  if (flag.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandReboot);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->bool_fields_ = flag[i];
  }
}

void GroupCommand::getError(const std::vector<bool> &flag) {
  if (flag.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandGetError);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->bool_fields_ = flag[i];
  }
}

void GroupCommand::clearError(const std::vector<bool> &flag) {
  if (flag.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandClearError);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->bool_fields_ = flag[i];
  }
}

void GroupCommand::resetLinearCount(const std::vector<float> &linearCount) {
  if (linearCount.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandResetLinearCount);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = linearCount[i];
  }
}

void GroupCommand::setMotionCtrlConfig(
    const std::vector<MotionControllerConfig *> &config) {
  if (config.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandMotionControllerConfig);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->motion_ctrl_config = config[i];
  }
}

void GroupCommand::setMotorConfig(const std::vector<MotorConfig *> &config) {
  if (config.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandMotorConfig);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->motor_config = config[i];
  }
}

void GroupCommand::setTrapTraj(const std::vector<TrapTraj *> &config) {
  if (config.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandTrapTraj);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->trap_traj = config[i];
  }
}

void GroupCommand::saveConfig(const std::vector<bool> &flag) {
  if (flag.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandSaveConfig);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->bool_fields_ = flag[i];
  }
}

void GroupCommand::setNetworkSetting(
    const std::vector<NetworkSetting *> &config) {
  if (config.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandNetworkSetting);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->network_setting = config[i];
  }
}

// Ctrl Box
void GroupCommand::ctrlBoxEnable(const std::vector<float> &status) {
  if (status.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandCtrlBoxPower);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = status[i];
  }
}

void GroupCommand::setLatencyTest(const std::vector<bool> &flag) {
  if (flag.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandLatencyTest);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->bool_fields_ = flag[i];
  }
}

void GroupCommand::setInputPositionPt(const std::vector<PosPtInfo> &param) {
  if (param.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandPositionPt);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->pos_pt_info_.pos = param[i].pos;
    commands_[i]->pos_pt_info_.vel = param[i].vel;
    commands_[i]->pos_pt_info_.torque = param[i].torque;
  }
}

void GroupCommand::setInputVelocityPt(const std::vector<float> &param) {
  if (param.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandVelocityPt);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = param[i];
  }
}

void GroupCommand::setInputTorquePt(const std::vector<float> &param) {
  if (param.size() != number_of_modules_) return;
  fourierGroupCommandSetType(internal_, FourierCommandTorquePt);
  for (size_t i = 0; i < number_of_modules_; ++i) {
    commands_[i]->float_fields_ = param[i];
  }
}

}  // namespace Fourier
