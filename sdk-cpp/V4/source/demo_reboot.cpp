/**
 * @file demo_reboot.cpp
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
    char ser_msg[1024] = {0};
    fsa->demo_broadcase();
    if (fsa->server_ip_num == 0)
    {
        Logger::get_instance()->print_trace_error("Cannot find server\n");
        return 0;
    }

    for (int i = 0; i < fsa->server_ip_num; i++)
    {
        std::printf("IP: %s sendto reboot fsa ---> ", fsa->server_ip[i].c_str());
        fsa->demo_reboot(fsa->server_ip[i], NULL, ser_msg);
        std::printf("%s\n", ser_msg);

        rapidjson::Document msg_json;
        if (msg_json.Parse(ser_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("fi_decode() failed\n");
            return 0;
        }
        Logger::get_instance()->print_trace_debug("reboot status: %s\n", msg_json["status"].GetString());
    }

    return 0;
}
