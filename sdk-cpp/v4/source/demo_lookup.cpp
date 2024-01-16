/**
 * @file demo_lookup.cpp
 * @author Afer
 * @brief
 * @version 0.1
 * @date 2023-12-21
 * @note pass-test
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"
using namespace Actuator;
using namespace Utils;
using namespace Predefine;
FSA *fsa = new FSA();
int main()
{
    if (!(fsa->demo_broadcase()))
    {
        for (int i = 0; i < fsa->server_ip_num; i++)
        {
            Logger::get_instance()->print_trace("%s\n", fsa->server_ip[i].c_str());
        }
        return FunctionResult::SUCCESS;
    }
    return 0;
}
