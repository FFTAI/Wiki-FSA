#include "Fsa.h"
#include <fstream>

FSA_CONNECT::FSA::FSA(const string &ip) {
    ip_ = ip;
    ctrl_udp_socket = std::make_shared<Transmit::UDPSocket>(ip, 2333);
    root_udp_socket = std::make_shared<Transmit::UDPSocket>(ip, 2334);
    // pt_udp_socket = std::make_shared<Transmit::UDPSocket>(ip, 10000);
};

int FSA_CONNECT::FSA::GetRootConfig() {
    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::FSAConfig;

    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (get_root_config) {

        case 0: //
            begin = std::chrono::steady_clock::now();
            std::cout << get_root_config_json.dump() << std::endl;
            ret = ctrl_udp_socket->SendData(get_root_config_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR : {} GET ROOT CONFIG SEND ERROR! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            get_root_config = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, GET ROOT CONFIG RECEIVE ERROR! ERROR CODE: {}", ip_, ret);
                get_root_config = 0;
                return ret;
            }

            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);

                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    get_root_config = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, GET ROOT CONFIG FAILED", ip_);
                    return GET_ROOT_FAILED;
                }

                get_root_config = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, GET ROOT CONFIG SUCCESS! ", ip_);
                FSA_CONNECT::LOG::INFO(recv_data_str);
                return SUCCESS;
            }

            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                get_root_config = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, GET ROOT CONFIG TIMEOUT", ip_);
                return FSA_CONNECT::ResultCode::TIMEOUT;
            }
            break;
        default:
            get_root_config = 0;
            break;
        };
    }

    return NOT_EXECUTE;
}

int FSA_CONNECT::FSA::GetState() {
    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::FSAConfig;

    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (get_root_config) {

        case 0: //
            begin = std::chrono::steady_clock::now();
            std::cout << get_state_json.dump() << std::endl;
            ret = root_udp_socket->SendData(get_state_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, GET STATE SEND ERROR! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            get_root_config = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = root_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, GET STATE RECEIVE ERROR! ERROR CODE: {}", ip_, ret);
                get_root_config = 0;
                return ret;
            }

            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);

                receive_state = recv_data_json.at("status");
                // std::cout << "json: " << receive_state << std::endl;
                //!="OK"
                if (receive_state.compare("OK")) {
                    get_root_config = 0;
                    return GET_ROOT_FAILED;
                }

                get_root_config = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, GET ROOT CONFIG SUCCESS! ", ip_);
                FSA_CONNECT::LOG::INFO(recv_data_str);
                return SUCCESS;
            }

            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                get_root_config = 0;
                return FSA_CONNECT::ResultCode::TIMEOUT;
            }
            break;
        default:
            get_root_config = 0;
            break;
        };
    }

    return NOT_EXECUTE;
}

int FSA_CONNECT::FSA::Enable() {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (poweron_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            enable_json["control_word"] = FSAControlWord::SERVO_ON;
            ret = ctrl_udp_socket->SendData(enable_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            poweron_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);

            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                poweron_state = 0;
                return ret;
            }

            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    poweron_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE FAILED! ", ip_);

                    return ENABLE_FAILED;
                }
                is_enabled = true;
                poweron_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                poweron_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            poweron_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::Disable() {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (poweroff_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            disable_json["control_word"] = FSAControlWord::SERVO_OFF;
            ret = ctrl_udp_socket->SendData(disable_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            poweroff_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);

            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                poweroff_state = 0;
                return ret;
            }

            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    poweroff_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, DISABLE FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                poweroff_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, DISABLE SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                poweroff_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, DISABLE TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            poweroff_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::SetControlConfig(const FSAConfig::FSAControlConfig &config) {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (set_ctrlcfg_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            set_control_mode_json["actuator_type"] = config.actuator_type;
            set_control_mode_json["actuator_reduction_ratio"] = config.actuator_reduction_ratio;
            set_control_mode_json["motor_index"] = config.motor_index;
            set_control_mode_json["motor_vbus"] = config.motor_vbus;
            set_control_mode_json["motor_direction"] = config.motor_direction;
            set_control_mode_json["motor_pole_pairs"] = config.motor_pole_pairs;
            set_control_mode_json["motor_max_speed"] = config.motor_max_speed;
            set_control_mode_json["encoder_direction"] = config.encoder_direction;
            set_control_mode_json["encoder_resolution"] = config.encoder_resolution;
            set_control_mode_json["encoder_phase_offset"] = config.encoder_phase_offset;

            ret = ctrl_udp_socket->SendData(set_control_mode_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            set_ctrlcfg_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);

            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                set_ctrlcfg_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    set_ctrlcfg_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, SET CONTROL CONFIG FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                set_ctrlcfg_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {},  SET CONTROL CONFIG SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                set_ctrlcfg_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, SET CONTROL CONFIG TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            set_ctrlcfg_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::GetControlConfig() {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (get_ctrlcfg_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();

            ret = ctrl_udp_socket->SendData(get_control_mode_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            get_ctrlcfg_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                get_ctrlcfg_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    get_ctrlcfg_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, GET CONTROL CONFIG FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                get_ctrlcfg_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {},  GET CONTROL CONFIG SUCCESS! ", ip_);
                FSA_CONNECT::LOG::INFO(recv_data_str);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                get_ctrlcfg_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, GET CONTROL CONFIG TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            get_ctrlcfg_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::SetPIDParams(const FSAConfig::FSAPIDParams &pidparams) {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (set_pid_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            // set_pid_params_json["control_position_kp"] = pidparams.control_position_kp;
            // set_pid_params_json["control_velocity_kp"] = pidparams.control_velocity_kp;
            // set_pid_params_json["control_velocity_ki"] = pidparams.control_velocity_ki;
            // set_pid_params_json["control_current_kp"] = pidparams.control_current_kp;
            // set_pid_params_json["control_current_ki"] = pidparams.control_current_ki;
            // set_pid_params_json["control_position_output_max"] = pidparams.control_position_output_max;
            // set_pid_params_json["control_position_output_min"] = pidparams.control_position_output_min;
            // set_pid_params_json["control_velocity_output_max"] = pidparams.control_velocity_output_max;
            // set_pid_params_json["control_velocity_output_min"] = pidparams.control_velocity_output_min;
            // set_pid_params_json["control_current_output_max"] = pidparams.control_current_output_max;
            // set_pid_params_json["control_current_output_min"] = pidparams.control_current_output_min;

            ret = ctrl_udp_socket->SendData(set_pid_params_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            set_pid_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                set_pid_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    set_pid_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, SET PID PARAMS FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                set_pid_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {},  SET PID PARAMS SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                set_pid_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, SET PID PARAMS TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            set_pid_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::GetPIDParams() {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (get_pid_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            ret = ctrl_udp_socket->SendData(get_pid_params_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            get_pid_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                get_pid_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    get_pid_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, SET PID PARAMS FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                get_pid_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {},  SET PID PARAMS SUCCESS! ", ip_);
                FSA_CONNECT::LOG::INFO(recv_data_str);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                get_pid_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, SET PID PARAMS TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            get_pid_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::EnablePosControl() {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (control_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            set_operation_mode_json["mode_of_operation"] = Status::POSITION_CONTROL;
            ret = ctrl_udp_socket->SendData(set_operation_mode_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            control_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                control_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    control_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE POSITION CONTROL FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                control_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {},  ENABLE POSITION CONTROL SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                control_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE POSITION CONTROL TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            control_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::EnableVelControl() {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (control_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            set_operation_mode_json["mode_of_operation"] = Status::VELOCITY_CONTROL;
            ret = ctrl_udp_socket->SendData(set_operation_mode_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            control_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                control_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    control_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE VELOCITY CONTROL FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                control_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {},  ENABLE VELOCITY CONTROL SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                control_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE VELOCITY CONTROL TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            control_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::EnableCurControl() {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (control_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            set_operation_mode_json["mode_of_operation"] = Status::CURRENT_CLOSE_LOOP_CONTROL;
            ret = ctrl_udp_socket->SendData(set_operation_mode_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            control_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                control_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    control_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE CURRENT CONTROL FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                control_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {},  ENABLE CURRENT CONTROL SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                control_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, ENABLE CURRENT CONTROL TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            control_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::SetPosition(const double &pos, const double &vel_ff, const double &cur_ff) {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (set_pos_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            set_pos_json["positon"] = pos;
            set_pos_json["velocity_ff"] = vel_ff;
            set_pos_json["current_ff"] = cur_ff;

            ret = ctrl_udp_socket->SendData(set_pos_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            set_pos_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                set_pos_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    set_pos_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, SET POSITION FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                set_pos_state = 0;
                // FSA_CONNECT::LOG::INFO("MOTOR: {},  SET POS SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                set_pos_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, SET POSITION TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            set_pos_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::SetVelocity(const double &vel, const double &cur_ff) {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (set_pos_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            set_vel_json["velocity"] = vel;
            set_vel_json["current_ff"] = cur_ff;

            ret = ctrl_udp_socket->SendData(set_vel_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            set_pos_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                set_pos_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    set_pos_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, SET VELOCITY FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                set_pos_state = 0;
                // FSA_CONNECT::LOG::INFO("MOTOR: {},  SET POS SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                set_pos_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, SET VELOCITY TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            set_pos_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::SetCurrent(const double &cur) {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (set_pos_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();

            set_cur_json["current"] = cur;

            ret = ctrl_udp_socket->SendData(set_cur_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            set_pos_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_nrt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                set_pos_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    set_pos_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, SET CURRENT FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                set_pos_state = 0;
                // FSA_CONNECT::LOG::INFO("MOTOR: {},  SET POS SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                set_pos_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, SET CURRENT TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            set_pos_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};

int FSA_CONNECT::FSA::GetPVC(double &pos, double &vel, double &cur) {

    using namespace FSA_CONNECT::JsonData;
    using namespace FSA_CONNECT::ResultCode;
    using namespace FSA_CONNECT::Status;
    int ret;
    std::string recv_data_str;
    json recv_data_json;
    std::string receive_state;
    while (1) {
        switch (set_pos_state) {
        case 0: // enable
            begin = std::chrono::steady_clock::now();
            ret = ctrl_udp_socket->SendData(get_pvc_json.dump());
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET SEND FAILED! ERROR CODE: {}", ip_, ret);
                return ret;
            }
            // data send succeed
            // clock_gettime(CLOCK_MONOTONIC,&start_udp_socket_time);
            set_pos_state = 1;
            break;

        case 1: // wait for feedback
            // receive error
            ret = ctrl_udp_socket->ReceiveData_rt(recv_data_str);
            if (ret < 0) {
                FSA_CONNECT::LOG::INFO("MOTOR: {}, UDP SOCKET RECEIVE FAILED! ERROR CODE: {}", ip_, ret);
                set_pos_state = 0;
                return ret;
            }
            // receive something
            if (!recv_data_str.empty()) {
                recv_data_json = json::parse(recv_data_str);
                receive_state = recv_data_json.at("status");
                //!="OK"
                if (receive_state.compare("OK")) {
                    set_pos_state = 0;
                    FSA_CONNECT::LOG::INFO("MOTOR: {}, SET CURRENT FAILED! ", ip_);

                    return DISABLE_FAILED;
                }
                set_pos_state = 0;
                pos = recv_data_json.at("position");
                vel = recv_data_json.at("velocity");
                cur = recv_data_json.at("current");
                // FSA_CONNECT::LOG::INFO("MOTOR: {},  SET POS SUCCESS! ", ip_);
                return SUCCESS;
            }

            // clock_gettime(CLOCK_MONOTONIC,&now_time);
            end = std::chrono::steady_clock::now();
            // time out
            int_ms = chrono::duration_cast<chrono::milliseconds>(end - begin);
            if (int_ms.count() > 3000) {
                set_pos_state = 0;
                FSA_CONNECT::LOG::INFO("MOTOR: {}, SET CURRENT TIMEOUT! ", ip_);
                return TIMEOUT;
            }
            break;

        default:
            set_pos_state = 0;
            break;
        };
    }

    return NOT_EXECUTE;
};