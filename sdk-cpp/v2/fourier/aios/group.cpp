#include "actuator.h"
#include "log.h"

FourierGroupPtr fourierGroupCreateFromFamily(FourierLookupPtr lookup,
                                             const char *family,
                                             int32_t timeout_ms) {
  FourierGroupPtr groupPtr = new FourierGroup_();
  auto entryList = fourierCreateLookupEntryList(lookup);

  for (const auto &elem : entryList->entryInfo) {
    if (elem.family == std::string(family)) {
      groupPtr->entryList.push_back(elem);
    }
  }

  for (const auto &elem : groupPtr->entryList) {
    std::unique_ptr<Fourier::Aios> tmp =
        std::make_unique<Fourier::Aios>(elem.ip);
    tmp->constructionSocket(timeout_ms);
    DEBUG("fw_version:" + elem.fw_version);
    if (elem.fw_version[0] >= '2') {
      tmp->fw_version_ = Fourier::FwVersion::FW_2;
    } else {
      tmp->fw_version_ = Fourier::FwVersion::FW_1;
    }

    groupPtr->aiosGroup.push_back(std::move(tmp));
    groupPtr->packedLossNum.push_back(0);
    groupPtr->actuatorOnline.push_back(true);

    auto fb = new Feedback();
    fb->position = std::numeric_limits<float>::quiet_NaN();
    groupPtr->g_preFb.feedback.push_back(fb);

    groupPtr->g_fb.feedback.push_back(new Feedback());

    groupPtr->feedbackError.push_back(new FeedbackError_());
  }
  groupPtr->g_fb.sendFeedbackRequestConfigFlag = true;
  groupPtr->g_fb.getNextFeedbackConfigFlag = true;
  groupPtr->maxPackedLossNum = 2;

  groupPtr->aiosStatus = AXIS_STATE_IDLE;
  groupPtr->preMode = UNKNOW;

  std::atomic_init(&groupPtr->exit_signal, false);

  lookup->group = groupPtr;
  groupPtr->lookup = lookup;
  return groupPtr;
}

void fourierGroupRelease(FourierGroupPtr group) {
  std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);

  // stop feedback thread
  group->feedbackFrequencyHz = 0;

  // exit command thread
  group->exit_signal.store(true);
  group->cmd_cv.notify_one();

  for (auto elem : group->g_preFb.feedback) {
    delete elem;
  }
  for (auto elem : group->g_fb.feedback) {
    delete elem;
  }
  for (auto elem : group->feedbackError) {
    delete elem;
  }

  if (group->cmd_thread && group->cmd_thread->joinable())
    group->cmd_thread->join();

  if (group->fb_thread && group->fb_thread->joinable()) {
    group->fb_thread->join();
  }

  delete group;
}

size_t fourierGroupGetSize(FourierGroupPtr group) {
  std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);
  return group->aiosGroup.size();
}

void handleCmdTrapezoidal(FourierGroupPtr group, FourierGroupCommandPtr command,
                          bool *send_flag, bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("Position: " + std::to_string(val));
    if (!std::isnan(val))
      if (group->aiosGroup[i]->trapezoidalMove(val, true) !=
          FourierStatusSuccess) {
        *send_flag = false;
      }
  }

  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    if (!std::isnan(val))
      if (group->aiosGroup[i]->recvRtData(nullptr) != FourierStatusSuccess) {
        *recv_flag = false;
        SPDERROR("ip:" + group->aiosGroup[i]->getIp() +
                 " idx: " + std::to_string(i) +
                 " Error receiving trapezoidal move buffer.");
      }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdPosition(FourierGroupPtr group, FourierGroupCommandPtr command,
                       bool *send_flag, bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("Position: " + std::to_string(val));
    if (!std::isnan(val))
      if (group->aiosGroup[i]->setPosition(val, 0, 0, true) !=
          FourierStatusSuccess)
        *send_flag = false;
  }

  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    if (!std::isnan(val))
      if (group->aiosGroup[i]->recvRtData(nullptr) != FourierStatusSuccess) {
        SPDERROR("Error receiving position buffer.");
        *recv_flag = false;
      }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdVelocity(FourierGroupPtr group, FourierGroupCommandPtr command,
                       bool *send_flag, bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("Velocity: " + std::to_string(val));
    if (!std::isnan(val)) {
      group->curVelocity = val;
      if (group->aiosGroup[i]->setVelocity(val, 0, true) !=
          FourierStatusSuccess)
        *send_flag = false;
    }
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    if (!std::isnan(val))
      if (group->aiosGroup[i]->recvRtData(nullptr) != FourierStatusSuccess) {
        SPDERROR("Error receiving setVelocity buffer.");
        *recv_flag = false;
      }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdVelocityRamp(FourierGroupPtr group,
                           FourierGroupCommandPtr command, bool *send_flag,
                           bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("Velocity Ramp: " + std::to_string(val));
    if (!std::isnan(val)) {
      group->curVelocity = val;
      if (group->aiosGroup[i]->velRampTarget(val) != FourierStatusSuccess)
        *send_flag = false;
    }
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    if (!std::isnan(val))
      if (group->aiosGroup[i]->recvRtData(nullptr) != FourierStatusSuccess) {
        SPDERROR("Error receiving velRampTarget buffer.");
        *recv_flag = false;
      }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdCurrent(FourierGroupPtr group, FourierGroupCommandPtr command,
                      bool *send_flag, bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("Current: " + std::to_string(val));
    if (!std::isnan(val))
      if (group->aiosGroup[i]->setCurrent(val, true) != FourierStatusSuccess)
        *send_flag = false;
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    if (!std::isnan(val))
      if (group->aiosGroup[i]->recvRtData(nullptr) != FourierStatusSuccess) {
        SPDERROR("Error receiving current buffer.");
        *recv_flag = false;
      }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdPositionPt(FourierGroupPtr group, FourierGroupCommandPtr command,
                         bool *send_flag, bool *recv_flag) {
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    DEBUG("PositionPt");
    auto &pos_info = command->command[i]->pos_pt_info_;
    Fourier::PosPtInfoActual actual_info;
    actual_info.pos = pos_info.pos;
    actual_info.vel = static_cast<short>(pos_info.vel * 200);
    actual_info.torque = static_cast<short>(pos_info.torque * 200);
    if (group->aiosGroup[i]->setInputPositionPt(actual_info) !=
        FourierStatusSuccess)
      *send_flag = false;
  }

  for (int i = 0; i < group->aiosGroup.size(); i++) {
    char recv_buf[17];
    std::string recv_ip;
    bool flag = group->aiosGroup[i]->recvPtData(recv_buf, false, &recv_ip) !=
                FourierStatusSuccess;
    int idx = 0;
    for (idx = 0; idx < group->aiosGroup.size(); ++idx) {
      if (group->entryList.at(idx).ip == recv_ip) break;
    }
    if (group->aiosGroup.size() == idx) idx = i;
    if (flag) {
      SPDERROR("Error receiving positionPt buffer.");
      *recv_flag = false;
      group->g_fb.feedback[idx]->position =
          std::numeric_limits<float>::quiet_NaN();
      group->g_fb.feedback[idx]->velocity =
          std::numeric_limits<float>::quiet_NaN();
      group->g_fb.feedback[idx]->current =
          std::numeric_limits<float>::quiet_NaN();
    } else {
      Fourier::CvpPtFeedback pos_pt_info;
      memcpy((void *)&pos_pt_info, recv_buf + 1,
             sizeof(Fourier::CvpPtFeedback));
      group->g_fb.feedback[idx]->position = pos_pt_info.pos;
      group->g_fb.feedback[idx]->velocity = pos_pt_info.vel;
      group->g_fb.feedback[idx]->current = pos_pt_info.torque;
      group->g_fb.feedback[idx]->error_code = pos_pt_info.error_code;
      DEBUG("PositionPt: position:{}, velocity:{}, current:{}, errno:{}",
            pos_pt_info.pos, pos_pt_info.vel, pos_pt_info.torque,
            pos_pt_info.error_code);
    }
    command->command[idx]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
    std::cout << "recv_pt_data end" << std::endl;
  }
  DEBUG("PositionPt end");
}

void handleCmdVelocityPt(FourierGroupPtr group, FourierGroupCommandPtr command,
                         bool *send_flag, bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("VelocityPt: " + std::to_string(val));
    if (!std::isnan(val)) {
      Fourier::VelPtInfo vel_pt_info;
      vel_pt_info.vel = val;
      vel_pt_info.torque = 0;
      if (group->aiosGroup[i]->setInputVelocityPt(vel_pt_info) !=
          FourierStatusSuccess)
        *send_flag = false;
    }
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    if (!std::isnan(val))
      if (group->aiosGroup[i]->recvPtData(nullptr, true) !=
          FourierStatusSuccess) {
        SPDERROR("Error receiving velRampTarget buffer.");
        *recv_flag = false;
      }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdTorquePt(FourierGroupPtr group, FourierGroupCommandPtr command,
                       bool *send_flag, bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("Current: " + std::to_string(val));
    if (!std::isnan(val)) {
      Fourier::TorquePtInfo torque_pt_info;
      torque_pt_info.torque = val;
      if (group->aiosGroup[i]->setInputTorquePt(torque_pt_info) !=
          FourierStatusSuccess)
        *send_flag = false;
    }
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    if (!std::isnan(val))
      if (group->aiosGroup[i]->recvPtData(nullptr) != FourierStatusSuccess) {
        SPDERROR("Error receiving current buffer.");
        *recv_flag = false;
      }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdEnable(FourierGroupPtr group, FourierGroupCommandPtr command,
                     bool *send_flag, bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("aiosGroup: " + std::to_string(i) +
          " enable: " + std::to_string(val));
    if (!std::isnan(val)) {
      if (val) {
        if (group->aiosGroup[i]->encoderIsReady() != FourierStatusSuccess) {
          SPDERROR("encoderIsReady failed.");
          *send_flag = false;
        }
        if (group->aiosGroup[i]->enable() != FourierStatusSuccess) {
          SPDERROR("enable failed.");
          *send_flag = false;
        }
        group->aiosStatus = AXIS_STATE_ENABLE;
      } else {
        if (group->aiosGroup[i]->disable() != FourierStatusSuccess) {
          SPDERROR("disable failed.");
          *send_flag = false;
        }
        group->aiosStatus = AXIS_STATE_IDLE;
      }
    }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdReboot(FourierGroupPtr group, FourierGroupCommandPtr command,
                     bool *send_flag, bool *recv_flag) {
  bool rebootFlag = false;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    rebootFlag = command->command[i]->bool_fields_;
    DEBUG("aiosGroup: " + std::to_string(i) +
          " reboot: " + std::to_string(rebootFlag));
    if (rebootFlag)
      if (group->aiosGroup[i]->reboot() != FourierStatusSuccess) {
        SPDERROR("reboot failed.");
        *send_flag = false;
      }
  }
}

void handleCmdGetError(FourierGroupPtr group, FourierGroupCommandPtr command,
                       bool *send_flag, bool *recv_flag) {
  bool getErrorFlag = false;
  char *recvBuf = new char[group->aiosGroup.at(0)->recv_buf_len_];

  for (int i = 0; i < group->aiosGroup.size(); i++) {
    getErrorFlag = command->command[i]->bool_fields_;
    DEBUG("aiosGroup: " + std::to_string(i) +
          " get error: " + std::to_string(getErrorFlag));
    if (getErrorFlag) {
      memset(recvBuf, 0, strlen(recvBuf));
      if (group->aiosGroup[i]->getError(recvBuf) != FourierStatusSuccess) {
        SPDERROR("getError failed.");
        delete[] recvBuf;
        *send_flag = false;
      }
      std::string recvStr(recvBuf);
      auto jsonObj = nlohmann::json::parse(recvStr);
      try {
        group->feedbackError.at(i)->axis = jsonObj.at("axis");
        group->feedbackError.at(i)->motor = jsonObj.at("motor");
        group->feedbackError.at(i)->encoder = jsonObj.at("encoder");
      } catch (const std::exception &) {
        SPDERROR("Not matching to Axis, Motor, Encoder information in " +
                 recvStr);
      }

      /*std::regex
      re(".*axis\":\"(\\w*).*motor\":\"(\\w*).*encoder\":\"(\\w*).*");
      std::smatch matchResult;
      bool ret = std::regex_match(recvStr, matchResult, re);
      if (ret) {
          group->feedbackError.at(i)->axis    = matchResult[1];
          group->feedbackError.at(i)->motor   = matchResult[2];
          group->feedbackError.at(i)->encoder = matchResult[3];
      }
      else {
          SPDERROR("No matching error message in string: {}", recvStr);
      }*/
    }
  }
  delete[] recvBuf;
}

void handleCmdClearError(FourierGroupPtr group, FourierGroupCommandPtr command,
                         bool *send_flag, bool *recv_flag) {
  bool clearErrorFlag = false;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    clearErrorFlag = command->command[i]->bool_fields_;
    DEBUG("aiosGroup: " + std::to_string(i) +
          " get error: " + std::to_string(clearErrorFlag));
    if (clearErrorFlag) {
      if (group->aiosGroup[i]->clearError() != FourierStatusSuccess) {
        SPDERROR("clearError failed.");
        *send_flag = false;
      }
      group->feedbackError.at(i)->axis = "";
      group->feedbackError.at(i)->motor = "";
      group->feedbackError.at(i)->encoder = "";
    }
  }
}

void handleCmdResetLinearCount(FourierGroupPtr group,
                               FourierGroupCommandPtr command, bool *send_flag,
                               bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  AxisState axis_state = AxisState::AXIS_STATE_UNKNOW;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    if (group->aiosGroup[i]->getState(&axis_state) != FourierStatusSuccess) {
      SPDERROR("aiosGroup: " + std::to_string(i) +
               " ResetLinearCount -> getState failed.");
      return;
    }
    if (axis_state != AxisState::AXIS_STATE_IDLE) {
      WARN("aiosGroup: " + std::to_string(i) +
           " current state is enable; Make sure the FSA disable when exec "
           "resetLinearCount!");
      return;
    }
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("Reset Linear Count: " + std::to_string(val));
    if (!std::isnan(val)) {
      if (group->aiosGroup[i]->setLinearCount(val) == FourierStatusFailure) {
        SPDERROR("aiosGroup: " + std::to_string(i) +
                 " Reset Linear Count failed.");
        *send_flag = false;
      }
    }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

void handleCmdMotionControllerConfig(FourierGroupPtr group,
                                     FourierGroupCommandPtr command,
                                     bool *send_flag, bool *recv_flag) {
  MotionControllerConfig *cfg;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    cfg = command->command[i]->motion_ctrl_config;
    DEBUG("Set Motion Controller Config: ");
    if (cfg != nullptr) {
      nlohmann::json data = cfg;
      if (group->aiosGroup[i]->setMotionCtrlConfig(data) ==
          FourierStatusFailure) {
        SPDERROR("aiosGroup: " + std::to_string(i) +
                 " Motion Controller Config failed.");
        *send_flag = false;
      }
    }
    command->command[i]->motion_ctrl_config = nullptr;
  }
  group->g_fb.getNextFeedbackConfigFlag = true;
  group->g_fb.sendFeedbackRequestConfigFlag = true;
}

void handleCmdMotorConfig(FourierGroupPtr group, FourierGroupCommandPtr command,
                          bool *send_flag, bool *recv_flag) {
  MotorConfig *cfg;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    cfg = command->command[i]->motor_config;
    DEBUG("Set motor config: ");
    if (cfg != nullptr) {
      nlohmann::json data = cfg;
      if (group->aiosGroup[i]->setMotorConfig(data) == FourierStatusFailure) {
        SPDERROR("aiosGroup: " + std::to_string(i) +
                 " set motor config failed.");
        *send_flag = false;
      }
    }
    command->command[i]->motor_config = nullptr;
  }
  group->g_fb.getNextFeedbackConfigFlag = true;
  group->g_fb.sendFeedbackRequestConfigFlag = true;
}

void handleCmdTrapTraj(FourierGroupPtr group, FourierGroupCommandPtr command,
                       bool *send_flag, bool *recv_flag) {
  TrapTraj *cfg;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    cfg = command->command[i]->trap_traj;
    DEBUG("Set trap traj: ");
    if (cfg != nullptr) {
      nlohmann::json data = cfg;
      if (group->aiosGroup[i]->setTrapTraj(data) == FourierStatusFailure) {
        SPDERROR("aiosGroup: " + std::to_string(i) + " set trap traj failed.");
        *send_flag = false;
      }
    }
    command->command[i]->trap_traj = nullptr;
  }
  group->g_fb.getNextFeedbackConfigFlag = true;
  group->g_fb.sendFeedbackRequestConfigFlag = true;
}

void handleCmdSaveConfig(FourierGroupPtr group, FourierGroupCommandPtr command,
                         bool *send_flag, bool *recv_flag) {
  bool saveCfgFlag = false;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    saveCfgFlag = command->command[i]->bool_fields_;
    DEBUG("aiosGroup: " + std::to_string(i) +
          " get error: " + std::to_string(saveCfgFlag));
    if (saveCfgFlag)
      if (group->aiosGroup[i]->saveConfig() != FourierStatusSuccess) {
        SPDERROR("clearError failed.");
        *send_flag = false;
      }
  }
}

void handleCmdNetworkSetting(FourierGroupPtr group,
                             FourierGroupCommandPtr command, bool *send_flag,
                             bool *recv_flag) {
  NetworkSetting *cfg;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    cfg = command->command[i]->network_setting;
    DEBUG("Set Network Setting: ");
    if (cfg != nullptr) {
      nlohmann::json data = cfg;
      if (group->aiosGroup[i]->setNetworkSetting(data) ==
          FourierStatusFailure) {
        SPDERROR("aiosGroup: " + std::to_string(i) +
                 " set network setting failed.");
        *send_flag = false;
      }
    }
    command->command[i]->network_setting = nullptr;
  }
  group->g_fb.getNextFeedbackConfigFlag = true;
  group->g_fb.sendFeedbackRequestConfigFlag = true;
}

void handleCmdLatencyTest(FourierGroupPtr group, FourierGroupCommandPtr command,
                          bool *send_flag, bool *recv_flag) {
  bool latency = false;
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    latency = command->command[i]->bool_fields_;
    DEBUG("aiosGroup: " + std::to_string(i) +
          " latency test: " + std::to_string(latency));
    if (latency) {
      if (group->aiosGroup[i]->dumFunc() != FourierStatusSuccess) {
        SPDERROR("dumFunc failed.");
        *send_flag = false;
      }
    }
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    latency = command->command[i]->bool_fields_;
    if (latency) {
      if (group->aiosGroup[i]->recvRtData(nullptr) != FourierStatusSuccess) {
        SPDERROR("dumFunc failed.");
        *recv_flag = false;
      }
    }
  }
}

void handleCmdCtrlBoxPower(FourierGroupPtr group,
                           FourierGroupCommandPtr command, bool *send_flag,
                           bool *recv_flag) {
  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    val = command->command[i]->float_fields_;
    DEBUG("aiosGroup: " + std::to_string(i) +
          " ctrlBox power: " + std::to_string(val));
    if (!std::isnan(val)) {
      if (val) {
        if (group->aiosGroup[i]->ctrlBoxEnable() != FourierStatusSuccess) {
          SPDERROR("ctrlBoxEnable failed.");
          *send_flag = false;
        }
      } else {
        if (group->aiosGroup[i]->ctrlBoxDisable() != FourierStatusSuccess) {
          SPDERROR("ctrlBoxDisable failed.");
          *recv_flag = false;
        }
      }
    }
    command->command[i]->float_fields_ =
        std::numeric_limits<float>::quiet_NaN();
  }
}

FourierStatusCode handleCommand(FourierGroupPtr group,
                                FourierGroupCommandPtr command) {
  std::lock_guard<std::mutex> lock_guard_cmd(command->cmd_enum_mutex);
  std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);
  std::lock_guard<std::timed_mutex> lock_guard_feedback(
      group->aios_feedback_mu);
  INFO("======= fourierGroupSendCommand begin");
  if (group->aiosGroup.size() == 0) {
    SPDERROR("Actuator Group == 0");
    for (auto &cmd : command->command) {
      cmd->commandEnum = UNKNOW;
    }
    return FourierStatusFailure;
  }
  if (command->command.size() == 0) {
    SPDERROR("command Group == 0");
    for (auto &cmd : command->command) {
      cmd->commandEnum = UNKNOW;
    }
    return FourierStatusFailure;
  }
  if (group->aiosGroup.size() != command->command.size()) {
    SPDERROR("Actuator size: {} != command size: {}", group->aiosGroup.size(),
             command->command.size());
    for (auto &cmd : command->command) {
      cmd->commandEnum = UNKNOW;
    }
    return FourierStatusFailure;
  }
  if (group->aiosGroup.size() != group->g_fb.feedback.size()) {
    SPDERROR("Actuator Group != group feedback");
    for (auto &cmd : command->command) {
      cmd->commandEnum = UNKNOW;
    }
    return FourierStatusFailure;
  }

  if (command->command[0]->commandEnum == FourierCommandPosition ||
      command->command[0]->commandEnum == FourierCommandTrapezoidal ||
      command->command[0]->commandEnum == FourierCommandVelocity ||
      command->command[0]->commandEnum == FourierCommandVelocityRamp ||
      command->command[0]->commandEnum == FourierCommandCurrent ||
      command->command[0]->commandEnum == FourierCommandTorquePt ||    // fw2.0
      command->command[0]->commandEnum == FourierCommandVelocityPt ||  // fw2.0
      command->command[0]->commandEnum == FourierCommandPositionPt) {  // fw2.0
    if (group->aiosStatus != AXIS_STATE_ENABLE) {
      SPDERROR("Actuator not enabled.");
      for (auto &cmd : command->command) {
        cmd->commandEnum = UNKNOW;
      }
      return FourierStatusFailure;
    }
  }

  // 速度斜坡模式需要切换模式
  bool send_flag = true;
  bool recv_flag = true;

  float val = std::numeric_limits<float>::quiet_NaN();
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    if (group->aiosGroup.at(i)->fw_version_ ==
        Fourier::FwVersion::FW_2) {  // fw2.0
      DEBUG("fw2.0");
      if (group->preMode != command->command[i]->commandEnum) {
        // TODO:电流模式
        DEBUG("Change input mode");
        // val = command->command[i]->float_fields_;
        // if (std::isnan(val)) continue;
        switch (command->command[i]->commandEnum) {
          case FourierCommandVelocity:
          case FourierCommandTorquePt:
          case FourierCommandVelocityPt:
          case FourierCommandPositionPt:
            if (group->aiosGroup[i]->inputMode(
                    InputMode::INPUT_MODE_PASSTHROUGH) ==
                FourierStatusSuccess) {
              INFO(
                  "Set the current input mode to INPUT_MODE_PASSTHROUGH "
                  "mode.");
            } else {
              SPDERROR("Failed to set INPUT_MODE_PASSTHROUGH input mode.");
              recv_flag = false;
            }
            break;
          case FourierCommandTrapezoidal:
            if (group->aiosGroup[i]->inputMode(
                    InputMode::INPUT_MODE_TRAP_TRAJ) == FourierStatusSuccess) {
              INFO("Set the current input mode to INPUT_MODE_TRAP_TRAJ mode.");
            } else {
              SPDERROR("Failed to set INPUT_MODE_TRAP_TRAJ input mode.");
              recv_flag = false;
            }
            break;
          default:
            break;
        }
      }
    } else {  // fw < 2.0
      DEBUG("fw1.0");
      if (group->preMode != command->command[0]->commandEnum &&
          command->command[i]->commandEnum == FourierCommandVelocityRamp) {
        val = command->command[i]->float_fields_;
        if (std::isnan(val)) continue;
        if (group->aiosGroup[i]->controlMode(ControlMode::VELOCITY_CONTROL) ==
                FourierStatusSuccess &&
            group->aiosGroup[i]->velRampEnable(true) == FourierStatusSuccess) {
          INFO("Set the current control mode to velocity mode.");
        } else {
          SPDERROR("Failed to set velocity control mode.");
          recv_flag = false;
        }
      }
    }
  }

  switch (command->command[0]->commandEnum) {
    case FourierCommandTrapezoidal:
      handleCmdTrapezoidal(group, command, &send_flag, &recv_flag);
      break;
    case FourierCommandPosition:
      handleCmdPosition(group, command, &send_flag, &recv_flag);
      break;
    case FourierCommandVelocity: {
      handleCmdVelocity(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandVelocityRamp: {
      handleCmdVelocityRamp(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandCurrent: {
      handleCmdCurrent(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandPositionPt: {
      handleCmdPositionPt(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandVelocityPt: {
      handleCmdVelocityPt(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandTorquePt: {
      handleCmdTorquePt(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandEnable:  // 使能执行器
      handleCmdEnable(group, command, &send_flag, &recv_flag);
      break;
    case FourierCommandReboot: {
      handleCmdReboot(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandGetError: {
      handleCmdGetError(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandClearError: {
      handleCmdClearError(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandResetLinearCount: {
      handleCmdResetLinearCount(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandMotionControllerConfig: {
      handleCmdMotionControllerConfig(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandMotorConfig: {
      handleCmdMotorConfig(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandTrapTraj: {
      handleCmdTrapTraj(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandSaveConfig: {
      handleCmdSaveConfig(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandNetworkSetting: {
      handleCmdNetworkSetting(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandLatencyTest: {
      handleCmdLatencyTest(group, command, &send_flag, &recv_flag);
      break;
    }
    case FourierCommandCtrlBoxPower: {
      handleCmdCtrlBoxPower(group, command, &send_flag, &recv_flag);
      break;
    }
    default:
      break;
  }
  group->preMode = command->command[0]->commandEnum;
  INFO("======= fourierGroupSendCommand end");
  if (send_flag == false || recv_flag == false) {
    for (auto &cmd : command->command) {
      cmd->commandEnum = UNKNOW;
    }
    return FourierStatusFailure;
  }
  for (auto &cmd : command->command) {
    cmd->commandEnum = UNKNOW;
  }
  return FourierStatusSuccess;
}

FourierStatusCode fourierGroupSendCommand(FourierGroupPtr group,
                                          FourierGroupCommandPtr command) {
  if (group->cmd_thread == nullptr) {
    group->cmd_thread = new std::thread([group, command]() {
      DEBUG("Command thread start");
      while (true) {
        // StartTimeChrono(handleCommand);
        std::unique_lock<std::mutex> lck(group->cmd_mtx);
        if (!group->cmd_ready) {
          group->cmd_cv.wait(lck);
          if (group->exit_signal) {
            break;
          }
        }
        handleCommand(group, command);
        group->cmd_ready = false;
        // EndTimeChrono(handleCommand);
      }
      DEBUG("Command thread end");
    });
    //  wait thread init
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  } else {
    std::unique_lock<std::mutex> lck(group->cmd_mtx);
    group->cmd_ready = true;
    group->cmd_cv.notify_one();
  }
  return FourierStatusSuccess;
}

FourierStatusCode fourierGroupSetFeedbackFrequencyHz(FourierGroupPtr group,
                                                     const float frequency) {
  if (frequency < 0 && frequency >= 500) {
    return FourierStatusFailure;
  }
  std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);
  for (auto &elem : group->aiosGroup) {
    elem->frequency_ = frequency;
  }
  group->feedbackFrequencyHz.store(frequency);

  if (group->fb_thread == nullptr) {
    group->fb_thread = new std::thread([group]() {
      DEBUG("Feedback thread start; frequency:" +
            std::to_string(group->feedbackFrequencyHz));
      while (true) {
        // StartTimeChrono(fourierGroupSendFeedbackRequest);
        std::unique_lock<std::mutex> lck(group->fb_mtx);
        auto t = Fourier::intervalTimeStart();
        if (group->exit_signal.load()) {
          break;
        }
        if (group->feedbackFrequencyHz.load() == 0) {
          DEBUG("Feedback thread pause");
          group->fb_cv.wait(lck);
          DEBUG("Feedback thread continue");
        }
        fourierGroupSendFeedbackRequest(group, FourierFeedbackCVP);
        Fourier::intervalTimeEnd(t, group->feedbackFrequencyHz);
        // EndTimeChrono(fourierGroupSendFeedbackRequest);
      }
      DEBUG("Feedback thread end");
    });
  } else if (frequency != 0) {
    std::unique_lock<std::mutex> lck(group->fb_mtx);
    group->fb_cv.notify_one();
  }
  return FourierStatusSuccess;
}

FourierStatusCode fourierGroupSetCommandLifetime(FourierGroupPtr group,
                                                 int32_t lifetime_ms) {
  std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);
  group->lifetime_ms = lifetime_ms;
  return FourierStatusSuccess;
}

FourierStatusCode fourierGroupSendFeedbackRequest(
    FourierGroupPtr group, FourierFeedbackCode feedbackCode) {
  // std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);
  std::lock_guard<std::timed_mutex> lock_guard(group->aios_feedback_mu);
  if (group->aiosGroup.size() == 0) {
    return FourierStatusFailure;
  }
  for (auto &elem : group->packedLossNum) {
    elem = 0;
  }

  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    if (group->aiosGroup.at(i)->fw_version_ == Fourier::FwVersion::FW_2) {
      group->aiosGroup[i]->getCvpPt();
    } else {
      group->aiosGroup[i]->getCVP(nullptr, false);
    }
  }
  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    if (group->aiosGroup.at(i)->fw_version_ == Fourier::FwVersion::FW_2) {
      std::shared_ptr<Fourier::CvpPtFeedback> cvp_pt_feedback =
          std::make_shared<Fourier::CvpPtFeedback>();
      if (group->aiosGroup.at(i)->recvPtData(cvp_pt_feedback) !=
          FourierStatusSuccess) {
        SPDERROR("getCVP failed.");
        ++(group->packedLossNum[i]);
        continue;
      }
      auto fb = group->g_fb.feedback.at(i);
      auto preFeedback = group->g_preFb.feedback.at(i);
      fb->position = cvp_pt_feedback->pos;
      fb->velocity = cvp_pt_feedback->vel;
      fb->current = cvp_pt_feedback->torque;
      fb->error_code = cvp_pt_feedback->error_code;

      preFeedback->position = fb->position;
      DEBUG("aiosGroup {} position:{}, velocity:{}, current:{}", i,
            fb->position, fb->velocity, fb->current);
    } else {
      std::shared_ptr<std::vector<double>> vPVC =
          std::make_shared<std::vector<double>>();
      if (group->aiosGroup.at(i)->recvCVPData(vPVC) != FourierStatusSuccess) {
        SPDERROR("getCVP failed.");
        ++(group->packedLossNum[i]);
        continue;
      }
      auto fb = group->g_fb.feedback.at(i);
      auto preFeedback = group->g_preFb.feedback.at(i);
      fb->position = vPVC->at(0);
      fb->velocity = vPVC->at(1);
      fb->current = vPVC->at(2);

      preFeedback->position = fb->position;
      DEBUG("aiosGroup {} position:{}, velocity:{}, current:{}", i,
            fb->position, fb->velocity, fb->current);
    }
  }
  if (feedbackCode == FourierFeedbackCVP) return FourierStatusSuccess;

  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    group->aiosGroup[i]->getRoot(nullptr, true);
  }
  char *recv = new char[group->aiosGroup.at(0)->recv_buf_len_];
  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    auto fb = group->g_fb.feedback.at(i);
    if (group->aiosGroup.at(i)->recvServData(recv, true) !=
        FourierStatusSuccess) {
      SPDERROR("getRoot failed.");
      ++(group->packedLossNum[i]);
      continue;
    }
    std::string recvStr(recv);
    auto jsonObj = nlohmann::json::parse(recvStr);
    try {
      fb->serial_number = jsonObj.at("serial_number");
      fb->connect_mode = jsonObj.at("connect_mode");
      fb->model = jsonObj.at("model");
      fb->drive_status = jsonObj.at("motor_drive_ready");
      fb->voltage = jsonObj.at("vbus_voltage");
      fb->motor_temp_m1 = jsonObj.at("motor_temp_m1");
      fb->inverter_temp_m1 = jsonObj.at("inverter_temp_m1");
      fb->hw_version = jsonObj.at("Hw_version");
      fb->fw_version = jsonObj.at("Fw_version");

      fb->ip = group->entryList.at(i).ip;
      fb->mac_address = group->entryList.at(i).mac_address;
    } catch (const std::exception &) {
      DEBUG("recvStr:" + recvStr);
      SPDERROR("Warring: getRoot match failed.");
    }
  }

  for (int i = 0; i < group->packedLossNum.size(); ++i) {
    if (group->packedLossNum[i] >=
        group->maxPackedLossNum) {  // 在feedback时连续丢两个包
      auto fb = group->g_fb.feedback.at(i);
      // 将其从组内移除
      group->lookup->ipAndActuatorInfo->erase(fb->ip);

      fb->position = fb->velocity = fb->current =
          std::numeric_limits<float>::quiet_NaN();

      fb->drive_status = false;

      fb->serial_number = fb->connect_mode = fb->model = fb->hw_version =
          fb->fw_version = fb->ip = fb->mac_address = "";

      fb->voltage = fb->motor_temp_m1 = fb->inverter_temp_m1 =
          std::numeric_limits<float>::quiet_NaN();

      fb->motion_ctrl_config.pos_gain = fb->motion_ctrl_config.vel_gain =
          fb->motion_ctrl_config.vel_integrator_gain =
              fb->motion_ctrl_config.vel_limit =
                  fb->motion_ctrl_config.vel_limit_tolerance =
                      std::numeric_limits<float>::quiet_NaN();

      fb->motor_config.current_lim = fb->motor_config.current_lim_margin =
          fb->motor_config.inverter_temp_limit_lower =
              fb->motor_config.inverter_temp_limit_upper =
                  fb->motor_config.requested_current_range =
                      fb->motor_config.current_control_bandwidth =
                          std::numeric_limits<int>::quiet_NaN();

      fb->trap_traj.accel_limit = fb->trap_traj.decel_limit =
          fb->trap_traj.vel_limit = std::numeric_limits<int>::quiet_NaN();

      group->actuatorOnline[i] = false;
      group->g_fb.getNextFeedbackConfigFlag = true;
    }
  }

  if (!group->g_fb.sendFeedbackRequestConfigFlag) {
    delete[] recv;
    return FourierStatusSuccess;
  }
  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    group->aiosGroup[i]->getMotionCtrlConfig();
  }
  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    auto fb = group->g_fb.feedback.at(i);
    memset(recv, 0, strlen(recv));
    if (group->aiosGroup.at(i)->recvServData(recv, true) !=
        FourierStatusSuccess)
      continue;
    std::string recvStr(recv);
    auto jsonObj = nlohmann::json::parse(recvStr);
    try {
      fb->motion_ctrl_config.pos_gain = jsonObj.at("pos_gain");
      fb->motion_ctrl_config.vel_gain = jsonObj.at("vel_gain");
      fb->motion_ctrl_config.vel_integrator_gain =
          jsonObj.at("vel_integrator_gain");
      fb->motion_ctrl_config.vel_limit = jsonObj.at("vel_limit");
      fb->motion_ctrl_config.vel_limit_tolerance =
          jsonObj.at("vel_limit_tolerance");
    } catch (const std::exception &) {
      DEBUG("recvStr:" + recvStr);
      SPDERROR("Warring: getMotionCtrlConfig match failed.");
    }
  }

  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    group->aiosGroup[i]->getMotorConfig();
  }
  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    auto fb = group->g_fb.feedback.at(i);
    memset(recv, 0, strlen(recv));
    if (group->aiosGroup.at(i)->recvServData(recv, true) !=
        FourierStatusSuccess)
      continue;
    std::string recvStr(recv);
    auto jsonObj = nlohmann::json::parse(recvStr);
    try {
      fb->motor_config.current_lim = jsonObj.at("current_lim");
      fb->motor_config.current_lim_margin = jsonObj.at("current_lim_margin");
      fb->motor_config.inverter_temp_limit_lower =
          jsonObj.at("inverter_temp_limit_lower");
      fb->motor_config.inverter_temp_limit_upper =
          jsonObj.at("inverter_temp_limit_upper");
      fb->motor_config.requested_current_range =
          jsonObj.at("requested_current_range");
      fb->motor_config.current_control_bandwidth =
          jsonObj.at("current_control_bandwidth");
    } catch (const std::exception &) {
      DEBUG("recvStr:" + recvStr);
      SPDERROR("Warring: getMotorConfig match failed.");
    }
  }

  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    group->aiosGroup[i]->getTrapTraj();
  }
  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    auto fb = group->g_fb.feedback.at(i);
    memset(recv, 0, strlen(recv));
    if (group->aiosGroup.at(i)->recvServData(recv, true) !=
        FourierStatusSuccess)
      continue;
    std::string recvStr(recv);
    auto jsonObj = nlohmann::json::parse(recvStr);
    try {
      fb->trap_traj.accel_limit = jsonObj.at("accel_limit");
      fb->trap_traj.decel_limit = jsonObj.at("decel_limit");
      fb->trap_traj.vel_limit = jsonObj.at("vel_limit");
    } catch (const std::exception &) {
      DEBUG("recvStr:" + recvStr);
      SPDERROR("Warring: getTrapTraj match failed.");
    }
  }

  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    group->aiosGroup[i]->getNetworkSetting();
  }
  for (int i = 0; i < group->aiosGroup.size(); ++i) {
    if (!group->actuatorOnline[i]) continue;
    auto fb = group->g_fb.feedback.at(i);
    memset(recv, 0, strlen(recv));
    if (group->aiosGroup.at(i)->recvServData(recv, true) !=
        FourierStatusSuccess)
      continue;
    std::string recvStr(recv);
    std::regex e1("\\[");
    std::regex e2("\\]");
    recvStr = std::regex_replace(recvStr, e1, "\"[");
    recvStr = std::regex_replace(recvStr, e2, "]\"");
    auto jsonObj = nlohmann::json::parse(recvStr);
    try {
      fb->network_setting.dhcp_enable = jsonObj.at("DHCP_enable");
      fb->network_setting.SSID = jsonObj.at("SSID");
      fb->network_setting.password = jsonObj.at("password");
      fb->network_setting.name = jsonObj.at("name");
      fb->network_setting.staticIP = jsonObj.at("staticIP");
      fb->network_setting.gateway = jsonObj.at("gateway");
      fb->network_setting.subnet = jsonObj.at("subnet");
      fb->network_setting.dns_1 = jsonObj.at("dns_1");
      fb->network_setting.dns_2 = jsonObj.at("dns_2");

      DEBUG(
          "DHCP_enable:{}, SSID:{}, password:{}, name:{}, staticIP:{}, "
          "gateway:{}, subnet:{}, dns_1:{}, dns_2:{}",
          fb->network_setting.dhcp_enable, fb->network_setting.SSID,
          fb->network_setting.password, fb->network_setting.name,
          fb->network_setting.staticIP, fb->network_setting.gateway,
          fb->network_setting.subnet, fb->network_setting.dns_1,
          fb->network_setting.dns_2);
    } catch (const std::exception &) {
      DEBUG("recvStr:" + recvStr);
      SPDERROR("Warring: getNetworkSetting match failed.");
    }
  }

  group->g_fb.sendFeedbackRequestConfigFlag = false;
  delete[] recv;

  return FourierStatusSuccess;
}

FourierStatusCode fourierGroupGetNextFeedback(FourierGroupPtr group,
                                              FourierGroupFeedbackPtr feedback,
                                              int32_t timeout_ms) {
  // std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);
  if (!group->aios_feedback_mu.try_lock_for(
          std::chrono::milliseconds(timeout_ms))) {
    SPDERROR("fourierGroupGetNextFeedback timeout: {}", timeout_ms);
    return FourierStatusFailure;
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    auto dst = feedback->feedback.at(i);
    auto src = group->g_fb.feedback.at(i);
    dst->position = src->position;
    dst->velocity = src->velocity;
    dst->current = src->current;
    dst->error_code = src->error_code;
    dst->voltage = src->voltage;
    dst->motor_temp_m0 = src->motor_temp_m0;  // reserved
    dst->motor_temp_m1 = src->motor_temp_m1;
    dst->inverter_temp_m0 = src->inverter_temp_m0;  // reserved
    dst->inverter_temp_m1 = src->inverter_temp_m1;
    dst->ip = src->ip;
    dst->serial_number = src->serial_number;
    dst->connect_mode = src->connect_mode;
    dst->model = src->model;
    dst->mac_address = src->mac_address;
    dst->drive_status = src->drive_status;
    dst->hw_version = src->hw_version;
    dst->fw_version = src->fw_version;
  }

  if (!group->g_fb.getNextFeedbackConfigFlag) {
    group->aios_feedback_mu.unlock();
    return FourierStatusSuccess;
  }
  for (int i = 0; i < group->aiosGroup.size(); i++) {
    auto dst = feedback->feedback.at(i);
    auto src = group->g_fb.feedback.at(i);
    dst->motion_ctrl_config.pos_gain = src->motion_ctrl_config.pos_gain;
    dst->motion_ctrl_config.vel_gain = src->motion_ctrl_config.vel_gain;
    dst->motion_ctrl_config.vel_integrator_gain =
        src->motion_ctrl_config.vel_integrator_gain;
    dst->motion_ctrl_config.vel_limit = src->motion_ctrl_config.vel_limit;
    dst->motion_ctrl_config.vel_limit_tolerance =
        src->motion_ctrl_config.vel_limit_tolerance;

    dst->motor_config.current_lim = src->motor_config.current_lim;
    dst->motor_config.current_lim_margin = src->motor_config.current_lim_margin;
    dst->motor_config.inverter_temp_limit_lower =
        src->motor_config.inverter_temp_limit_lower;
    dst->motor_config.inverter_temp_limit_upper =
        src->motor_config.inverter_temp_limit_upper;
    dst->motor_config.requested_current_range =
        src->motor_config.requested_current_range;
    dst->motor_config.current_control_bandwidth =
        src->motor_config.current_control_bandwidth;

    dst->trap_traj.accel_limit = src->trap_traj.accel_limit;
    dst->trap_traj.decel_limit = src->trap_traj.decel_limit;
    dst->trap_traj.vel_limit = src->trap_traj.vel_limit;

    dst->network_setting.dhcp_enable = src->network_setting.dhcp_enable;
    dst->network_setting.SSID = src->network_setting.SSID;
    dst->network_setting.password = src->network_setting.password;
    dst->network_setting.name = src->network_setting.name;
    dst->network_setting.staticIP = src->network_setting.staticIP;
    dst->network_setting.gateway = src->network_setting.gateway;
    dst->network_setting.subnet = src->network_setting.subnet;
    dst->network_setting.dns_1 = src->network_setting.dns_1;
    dst->network_setting.dns_2 = src->network_setting.dns_2;
  }
  group->g_fb.getNextFeedbackConfigFlag = false;
  group->aios_feedback_mu.unlock();
  return FourierStatusSuccess;
}

float fourierGroupGetFeedbackFrequencyHz(FourierGroupPtr group) {
  std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);
  return group->feedbackFrequencyHz;
}

FourierStatusCode fourierGroupRegisterFeedbackHandler(
    FourierGroupPtr group, GroupFeedbackHandlerFunction handler,
    void *user_data) {
  // TODO
  return FourierStatusSuccess;
}

void fourierGroupClearFeedbackHandlers(FourierGroupPtr group) {
  // TODO
}

FourierFeedbackErrorPtr fourierGroupFeedbackError(FourierGroupPtr group,
                                                  int idx) {
  std::lock_guard<std::mutex> lock_guard(group->aiosGroupMutex);
  if (idx < 0 || idx > group->aiosGroup.size() - 1) {
    return nullptr;
  }
  return group->feedbackError.at(idx);
}
