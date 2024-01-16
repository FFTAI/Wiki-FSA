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

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
        fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
        std::printf("%s\n", ser_msg);
    }

    char set_ctrl_config[1024] = {0};
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    FSAFlagState flagState;

    writer.StartObject();

    writer.Key("method");
    writer.String("SET");

    writer.Key("reqTarget");
    writer.String("/flag_of_operation");

    writer.Key("property");
    writer.String("");

    writer.Key("flag_do_use_store_actuator_param");
    writer.Int(flagState.CLEAR);

    writer.Key("flag_do_use_store_motor_param");
    writer.Int(flagState.CLEAR);

    writer.Key("flag_do_use_store_encoder_param");
    writer.Int(flagState.CLEAR);

    writer.Key("flag_do_use_store_pid_param");
    writer.Int(flagState.CLEAR);

    // writer.Key("flag_do_use_store_actuator_param");
    // writer.Int(flagState.SET);

    // writer.Key("flag_do_use_store_motor_param");
    // writer.Int(flagState.SET);

    // writer.Key("flag_do_use_store_encoder_param");
    // writer.Int(flagState.SET);

    // writer.Key("flag_do_use_store_pid_param");
    // writer.Int(flagState.SET);

    writer.EndObject();
    memset(set_ctrl_config, 0, sizeof(set_ctrl_config));
    memcpy(set_ctrl_config, &buffer, sizeof(buffer));

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto set ctrl config fsa ---> ", ser_list[i].c_str());
        fsa->demo_flag_of_operation_set(ser_list[i], set_ctrl_config, ser_msg);
        std::printf("%s\n", ser_msg);

        rapidjson::Document msg_json;
        if (msg_json.Parse(ser_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("fi_decode() failed\n");
            return 0;
        }
        Logger::get_instance()->print_trace_debug("status : %s\n", msg_json["status"].GetString());
    }

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
        fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
    }

    for (int i = 0; i < fsa->server_ip_num; i++)
    {
        std::printf("IP: %s sendto reboot fsa ---> ", fsa->server_ip[i].c_str());
        fsa->demo_reboot(fsa->server_ip[i], NULL, ser_msg);
    }

    return 0;
}
