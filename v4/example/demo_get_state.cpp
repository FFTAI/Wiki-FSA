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
    return 0;
}
