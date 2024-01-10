#include "main.h"
using namespace Sensor;
using namespace Utils;
using namespace Predefine;

FSA *fsa = new FSA();

int main()
{
    char ser_msg[1024] = {0};
    fsa->demo_broadcase_filter(ACTUATOR);
    if (fsa->server_ip_filter_num == 0)
    {
        Logger::get_instance()->print_trace_error("Cannot find server\n");
        return 0;
    }

    std::string ser_list[254] = {""};
    memset(ser_list, 0, sizeof(ser_list));
    memcpy(ser_list, fsa->server_ip_filter, sizeof(ser_list));
    int ser_num = fsa->server_ip_filter_num;

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
        fsa->demo_pid_param_imm_get(ser_list[i], NULL, ser_msg);
        std::printf("%s\n", ser_msg);
    }

    char set_ctrl_config[1024] = {0};
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    FSAActuatorType actuatorType;
    FSAActuatorDirection actuatorDir;
    FSAActuatorReductionRatio actuatorRedTat;

    FSAMotorType motorType;
    FSAHardwareType hardwareType;
    FSAMotorVBUS motorBus;
    FSAMotorDirection motorDir;
    FSAMotorPolePairs motorPolePairs;
    FSAMotorMaxSpeed motorMaxSpeed;
    FSAMotorMaxAcceleration motorMaxAcc;
    FSAEncoderDirection encoderDir;

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto set ctrl config fsa ---> ", ser_list[i].c_str());
        // make send json
        writer.StartObject();

        writer.Key("method");
        writer.String("SET");
        
        writer.Key("reqTarget");
        writer.String("/config");
        
        writer.Key("property");
        writer.String("");

        writer.Key("actuator_type");
        writer.Uint64(actuatorType.TYPE_DEFAULT);
        
        writer.Key("actuator_direction");
        writer.Int(actuatorDir.DIRECTION_NORMAL);
        
        writer.Key("actuator_reduction_ratio");
        writer.Int(actuatorRedTat.REDUCTION_RATIO_30);

        writer.Key("motor_type");
        writer.Int(motorType.FSA80_10V0);
        
        writer.Key("motor_hardware_type");
        writer.Int(hardwareType.TYPE_H66V104);
        
        writer.Key("motor_vbus");
        writer.Int(motorBus.VBUS_36V);

        writer.Key("motor_direction");
        writer.Int(motorDir.ACB);
        
        writer.Key("motor_pole_pairs");
        writer.Int(motorPolePairs.POLE_PAIRS_10);
        
        writer.Key("motor_max_speed");
        writer.Int(motorMaxSpeed.MAX_SPEED_3000);

        writer.Key("motor_max_acceleration");
        writer.Int(motorMaxAcc.MAX_ACCELERATION_60000);
        
        writer.Key("motor_max_current");
        writer.Int(10);
        
        writer.Key("encoder_direction");
        writer.Int(encoderDir.DIRECTION_CCW);
        writer.EndObject();

        memcpy(set_ctrl_config, &buffer, sizeof(buffer));

        fsa->demo_ctrl_config_set(ser_list[i], set_ctrl_config, ser_msg);
        std::printf("%s\n", ser_msg);

        rapidjson::Document msg_json;
        if (msg_json.Parse(ser_msg).HasParseError())
        {
            Logger::get_instance()->print_trace_error("fi_decode() failed\n");
            return 0;
        }
        Logger::get_instance()->print_trace_debug("status : %s\n", msg_json["status"].GetString());
    }

    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s sendto get ctrl config fsa ---> ", ser_list[i].c_str());
        fsa->demo_ctrl_config_get(ser_list[i], NULL, ser_msg);
    }

    for (int i = 0; i < fsa->server_ip_num; i++)
    {
        std::printf("IP: %s sendto reboot fsa ---> ", fsa->server_ip[i].c_str());
        fsa->demo_reboot(fsa->server_ip[i], NULL, ser_msg);
    }

    return 0;
}
