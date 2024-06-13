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
        if (comm_config_set(ip, "FSA", false, "fftai-12", "fftai2015", 204) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s set communication failed\n", ser_list[i].c_str());
            return FunctionResult::FAILURE;
        }
    }
    return 0;
}
