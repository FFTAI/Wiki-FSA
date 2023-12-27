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
using namespace Sensor;
using namespace Utils;
using namespace Predefine;
FSA *fse = new FSA();
int main()
{
    if (!(fse->demo_broadcase()))
    {
        for (int i = 0; i < fse->server_ip_num; i++)
        {
            Logger::get_instance()->print_trace("%s\n", fse->server_ip[i]);
        }
        return FunctionResult::SUCCESS;
    }
    return 0;
}
