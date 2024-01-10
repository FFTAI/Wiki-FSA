#include "main.h"
using namespace Sensor;
using namespace Utils;
using namespace Predefine;

FSA *fsa = new FSA();

class SpeedParamList
{
public:
    float MOTOR_NULL[10] = {2, 0.0, 0.1, 0.01, 0, 0, 0, 0, 5000, 600};
    float JioaLong[10] = {2, 0.0, 0.005, 0.0001, 0, 0, 0, 0, 5000, 600};
    float FSA13020V0[10] = {2, 0.0, 0.055, 0.00005, 100, 200, 10000, 0, 500, 400};
    float FSA10015V0[10] = {2, 0.0, 0.05, 0.001, 100, 200, 10000, 0, 1000, 600};
    float FSA8010V0[10] = {2, 0.0, 0.005, 0.00001, 100, 200, 10000, 0, 500, 400};
    float FSA6008V0[10] = {2, 0.0, 0.04, 0.0001, 20, 100, 50000, 1, 5000, 600};
    float FSA3608V0[10] = {2, 0.0, 0.0025, 0.00001, 100, 200, 10000, 0, 5000, 600};
    float FSA3610V0[10] = {2, 0.0, 0.007, 0.00001, 100, 200, 10000, 0, 5000, 600};
    float FSA2508V0[10] = {2, 0.0, 0.05, 0.0001, 200, 800, 5000, 0, 5000, 600};
};

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

    // get pid param
    for (int i = 0; i < ser_num; i++)
    {
        std::printf("IP: %s demo_pid_param_get fsa ---> ", ser_list[i].c_str());
        fsa->demo_pid_param_imm_get(ser_list[i], NULL, ser_msg);
        std::printf("%s\n", ser_msg);
    }

    return 0;
}
