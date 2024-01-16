#include "fsa.h"

using namespace Predefine;
using namespace Utils;
using namespace Actuator;

FSA::FSA(/* args */) {}

FSA::~FSA() {}

// =====================================================================================================

int FSA::fi_init_network()
{
    struct timeval tv;
    tv.tv_sec = 1;
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
        usleep(100);
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
        Logger::get_instance()->print_trace("init fsa\n");
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
    Logger::get_instance()->print_trace_warning("send json-> %s\n", this->send_buff);

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
                    Logger::get_instance()->print_trace("broadcast recvfrom() successfully\n");
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
        Logger::get_instance()->print_trace("error code : %s\n", client_recv_msg);
        if (this->msg_json.Parse(client_recv_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("decode failed\n");
            return FunctionResult::FAILURE;
        }
        Logger::get_instance()->print_trace_debug("error code : %d\n", msg_json["error_code"].GetInt());
        // clear error code
        if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_clear_fault, client_recv_msg)))
        {
            // clear error code successfully
            Logger::get_instance()->print_trace("clear fault: %s\n", client_recv_msg);
            return FunctionResult::SUCCESS;
        }
        return FunctionResult::FAILURE;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_comm_config_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_comm_config_get\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_get_comm_config, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_comm_config_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_comm_config_set\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_get_comm_config, client_recv_msg)))
    {
        // getting current config now
        Logger::get_instance()->print_trace("get_comm_config: %s\n", client_recv_msg);
        memset(client_recv_msg, 0, BUFFER_SIZE);
        sleep(1);
        if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, define_msg_sendto, client_recv_msg)))
        {
            // setting config
            Logger::get_instance()->print_trace("set_comm_config feedback: %s\n", client_recv_msg);
            memset(client_recv_msg, 0, BUFFER_SIZE);
            sleep(1);
            if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_get_comm_config, client_recv_msg)))
            {
                // check current config after setting
                Logger::get_instance()->print_trace("get_comm_config: %s\n", client_recv_msg);
                memset(client_recv_msg, 0, BUFFER_SIZE);
                sleep(1);
                if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_reboot, client_recv_msg)))
                {
                    // reboot update
                    Logger::get_instance()->print_trace("reboot feedback: %s\n", client_recv_msg);
                    return FunctionResult::SUCCESS;
                }
                Logger::get_instance()->print_trace_error("reboot failed\n");
                return FunctionResult::FAILURE;
            }
            Logger::get_instance()->print_trace_error("get_comm_config failed after setting it\n");
            return FunctionResult::FAILURE;
        }
        Logger::get_instance()->print_trace_error("set_comm_config failed\n");
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace_error("get_comm_config failed \n");
    return FunctionResult::FAILURE;
}

int FSA::demo_control_current_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_control_param_imm_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_control_param_imm_get\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_control_param_imm, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_control_param_imm_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_control_param_imm_set\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_control_param_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_comm_config_get\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_control_param, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_control_param_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_control_param_set\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_control_position_ff_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_control_position_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}
int FSA::demo_control_velocity_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_ctrl_config_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ctrl_config_get\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_config, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_ctrl_config_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) 
{
    Logger::get_instance()->print_trace_debug("demo_ctrl_config_set\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_ctrl_config_save(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) 
{
    Logger::get_instance()->print_trace_debug("demo_ctrl_config_get\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_save_config, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_disable_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_disable_set\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_set_disenable, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_enable_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_enable_set\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_set_enable, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_flag_of_operation_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) 
{
    Logger::get_instance()->print_trace_debug("demo_flag_of_operation_get\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_flag_of_operation, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_flag_of_operation_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_get_abs_encoder_value(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) 
{
    Logger::get_instance()->print_trace_debug("demo_get_abs_encoder_value\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_abs_encoder_angle, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_get_measured(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) {}

int FSA::demo_get_pvc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_get_pvc\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_pvc, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_get_pvcc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_get_pvcc\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_pvcc, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_get_pvcccc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_get_pvcccc\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_pvcccc, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_get_state(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_get_state\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_state, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_home_offset_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) 
{
    Logger::get_instance()->print_trace_debug("demo_get_state\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_home_offset, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_home_offset_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) 
{
    Logger::get_instance()->print_trace_debug("demo_get_state\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_home_position_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) 
{
    Logger::get_instance()->print_trace_debug("demo_get_state\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_set_home_position, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_lookup_abs_encoder(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_lookup_actuator\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_get_root, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_lookup_actuator(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_lookup_actuator\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_get_root, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_lookup_ctrlbox(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_lookup_ctrlbox\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_get_root, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_lookup(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_lookup\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_get_root, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_new_motor_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_new_motor_test\n");
    // this->work_mode = WorkMode::SIGLE_MODE;
    // if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota_cloud, client_recv_msg)))
    // {
    //     return FunctionResult::SUCCESS;
    // }
    return FunctionResult::FAILURE;
}

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
int FSA::demo_ota_devel(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ota_devel()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota_driver_devel, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_ota_driver_cloud(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ota_driver_cloud()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota_driver_cloud, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
int FSA::demo_ota_driver_devel(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ota_driver_devel()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota_driver_devel, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_ota_driver_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ota_driver_devel()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota_driver_test, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_ota_driver(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_ota_driver()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, this->json_ota_driver, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_pid_param_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_pid_param_get()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_pid_param, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_pid_param_imm_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_pid_param_imm_get()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_get_pid_imm_param, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_pid_param_imm_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_pid_param_imm_set()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
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

int FSA::demo_reboot_actuator(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_reboot()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_reboot, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_set_calibrate_encoder(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg) 
{
    Logger::get_instance()->print_trace_debug("demo_set_calibrate_encoder()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, this->json_set_calibrate_encoder, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::demo_set_encrypt(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("demo_set_encrypt()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_COMM, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::interface_set_current_control(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("interface_set_current_control()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::interface_set_mode_operation(std::string sigle_ip, int mode, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("interface_set_mode_operation()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    char *define_msg_sendto = "";
    switch (mode)
    {
    case 0:
        define_msg_sendto = "{\"method\":\"SET\", \
            \"reqTarget\":\"/mode_of_operation\", \
            \"mode_of_operation\":0}";
        break;
    case 1:
        define_msg_sendto = "{\"method\":\"SET\", \
            \"reqTarget\":\"/mode_of_operation\", \
            \"mode_of_operation\":1}";
        break;
    case 3:
        define_msg_sendto = "{\"method\":\"SET\", \
            \"reqTarget\":\"/mode_of_operation\", \
            \"mode_of_operation\":3}";
        break;
    case 4:
        define_msg_sendto = "{\"method\":\"SET\", \
            \"reqTarget\":\"/mode_of_operation\", \
            \"mode_of_operation\":4}";
        break;
    case 5:
        define_msg_sendto = "{\"method\":\"SET\", \
            \"reqTarget\":\"/mode_of_operation\", \
            \"mode_of_operation\":5}";
        break;

    default:
        Logger::get_instance()->print_trace_error("Please set really mode\n");
        return FunctionResult::FAILURE;
    }
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::interface_set_position_control(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("interface_set_position_control()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::interface_set_velocity_control(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg)
{
    Logger::get_instance()->print_trace_debug("interface_set_velocirt_control()\n");
    this->work_mode = WorkMode::SIGLE_MODE;
    if (!(this->fi_fsa_comm(sigle_ip, SERVER_PORT_CTRL, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

// FSA INTERFACE END
