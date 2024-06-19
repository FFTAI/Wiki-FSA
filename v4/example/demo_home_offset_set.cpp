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

    double get_angle_val = 0.0;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( home_offset_get( ip, get_angle_val ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s get home offset failed\n", ser_list[ i ].c_str() );
        }
        Logger::get_instance()->print_trace( "get angle: %lf \n", get_angle_val );
    }

    get_angle_val = 0.0;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( home_offset_set( ip, get_angle_val ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s set home offset failed\n", ser_list[ i ].c_str() );
        }
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( reboot_actuator( ip ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s reboot actuator failed\n", ser_list[ i ].c_str() );
        }
    }
    return 0;
}
