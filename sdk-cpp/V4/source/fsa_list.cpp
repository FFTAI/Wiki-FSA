/**
 * @file fsa_list.cpp
 * @author Afer
 * @brief
 * @version 0.1
 * @date 2024-1-10
 * @note
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fstream>
#include <sstream>
#include <vector>
#include "main.h"

using namespace Sensor;
using namespace Utils;
using namespace Predefine;

FSA *fsa = new FSA();

std::string int_array_to_ip(const std::vector<int> &int_array)
{
    std::ostringstream oss;
    for (size_t i = 0; i < int_array.size(); ++i)
    {
        if (i > 0)
        {
            oss << ".";
        }
        oss << int_array[i];
    }
    return oss.str();
}

int main()
{
    std::ifstream json_file("../config/config.json");
    std::stringstream json_buff;
    json_buff << json_file.rdbuf();
    std::string json_string = json_buff.str();

    // parse json string
    rapidjson::Document msg_json;
    if (msg_json.Parse(json_string.c_str()).HasParseError())
    {
        Logger::get_instance()->print_trace_error("fi_decode() failed\n");
        return FunctionResult::FAILURE;
    }
    // test
    std::cout << "Device: " << msg_json["Device"].GetString() << std::endl;

    // find key was enable
    if ((msg_json["Device"].GetString()) == "FSE")
    {
        Logger::get_instance()->print_trace_error("Please set \"Device\":\"FSE\"\n");
        return FunctionResult::FAILURE;
    }

    const rapidjson::Value &commandType = msg_json["Command"];
    const rapidjson::Value &param = msg_json["Param"];

    // boradcast
    char ser_msg[1024] = {0};
    if (fsa->demo_broadcase_filter(ACTUATOR) != FunctionResult::SUCCESS)
    {
        Logger::get_instance()->print_trace_error("fsa_list(): broadcast failed\n");
        return FunctionResult::FAILURE;
    }

    if (fsa->server_ip_filter_num == 0)
    {
        Logger::get_instance()->print_trace_error("Cannot find server\n");
        return FunctionResult::FAILURE;
    }

    std::string ser_list[254] = {""};
    memset(ser_list, 0, sizeof(ser_list));
    memcpy(ser_list, fsa->server_ip_filter, sizeof(ser_list));
    int ser_num = fsa->server_ip_filter_num;

    // parse command and operate param
    if (commandType["demo_clear_fault"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto clear fault ---> \n", ser_list[i].c_str());
            if (fsa->demo_clear_fault(ser_list[i], NULL, ser_msg) != FunctionResult::SUCCESS)
            {
                Logger::get_instance()->print_trace_error("send clear fault to server failed\n");
                return FunctionResult::FAILURE;
            }
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_comm_config_get"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());
            fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_comm_config_set"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            // step1: get comm config
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());
            fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);

            // step2: make json msg
            const rapidjson::Value &comm_set_msg = param["demo_comm_config_set"];
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

            writer.StartObject();
            writer.Key("method");
            writer.String("SET");

            writer.Key("reqTarget");
            writer.String("/config");

            writer.Key("property");
            writer.String("");

            writer.Key("name");
            writer.String("FSA");

            writer.Key("DHCP_enable");
            writer.Bool(comm_set_msg["DHCP_enable"].GetBool());

            writer.Key("SSID");
            writer.String(comm_set_msg["SSID"].GetString());

            writer.Key("password");
            writer.String(comm_set_msg["password"].GetString());

            writer.Key("static_IP");
            writer.StartArray();
            writer.Int(comm_set_msg["static_IP"][i][0].GetInt());
            writer.Int(comm_set_msg["static_IP"][i][1].GetInt());
            writer.Int(comm_set_msg["static_IP"][i][2].GetInt());
            writer.Int(comm_set_msg["static_IP"][i][3].GetInt());
            writer.EndArray();

            writer.Key("gateway");
            writer.StartArray();
            writer.Int(192);
            writer.Int(168);
            writer.Int(137);
            writer.Int(1);
            writer.EndArray();

            writer.Key("subnet_mask");
            writer.StartArray();
            writer.Int(255);
            writer.Int(255);
            writer.Int(255);
            writer.Int(0);
            writer.EndArray();

            writer.Key("dns_1");
            writer.StartArray();
            writer.Int(114);
            writer.Int(114);
            writer.Int(114);
            writer.Int(114);
            writer.EndArray();

            writer.Key("dns_2");
            writer.StartArray();
            writer.Int(8);
            writer.Int(8);
            writer.Int(8);
            writer.Int(8);
            writer.EndArray();

            writer.EndObject();

            std::string comm_set_config_msg = buffer.GetString();
            char send_set_config[1024] = {0};
            comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
            send_set_config[sizeof(send_set_config) - 1] = '\0';

            // step3: send msg to server
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());
            fsa->demo_comm_config_set(ser_list[i], send_set_config, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
            sleep(1);

            // step4: reboot fsa
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto reboot fsa ---> \n", ser_list[i].c_str());
            fsa->demo_reboot_actuator(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
            Logger::get_instance()->print_trace_warning("restarting ...\n");
            for (uint8_t i = 0; i < 7; i++)
            {
                sleep(1);
                Logger::get_instance()->print_trace(".\n");
            }

            // step5: if static ip was changed, it need ip newly.
            std::vector<int> int_array = {comm_set_msg["static_IP"][i][0].GetInt(),
                                          comm_set_msg["static_IP"][i][1].GetInt(), comm_set_msg["static_IP"][i][2].GetInt(), comm_set_msg["static_IP"][i][3].GetInt()};
            std::string new_ip = int_array_to_ip(int_array);

            // step6: if new_ip = broadcast_ip, then broadcast_ip will be used, else, new_ip used
            /**
             * @bug it will debug question that ip was changed, cannot receive message for getting config
             */
            memset(ser_msg, 0, sizeof(ser_msg));
            if (new_ip == ser_list[i])
            {
                memset(ser_msg, 0, sizeof(ser_msg));
                Logger::get_instance()->print_trace_warning("IP: %s sendto get_comm_config ---> \n", ser_list[i].c_str());
                if (fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg) == FunctionResult::FAILURE)
                {
                    Logger::get_instance()->print_trace_error("new ip: demo get communication config failed\n");
                    return FunctionResult::FAILURE;
                }
            }
            else
            {
                memset(ser_msg, 0, sizeof(ser_msg));
                Logger::get_instance()->print_trace_warning("IP: %s sendto get_comm_config ---> \n", new_ip.c_str());
                if (fsa->demo_comm_config_get(new_ip, NULL, ser_msg) == FunctionResult::FAILURE)
                {
                    Logger::get_instance()->print_trace_error("old ip: demo get communication config failed\n");
                    return FunctionResult::FAILURE;
                }
            }
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_control_current_mode"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            // demo_get_state
            std::printf("IP: %s sendto get state fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_state(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            // demo_ctrl_config_get
            std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
            fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            // demo_get_pvc
            std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_pvc(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
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

        for (int i = 0; i < ser_num; i++)
        {
            FsaModeOfOperation modeOfOper;
            // interface_set_current_mode
            std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
            fsa->interface_set_mode_operation(ser_list[i], modeOfOper.CURRENT_CLOSE_LOOP_CONTROL, ser_msg);
        }

        for (int i = 0; i < ser_num; i++)
        {
            // demo_enable_set
            std::printf("IP: %s sendto set enable fsa ---> ", ser_list[i].c_str());
            fsa->demo_enable_set(ser_list[i], NULL, ser_msg);
        }

        // step2: make json msg
        const rapidjson::Value &current_set = param["demo_control_current_mode"];
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        writer.StartObject();
        writer.Key("method");
        writer.String("SET");

        writer.Key("reqTarget");
        writer.String("/current_control");

        writer.Key("reply_enable");
        writer.Bool(true);

        writer.Key("current");
        writer.Double(current_set["current_val"].GetDouble());

        writer.EndObject();

        std::string comm_set_config_msg = buffer.GetString();
        char send_set_config[1024] = {0};
        comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
        send_set_config[sizeof(send_set_config) - 1] = '\0';

        for (int i = 0; i < 200; i++)
        {
            for (int i = 0; i < ser_num; i++)
            {
                // interface_set_current_mode
                std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
                fsa->interface_set_current_control(ser_list[i], send_set_config, ser_msg);
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
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_control_param_get"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            fsa->demo_control_param_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_control_param_imm_get"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            fsa->demo_control_param_imm_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_control_param_imm_set"].GetBool())
    {
        // enable
        // step2: make json msg
        const rapidjson::Value &control_param_imm_set = param["demo_control_param_imm_set"];
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        writer.StartObject();
        writer.Key("method");
        writer.String("SET");

        writer.Key("reqTarget");
        writer.String("/control_param_imm");

        writer.Key("property");
        writer.String("");

        writer.Key("name");
        writer.String("FSA");

        writer.Key("motor_max_speed_imm");
        writer.Uint64(control_param_imm_set["motor_max_speed_imm"].GetUint64());

        writer.Key("motor_max_acceleration_imm");
        writer.Uint64(control_param_imm_set["motor_max_acceleration_imm"].GetUint64());

        writer.Key("motor_max_current_imm");
        writer.Uint64(control_param_imm_set["motor_max_current_imm"].GetUint64());

        writer.EndObject();

        std::string comm_set_config_msg = buffer.GetString();
        char send_set_config[1024] = {0};
        comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
        send_set_config[sizeof(send_set_config) - 1] = '\0';

        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto set imm param ---> \n", ser_list[i].c_str());

            fsa->demo_control_param_imm_set(ser_list[i], send_set_config, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto get imm param ---> \n", ser_list[i].c_str());

            fsa->demo_control_param_imm_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }

        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_control_param_set"].GetBool())
    {
        // step2: make json msg
        const rapidjson::Value &control_param_imm_set = param["demo_control_param_set"];
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        writer.StartObject();
        writer.Key("method");
        writer.String("SET");

        writer.Key("reqTarget");
        writer.String("/control_param");

        writer.Key("property");
        writer.String("");

        writer.Key("name");
        writer.String("FSA");

        writer.Key("motor_max_speed");
        writer.Uint64(control_param_imm_set["motor_max_speed"].GetUint64());

        writer.Key("motor_max_acceleration");
        writer.Uint64(control_param_imm_set["motor_max_acceleration"].GetUint64());

        writer.Key("motor_max_current");
        writer.Uint64(control_param_imm_set["motor_max_current"].GetUint64());

        writer.EndObject();

        std::string comm_set_config_msg = buffer.GetString();
        char send_set_config[1024] = {0};
        comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
        send_set_config[sizeof(send_set_config) - 1] = '\0';
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto set control param ---> \n", ser_list[i].c_str());

            fsa->demo_control_param_set(ser_list[i], send_set_config, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            fsa->demo_control_param_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }

        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_control_position_ff_mode"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            // demo_get_state
            std::printf("IP: %s sendto get state fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_state(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            // demo_ctrl_config_get
            std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
            fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            // demo_get_pvc
            std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_pvc(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
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
            std::printf("%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            FsaModeOfOperation modeOfOper;
            // interface_set_current_mode
            std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
            fsa->interface_set_mode_operation(ser_list[i], modeOfOper.POSITION_CONTROL, ser_msg);
            std::printf("%s\n", ser_msg);
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
        // step2: make json msg
        const rapidjson::Value &control_position_ff_mode = param["demo_control_position_ff_mode"];

        const rapidjson::Value &set_pid_imm_param = control_position_ff_mode["set_pid_imm_param"];
        const rapidjson::Value &set_position_control = control_position_ff_mode["set_position_control"];

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        writer.StartObject();
        writer.Key("method");
        writer.String("SET");

        writer.Key("reqTarget");
        writer.String("/pid_param_imm");

        writer.Key("property");
        writer.String("");

        writer.Key("control_position_kp_imm");
        writer.Double(set_pid_imm_param["control_position_kp_imm"].GetDouble());

        writer.Key("control_velocity_kp_imm");
        writer.Double(set_pid_imm_param["control_velocity_kp_imm"].GetDouble());

        writer.Key("control_velocity_ki_imm");
        writer.Double(set_pid_imm_param["control_velocity_ki_imm"].GetDouble());

        writer.Key("control_current_kp_imm");
        writer.Double(set_pid_imm_param["control_current_kp_imm"].GetDouble());

        writer.Key("control_current_ki_imm");
        writer.Double(set_pid_imm_param["control_current_ki_imm"].GetDouble());

        writer.EndObject();

        std::string comm_set_config_msg = buffer.GetString();
        char send_set_config[1024] = {0};
        comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
        send_set_config[sizeof(send_set_config) - 1] = '\0';

        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s demo_pid_param_set fsa ---> ", ser_list[i].c_str());
            fsa->demo_pid_param_imm_set(ser_list[i], send_set_config, ser_msg);
            std::printf("%s\n", ser_msg);
        }

        for (uint32_t i = 0; i < 100000; i++)
        {
            for (int j = 0; j < ser_num; j++)
            {
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                writer.StartObject();
                writer.Key("method");
                writer.String("SET");
                writer.Key("reqTarget");
                writer.String("/position_control");
                writer.Key("reply_enable");
                writer.Bool(true);
                writer.Key("position");
                writer.Double(set_position_control["position"].GetDouble() * sin(i / 1000.0));
                writer.Key("velocity_ff");
                writer.Double(set_position_control["velocity_ff"].GetDouble());
                writer.Key("current_ff");
                writer.Double(set_position_control["current_ff"].GetDouble());
                writer.EndObject();

                std::string comm_set_config_msg = buffer.GetString();
                buffer.Clear();
                char send_set_config[1024] = {0};
                comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
                send_set_config[sizeof(send_set_config) - 1] = '\0';

                buffer.Clear();

                // interface_set_position_control
                fsa->interface_set_position_control(ser_list[j], send_set_config, ser_msg);
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
            FsaModeOfOperation modeOfOper;
            fsa->interface_set_mode_operation(ser_list[i], modeOfOper.POSITION_CONTROL, ser_msg);
        }

        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_control_position_mode"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            // demo_get_state
            std::printf("IP: %s sendto get state fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_state(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            // demo_ctrl_config_get
            std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
            fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        sleep(1);

        for (int i = 0; i < ser_num; i++)
        {
            // demo_get_pvc
            std::printf("IP: %s sendto get pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_pvc(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
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

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        const rapidjson::Value &control_position_mode = param["demo_control_position_mode"];

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
                writer.Double(control_position_mode["position"].GetDouble() * sin(i / 1000.0));
                writer.Key("velocity_ff");
                writer.Double(control_position_mode["velocity_ff"].GetDouble());
                writer.Key("current_ff");
                writer.Double(control_position_mode["current_ff"].GetDouble());
                writer.EndObject();

                std::string comm_set_config_msg = buffer.GetString();
                buffer.Clear();
                char send_set_config[1024] = {0};
                comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
                send_set_config[sizeof(send_set_config) - 1] = '\0';

                buffer.Clear();
                // interface_set_position_control
                fsa->interface_set_position_control(ser_list[j], send_set_config, ser_msg);
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
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ctrl_config_get"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ctrl_config_save"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            fsa->demo_ctrl_config_save(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ctrl_config_set"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
            fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        sleep(1);

        char set_ctrl_config[1024] = {0};
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        FSAActuatorType actuatorType;
        FSAActuatorDirection actuatorDir;
        FSAActuatorReductionRatio actuatorRedTat;

        FSAMotorType motorType;
        FSAHardwareType hardwareType;
        FSAMotorVBUS motorBus;
        FSAMotorDirection motorDir;
        FSAMotorPolePairs motorPolePairs;
        FSAMotorMaxSpeed motorMaxSpeed;
        FSAMotorMaxAcceleration motorMaxAcc;
        FSAEncoderDirection encoderDir;

        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto set ctrl config fsa ---> ", ser_list[i].c_str());
            // make send json
            writer.StartObject();

            writer.Key("method");
            writer.String("SET");

            writer.Key("reqTarget");
            writer.String("/config");

            writer.Key("property");
            writer.String("");

            writer.Key("actuator_type");
            writer.Uint64(actuatorType.TYPE_DEFAULT);

            writer.Key("actuator_direction");
            writer.Int(actuatorDir.DIRECTION_NORMAL);

            writer.Key("actuator_reduction_ratio");
            writer.Int(actuatorRedTat.REDUCTION_RATIO_30);

            writer.Key("motor_type");
            writer.Int(motorType.FSA80_10V0);

            writer.Key("motor_hardware_type");
            writer.Int(hardwareType.TYPE_H66V104);

            writer.Key("motor_vbus");
            writer.Int(motorBus.VBUS_36V);

            writer.Key("motor_direction");
            writer.Int(motorDir.ACB);

            writer.Key("motor_pole_pairs");
            writer.Int(motorPolePairs.POLE_PAIRS_10);

            writer.Key("motor_max_speed");
            writer.Int(motorMaxSpeed.MAX_SPEED_3000);

            writer.Key("motor_max_acceleration");
            writer.Int(motorMaxAcc.MAX_ACCELERATION_60000);

            writer.Key("motor_max_current");
            writer.Int(10);

            writer.Key("encoder_direction");
            writer.Int(encoderDir.DIRECTION_CCW);
            writer.EndObject();

            memcpy(set_ctrl_config, &buffer, sizeof(buffer));

            fsa->demo_ctrl_config_set(ser_list[i], set_ctrl_config, ser_msg);
            std::printf("%s\n", ser_msg);

            usleep(1000); // The necessary delay for running multiple scripts
        }
    }

    if (commandType["demo_disable_set"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            fsa->demo_disable_set(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_enable_set"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            fsa->demo_enable_set(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_flag_of_operation_get"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            fsa->demo_flag_of_operation_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_flag_of_operation_set"].GetBool())
    {
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
        const rapidjson::Value &flag_of_operation_set = param["demo_flag_of_operation_set"];

        writer.StartObject();

        writer.Key("method");
        writer.String("SET");

        writer.Key("reqTarget");
        writer.String("/flag_of_operation");

        writer.Key("property");
        writer.String("");

        writer.Key("flag_do_use_store_actuator_param");
        writer.Int(flag_of_operation_set["flag_do_use_store_actuator_param"].GetInt());

        writer.Key("flag_do_use_store_motor_param");
        writer.Int(flag_of_operation_set["flag_do_use_store_motor_param"].GetInt());

        writer.Key("flag_do_use_store_encoder_param");
        writer.Int(flag_of_operation_set["flag_do_use_store_encoder_param"].GetInt());

        writer.Key("flag_do_use_store_pid_param");
        writer.Int(flag_of_operation_set["flag_do_use_store_pid_param"].GetInt());

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
        }

        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
            fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }

        for (int i = 0; i < fsa->server_ip_num; i++)
        {
            std::printf("IP: %s sendto reboot fsa ---> ", fsa->server_ip[i].c_str());
            fsa->demo_reboot(fsa->server_ip[i], NULL, ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_get_abs_encoder_value"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_abs_encoder_value(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("angle : %f", msg_json["angle"].GetDouble());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_get_measured"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_pvc(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("position : %f, velocity : %f, current : %f\n",
                                                      msg_json["position"].GetDouble(), msg_json["velocity"].GetDouble(), msg_json["current"].GetDouble());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_get_pvc"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_pvc(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("position : %f, velocity : %f, current : %f\n",
                                                      msg_json["position"].GetDouble(), msg_json["velocity"].GetDouble(), msg_json["current"].GetDouble());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_get_pvcc"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_pvcc(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("position : %f, velocity : %f, current : %f\n",
                                                      msg_json["position"].GetDouble(), msg_json["velocity"].GetDouble(), msg_json["current"].GetDouble());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_get_pvcccc"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvcccc fsa ---> ", ser_list[i].c_str());
            fsa->demo_get_pvcccc(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("position : %f, velocity : %f, current : %f\n",
                                                      msg_json["position"].GetDouble(), msg_json["velocity"].GetDouble(), msg_json["current"].GetDouble());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_get_state"].GetBool())
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
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_home_offset_get"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_home_offset_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("angle : %f", msg_json["angle"].GetDouble());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_home_offset_set"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_home_offset_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        const rapidjson::Value &demo_home_offset_set = param["demo_home_offset_set"];

        writer.StartObject();
        writer.Key("method");
        writer.String("SET");
        writer.Key("reqTarget");
        writer.String("/home_offset");
        writer.Key("home_offset");
        writer.Double(demo_home_offset_set["home_offset"].GetDouble());
        writer.EndObject();

        std::string comm_set_config_msg = buffer.GetString();
        buffer.Clear();
        char send_set_config[1024] = {0};
        comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
        send_set_config[sizeof(send_set_config) - 1] = '\0';

        buffer.Clear();
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_home_offset_set(ser_list[i], send_set_config, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_home_position_set"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_home_position_set(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_lookup_abs_encoder"].GetBool())
    {
        if (!(fsa->demo_broadcase_filter(ABSCODER)))
        {
            for (int i = 0; i < fsa->server_ip_filter_num; i++)
            {
                Logger::get_instance()->print_trace("%s\n", fsa->server_ip_filter[i]);
            }
            return FunctionResult::SUCCESS;
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_lookup_actuator"].GetBool())
    {
        if (!(fsa->demo_broadcase_filter(ACTUATOR)))
        {
            for (int i = 0; i < fsa->server_ip_filter_num; i++)
            {
                Logger::get_instance()->print_trace("%s\n", fsa->server_ip_filter[i]);
            }
            return FunctionResult::SUCCESS;
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_lookup_ctrlbox"].GetBool())
    {
        if (!(fsa->demo_broadcase_filter(CTRLBOX)))
        {
            for (int i = 0; i < fsa->server_ip_filter_num; i++)
            {
                Logger::get_instance()->print_trace("%s\n", fsa->server_ip_filter[i]);
            }
            return FunctionResult::SUCCESS;
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_lookup"].GetBool())
    {
        if (!(fsa->demo_broadcase()))
        {
            for (int i = 0; i < fsa->server_ip_num; i++)
            {
                Logger::get_instance()->print_trace("%s\n", fsa->server_ip[i].c_str());
            }
            return FunctionResult::SUCCESS;
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_new_motor_test"].GetBool())
    {
        for (int i = 0; i < fsa->server_ip_filter_num; i++)
        {
            std::printf("IP: %s sendto ota fsa ---> ", fsa->server_ip_filter[i].c_str());
            fsa->demo_new_motor_test(fsa->server_ip_filter[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            // rapidjson::Document msg_json;
            // if (msg_json.Parse(ser_msg).HasParseError())
            // {
            //     Logger::get_instance()->print_trace_error("fi_decode() failed\n");
            //     return 0;
            // }
            // Logger::get_instance()->print_trace_debug("OTAstatus : %s\n", msg_json["OTAstatus"].GetString());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ota_cloud"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_ota_cloud(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ota_devel"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_ota_devel(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ota"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_ota(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ota_test"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_ota_test(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ota_driver_cloud"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_ota_driver_cloud(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ota_driver_devel"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_ota_driver_devel(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ota_driver_test"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_ota_driver_test(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_ota_driver"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s sendto demo_get_pvc fsa ---> ", ser_list[i].c_str());
            fsa->demo_ota_driver(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_pid_param_get"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            std::printf("IP: %s demo_pid_param_get fsa ---> ", fsa->server_ip_filter[i].c_str());
            fsa->demo_pid_param_get(fsa->server_ip_filter[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_pid_param_imm_set"].GetBool())
    {
        // get pid param
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s demo_pid_param_get fsa ---> ", ser_list[i].c_str());
            fsa->demo_pid_param_imm_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        const rapidjson::Value &set_pid_imm_param = param["set_pid_imm_param"];

        writer.StartObject();
        writer.Key("method");
        writer.String("SET");
        writer.Key("reqTarget");
        writer.String("/pid_param_imm");
        writer.Key("property");
        writer.String("");
        writer.Key("control_position_kp_imm");
        writer.Double(set_pid_imm_param["control_position_kp_imm"].GetDouble());
        writer.Key("control_velocity_kp_imm");
        writer.Double(set_pid_imm_param["control_velocity_kp_imm"].GetDouble());
        writer.Key("control_velocity_ki_imm");
        writer.Double(set_pid_imm_param["control_velocity_ki_imm"].GetDouble());
        writer.Key("control_current_kp_imm");
        writer.Double(set_pid_imm_param["control_current_kp_imm"].GetDouble());
        writer.Key("control_current_ki_imm");
        writer.Double(set_pid_imm_param["control_current_ki_imm"].GetDouble());
        writer.EndObject();

        std::string comm_set_config_msg = buffer.GetString();
        buffer.Clear();
        char send_set_config[1024] = {0};
        comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
        send_set_config[sizeof(send_set_config) - 1] = '\0';

        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s demo_pid_param_set fsa ---> ", ser_list[i].c_str());
            fsa->demo_pid_param_imm_set(ser_list[i], send_set_config, ser_msg);
            std::printf("%s\n", ser_msg);
        }

        // get pid param
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s demo_pid_param_get fsa ---> ", ser_list[i].c_str());
            fsa->demo_pid_param_imm_get(ser_list[i], NULL, ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_pid_param_set"].GetBool())
    {
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s demo_pid_param_set fsa ---> ", ser_list[i].c_str());
            fsa->demo_pid_param_get(ser_list[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);
        }

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        const rapidjson::Value &set_pid_param = param["set_pid_param"];

        writer.StartObject();
        writer.Key("method");
        writer.String("SET");
        writer.Key("reqTarget");
        writer.String("/pid_param");
        writer.Key("property");
        writer.String("");
        writer.Key("control_position_kp_imm");
        writer.Double(set_pid_param["control_position_kp_imm"].GetDouble());
        writer.Key("control_velocity_kp_imm");
        writer.Double(set_pid_param["control_velocity_kp_imm"].GetDouble());
        writer.Key("control_velocity_ki_imm");
        writer.Double(set_pid_param["control_velocity_ki_imm"].GetDouble());
        writer.Key("control_current_kp_imm");
        writer.Double(set_pid_param["control_current_kp_imm"].GetDouble());
        writer.Key("control_current_ki_imm");
        writer.Double(set_pid_param["control_current_ki_imm"].GetDouble());
        writer.EndObject();

        std::string comm_set_config_msg = buffer.GetString();
        buffer.Clear();
        char send_set_config[1024] = {0};
        comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
        send_set_config[sizeof(send_set_config) - 1] = '\0';

        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s demo_pid_param_set fsa ---> ", ser_list[i].c_str());
            fsa->demo_pid_param_imm_set(ser_list[i], send_set_config, ser_msg);
            std::printf("%s\n", ser_msg);
        }

        // get pid param
        for (int i = 0; i < ser_num; i++)
        {
            std::printf("IP: %s demo_pid_param_get fsa ---> ", ser_list[i].c_str());
            fsa->demo_reboot(ser_list[i], NULL, ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_reboot_actuator"].GetBool())
    {
        for (int i = 0; i < fsa->server_ip_filter_num; i++)
        {
            std::printf("IP: %s sendtodemo_set_encrypt fsa ---> ", fsa->server_ip_filter[i].c_str());
            fsa->demo_reboot_actuator(fsa->server_ip_filter[i], NULL, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("status : %s\n", msg_json["status"].GetString());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_reboot"].GetBool())
    {
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
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_set_calibrate_encoder"].GetBool())
    {
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
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["demo_set_encrypt"].GetBool())
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        const rapidjson::Value &demo_set_encrypt = param["demo_set_encrypt"];

        writer.StartObject();
        writer.Key("method");
        writer.String("SET");
        writer.Key("reqTarget");
        writer.String("/encrypt");
        writer.Key("property");
        writer.String("");
        writer.Key("username");
        writer.String(demo_set_encrypt["username"].GetString());
        writer.Key("password");
        writer.String(demo_set_encrypt["password"].GetString());
        writer.EndObject();

        std::string comm_set_config_msg = buffer.GetString();
        buffer.Clear();
        char send_set_config[1024] = {0};
        comm_set_config_msg.copy(send_set_config, sizeof(send_set_config) - 1);
        send_set_config[sizeof(send_set_config) - 1] = '\0';

        for (int i = 0; i < fsa->server_ip_filter_num; i++)
        {
            std::printf("IP: %s sendtodemo_set_encrypt fsa ---> ", fsa->server_ip_filter[i].c_str());
            fsa->demo_set_encrypt(fsa->server_ip_filter[i], send_set_config, ser_msg);
            std::printf("%s\n", ser_msg);

            rapidjson::Document msg_json;
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("status : %s\n", msg_json["status"].GetString());

            fsa->demo_reboot(fsa->server_ip[i], NULL, ser_msg);
            if (msg_json.Parse(ser_msg).HasParseError())
            {
                Logger::get_instance()->print_trace_error("fi_decode() failed\n");
                return 0;
            }
            Logger::get_instance()->print_trace_debug("status : %s\n", msg_json["status"].GetString());
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    return 0;
}