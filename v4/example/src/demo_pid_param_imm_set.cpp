#include "main.h"
using namespace Actuator;
using namespace Utils;
using namespace Predefine;

class SpeedParamList {
public:
    float MOTOR_NULL[ 10 ] = { 2, 0.0, 0.1, 0.01, 0, 0, 0, 0, 5000, 600 };
    float JioaLong[ 10 ]   = { 2, 0.0, 0.005, 0.0001, 0, 0, 0, 0, 5000, 600 };
    float FSA13020V0[ 10 ] = { 2, 0.0, 0.055, 0.00005, 100, 200, 10000, 0, 500, 400 };
    float FSA10015V0[ 10 ] = { 2, 0.0, 0.05, 0.001, 100, 200, 10000, 0, 1000, 600 };
    float FSA8010V0[ 10 ]  = { 2, 0.0, 0.005, 0.00001, 100, 200, 10000, 0, 500, 400 };
    float FSA6008V0[ 10 ]  = { 2, 0.0, 0.04, 0.0001, 20, 100, 50000, 1, 5000, 600 };
    float FSA3608V0[ 10 ]  = { 2, 0.0, 0.0025, 0.00001, 100, 200, 10000, 0, 5000, 600 };
    float FSA3610V0[ 10 ]  = { 2, 0.0, 0.007, 0.00001, 100, 200, 10000, 0, 5000, 600 };
    float FSA2508V0[ 10 ]  = { 2, 0.0, 0.05, 0.0001, 200, 800, 5000, 0, 5000, 600 };
};

int main() {
    std::string ser_list[ 254 ] = { "" };
    int         ip_num          = 0;
    if ( broadcast( ( char* )ser_list, ip_num, ACTUATOR ) == FunctionResult::FAILURE ) {
        Logger::get_instance()->print_trace_error( "broadcast failed\n" );
        return 0;
    }

    rapidjson::Document pid_json;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( pid_param_imm_get( ip, &pid_json ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "get pid param failed\n" );
            return FunctionResult::FAILURE;
        }
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( pid_param_imm_set( ip, 0.005714, 0.02, 0.5, 0.01, 0.0001 ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "set pid param failed\n" );
            return FunctionResult::FAILURE;
        }
    }
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( pid_param_imm_get( ip, &pid_json ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "get pid param failed\n" );
            return FunctionResult::FAILURE;
        }
    }

    return 0;
}
