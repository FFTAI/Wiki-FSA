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
    if(fsa->demo_broadcase_filter(ACTUATOR) != FunctionResult::SUCCESS)
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
            if(fsa->demo_clear_fault(ser_list[i], NULL, ser_msg) != FunctionResult::SUCCESS)
            {
                Logger::get_instance()->print_trace_error("send clear fault to server failed\n");
                return FunctionResult::FAILURE;
            }
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    
    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    if (commandType["xxxxxx"].GetBool())
    {
        // enable
        for (int i = 0; i < ser_num; i++)
        {
            memset(ser_msg, 0, sizeof(ser_msg));
            Logger::get_instance()->print_trace_warning("IP: %s sendto  ---> \n", ser_list[i].c_str());

            // fsa->demo_comm_config_get(ser_list[i], NULL, ser_msg);
            Logger::get_instance()->print_trace_debug("recv msg:%s\n", ser_msg);
        }
        usleep(1000); // The necessary delay for running multiple scripts
    }

    




    return 0;
}