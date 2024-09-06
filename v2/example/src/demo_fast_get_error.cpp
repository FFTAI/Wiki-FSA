#include "main.h"
using namespace Actuator;
using namespace Utils;
using namespace Predefine;

int main()
{
    std::string ser_list[254] = {""};
    int ip_num = 0;
    if (broadcast((char*)ser_list, ip_num, ACTUATOR) == FunctionResult::FAILURE)
    {
        Logger::get_instance()->print_trace_error("broadcast failed\n");
        return 0;
    }

    int error_code = -1;
    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (fast_get_error(ip, error_code) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s clear fault failed\n", ser_list[i].c_str());
            return FunctionResult::FAILURE;
        }
        Logger::get_instance()->print_trace("error code: %d\n", error_code);
    }
    return 0;
}
