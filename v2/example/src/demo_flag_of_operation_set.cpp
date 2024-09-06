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

    rapidjson::Document ctrl_config_json;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( ctrl_config_get( ip, &ctrl_config_json ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s clear fault failed\n", ser_list[ i ].c_str() );
        }
        Logger::get_instance()->print_trace_debug( "status: %s\n", ctrl_config_json[ "status" ].GetString() );
    }

    FSAFlagState flagState;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( flag_of_operation_set( ip, flagState.CLEAR, flagState.CLEAR, flagState.CLEAR, flagState.CLEAR ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "set flag of operation failed\n" );
            return FunctionResult::FAILURE;
        }
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( ctrl_config_get( ip, &ctrl_config_json ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s get control config failed\n", ser_list[ i ].c_str() );
        }
        Logger::get_instance()->print_trace_debug( "status: %s\n", ctrl_config_json[ "status" ].GetString() );
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( reboot_actuator( ip ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s reboot actuator failed\n", ser_list[ i ].c_str() );
        }
        Logger::get_instance()->print_trace_debug( "status: %s\n", ctrl_config_json[ "status" ].GetString() );
    }
    return 0;
}
