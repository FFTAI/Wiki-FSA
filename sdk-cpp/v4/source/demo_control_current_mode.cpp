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

    int state_obj = -1;
    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (get_state(ip, state_obj) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s get state failed\n", ser_list[i]);
        }
        Logger::get_instance()->print_trace_debug("%s, state: %d\n", ip, state_obj);
    }

    rapidjson::Document ctrl_doc;
    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (comm_config_get(ip, &ctrl_doc) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s get communication configure failed\n", ser_list[i]);
        }
    }

    double pos = 0, vel = 0, cur = 0;
    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (get_pvc(ip, pos, vel, cur) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s get position velocity current failed\n", ser_list[i].c_str());
        }
    }
    
    Logger::get_instance()->print_trace("pos: %lf, vel: %lf, current: %lf\n", pos, vel, cur);

    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (set_current_control(ip, 0.0) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s set current value failed\n", ser_list[i].c_str());
        }
    }

    FsaModeOfOperation modeOfOper;
    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (set_mode_of_operation(ip, modeOfOper.CURRENT_CLOSE_LOOP_CONTROL) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s set current close loop mode failed\n", ser_list[i].c_str());
        }
    }

    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (enable_set(ip) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s enable failed\n", ser_list[i].c_str());
        }
    }

    for (int i = 0; i < 100; i++)
    {
        for (int i = 0; i < ip_num; i++)
        {
            char *ip = (char *)ser_list[i].c_str();
            if (set_current_control(ip, 1.0) == FunctionResult::FAILURE)
            {
                Logger::get_instance()->print_trace_error("%s set current value failed\n", ser_list[i].c_str());
            }
        }

        usleep(1000);
    }

    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (set_current_control(ip, 0.0) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s set current value failed\n", ser_list[i].c_str());
        }
    }

    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (disable_set(ip) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s disable failed\n", ser_list[i].c_str());
        }
    }

    for (int i = 0; i < ip_num; i++)
    {
        char *ip = (char *)ser_list[i].c_str();
        if (set_mode_of_operation(ip, modeOfOper.CURRENT_CLOSE_LOOP_CONTROL) == FunctionResult::FAILURE)
        {
            Logger::get_instance()->print_trace_error("%s set current close loop mode failed\n", ser_list[i].c_str());
        }
    }
    return 0;
}
