#include <vector>

#include "actuator.h"
#include "log.h"

FourierGroupCommandPtr fourierGroupCommandCreate(size_t size) {
  FourierGroupCommandPtr ptr = new FourierGroupCommand_();
  for (int i = 0; i < size; i++) {
    auto tmp = new Command_();
    tmp->float_fields_ = std::numeric_limits<float>::quiet_NaN();
    tmp->bool_fields_ = false;
    tmp->motion_ctrl_config = nullptr;
    tmp->motor_config = nullptr;
    tmp->trap_traj = nullptr;
    tmp->network_setting = nullptr;
    tmp->commandEnum = UNKNOW;
    ptr->command.push_back(tmp);
  }

  return ptr;
}

size_t fourierGroupCommandGetSize(FourierGroupCommandPtr command) {
  return command->command.size();
}

FourierCommandPtr fourierGroupCommandGetModuleCommand(
    FourierGroupCommandPtr command, size_t module_index) {
  return command->command.at(module_index);
}

bool fourierGroupCommandSetType(FourierGroupCommandPtr command,
                                FourierCommandEnum command_enum) {
  std::lock_guard<std::mutex> lock_guardG(command->cmd_enum_mutex);
  for (auto& cmd : command->command) {
    cmd->commandEnum = command_enum;
  }
  return true;
}

void fourierGroupCommandClear(FourierGroupCommandPtr command) {
  for (auto elem : command->command) {
    elem->float_fields_ = std::numeric_limits<float>::quiet_NaN();
    elem->bool_fields_ = false;
    elem->commandEnum = UNKNOW;
  }
}

void fourierGroupCommandRelease(FourierGroupCommandPtr command) {
  // wait for the last command to complete
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::lock_guard<std::mutex> lock_guard_cmd(command->cmd_enum_mutex);
  for (auto elem : command->command) {
    delete elem;
  }
  delete command;
  DEBUG("Group Command Release");
}
