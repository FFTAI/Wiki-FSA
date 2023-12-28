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
        // interface_set_position_control
        std::printf("IP: %s sendto set position control fsa ---> ", ser_list[i].c_str());
        char *json_set_position = "{\"method\":\"SET\", \
            \"reqTarget\":\"/current_control\", \
            \"reply_enable\":true, \
            \"position\":0.0, \
            \"velocity_ff\":0.0, \
            \"current_ff\":0.0}";
        fsa->interface_set_position_control(ser_list[i], json_set_position, ser_msg);
        std::printf("%s\n", ser_msg);
    }
    sleep(1);

    for (int i = 0; i < ser_num; i++)
    {
        // demo_enable_set
        std::printf("IP: %s sendto set enable fsa ---> ", ser_list[i].c_str());
        fsa->demo_enable_set(ser_list[i], NULL, ser_msg);
    }
    sleep(1);

    FsaModeOfOperation modeOfOper;
    for (int i = 0; i < ser_num; i++)
    {
        // interface_set_current_mode
        std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
        fsa->interface_set_mode_operation(ser_list[i], modeOfOper.POSITION_CONTROL, ser_msg);
    }
    sleep(1);

    for (int i = 0; i < ser_num; i++)
    {
        // interface_set_position_control
        std::printf("IP: %s sendto set position control fsa ---> ", ser_list[i].c_str());
        char *json_set_position = "{\"method\":\"SET\", \
            \"reqTarget\":\"/current_control\", \
            \"reply_enable\":true, \
            \"position\":0.0, \
            \"velocity_ff\":0.0, \
            \"current_ff\":0.0}";
        fsa->interface_set_position_control(ser_list[i], json_set_position, ser_msg);
        std::printf("%s\n", ser_msg);
    }
    sleep(1);

    // set pid param
    char *set_pid_imm_param = "{\"method\":\"SET\", \
        \"reqTarget\":\"/pid_param_imm\", \
        \"property\":\"\", \
        \"control_position_kp_imm\":0.0, \
        \"control_velocity_kp_imm\":0.0, \
        \"control_velocity_ki_imm\":0.0, \
        \"control_current_kp_imm\":7.25, \
        \"control_current_ki_imm\":0.08}";

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s demo_pid_param_set fsa ---> ", ser_list[i].c_str());
        fsa->demo_pid_param_imm_set(ser_list[i], set_pid_imm_param, ser_msg);
        std::printf("%s\n", ser_msg);

        rapidjson::Document msg_json;
        if (msg_json.Parse(ser_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("fi_decode() failed\n");
            return 0;
        }
        Logger::get_instance()->print_trace_debug("status : %s\n", msg_json["status"].GetString());
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    char send_postion_msg[1024] = {0};

    for (uint32_t i = 0; i < 10000000; i++)
    {
        for (int j = 0; j < ser_num; j++)
        {
            writer.StartObject();
            writer.Key("method");
            writer.String("SET");
            writer.Key("reqTarget");
            writer.String("/position_control");
            writer.Key("reply_enable");
            writer.Bool(true);
            writer.Key("position");
            writer.Double(360 * sin(i / 1000.0));
            writer.Key("velocity_ff");
            writer.Double(0.0);
            writer.Key("current_ff");
            writer.Double(0.0);
            writer.EndObject();
            memset(send_postion_msg, 0, sizeof(send_postion_msg));
            memcpy(send_postion_msg, &buffer, sizeof(buffer));
            // interface_set_position_control
            fsa->interface_set_position_control(ser_list[j], send_postion_msg, ser_msg);
        }
        usleep(10000);
    }
    sleep(1);

    for (int i = 0; i < ser_num; i++)
    {
        fsa->demo_disable_set(ser_list[i], NULL, ser_msg);
    }
    sleep(1);

    for (int i = 0; i < ser_num; i++)
    {
        fsa->interface_set_mode_operation(ser_list[i], modeOfOper.POSITION_CONTROL, ser_msg);
    }

    return 0;
}
