#include "main.h"
using namespace Actuator;
using namespace Utils;
using namespace Predefine;

int main()
{
    std::string ser_list[254] = {""};
    int ip_num = 0;
    if (broadcast((char *)ser_list, ip_num, ACTUATOR) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("broadcast failed\n");
        return 0;
    }
    rapidjson::Document msg_json;
    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (comm_config_get(ip, &msg_json) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s get communication configure failed\n", ser_list[i]);
            return FunctionResult::FAILURE;
        }
        Logger::get_instance()->print_trace_debug("status: %s, name: %s\n", msg_json["status"].GetString(), msg_json["name"].GetString());
    }
    return 0;
}