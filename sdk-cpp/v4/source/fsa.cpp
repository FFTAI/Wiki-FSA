#include "fsa.h"
#include <pthread.h>

using namespace Predefine;
using namespace Utils;
using namespace Actuator;

FSA::FSA(/* args */) {}

FSA::~FSA() {}

// =====================================================================================================

int FSA::init_network()
{
    struct timeval timeout;

    timeout.tv_sec = 1;
    timeout.tv_usec = 10;

    int tv = 1;

    this->broadcast_socket = socket(AF_INET, SOCK_DGRAM, 0);
    short set_sock_opt_flg = setsockopt(this->broadcast_socket, SOL_SOCKET, SO_BROADCAST, &tv, sizeof(tv));
    if (set_sock_opt_flg == -1)
    {
        Logger::get_instance()->print_trace_error("Error: init_network() setsockopt() failed\n");
        return FunctionResult::FAILURE;
    }

    this->fsa_socket = socket(AF_INET, SOCK_DGRAM, 0);
    set_sock_opt_flg = setsockopt(this->fsa_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if (set_sock_opt_flg == -1)
    {
        Logger::get_instance()->print_trace_error("Error: init_network() setsockopt() failed\n");
        return FunctionResult::FAILURE;
    }

    // COMM
    this->fsa_sockaddr_in_comm.sin_family = AF_INET;
    this->fsa_sockaddr_in_comm.sin_port = htons(SERVER_PORT_COMM);
    inet_pton(AF_INET, this->broadcast_ip, &this->fsa_sockaddr_in_comm.sin_addr.s_addr);

    // CTRL
    this->fsa_sockaddr_in_ctrl.sin_family = AF_INET;
    this->fsa_sockaddr_in_ctrl.sin_port = htons(SERVER_PORT_CTRL);
    inet_pton(AF_INET, this->broadcast_ip, &this->fsa_sockaddr_in_ctrl.sin_addr.s_addr);

    return FunctionResult::SUCCESS;
}

int FSA::communicate(char *ip, int port, char *msg, char *client_recv_msg)
{
    if (!(this->send_msg(ip, port, msg)))
    {
        usleep(100);
        if (!(this->recv_msg(client_recv_msg)))
        {
            return FunctionResult::SUCCESS;
        }
        return FunctionResult::FAILURE;
    }
    return FunctionResult::FAILURE;
}

int FSA::decode(char *msg)
{
    // usleep(100);
    if (this->msg_json.Parse(msg).HasParseError())
    {
        Logger::get_instance()->print_trace_error("decode failed\n");
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("decode: \n\t ip: %s \n\t filter_type: %s \n\t msg: %s \n", this->msg_json["IP"].GetString(), this->server_filter_type, msg);

    switch (this->work_mode)
    {
    case WorkMode::NONE:
        break;
    case WorkMode::BROADCASE_FILTER_MODE:
    {
        std::string filter_type(this->server_filter_type);
        if (this->msg_json["type"].GetString() == filter_type)
        {
            std::string ip = this->msg_json["IP"].GetString();
            this->server_ip_filter[this->server_ip_filter_num] = ip;
            this->server_ip_filter_num++;
        }

        return FunctionResult::SUCCESS;
        break;
    }
    case WorkMode::BROADCAST_MODE:
    {
        break;
    }
    case WorkMode::SERVER_IP_MODE:
    {
        break;
    }
    default:
        break;
    }
    Logger::get_instance()->print_trace_debug("%s have %d servers\n", this->server_filter_type, this->server_ip_filter_num);
    return FunctionResult::SUCCESS;
}

int FSA::encode()
{
    Logger::get_instance()->print_trace("encode data\n");
    return FunctionResult::SUCCESS;
}

int FSA::send_msg(char *ip, int port, char *msg)
{
    if (!this->is_init)
    {
        this->init_network();
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
        ret = sendto(this->broadcast_socket, this->send_buff, BUFFER_SIZE, 0, (struct sockaddr *)&this->fsa_sockaddr_in_comm, sizeof(this->fsa_sockaddr_in_comm));
        if (ret == -1)
        {
            Logger::get_instance()->print_trace_error("broadcast sendto() failed\n");
            return FunctionResult::FAILURE;
        }
        break;
    }
    case WorkMode::SERVER_IP_MODE:
    {
        this->work_ip = ip;
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

int FSA::recv_msg(char *client_recv_msg)
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
            ret = recvfrom(this->broadcast_socket, this->recv_buff, sizeof(this->recv_buff), 0, (struct sockaddr *)&this->fsa_sockaddr_in_recv, &this->sockaddr_len);
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
                if (this->decode(this->recv_buff) != FunctionResult::SUCCESS)
                {
                    Logger::get_instance()->print_trace_error("decode(): failed");
                }
            }
            memset(this->recv_buff, 0, sizeof(this->recv_buff));
        }
        return FunctionResult::SUCCESS;
        break;
    }
    case WorkMode::SERVER_IP_MODE:
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

int FSA::init()
{
    Logger::get_instance()->print_trace_debug("init()\n");

    this->init_network();

    return FunctionResult::SUCCESS;
}

// FSA INTERFACE START
int FSA::broadcast()
{
    Logger::get_instance()->print_trace_debug("broadcast all\n");
    this->work_mode = WorkMode::BROADCAST_MODE;
    if (!(this->communicate((char *)this->broadcast_ip, SERVER_PORT_COMM, this->json_broadcast_msg, NULL)))
    {
        Logger::get_instance()->print_trace_debug("broadcast finished\n");
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::broadcast_filter()
{
    Logger::get_instance()->print_trace_debug("broadcast all with filter\n");
    this->work_mode = WorkMode::BROADCASE_FILTER_MODE;

    if (!(this->communicate((char *)this->broadcast_ip, this->server_port_comm, this->json_broadcast_msg, NULL)))
    {
        Logger::get_instance()->print_trace_debug("filter broadcast finished\n");
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::clear_fault(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_error_code, client_recv_msg)))
    {
        if (this->msg_json.Parse(client_recv_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("Json parse failed\n");
            return FunctionResult::FAILURE;
        }
        Logger::get_instance()->print_trace_debug("error code : %d\n", msg_json["error_code"].GetInt());
        // clear error code
        if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_clear_fault, client_recv_msg)))
        {
            if (this->msg_json.Parse(client_recv_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("Json parse failed\n");
                return FunctionResult::FAILURE;
            }
            std::string right_flag = "OK";
            if (msg_json["status"].GetString() == right_flag)
            {
                return FunctionResult::SUCCESS;
            }
            return FunctionResult::FAILURE;
        }
        return FunctionResult::FAILURE;
    }
    return FunctionResult::FAILURE;
}

int FSA::comm_config_get(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_get_comm_config, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::comm_config_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;

    if (!(this->communicate(server_ip, this->server_port_comm, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

// int FSA::control_current_mode(char* server_ip, char *define_msg_sendto, char *client_recv_msg);

int FSA::control_param_imm_get(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_control_param_imm, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::control_param_imm_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::control_param_get(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_control_param, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::control_param_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ctrl_config_get(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_config, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ctrl_config_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ctrl_config_save(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_save_config, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::disable_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_set_disenable, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::enable_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_set_enable, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::flag_of_operation_get(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_flag_of_operation, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::flag_of_operation_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::get_abs_encoder_value(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_abs_encoder_angle, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::get_measured(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_pvc, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::get_pvc(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_pvc, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::get_pvcc(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_pvcc, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::get_pvcccc(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_pvcccc, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::get_state(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_state, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::home_offset_get(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_home_offset, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::home_offset_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::home_position_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_set_home_position, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::lookup_abs_encoder(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_get_root, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::lookup_actuator(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_get_root, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::lookup_ctrlbox(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_get_root, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::lookup(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_get_root, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ota_cloud(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_ota_cloud, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ota_test(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_ota_test, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ota(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_ota, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ota_devel(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_ota_driver_devel, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ota_driver_cloud(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_ota_driver_cloud, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ota_driver_devel(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_ota_driver_devel, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ota_driver_test(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_ota_driver_test, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::ota_driver(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, this->json_ota_driver, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::pid_param_get(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_pid_param, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::pid_param_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::pid_param_imm_get(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_get_pid_imm_param, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::pid_param_imm_set(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::reboot(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_reboot, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::reboot_actuator(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_reboot, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::set_calibrate_encoder(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, this->json_set_calibrate_encoder, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::set_encrypt(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_comm, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::set_mode_of_operation(char *server_ip, int mode, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
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
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::set_position_control(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::set_velocity_control(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int FSA::set_current_control(char *server_ip, char *define_msg_sendto, char *client_recv_msg)
{
    this->work_mode = WorkMode::SERVER_IP_MODE;
    if (!(this->communicate(server_ip, this->server_port_ctrl, define_msg_sendto, client_recv_msg)))
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

// FSA INTERFACE END

// =================================user command================================
FSA *fsa = new FSA();

/// @brief 将整数转化为字符串ip格式
/// @param int_array
/// @return string ip
std::string Actuator::int_array_to_ip(const std::vector<int> &int_array)
{
    std::ostringstream oss;
    for (
        size_t i = 0; i < int_array.size(); ++i)
    {
        if (i > 0)
        {
            oss << ".";
        }
        oss << int_array[i];
    }
    return oss.str();
}

/// @brief 广播线程，设置为1s结束广播
/// @param pv
/// @return
void *broadcast_recv(void *pv)
{
    fsa->broadcast();
}

/// @brief 广播线程
/// @param pv
/// @return
void *broadcast_filter_recv(void *pv)
{
    fsa->broadcast_filter();
}

/// @brief 广播发现所有ip地址，1s结束广播
/// @param ip_ser 保存ip数组指针
/// @param ser_num 发现ip数目
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::broadcast(char *ip_ser, int &ser_num)
{
    memset(ip_ser, 0, sizeof(ip_ser));
    ser_num = 0;

    pthread_t boreadcast_thread_pd;
    pthread_create(&boreadcast_thread_pd, NULL, broadcast_recv, NULL);
    pthread_detach(boreadcast_thread_pd);
    sleep(1);

    memcpy(ip_ser, fsa->server_ip, sizeof(ip_ser));
    ser_num = fsa->server_ip_num;
    pthread_cancel(boreadcast_thread_pd);
    if (ser_num > 0)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 广播发现所有ip地址，1s结束广播
/// @param ip_ser 保存ip数组指针
/// @param ser_num 发现ip数目
/// @param type 回复ip筛选，可设置参数：ABSCODER、ACTUATOR、CTRLBOX
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::broadcast(char *ip_ser, int &ser_num, char *type)
{
    fsa->server_filter_type = type;

    memset(ip_ser, 0, sizeof(ip_ser));
    ser_num = 0;

    pthread_t boreadcast_thread_pd;
    pthread_create(&boreadcast_thread_pd, NULL, broadcast_filter_recv, NULL);
    pthread_detach(boreadcast_thread_pd);
    usleep(10000);

    memcpy(ip_ser, fsa->server_ip_filter, sizeof(ip_ser));
    ser_num = fsa->server_ip_filter_num;
    pthread_cancel(boreadcast_thread_pd);
    if (ser_num > 0)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 对应ip执行器错误清除，终端打印发现错误id
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::clear_fault(char *ip)
{
    char recvmsg[1024] = {0};
    return fsa->clear_fault(ip, NULL, recvmsg);
}

/// @brief 获取通信配置
/// @param ip
/// @param config_buffer rapidjson文件格式json,定义：rapidjson::Document msg_json;
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::comm_config_get(char *ip, rapidjson::Document *config_buffer)
{
    char comm_config_buffer[1024] = {0};
    if (fsa->comm_config_get(ip, NULL, comm_config_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s communication failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("communication reply: %s\n", comm_config_buffer);
    if (config_buffer->Parse(comm_config_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    return FunctionResult::SUCCESS;
}

/// @brief 修改电机通信配置参数
/// @param ip 当前设置电机ip
/// @param name 当前设置电机名称 不修改则 “”
/// @param dhcp_enable 是否自动分配ip true or false, 默认false
/// @param ssid wifi名称 不修改则为 ”“
/// @param passworld wifi密码 不修改则为 ”“
/// @param static_ip 电机ip："192.168.137.static_ip"，不修改则填入0
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::comm_config_set(char *ip, std::string name, bool dhcp_enable, std::string ssid, std::string password, uint8_t static_ip)
{
    // 获取电机配置，为了能够在用户设置非法参数或者未设置对应参数作为合法参数输入
    rapidjson::Document get_config_json;
    if (comm_config_get(ip, &get_config_json) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("get config failed\n");
        return FunctionResult::FAILURE;
    }

    // 通过输入参数和本身协议组成json
    rapidjson::StringBuffer config_buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(config_buffer);

    writer.StartObject();
    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/config");

    writer.Key("property");
    writer.String("");

    writer.Key("name");
    if (name != "")
    {
        writer.String(name.c_str());
    }
    else
    {
        writer.String(get_config_json["name"].GetString());
    }

    writer.Key("DHCP_enable");
    if (dhcp_enable != true)
    {
        writer.Bool(false);
    }
    else
    {
        writer.Bool(true);
    }

    writer.Key("SSID");
    if (ssid != "")
    {
        writer.String(ssid.c_str());
    }
    else
    {
        writer.String(get_config_json["SSID"].GetString());
    }

    writer.Key("password");
    if (ssid != "")
    {
        writer.String(password.c_str());
    }
    else
    {
        writer.String(get_config_json["password"].GetString());
    }

    writer.Key("static_IP");
    writer.StartArray();
    writer.Int(get_config_json["static_IP"][0].GetUint());
    writer.Int(get_config_json["static_IP"][1].GetInt());
    writer.Int(get_config_json["static_IP"][2].GetInt());
    if ((static_ip > 0) && (static_ip < 255))
    {
        writer.Int(static_ip);
    }
    else
    {
        writer.Int(get_config_json["static_IP"][3].GetInt());
    }
    writer.EndArray();

    writer.Key("gateway");
    writer.StartArray();
    writer.Int(192);
    writer.Int(168);
    writer.Int(137);
    writer.Int(1);
    writer.EndArray();

    writer.Key("subnet_mask");
    writer.StartArray();
    writer.Int(255);
    writer.Int(255);
    writer.Int(255);
    writer.Int(0);
    writer.EndArray();

    writer.Key("dns_1");
    writer.StartArray();
    writer.Int(114);
    writer.Int(114);
    writer.Int(114);
    writer.Int(114);
    writer.EndArray();

    writer.Key("dns_2");
    writer.StartArray();
    writer.Int(8);
    writer.Int(8);
    writer.Int(8);
    writer.Int(8);
    writer.EndArray();

    writer.EndObject();

    char set_ctrl_config[1024] = {0}, set_feedback[1024] = {0};
    memset(set_ctrl_config, 0, sizeof(set_ctrl_config));
    memcpy(set_ctrl_config, config_buffer.GetString(), config_buffer.GetSize());

    // 发送配置通信参数
    if (fsa->comm_config_set(ip, set_ctrl_config, set_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_debug("set %s communication failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    rapidjson::Document fed_json;
    if (fed_json.Parse(set_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("set communication feedback failed\n");
        return FunctionResult::FAILURE;
    }
    
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 获取执行器状态
/// @param ip 执行器ip
/// @param state_val 状态存储变量
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::get_state(char *ip, int &state_val)
{
    char get_state_buffer[1024] = {0};
    if (fsa->get_state(ip, NULL, get_state_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s state failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document msg_json;
    if (msg_json.Parse(get_state_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    state_val = msg_json["state"].GetInt();
    return FunctionResult::SUCCESS;
}

/// @brief 执行器控制参数获取
/// @param ip
/// @param config_buffer 获取执行器参数存储变量，json调用
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ctrl_config_get(char *ip, rapidjson::Document *config_buffer)
{
    char ctrl_config_buffer[1024] = {0};
    if (fsa->ctrl_config_get(ip, NULL, ctrl_config_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s control config failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("server reply: %s\n", ctrl_config_buffer);
    if (config_buffer->Parse(ctrl_config_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    return FunctionResult::SUCCESS;
}

/// @brief 配置执行器控制参数
/// @param ip 对象ip
/// @param act_type FSAActuatorType 设置类中对应参数
/// @param dir FSAActuatorDirection  设置类中对应参数
/// @param redTat FSAActuatorReductionRatio 设置类中对应参数
/// @param motor_type FSAMotorType 设置类中对应参数
/// @param hardware_type FSAHardwareType 设置类中对应参数
/// @param vbus FSAMotorVBUS 设置类中对应参数
/// @param motor_dir FSAMotorDirection 设置类中对应参数
/// @param pole_pairs FSAMotorPolePairs 设置类中对应参数
/// @param max_speed FSAMotorMaxSpeed 设置类中对应参数
/// @param max_acc FSAMotorMaxAcceleration 设置类中对应参数
/// @param encoder_dir FSAEncoderDirection 设置类中对应参数
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ctrl_config_set(char *ip, uint64_t act_type, int dir, int redTat, int motor_type, int hardware_type, int vbus, int motor_dir, int pole_pairs, int max_speed, int max_acc, int encoder_dir)
{
    char ctrl_config_buffer[1024] = {0};
    if (fsa->ctrl_config_get(ip, NULL, ctrl_config_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s control config failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("get control config :%s\n", ctrl_config_buffer);

    rapidjson::StringBuffer ctrl_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(ctrl_json);

    writer.StartObject();

    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/config");

    writer.Key("property");
    writer.String("");

    writer.Key("actuator_type");
    writer.Uint64(act_type);

    writer.Key("actuator_direction");
    writer.Int(dir);

    writer.Key("actuator_reduction_ratio");
    writer.Int(redTat);

    writer.Key("motor_type");
    writer.Int(motor_type);

    writer.Key("motor_hardware_type");
    writer.Int(hardware_type);

    writer.Key("motor_vbus");
    writer.Int(vbus);

    writer.Key("motor_direction");
    writer.Int(motor_dir);

    writer.Key("motor_pole_pairs");
    writer.Int(pole_pairs);

    writer.Key("motor_max_speed");
    writer.Int(max_speed);

    writer.Key("motor_max_acceleration");
    writer.Int(max_acc);

    writer.Key("motor_max_current");
    writer.Int(10);

    writer.Key("encoder_direction");
    writer.Int(encoder_dir);
    writer.EndObject();

    memset(ctrl_config_buffer, 0, sizeof(ctrl_config_buffer));
    memcpy(ctrl_config_buffer, ctrl_json.GetString(), ctrl_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->ctrl_config_set(ip, ctrl_config_buffer, feedback_buffer) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s set control config failed", ip);
        return FunctionResult::FAILURE;
    }

    memset(ctrl_config_buffer, 0, sizeof(ctrl_config_buffer));
    if (fsa->ctrl_config_get(ip, NULL, ctrl_config_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s control config failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("get control config :%s\n", ctrl_config_buffer);

    // need reboot
}

/// @brief 保存设置的配置参数，写入文件系统
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ctrl_config_save(char *ip)
{
    char save_config_buffer[1024] = {0};
    if (fsa->ctrl_config_save(ip, NULL, save_config_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("save %s control config failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document msg_json;
    if (msg_json.Parse(save_config_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    std::string flg = "OK";
    if (msg_json["status"].GetString() == flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 获取位置速度电流信息
/// @param ip
/// @param pos
/// @param vel
/// @param cur
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::get_pvc(char *ip, double &pos, double &vel, double &cur)
{
    char get_pvc_buffer[1024] = {0};
    if (fsa->get_pvc(ip, NULL, get_pvc_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s pvc failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("%s\n", get_pvc_buffer);
    rapidjson::Document msg_json;
    if (msg_json.Parse(get_pvc_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    std::string flg = "OK";
    if (msg_json["status"].GetString() == flg)
    {
        pos = msg_json["position"].GetDouble();
        vel = msg_json["velocity"].GetDouble();
        cur = msg_json["current"].GetDouble();
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 获取位置速度电流信息
/// @param ip
/// @param pos
/// @param vel
/// @param cur
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::get_pvcc(char *ip, double &pos, double &vel, double &cur)
{
    char get_pvcc_buffer[1024] = {0};
    if (fsa->get_pvcc(ip, NULL, get_pvcc_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s pvc failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("%s\n", get_pvcc_buffer);
    rapidjson::Document msg_json;
    if (msg_json.Parse(get_pvcc_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    pos = msg_json["position"].GetDouble();
    vel = msg_json["velocity"].GetDouble();
    cur = msg_json["current"].GetDouble();
    std::string flg = "OK";
    if (msg_json["status"].GetString() == flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 获取位置速度电流信息
/// @param ip
/// @param pos
/// @param vel
/// @param cur
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::get_pvcccc(char *ip, double &pos, double &vel, double &cur)
{
    char get_pvcccc_buffer[1024] = {0};
    if (fsa->get_pvcccc(ip, NULL, get_pvcccc_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s pvc failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("%s\n", get_pvcccc_buffer);
    rapidjson::Document msg_json;
    if (msg_json.Parse(get_pvcccc_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    pos = msg_json["position"].GetDouble();
    vel = msg_json["velocity"].GetDouble();
    cur = msg_json["current"].GetDouble();
    std::string flg = "OK";
    if (msg_json["status"].GetString() == flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 设置电流目标值
/// @param ip
/// @param current_val
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::set_current_control(char *ip, double current_val)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();

    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/current_control");

    writer.Key("reply_enable");
    writer.Bool(true);

    writer.Key("current");
    writer.Double(current_val);
    writer.EndObject();

    char set_current_param[1024] = {0};
    memset(set_current_param, 0, sizeof(set_current_param));
    memcpy(set_current_param, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->set_current_control(ip, set_current_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 设置位置控制参数
/// @param ip
/// @param pos
/// @param vel
/// @param acc
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::set_position_control(char *ip, double pos, double vel, double acc)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();

    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/position_control");

    writer.Key("reply_enable");
    writer.Bool(true);

    writer.Key("position");
    writer.Double(pos);

    writer.Key("velocity_ff");
    writer.Double(vel);

    writer.Key("current_ff");
    writer.Double(acc);
    writer.EndObject();

    char set_param[1024] = {0};
    memset(set_param, 0, sizeof(set_param));
    memcpy(set_param, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->set_position_control(ip, set_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::set_velocity_control(char *ip, double vel, double cur_ff)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();

    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/velocity_control");

    writer.Key("reply_enable");
    writer.Bool(true);

    writer.Key("velocity");
    writer.Double(vel);

    writer.Key("current_ff");
    writer.Double(cur_ff);
    writer.EndObject();

    char set_param[1024] = {0};
    memset(set_param, 0, sizeof(set_param));
    memcpy(set_param, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->set_velocity_control(ip, set_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::set_mode_of_operation(char *ip, uint8_t mode)
{
    char feedback_buffer[1024] = {0};
    if (fsa->set_mode_of_operation(ip, mode, feedback_buffer) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s set control config failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 使能电机
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::enable_set(char *ip)
{
    char enable_feedback[1024] = {0};
    if (fsa->enable_set(ip, NULL, enable_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s enable failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(enable_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 失能电机
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::disable_set(char *ip)
{
    char disable_feedback[1024] = {0};
    if (fsa->disable_set(ip, NULL, disable_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s disable failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(disable_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 设置最大速度、最大加速度、最大电流值
/// @param ip
/// @param vel
/// @param acc
/// @param cur
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::control_param_set(char *ip, float vel, float acc, float cur)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();

    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/config");

    writer.Key("property");
    writer.String("");

    writer.Key("name");
    writer.String("FSA");

    writer.Key("motor_max_speed");
    writer.Uint(vel);

    writer.Key("motor_max_acceleration");
    writer.Uint(acc);

    writer.Key("motor_max_current");
    writer.Uint(cur);
    writer.EndObject();

    char set_control_param[1024] = {0};
    memset(set_control_param, 0, sizeof(set_control_param));
    memcpy(set_control_param, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->control_param_set(ip, set_control_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 获取最大速度、最大加速度、最大电流值
/// @param ip
/// @param vel
/// @param acc
/// @param cur
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::control_param_get(char *ip, float &vel, float &acc, float &cur)
{
    char feedback_buffer[1024] = {0};
    if (fsa->control_param_get(ip, NULL, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("get control param reply: %s\n", feedback_buffer);
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        vel = fed_json["motor_max_speed"].GetFloat();
        acc = fed_json["motor_max_acceleration"].GetFloat();
        cur = fed_json["motor_max_current"].GetFloat();
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 获取当前最大速度、最大加速度、最大电流值
/// @param ip
/// @param vel
/// @param acc
/// @param cur
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::control_param_imm_get(char *ip, float &vel, float &acc, float &cur)
{
    char feedback_buffer[1024] = {0};
    if (fsa->control_param_imm_get(ip, NULL, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("get control imm param reply: %s\n", feedback_buffer);
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        vel = fed_json["motor_max_speed_imm"].GetFloat();
        acc = fed_json["motor_max_acceleration_imm"].GetFloat();
        cur = fed_json["motor_max_current_imm"].GetFloat();
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 设置最大速度、最大加速度、最大电流值，立即生效
/// @param ip
/// @param vel
/// @param acc
/// @param cur
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::control_param_imm_set(char *ip, float vel, float acc, float cur)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();

    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/config");

    writer.Key("property");
    writer.String("");

    writer.Key("name");
    writer.String("FSA");

    writer.Key("motor_max_speed");
    writer.Uint(vel);

    writer.Key("motor_max_acceleration");
    writer.Uint(acc);

    writer.Key("motor_max_current");
    writer.Uint(cur);
    writer.EndObject();

    char set_control_param[1024] = {0};
    memset(set_control_param, 0, sizeof(set_control_param));
    memcpy(set_control_param, &set_json, sizeof(set_json));
    char feedback_buffer[1024] = {0};
    if (fsa->control_param_imm_set(ip, set_control_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief ota 通信板卡
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ota(char *ip)
{
    char ota_feedback[1024] = {0};
    if (fsa->ota(ip, NULL, ota_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s ota failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(ota_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief ota 通信板卡
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ota_test(char *ip)
{
    char ota_feedback[1024] = {0};
    if (fsa->ota_test(ip, NULL, ota_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s ota test failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(ota_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief ota 通信板卡
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ota_devel(char *ip)
{
    char ota_feedback[1024] = {0};
    if (fsa->ota_devel(ip, NULL, ota_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s ota devel failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(ota_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief ota 通信板卡
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ota_cloud(char *ip)
{
    char ota_feedback[1024] = {0};
    if (fsa->ota_cloud(ip, NULL, ota_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s ota cloud failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(ota_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief ota 驱动板卡
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ota_driver(char *ip)
{
    char ota_feedback[1024] = {0};
    if (fsa->ota_driver(ip, NULL, ota_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s ota driver failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(ota_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief ota 驱动板卡
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ota_driver_test(char *ip)
{
    char ota_feedback[1024] = {0};
    if (fsa->ota_driver_test(ip, NULL, ota_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s ota driver test failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(ota_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief ota 驱动板卡
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ota_driver_devel(char *ip)
{
    char ota_feedback[1024] = {0};
    if (fsa->ota_driver_devel(ip, NULL, ota_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s ota driver devel failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(ota_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief ota 驱动板卡
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::ota_driver_cloud(char *ip)
{
    char ota_feedback[1024] = {0};
    if (fsa->ota_driver_cloud(ip, NULL, ota_feedback) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s ota driver cloud failed", ip);
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(ota_feedback).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

/// @brief 寻找192.168.137.xx所有server
/// @param ip_ser
/// @param ser_num
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::lookup(char *ip_ser, int &ser_num)
{
    return broadcast(ip_ser, ser_num);
}

/// @brief 寻找 absEncoder sever
/// @param ip_ser
/// @param ser_num
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::lookup_abs_encoder(char *ip_ser, int &ser_num)
{
    return broadcast(ip_ser, ser_num, ABSCODER);
}

/// @brief 寻找 ctrlbox sever
/// @param ip_ser
/// @param ser_num
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::lookup_ctrlbox(char *ip_ser, int &ser_num)
{
    return broadcast(ip_ser, ser_num, CTRLBOX);
}

/// @brief 寻找 actator sever
/// @param ip_ser
/// @param ser_num
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::lookup_actuator(char *ip_ser, int &ser_num)
{
    return broadcast(ip_ser, ser_num, ACTUATOR);
}

int Actuator::pid_param_get(char *ip, rapidjson::Document *config_buffer)
{
    char feedback_buffer[1024] = {0};
    if (fsa->pid_param_get(ip, NULL, feedback_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s communication failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("communication reply: %s\n", feedback_buffer);
    if (config_buffer->Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    return FunctionResult::SUCCESS;
}

int Actuator::pid_param_set(char *ip, double pos_kp, double vel_kp, double vel_ki, double cur_kp, double cur_ki)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();
    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/pid_param");

    writer.Key("property");
    writer.String("");

    writer.Key("control_position_kp");
    writer.Double(pos_kp);

    writer.Key("control_velocity_kp");
    writer.Double(vel_kp);

    writer.Key("control_velocity_ki");
    writer.Double(vel_ki);

    writer.Key("control_current_kp");
    writer.Double(cur_kp);

    writer.Key("control_current_ki");
    writer.Double(cur_ki);
    writer.EndObject();

    char set_pid_control_param[1024] = {0};
    memset(set_pid_control_param, 0, sizeof(set_pid_control_param));
    memcpy(set_pid_control_param, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->control_param_set(ip, set_pid_control_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::pid_param_imm_get(char *ip, rapidjson::Document *config_buffer)
{
    char feedback_buffer[1024] = {0};
    if (fsa->pid_param_imm_get(ip, NULL, feedback_buffer) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_debug("get %s communication failed, check ip, is right?", ip);
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("communication reply: %s\n", feedback_buffer);
    if (config_buffer->Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("Rapidjson parse %s failed\n", ip);
        return FunctionResult::FAILURE;
    }
    return FunctionResult::SUCCESS;
}

int Actuator::pid_param_imm_set(char *ip, double pos_imm_kp, double vel_imm_kp, double vel_imm_ki, double cur_imm_kp, double cur_imm_ki)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();
    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/pid_param_imm");

    writer.Key("property");
    writer.String("");

    writer.Key("control_position_kp_imm");
    writer.Double(pos_imm_kp);

    writer.Key("control_velocity_kp_imm");
    writer.Double(vel_imm_kp);

    writer.Key("control_velocity_ki_imm");
    writer.Double(vel_imm_ki);

    writer.Key("control_current_kp_imm");
    writer.Double(cur_imm_kp);

    writer.Key("control_current_ki_imm");
    writer.Double(cur_imm_ki);
    writer.EndObject();

    char set_pid_control_param_imm[1024] = {0};
    memset(set_pid_control_param_imm, 0, sizeof(set_pid_control_param_imm));
    memcpy(set_pid_control_param_imm, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->control_param_imm_set(ip, set_pid_control_param_imm, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::home_offset_get(char *ip, double &angle)
{
    char feedback_buffer[1024] = {0};
    if (fsa->home_offset_get(ip, NULL, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("get home offset reply: %s\n", feedback_buffer);
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        angle = fed_json["home_offset"].GetDouble();
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::home_offset_set(char *ip, double angle)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();
    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/home_offset");

    writer.Key("home_offset");
    writer.Double(angle);
    writer.EndObject();

    char set_home_param[1024] = {0};
    memset(set_home_param, 0, sizeof(set_home_param));
    memcpy(set_home_param, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->home_offset_set(ip, set_home_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::home_position_set(char *ip)
{
    char feedback_buffer[1024] = {0};
    if (fsa->home_position_set(ip, NULL, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("set home position reply: %s\n", feedback_buffer);
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::flag_of_operation_set(char *ip, int act_val, int motor_val, int encoder_val, int pid_val)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();
    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/flag_of_operation");

    writer.Key("property");
    writer.String("");

    writer.Key("flag_do_use_store_actuator_param");
    writer.Int(act_val);

    writer.Key("flag_do_use_store_motor_param");
    writer.Int(motor_val);

    writer.Key("flag_do_use_store_encoder_param");
    writer.Int(encoder_val);

    writer.Key("flag_do_use_store_pid_param");
    writer.Int(pid_val);
    writer.EndObject();

    char set_flag_of_operation_param[1024] = {0};
    memset(set_flag_of_operation_param, 0, sizeof(set_flag_of_operation_param));
    memcpy(set_flag_of_operation_param, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->flag_of_operation_set(ip, set_flag_of_operation_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("set flag of operation reply: %s\n", feedback_buffer);
    return 0;
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;

}

int Actuator::flag_of_operation_get(char *ip, bool &act_val, bool &motor_val, bool &encoder_val, bool &pid_val)
{
    char feedback_buffer[1024] = {0};
    if (fsa->flag_of_operation_get(ip, NULL, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        act_val = fed_json["flag_do_use_store_actuator_param"].GetInt();
        motor_val = fed_json["flag_do_use_store_motor_param"].GetInt();
        encoder_val = fed_json["flag_do_use_store_encoder_param"].GetInt();
        pid_val = fed_json["flag_do_use_store_pid_param"].GetInt();
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::get_abs_encoder_value(char *ip, double &angle)
{
    char feedback_buffer[1024] = {0};
    if (fsa->get_abs_encoder_value(ip, NULL, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("get angle reply: %s\n", feedback_buffer);
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        angle = fed_json["angle"].GetDouble();
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}

int Actuator::get_measured(char *ip, double &pos, double &vel, double &cur)
{
    return get_pvc(ip, pos, vel, cur);
}

/// @brief 重启所有设备
/// @param ip
/// @return
int Actuator::reboot(char *ip)
{
    char feedback_buffer[1024] = {0};
    return fsa->reboot(ip, NULL, feedback_buffer);
}

/// @brief 重启执行器
/// @param ip
/// @return
int Actuator::reboot_actuator(char *ip)
{
    char feedback_buffer[1024] = {0};
    if (fsa->reboot_actuator(ip, NULL, feedback_buffer))
    {
        Logger::get_instance()->print_trace_error("reboot actuator failed\n");
        return FunctionResult::FAILURE;
    }
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::SUCCESS;
}

/// @brief 校准编码器
/// @param ip
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::set_calibrate_encoder(char *ip)
{
    char feedback_buffer[1024] = {0};
    if (fsa->set_calibrate_encoder(ip, NULL, feedback_buffer))
    {
        Logger::get_instance()->print_trace_error("set calibrate encoder failed\n");
        return FunctionResult::FAILURE;
    }

    int state_motor = -1;
    for (int i = 0; i < 5; i++)
    {
        if (state_motor != 4)
        {
            if (get_state(ip, state_motor) == FunctionResult::FAILURE)
            {
                Logger::get_instance()->print_trace_error("%s get state failed", ip);
                return FunctionResult::FAILURE;
            }
        }
        else
        {
            if (disable_set(ip) == FunctionResult::FAILURE)
            {
                Logger::get_instance()->print_trace_error("%s disable failed", ip);
                return FunctionResult::FAILURE;
            }
        }
    }
    if (disable_set(ip) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("%s disable failed", ip);
        return FunctionResult::FAILURE;
    }
    return FunctionResult::SUCCESS;
}

/// @brief 设置用户名和密码
/// @param ip
/// @param username 用户名
/// @param passwold 密码
/// @return 0:  Function::SUCCESS,
/// @return -1:  Function:FAILURE
int Actuator::set_encrypt(char *ip, char *username, char *passwold)
{
    rapidjson::StringBuffer set_json;
    rapidjson::Writer<rapidjson::StringBuffer> writer(set_json);

    writer.StartObject();

    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/encrypt");

    writer.Key("property");
    writer.String("");

    writer.Key("username");
    writer.String(username);

    writer.Key("password");
    writer.String(passwold);
    writer.EndObject();

    char set_param[1024] = {0};
    memset(set_param, 0, sizeof(set_param));
    memcpy(set_param, set_json.GetString(), set_json.GetSize());
    char feedback_buffer[1024] = {0};
    if (fsa->set_encrypt(ip, set_param, feedback_buffer) == FunctionResult::FAILURE)
    {
        return FunctionResult::FAILURE;
    }
    Logger::get_instance()->print_trace("set encrypt reply: %s\n", feedback_buffer);
    rapidjson::Document fed_json;
    if (fed_json.Parse(feedback_buffer).HasParseError())
    {
        Logger::get_instance()->print_trace_error("rapidjson parse failed\n");
        return FunctionResult::FAILURE;
    }
    std::string success_flg = "OK";
    if (fed_json["status"].GetString() == success_flg)
    {
        return FunctionResult::SUCCESS;
    }
    return FunctionResult::FAILURE;
}
