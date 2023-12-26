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
        char *test = "{\"test\":\"param\",\"test\":\"param\"}";
        char *broadcast_msg = "Is any fourier smart server here?";
        // char *comm_config_get = "{\"method\":\"GET\",\"reqTarget\":\"/config\",\"property\":\"\"}";
        // char *ctrl_config_get = "{\"method\":\"GET\",\"reqTarget\":\"/config\",\"property\":\"\"}";
        // char *get_measured = "{\"method\":\"GET\",\"reqTarget\":\"/measured\",\"property\":\"\"}";
        // char *get_home_offset = "{\"method\":\"GET\",\"reqTarget\":\"/home_offset\",\"property\":\"\"}";
        // char *set_home_position = "{\"method\":\"SET\",\"reqTarget\":\"/home_position\",}";
        char *ota = "{\"method\":\"SET\",\"reqTarget\":\"/ota\",\"property\":\"\"}";
        char *ota_test = "{\"method\":\"SET\",\"reqTarget\":\"/ota_test\",\"property\":\"\"}";
        char *ota_cloud = "{\"method\":\"SET\",\"reqTarget\":\"/ota_cloud\",\"property\":\"\"}";
        // char *reboot_fse = "{\"method\":\"SET\",\"reqTarget\":\"/reboot\",\"property\":\"\"}";
        char *reboot_all = "{\"method\":\"SET\",\"reqTarget\":\"/reboot\",\"property\":\"\"}";
        // char *ctrl_config_save = "{\"method\":\"SET\",\"reqTarget\":\"/config\",\"property\":\"save\"}";

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

    public:
        FSA(/* args */);
        ~FSA();
        int demo_lookup();
        int demo_ota_cloud(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ota_test(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_ota(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_reboot(std::string sigle_ip, char *define_msg_sendto, char *client_recv_msg);
        int demo_broadcase();
        int demo_broadcase_filter(std::string filter_type);
    };
}

#endif // !__IF_FSA_H__
