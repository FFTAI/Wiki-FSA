/**
 * @file demo_.cpp
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

rapidjson::Document msg_json;

int main() {

    std::string ser_list[ 254 ] = { "" };
    int         ip_num          = 0;
    if ( broadcast( ( char* )ser_list, ip_num, ACTUATOR ) == FunctionResult::FAILURE ) {
        Logger::get_instance()->print_trace_error( "broadcast failed\n" );
        return 0;
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( set_calibrate_encoder( ip ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s reboot failed\n", ser_list[ i ].c_str() );
        }
    }
    return 0;
}
