/**
 * @file demo_.cpp
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
rapidjson::Document msg_json;

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

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
        fsa->demo_set_calibrate_encoder(ser_list[i], NULL, ser_msg);
        std::printf("%s\n", ser_msg);

        if (msg_json.Parse(ser_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("fi_decode() failed\n");
            return 0;
        }
    }
    int state_motor[255] = {0};
    memset(state_motor, 0, sizeof(state_motor));

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < ser_num; j++)
        {
            if (state_motor[j] != 4)
            {
                memset(ser_msg, 0, sizeof(ser_msg));
                fsa->demo_get_state(ser_list[i], NULL, ser_msg);
                if (msg_json.Parse(ser_msg).HasParseError())
                {
                    Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                    return 0;
                }
                state_motor[j] = msg_json["state"].GetInt();
            }
        }

        for (int k = 0; k < ser_num; k++)
        {
            if (state_motor[k] == 4)
            {
                memset(ser_msg, 0, sizeof(ser_msg));
                fsa->demo_disable_set(ser_list[i], NULL, ser_msg);
            }
        }
    }

    for (int i = 0; i < ser_num; i++)
    {
        fsa->demo_disable_set(ser_list[i], NULL, ser_msg);
    }

    return 0;
}
