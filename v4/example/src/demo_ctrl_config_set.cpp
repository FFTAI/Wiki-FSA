#include "main.h"
using namespace Actuator;
using namespace Utils;
using namespace Predefine;

int main() {
    FSAActuatorType           actuatorType;
    FSAActuatorDirection      actuatorDir;
    FSAActuatorReductionRatio actuatorRedTat;
    FSAMotorType              motorType;
    FSAHardwareType           hardwareType;
    FSAMotorVBUS              motorBus;
    FSAMotorDirection         motorDir;
    FSAMotorPolePairs         motorPolePairs;
    FSAMotorMaxSpeed          motorMaxSpeed;
    FSAMotorMaxAcceleration   motorMaxAcc;
    FSAEncoderDirection       encoderDir;

    std::string ser_list[ 254 ] = { "" };
    int         ip_num          = 0;
    if ( broadcast( ( char* )ser_list, ip_num, ACTUATOR ) == FunctionResult::FAILURE ) {
        Logger::get_instance()->print_trace_error( "broadcast failed\n" );
        return 0;
    }
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( ctrl_config_set( ip, actuatorType.TYPE_DEFAULT, actuatorDir.DIRECTION_NORMAL, actuatorRedTat.REDUCTION_RATIO_30, motorType.FSA80_10V0, hardwareType.TYPE_H66V104, motorBus.VBUS_36V,
                              motorDir.ACB, motorPolePairs.POLE_PAIRS_10, motorMaxSpeed.MAX_SPEED_3000, motorMaxAcc.MAX_ACCELERATION_60000, encoderDir.DIRECTION_CCW )
             == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s save control config failed\n", ser_list[ i ].c_str() );
        }
    }

    rapidjson::Document ctrl_config_json;
    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( ctrl_config_get( ip, &ctrl_config_json ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s clear fault failed\n", ser_list[ i ].c_str() );
        }
        Logger::get_instance()->print_trace_debug( "status: %s\n", ctrl_config_json[ "status" ].GetString() );
    }

    for ( int i = 0; i < ip_num; i++ ) {
        char* ip = ( char* )ser_list[ i ].c_str();
        if ( reboot_actuator( ip ) == FunctionResult::FAILURE ) {
            Logger::get_instance()->print_trace_error( "%s reboot actuator failed\n", ser_list[ i ].c_str() );
            return FunctionResult::FAILURE;
        }
    }

    return 0;
}
