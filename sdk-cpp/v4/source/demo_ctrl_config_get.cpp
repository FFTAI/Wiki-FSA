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

    rapidjson::Document ctrl_config_json;
    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (ctrl_config_get(ip, &ctrl_config_json) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s clear fault failed\n", ser_list[i]);
        }
        Logger::get_instance()->print_trace_debug("status: %s\n", ctrl_config_json["status"].GetString());
    }
    return 0;
}
