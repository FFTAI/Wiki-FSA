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

    for (int i = 0; i < ser_num; i++)
    {
        // demo_get_state
        std::printf("IP: %s sendto get state fsa ---> ", ser_list[i].c_str());
        fsa->demo_get_state(ser_list[i], NULL, ser_msg);
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
        // demo_ctrl_config_get
        std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
        fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
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
        // demo_get_pvc
        std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
        fsa->demo_get_pvc(ser_list[i], NULL, ser_msg);
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
        // interface_set_current_mode
        std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
        char *json_set_current = "{\"method\":\"SET\", \
            \"reqTarget\":\"/current_control\", \
            \"reply_enable\":true, \
            \"current\":0.0}";
        fsa->interface_set_current_control(ser_list[i], json_set_current, ser_msg);
        std::printf("%s\n", ser_msg);
    }
    sleep(1);

    for (int i = 0; i < 100000; i++)
    {
        for (int i = 0; i < ser_num; i++)
        {
            // interface_set_current_mode
            std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());

            // you need to modify 0.4 -> value expected
            char *json_set_current = "{\"method\":\"SET\", \
            \"reqTarget\":\"/current_control\", \
            \"reply_enable\":true, \
            \"current\":0.4}";

            fsa->interface_set_current_control(ser_list[i], json_set_current, ser_msg);
        }
        usleep(10000);
    }

    for (int i = 0; i < ser_num; i++)
    {
        // interface_set_current_mode
        std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
        char *json_set_current = "{\"method\":\"SET\", \
            \"reqTarget\":\"/current_control\", \
            \"reply_enable\":true, \
            \"current\":0.0}";
        fsa->interface_set_current_control(ser_list[i], json_set_current, ser_msg);
    }
    sleep(1);

    for (int i = 0; i < ser_num; i++)
    {
        // interface_set_current_mode
        std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
        fsa->demo_disable_set(ser_list[i], NULL, ser_msg);
    }
    sleep(1);
    FsaModeOfOperation modeOfOper;
    for (int i = 0; i < ser_num; i++)
    {
        // interface_set_current_mode
        std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
        fsa->interface_set_mode_operation(ser_list[i], modeOfOper.CURRENT_CLOSE_LOOP_CONTROL, ser_msg);
    }

    return 0;
}
