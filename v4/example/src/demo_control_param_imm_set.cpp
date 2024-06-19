#include "main.h"
using namespace Actuator;
using namespace Utils;
using namespace Predefine;

int main() {
    std::string ser_list[ 254 ] = { "" };
    int         ip_num          = 0;
    if ( broadcast( ( char* )ser_list, ip_num, ACTUATOR ) == FunctionResult::FAILURE ) {
        Logger::get_instance()->print_trace_error( "broadcast failed\n" );
        return 0;
    }

    float motor_max_speed = 2500, motor_max_acc = 5714, max_cur = 10;

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( control_param_imm_set( ip, motor_max_speed, motor_max_acc, max_cur ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s set control param failed\n", ser_list[ i ].c_str() );
            return FunctionResult::FAILURE;
        }
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( control_param_imm_get( ip, motor_max_speed, motor_max_acc, max_cur ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s set control param failed\n", ser_list[ i ].c_str() );
            return FunctionResult::FAILURE;
        }
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( reboot_actuator( ip ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s reboot actuator failed\n", ser_list[ i ].c_str() );
            return FunctionResult::FAILURE;
        }
    }
}
