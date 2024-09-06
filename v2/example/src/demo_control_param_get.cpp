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

    float motor_max_speed = 0, motor_max_acc = 0, max_cur = 0;

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( control_param_get( ip, motor_max_speed, motor_max_acc, max_cur ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s set control param failed\n", ser_list[ i ].c_str() );
        }
        Logger::get_instance()->print_trace( "motor_max_speed: %d, motor_max_acc: %d, motor_cur: %d \n", motor_max_speed, motor_max_acc, max_cur );
    }
}
