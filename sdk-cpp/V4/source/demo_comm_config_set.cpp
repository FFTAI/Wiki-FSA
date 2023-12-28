/**
 * @file demo_comm_config_set.cpp
 * @author Afer
 * @brief
 * @version 0.1
 * @date 2023-12-21
 * @note pass-test
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"
using namespace Sensor;
using namespace Utils;
using namespace Predefine;

FSA *fsa = new FSA();

int main()
{
    char *comm_config_set = "{\"method\":\"SET\", \
        \"reqTarget\":\"/config\", \
        \"property\":\"\", \
        \"name\":\"FSA\", \
        \"DHCP_enable\":false, \
        \"SSID\":\"fftai-12\", \
        \"password\":\"fftai2015\", \
        \"static_IP\":[192,168,137,202], \
        \"gateway\":[192,168,137,1], \
        \"subnet_mask\":[255,255,255,0], \
        \"dns_1\":[114,114,114,114], \
        \"dns_2\":[8,8,8,8]}";

    char ser_msg[1024] = {0};
    fsa->demo_broadcase_filter(ACTUATOR);
    if (fsa->server_ip_filter_num == 0)
    {
        Logger::get_instance()->print_trace_error("Cannot find server\n");
        return 0;
    }

    for (int i = 0; i < fsa->server_ip_filter_num; i++)
    {
        std::printf("IP: %s sendto demo_comm_config_set fsa ---> ", fsa->server_ip_filter[i].c_str());
        fsa->demo_comm_config_set(fsa->server_ip_filter[i], comm_config_set, ser_msg);
        std::printf("%s\n", ser_msg);

        rapidjson::Document msg_json;
        if (msg_json.Parse(ser_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("fi_decode() failed\n");
            return 0;
        }
        Logger::get_instance()->print_trace_debug("status : %s\n", msg_json["status"].GetString());
    }

    return 0;
}
