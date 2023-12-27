#ifndef _FSA_H_
#define _FSA_H_

#include <iostream>
#include <string.h>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

#include "os.h"
#include "logger.h"
#include "function_result.h"

#define SERVER_IP "192.168.137.255"
#define SERVER_PORT_COMM 2334
#define SERVER_PORT_CTRL 2333
#define BUFFER_SIZE 1024

namespace Sensor
{
    class FSA
    {
    private:
        int fsa_socket;
        struct sockaddr_in fsa_sockaddr_in_comm, fsa_sockaddr_in_ctrl, fsa_sockaddr_in_work, fsa_sockaddr_in_recv;
        socklen_t sockaddr_len = sizeof(struct sockaddr_in);
        std::string server_filter_type = " ";
        char *work_ip;
        int is_init = 0;
        rapidjson::Document msg_json;

        char recv_buff[BUFFER_SIZE];
        char send_buff[BUFFER_SIZE];

        int work_mode = 0;

        // send json
        char *json_test = "{\"test\":\"param\",\"test\":\"param\"}";
        char *json_broadcast_msg = "Is any fourier smart server here?";

        char *json_get_root = "{\"method\":\"GET\",\"reqTarget\":\"/\",\"property\":\"\"}";
        char *json_set_enable = "{\"method\":\"SET\",\"reqTarget\":\"/control_word\",\"property\":\"\",\"control_word\":15}";
        char *json_set_disenable = "{\"method\":\"SET\",\"reqTarget\":\"/control_word\",\"property\":\"\",\"control_word\":6}";
        char *json_set_calibrate_encoder = "{\"method\":\"SET\",\"reqTarget\":\"/control_word\",\"property\":\"\",\"control_word\":163}";
        char *json_clear_fault = "{\"method\":\"SET\",\"reqTarget\":\"/control_word\",\"property\":\"\",\"control_word\":134}";
        char *json_get_state = "{\"method\":\"GET\",\"reqTarget\":\"/state\",\"property\":\"\"}";

        char *json_get_home_offset = "{\"method\":\"GET\",\"reqTarget\":\"/home_offset\",\"property\":\"\"}";
        char *json_set_home_position = "{\"method\":\"SET\",\"reqTarget\":\"/home_position\",}";
        char *json_get_pid_param = "{\"method\":\"GET\",\"reqTarget\":\"/pid_param\",\"property\":\"\"}";

        char *json_clear_pid_param = "{\"method\":\"SET\",\"reqTarget\":\"/pid_param\",\"property\":\"clear\"}";

        char *json_get_control_param = "{\"method\":\"GET\",\"reqTarget\":\"/control_param\",\"property\":\"\"}";

        char *json_get_control_param_imm = "{\"method\":\"GET\",\"reqTarget\":\"/control_param_imm\",\"property\":\"\"}";

        char *json_get_flag_of_operation = "{\"method\":\"GET\",\"reqTarget\":\"/flag_of_operation\",\"property\":\"\"}";

        char *json_clear_flag_of_operation = "{\"method\":\"GET\",\"reqTarget\":\"/flag_of_operation\",\"property\":\"clear\"}";
        char *json_get_config = "{\"method\":\"GET\",\"reqTarget\":\"/config\",\"property\":\"\"}";
        char *json_save_config = "{\"method\":\"SET\",\"reqTarget\":\"/config\",\"property\":\"save\"}";
        char *json_erase_config = "{\"method\":\"SET\",\"reqTarget\":\"/config\",\"property\":\"erase\"}";
        char *json_reboot = "{\"method\":\"SET\",\"reqTarget\":\"/reboot\",\"property\":\"\"}";
        char *json_get_error_code = "{\"method\":\"GET\",\"reqTarget\":\"/error_code\",\"property\":\"\"}";
        char *json_get_pvc = "{\"method\":\"GET\",\"reqTarget\":\"/measured\",\"position\":True,\"velocity\":True,\"current\":True}";
        char *json_get_pvcc = "{\"method\":\"GET\",\"reqTarget\":\"/measured\",\"position\":True,\"velocity\":True,\"current\":True,\"current_id\":True}";
        char *json_get_pvcccc = "{\"method\":\"GET\",\"reqTarget\":\"/measured\",\"position\":True,\"velocity\":True,\"current\":True,\"current_id\":True,\"phase_current_ib\":True,\"phase_current_ic\":True}";

        // GROUP START

        // GROUP END

        char *json_get_comm_root = "{\"method\":\"GET\",\"reqTarget\":\"/\",\"property\":\"\"}";
        char *json_get_comm_config = "{\"method\":\"GET\",\"reqTarget\":\"/config\",\"property\":\"\"}";
        char *json_save_comm_config = "{\"method\":\"SET\",\"reqTarget\":\"/config\",\"property\":\"save\"}";
        char *json_reboot_comm = "{\"method\":\"SET\",\"reqTarget\":\"/reboot\",\"property\":\"\"}";

        char *json_get_abs_encoder_angle = "{\"method\":\"GET\",\"reqTarget\":\"/measured\",\"property\":\"\"}";

        // OTA START
        char *json_ota = "{\"method\":\"SET\",\"reqTarget\":\"/ota\",\"property\":\"\"}";
        char *json_ota_test = "{\"method\":\"SET\",\"reqTarget\":\"/ota_test\",\"property\":\"\"}";
        char *json_ota_devel = "{\"method\":\"SET\",\"reqTarget\":\"/ota_devel\",\"property\":\"\"}";
        char *json_ota_cloud = "{\"method\":\"SET\",\"reqTarget\":\"/ota_cloud\",\"property\":\"\"}";
        char *json_ota_driver = "{\"method\":\"SET\",\"reqTarget\":\"/ota_driver\",\"property\":\"\"}";
        char *json_ota_driver_test = "{\"method\":\"SET\",\"reqTarget\":\"/ota_driver_test\",\"property\":\"\"}";
        char *json_ota_driver_devel = "{\"method\":\"SET\",\"reqTarget\":\"/ota_driver_devel\",\"property\":\"\"}";
        char *json_ota_driver_cloud = "{\"method\":\"SET\",\"reqTarget\":\"/ota_driver_cloud\",\"property\":\"\"}";
        // OTA END

    public:
        enum WorkMode
        {
            NONE = 0,
            BROADCAST_MODE,
            BROADCASE_FILTER_MODE,
            SIGLE_MODE,
        };
        std::string server_ip[254];
        std::string server_ip_filter[254];
        int server_ip_num = 0;
        int server_ip_filter_num = 0;

    private:
        int fi_init_network();
        int fi_init_fse();
        int fi_fsa_comm(std::string ip, int port, char *sendmsg, char *client_recv_msg);
        int fi_decode(char *msg);
        int fi_encode();
        int fi_send_msg(std::string ip, int port, char *msg);
        int fi_recv_msg(char *client_recv_msg);

        // FSA BASIC FUNC START
        int get_root();
        int set_enable();
        int set_disenable();
        int set_calibrate_encoder();
        int clear_fault();
        int get_state();
        int set_mode_of_operation();
        int get_home_offset();
        int set_home_offset();
        int set_home_position();
        int get_pid_param();
        int set_pid_param();
        int clear_pid_param();
        int set_pid_param_imm();
        int get_control_param();
        int set_control_param();
        int get_control_param_imm();
        int set_control_param_imm();
        int get_flag_of_operation();
        int set_flag_of_operation();
        int clear_flag_of_operation();
        int get_config();
        int set_config();
        int save_config();
        int erase_config();
        int reboot();
        int get_error_code();
        int get_pvc();
        int get_pvcc();
        int get_pvcccc();
        int set_position_control();
        int set_velocity_control();
        int set_current_control();
        int set_torque_control();
        // GROUP START
        int enable_group();
        int disable_group();
        int get_state_group();
        int get_error_group();
        int clear_error_group();
        int get_pvc_group();
        int set_mode_of_operation_group();
        int reboot_comm_group();
        // GROUP END
        int get_comm_root();
        int get_comm_config();
        int set_comm_config();
        int save_comm_config();
        int reboot_comm();
        int get_abs_encoder_angle();
        int broadcast_func();
        int broadcast_func_with_filter();
        // OTA START
        int ota();
        int ota_test();
        int ota_devel();
        int ota_cloud();
        int ota_driver();
        int ota_driver_test();
        int ota_driver_devel();
        int ota_driver_cloud();
        // OTA END
        int encrypt();
        // FSA BASIC FUNC END

    public:
        FSA(/* args */);
        ~FSA();
        int demo_broadcase();

        int demo_broadcase_filter(std::string filter_type);

        int demo_clear_fault(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_comm_config_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_comm_config_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_control_current_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_control_param_imm_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_control_param_imm_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_control_param_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_control_param_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_control_position_ff_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_control_position_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_control_velocity_mode(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_ctrl_config_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ctrl_config_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_comm_config_save(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_disable_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_enable_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_flag_of_operation_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_flag_of_operation_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_get_abs_encoder_value(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_get_measured(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_get_pvc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_get_pvcc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_get_pvcccc(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_get_state(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_home_offset_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_home_offset_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_home_position_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_lookup_abs_encoder(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_lookup_actuator(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_lookup_ctrlbox(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_lookup(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_new_motor_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_ota_cloud(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ota_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ota(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ota_devel(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_ota_driver_cloud(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ota_driver_devel(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ota_driver_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ota_driver(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_pid_param_get(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_pid_param_imm_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_pid_param_set(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_reboot(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_reboot_actuator(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);

        int demo_set_calibrate_encoder(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_set_encrypt(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
    };
}

#endif // !__IF_FSA_H__
