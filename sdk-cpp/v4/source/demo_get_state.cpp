/**
 * @file demo_get_state.cpp
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
using namespace Actuator;
using namespace Utils;
using namespace Predefine;

FSA *fsa = new FSA();

int main()
{
    char ser_msg[1024] = {0};
    fsa->demo_broadcase_filter(ACTUATOR);
    if (fsa->server_ip_filter_num == 0)
    {
        Logger::get_instance()->print_trace_error("Cannot find server\n");
        return 0;
    }
    std::string ser_list[254] = {""};
    memset(ser_list, 0, sizeof(ser_list));
    memcpy(ser_list, fsa->server_ip_filter, sizeof(ser_list));
    int ser_num = fsa->server_ip_filter_num;

    while (1)
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto ota fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_state(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("state : %d\n", msg_json["state"].GetInt());
        }
        sleep(1);
    }

    return 0;
}
