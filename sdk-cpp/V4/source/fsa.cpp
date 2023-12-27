#include "fsa.h"

using namespace Predefine;
using namespace Utils;
using namespace Sensor;

FSA::FSA(/* args */) {}

FSA::~FSA() {}

// =====================================================================================================

int FSA::fi_init_network()
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10;

    // left
    this->fsa_socket = socket(AF_INET, SOCK_DGRAM, 0);
    short set_sock_opt_flg = setsockopt(this->fsa_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (set_sock_opt_flg == -1)
    {
        Logger::get_instance()->print_trace_error("Error: fi_init_network() setsockopt() failed\n");
        return FunctionResult::FAILURE;
    }
    // COMM
    this->fsa_sockaddr_in_comm.sin_family = AF_INET;
    this->fsa_sockaddr_in_comm.sin_port = htons(SERVER_PORT_COMM);
    inet_pton(AF_INET, SERVER_IP, &this->fsa_sockaddr_in_comm.sin_addr.s_addr);
    // CTRL
    this->fsa_sockaddr_in_ctrl.sin_family = AF_INET;
    this->fsa_sockaddr_in_ctrl.sin_port = htons(SERVER_PORT_CTRL);
    inet_pton(AF_INET, SERVER_IP, &this->fsa_sockaddr_in_ctrl.sin_addr.s_addr);
    return FunctionResult::SUCCESS;
}

int FSA::fi_init_fse()
{
    if (!(this->fi_init_network()))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::fi_fsa_comm(std::string ip, int port, char *msg, char *client_recv_msg)
{
    if (!(this->fi_send_msg(ip, port, msg)))
    {
        usleep(400);
        if (!(this->fi_recv_msg(client_recv_msg)))
        {
            return FunctionResult::SUCCESS;
        }
        return FunctionResult::FAILURE;
    }
    return FunctionResult::FAILURE;
}

int FSA::fi_decode(char *msg)
{
    Logger::get_instance()->print_trace("decode-> %s\n", msg);
    usleep(100);
    if (this->msg_json.Parse(msg).HasParseError())
    {
        Logger::get_instance()->print_trace_error("decode failed\n");
        return FunctionResult::FAILURE;
    }
    switch (this->work_mode)
    {
    case WorkMode::NONE:
        break;
    case WorkMode::BROADCASE_FILTER_MODE:
    {
        if (this->msg_json["type"].GetString() == this->server_filter_type)
        {
            this->server_ip_filter[this->server_ip_filter_num] = this->msg_json["IP"].GetString();
            this->server_ip_filter_num++;
        }
        break;
    }
    case WorkMode::BROADCAST_MODE:
    {
        break;
    }
    case WorkMode::SIGLE_MODE:
    {
        break;
    }
    default:
        break;
    }
    return FunctionResult::SUCCESS;
}

int FSA::fi_encode()
{
    Logger::get_instance()->print_trace("encode data\n");
    return FunctionResult::SUCCESS;
}

int FSA::fi_send_msg(std::string ip, int port, char *msg)
{
    if (!this->is_init)
    {
        this->fi_init_fse();
        Logger::get_instance()->print_trace_debug("init fse\n");
        this->is_init = 1;
        usleep(20);
    }
    int ret = -1;

    memset(this->send_buff, 0, sizeof(this->send_buff));
    mempcpy(this->send_buff, msg, sizeof(this->send_buff));
    memset(this->server_ip_filter, 0, sizeof(this->server_ip_filter));
    memset(this->server_ip, 0, sizeof(this->server_ip));
    this->server_ip_filter_num = 0;
    this->server_ip_num = 0;
    Logger::get_instance()->print_trace("send json-> %s\n", this->send_buff);

    switch (this->work_mode)
    {
    case WorkMode::NONE:
        /* code */
        break;
    case WorkMode::BROADCASE_FILTER_MODE:
    case WorkMode::BROADCAST_MODE:
    {
        ret = sendto(this->fsa_socket, this->send_buff, BUFFER_SIZE, 0, (struct sockaddr *)&this->fsa_sockaddr_in_comm, sizeof(this->fsa_sockaddr_in_comm));
        if (ret == -1)
        {
            Logger::get_instance()->print_trace_error("broadcast sendto() failed\n");
            return FunctionResult::FAILURE;
        }
        break;
    }
    case WorkMode::SIGLE_MODE:
    {
        this->work_ip = &*ip.begin();
        this->fsa_sockaddr_in_work.sin_family = AF_INET;
        this->fsa_sockaddr_in_work.sin_port = htons(port);
        inet_pton(AF_INET, this->work_ip, &this->fsa_sockaddr_in_work.sin_addr.s_addr);
        usleep(1);
        ret = sendto(this->fsa_socket, this->send_buff, BUFFER_SIZE, 0, (struct sockaddr *)&this->fsa_sockaddr_in_work, sizeof(this->fsa_sockaddr_in_work));
        if (ret == -1)
        {
            Logger::get_instance()->print_trace_error("sigle sendto() failed\n");
            return FunctionResult::FAILURE;
        }
        break;
    }
    default:
        break;
    }
    return FunctionResult::SUCCESS;
}

int FSA::fi_recv_msg(char *client_recv_msg)
{
    int ret = -1;
    memset(this->recv_buff, 0, sizeof(this->recv_buff));

    switch (this->work_mode)
    {
    case WorkMode::NONE:
        break;
    case WorkMode::BROADCASE_FILTER_MODE:
    case WorkMode::BROADCAST_MODE:
    {
        this->server_ip_num = 0;
        while (1)
        {
            ret = recvfrom(this->fsa_socket, this->recv_buff, sizeof(this->recv_buff), 0, (struct sockaddr *)&this->fsa_sockaddr_in_recv, &this->sockaddr_len);
            if (ret == -1)
            {
                if (this->server_ip_num == 0)
                {
                    Logger::get_instance()->print_trace_error("broadcast recvfrom() failed\n");
                    return FunctionResult::FAILURE;
                }
                else
                {
                    Logger::get_instance()->print_trace_debug("broadcast recvfrom() successfully\n");
                    return FunctionResult::SUCCESS;
                }
                break;
            }
            else
            {
                char *ip_sigle = inet_ntoa(this->fsa_sockaddr_in_recv.sin_addr);
                this->server_ip[this->server_ip_num] = ip_sigle;
                this->server_ip_num++;
                if (this->fi_decode(this->recv_buff) != FunctionResult::SUCCESS)
                {
                    Logger::get_instance()->print_trace_error("fi_decode(): failed");
                }
            }
            memset(this->recv_buff, 0, sizeof(this->recv_buff));
        }
        break;
    }
    case WorkMode::SIGLE_MODE:
    {
        ret = recvfrom(this->fsa_socket, this->recv_buff, sizeof(this->recv_buff), 0, NULL, NULL);
        if (ret == -1)
        {
            Logger::get_instance()->print_trace_error("sigle recvfrom() failed\n");
            break;
        }
        else
        {
            memcpy(client_recv_msg, this->recv_buff, sizeof(this->recv_buff));
        }
        break;
    }
    default:
        break;
    }
    return FunctionResult::SUCCESS;
}

// =====================================================================================================

// FSA BASIC FUNC START
int FSA::get_root()
{
}
int FSA::set_enable() {}
int FSA::set_disenable() {}
int FSA::set_calibrate_encoder() {}
int FSA::clear_fault() {}
int FSA::get_state() {}
int FSA::set_mode_of_operation() {}
int FSA::get_home_offset() {}
int FSA::set_home_offset() {}
int FSA::set_home_position() {}
int FSA::get_pid_param() {}
int FSA::set_pid_param() {}
int FSA::clear_pid_param() {}
int FSA::set_pid_param_imm() {}
int FSA::get_control_param() {}
int FSA::set_control_param() {}
int FSA::get_control_param_imm() {}
int FSA::set_control_param_imm() {}
int FSA::get_flag_of_operation() {}
int FSA::set_flag_of_operation() {}
int FSA::clear_flag_of_operation() {}
int FSA::get_config() {}
int FSA::set_config() {}
int FSA::save_config() {}
int FSA::erase_config() {}
int FSA::reboot() {}
int FSA::get_error_code() {}
int FSA::get_pvc() {}
int FSA::get_pvcc() {}
int FSA::get_pvcccc() {}
int FSA::set_position_control() {}
int FSA::set_velocity_control() {}
int FSA::set_current_control() {}
int FSA::set_torque_control() {}
// GROUP START
int FSA::enable_group() {}
int FSA::disable_group() {}
int FSA::get_state_group() {}
int FSA::get_error_group() {}
int FSA::clear_error_group() {}
int FSA::get_pvc_group() {}
int FSA::set_mode_of_operation_group() {}
int FSA::reboot_comm_group() {}
// GROUP END
int FSA::get_comm_root() {}
int FSA::get_comm_config() {}
int FSA::set_comm_config() {}
int FSA::save_comm_config() {}
int FSA::reboot_comm() {}
int FSA::get_abs_encoder_angle() {}
int FSA::broadcast_func() {}
int FSA::broadcast_func_with_filter() {}
// OTA START
int FSA::ota() {}
int FSA::ota_test() {}
int FSA::ota_devel() {}
int FSA::ota_cloud() {}
int FSA::ota_driver() {}
int FSA::ota_driver_test() {}
int FSA::ota_driver_devel() {}
int FSA::ota_driver_cloud() {}
// OTA END
int FSA::encrypt() {}
// FSA BASIC FUNC END

// =====================================================================================================

// FSA INTERFACE START
int FSA::demo_broadcase()
{
    Logger::get_instance()->print_trace_debug("broadcase all\n");
    this->work_mode = WorkMode::BROADCAST_MODE;
    if (!(this->fi_fsa_comm(SERVER_IP, SERVER_PORT_COMM, this->json_broadcast_msg, NULL)))
    {
        Logger::get_instance()->print_trace_debug("broadcase finished\n");
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_broadcase_filter(std::string filter_type)
{
    Logger::get_instance()->print_trace_debug("broadcase all with filter\n");
    this->work_mode = WorkMode::BROADCASE_FILTER_MODE;
    this->server_filter_type = filter_type;
    if (!(this->fi_fsa_comm(SERVER_IP, SERVER_PORT_COMM, this->json_broadcast_msg, NULL)))
    {
        Logger::get_instance()->print_trace_debug("filter broadcase finished\n");
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_clear_fault(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_clear_fault\n");
    this->work_mode = WorkMode::SIGLE_MODE;

    // get error code
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_error_code, client_recv_msg)))
    {
        Logger::get_instance()->print_trace("error code : %s", client_recv_msg);
        // clear error code
        if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_clear_fault, client_recv_msg)))
        {
            // clear error code successfully
            return FunctionResult::SUCCESS;
        }
        return FunctionResult::FAILURE;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_comm_config_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_comm_config_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_control_current_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_control_param_imm_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_control_param_imm_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_control_param_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_control_param_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_control_position_ff_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_control_position_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_control_velocity_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_ctrl_config_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_ctrl_config_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_comm_config_save(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_disable_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_enable_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_flag_of_operation_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_flag_of_operation_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_get_abs_encoder_value(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_get_measured(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_get_pvc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_get_pvcc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_get_pvcccc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_get_state(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_home_offset_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_home_offset_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_home_position_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_lookup_abs_encoder(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_lookup_actuator(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_lookup_ctrlbox(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_lookup(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_new_motor_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_ota_cloud(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ota\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota_cloud, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_ota_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ota_test\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota_test, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_ota(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ota\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_ota_devel(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_ota_driver_cloud(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_ota_driver_devel(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_ota_driver_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_ota_driver(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_pid_param_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_pid_param_imm_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_pid_param_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_reboot(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_reboot()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_reboot, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_reboot_actuator(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_set_calibrate_encoder(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_set_encrypt(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

// FSA INTERFACE END
