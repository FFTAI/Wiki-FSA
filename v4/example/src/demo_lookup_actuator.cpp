#include "main.h"
using namespace Actuator;
using namespace Utils;
using namespace Predefine;

int main() {
    std::string ser_list[ 254 ] = { "" };
    int         ip_num          = 0;
    if ( lookup_actuator( ( char* )ser_list, ip_num ) == FunctionResult::FAILURE ) {
        Logger::get_instance()->print_trace_error( "lookup actuator failed\n" );
        return 0;
    }
    for ( int i = 0; i < ip_num; i++ ) {
        Logger::get_instance()->print_trace_debug( "ip%d: %s\n", i, ser_list[ i ].c_str() );
    }
    return 0;
}
