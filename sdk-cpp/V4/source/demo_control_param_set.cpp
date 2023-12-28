#include "main.h"
using namespace Sensor;
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

    char *set_control_param = "{\"method\":\"SET\", \
        \"reqTarget\":\"/control_param\", \
        \"property\":\"\", \
        \"name\":\"FSA\", \
        \"motor_max_speed_imm\":3000, \
        \"motor_max_acceleration_imm\":60000, \
        \"motor_max_current_imm\":8";

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto control param set fsa ---> ", ser_list[i].c_str());
        fsa->demo_control_param_set(ser_list[i], set_control_param, ser_msg);
        std::printf("%s\n", ser_msg);

        rapidjson::Document msg_json;
        if (msg_json.Parse(ser_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("fi_decode() failed\n");
            return 0;
        }
        Logger::get_instance()->print_trace_debug("status : %s\n", msg_json["status"].GetString());
    }
    sleep(1);

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto control param get fsa ---> ", ser_list[i].c_str());
        fsa->demo_control_param_get(ser_list[i], NULL, ser_msg);
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
