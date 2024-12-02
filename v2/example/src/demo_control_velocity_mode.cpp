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

    int state_obj = -1;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( get_state( ip, state_obj ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s get state failed\n", ser_list[ i ].c_str() );
        }
        Logger::get_instance()->print_trace_debug( "state: %d\n", state_obj );
    }

    rapidjson::Document ctrl_config_json;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( ctrl_config_get( ip, &ctrl_config_json ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s clear fault failed\n", ser_list[ i ].c_str() );
        }
        Logger::get_instance()->print_trace_debug( "status: %s\n", ctrl_config_json[ "status" ].GetString() );
    }

    double pos = 0, vel = 0, cur = 0;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( get_pvc( ip, pos, vel, cur ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s enable failed\n", ser_list[ i ].c_str() );
        }
    }
    Logger::get_instance()->print_trace( "pos: %lf, vel: %lf, current: %lf\n", pos, vel, cur );

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( set_velocity_control( ip, 0.0, 0.0 ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s enable failed\n", ser_list[ i ].c_str() );
        }
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( enable_set( ip ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s enable failed\n", ser_list[ i ].c_str() );
        }
    }

    FsaModeOfOperation modeOfOper;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( set_mode_of_operation( ip, modeOfOper.VELOCITY_CONTROL ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s set velocity mode failed\n", ser_list[ i ].c_str() );
        }
    }

    for ( uint32_t j = 0; j < 1000; j++ ) {
        for ( int i = 0; i < ip_num; i++ ) {
            char* ip = ( char* )ser_list[ i ].c_str();
            if ( set_velocity_control( ip, 360 * sin( j / 100000.0 ), 0.0 ) == FunctionResult::FAILURE ) {
                Logger::get_instance()->print_trace_error( "%s set velocity mode failed\n", ser_list[ i ].c_str() );
            }
        }
        // usleep( 1000 );
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( set_velocity_control( ip, 0.0, 0.0 ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s enable failed\n", ser_list[ i ].c_str() );
        }
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( disable_set( ip ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s enable failed\n", ser_list[ i ].c_str() );
        }
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( set_mode_of_operation( ip, modeOfOper.VELOCITY_CONTROL ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s set velocity mode failed\n", ser_list[ i ].c_str() );
        }
    }

    return 0;
}