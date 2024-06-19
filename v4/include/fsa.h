#ifndef _FSA_H_
#define _FSA_H_

#include <arpa/inet.h>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

#include "function_result.h"
#include "logger.h"
#include "os.h"
// #include "actuator.h"
#define BUFFER_SIZE 1024
#define SERVER_PORT_COMM 2334
#define SERVER_PORT_CTRL 2333

#define ABSCODER "AbsEncoder"
#define ACTUATOR "Actuator"
#define CTRLBOX "CtrlBox"

// using namespace Interface;

namespace Actuator {
// class FSA : public Actuator
class FSA {
private:
    const char* broadcast_ip     = "192.168.137.255";
    int         server_port_comm = 2334;
    int         server_port_ctrl = 2333;

    int                 fsa_socket;
    int                 broadcast_socket;
    struct sockaddr_in  fsa_sockaddr_in_comm, fsa_sockaddr_in_ctrl, fsa_sockaddr_in_work, fsa_sockaddr_in_recv;
    socklen_t           sockaddr_len = sizeof( struct sockaddr_in );
    char*               work_ip;
    int                 is_init = 0;
    rapidjson::Document msg_json;

    char recv_buff[ BUFFER_SIZE ];
    char send_buff[ BUFFER_SIZE ];

    int work_mode = 0;

    // send json
    const char* json_test          = "{\"test\":\"param\",\"test\":\"param\"}";
    const char* json_broadcast_msg = "Is any fourier smart server here?";

    const char* json_get_root              = "{\"method\":\"GET\",\"reqTarget\":\"/\",\"property\":\"\"}";
    const char* json_set_enable            = "{\"method\":\"SET\",\"reqTarget\":\"/"
                                             "control_word\",\"property\":\"\",\"control_word\":15}";
    const char* json_set_disenable         = "{\"method\":\"SET\",\"reqTarget\":\"/"
                                             "control_word\",\"property\":\"\",\"control_word\":6}";
    const char* json_set_calibrate_encoder = "{\"method\":\"SET\",\"reqTarget\":\"/"
                                             "control_word\",\"property\":\"\",\"control_word\":163}";
    const char* json_clear_fault           = "{\"method\":\"SET\",\"reqTarget\":\"/"
                                             "control_word\",\"property\":\"\",\"control_word\":134}";
    const char* json_get_state             = "{\"method\":\"GET\",\"reqTarget\":\"/state\",\"property\":\"\"}";

    const char* json_get_home_offset   = "{\"method\":\"GET\",\"reqTarget\":\"/home_offset\",\"property\":\"\"}";
    const char* json_set_home_position = "{\"method\":\"SET\",\"reqTarget\":\"/home_position\",}";
    const char* json_get_pid_param     = "{\"method\":\"GET\",\"reqTarget\":\"/pid_param\",\"property\":\"\"}";
    const char* json_get_pid_imm_param = "{\"method\":\"GET\",\"reqTarget\":\"/pid_param_imm\",\"property\":\"\"}";

    const char* json_clear_pid_param = "{\"method\":\"SET\",\"reqTarget\":\"/"
                                       "pid_param\",\"property\":\"clear\"}";

    const char* json_get_control_param = "{\"method\":\"GET\",\"reqTarget\":\"/control_param\",\"property\":\"\"}";

    const char* json_get_control_param_imm = "{\"method\":\"GET\",\"reqTarget\":\"/"
                                             "control_param_imm\",\"property\":\"\"}";

    const char* json_get_flag_of_operation = "{\"method\":\"GET\",\"reqTarget\":\"/"
                                             "flag_of_operation\",\"property\":\"\"}";

    const char* json_clear_flag_of_operation = "{\"method\":\"GET\",\"reqTarget\":\"/"
                                               "flag_of_operation\",\"property\":\"clear\"}";
    const char* json_get_config              = "{\"method\":\"GET\",\"reqTarget\":\"/config\",\"property\":\"\"}";
    const char* json_save_config             = "{\"method\":\"SET\",\"reqTarget\":\"/config\",\"property\":\"save\"}";
    const char* json_erase_config            = "{\"method\":\"SET\",\"reqTarget\":\"/config\",\"property\":\"erase\"}";
    const char* json_reboot                  = "{\"method\":\"SET\",\"reqTarget\":\"/reboot\",\"property\":\"\"}";
    const char* json_get_error_code          = "{\"method\":\"GET\",\"reqTarget\":\"/error_code\",\"property\":\"\"}";
    const char* json_get_pvc                 = "{\"method\":\"GET\",\"reqTarget\":\"/"
                                               "measured\",\"position\":True,\"velocity\":True,\"current\":True}";
    const char* json_get_pvcc                = "{\"method\":\"GET\",\"reqTarget\":\"/"
                                               "measured\",\"position\":True,\"velocity\":True,"
                                               "\"current\":True,\"current_id\":True}";
    const char* json_get_pvcccc              = "{\"method\":\"GET\",\"reqTarget\":\"/"
                                               "measured\",\"position\":True,\"velocity\":True,"
                                               "\"current\":True,\"current_id\":True,\"phase_"
                                               "current_ib\":True,\"phase_current_ic\":True}";

    // GROUP START
    const char* json_get_abs_encoder_angle = "{\"method\":\"GET\",\"reqTarget\":\"/measured\",\"property\":\"\"}";
    // GROUP END

    const char* json_get_comm_root    = "{\"method\":\"GET\",\"reqTarget\":\"/\",\"property\":\"\"}";
    const char* json_get_comm_config  = "{\"method\":\"GET\",\"reqTarget\":\"/config\",\"property\":\"\"}";
    const char* json_save_comm_config = "{\"method\":\"SET\",\"reqTarget\":\"/config\",\"property\":\"save\"}";
    // cha
    // OTA START
    const char* json_ota              = "{\"method\":\"SET\",\"reqTarget\":\"/ota\",\"property\":\"\"}";
    const char* json_ota_test         = "{\"method\":\"SET\",\"reqTarget\":\"/ota_test\",\"property\":\"\"}";
    const char* json_ota_devel        = "{\"method\":\"SET\",\"reqTarget\":\"/ota_devel\",\"property\":\"\"}";
    const char* json_ota_cloud        = "{\"method\":\"SET\",\"reqTarget\":\"/ota_cloud\",\"property\":\"\"}";
    const char* json_ota_driver       = "{\"method\":\"SET\",\"reqTarget\":\"/ota_driver\",\"property\":\"\"}";
    const char* json_ota_driver_test  = "{\"method\":\"SET\",\"reqTarget\":\"/"
                                        "ota_driver_test\",\"property\":\"\"}";
    const char* json_ota_driver_devel = "{\"method\":\"SET\",\"reqTarget\":\"/"
                                        "ota_driver_devel\",\"property\":\"\"}";
    const char* json_ota_driver_cloud = "{\"method\":\"SET\",\"reqTarget\":\"/"
                                        "ota_driver_cloud\",\"property\":\"\"}";
    // OTA END

public:
    const char* server_filter_type = " ";
    enum WorkMode {
        NONE = 0,
        BROADCAST_MODE,
        BROADCASE_FILTER_MODE,
        SERVER_IP_MODE,
    };
    char*       server_ip[ 254 ];
    std::string server_ip_filter[ 254 ];
    int         server_ip_num        = 0;
    int         server_ip_filter_num = 0;

private:
    int init_network();
    int communicate( char* ip, int port, const char* sendmsg, char* client_recv_msg );
    int decode( char* msg );
    int encode();
    int send_msg( char* ip, int port, const char* msg );
    int recv_msg( char* client_recv_msg );

public:
    FSA( /* args */ );
    ~FSA();

    int init();

    int broadcast();

    int broadcast_filter();

    int clear_fault( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int comm_config_get( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int comm_config_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int control_current_mode( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int control_param_imm_get( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int control_param_imm_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int control_param_get( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int control_param_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int ctrl_config_get( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int ctrl_config_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int ctrl_config_save( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int disable_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int enable_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int flag_of_operation_get( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int flag_of_operation_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int get_abs_encoder_value( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int get_measured( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int get_pvc( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int get_pvcc( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int get_pvcccc( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int get_state( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int home_offset_get( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int home_offset_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int home_position_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int lookup_abs_encoder( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int lookup_actuator( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int lookup_ctrlbox( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int lookup( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int ota_cloud( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int ota_test( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int ota( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int ota_devel( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int ota_driver_cloud( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int ota_driver_devel( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int ota_driver_test( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int ota_driver( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int pid_param_get( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int pid_param_imm_get( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int pid_param_imm_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int pid_param_set( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int reboot( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int reboot_actuator( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int set_calibrate_encoder( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int set_encrypt( char* server_ip, char* define_msg_sendto, char* client_recv_msg );

    int set_mode_of_operation( char* server_ip, int mode, char* client_recv_msg );
    int set_position_control( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int set_velocity_control( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
    int set_current_control( char* server_ip, char* define_msg_sendto, char* client_recv_msg );
};

class FSAActuatorType {
public:
    uint64_t TYPE_DEFAULT     = 0x00000001;
    uint64_t TYPE_25_10_C_1   = 0x01010301;
    uint64_t TYPE_25_10_C_30  = 0x01010303;
    uint64_t TYPE_25_10_D_1   = 0x01010401;
    uint64_t TYPE_25_10_D_FSA = 0x02010403;
    uint64_t TYPE_60_10_D_1   = 0x03010401;
    uint64_t TYPE_60_10_D_120 = 0x03010405;
    uint64_t TYPE_80_21_C_1   = 0x04020301;
    uint64_t TYPE_80_21_C_30  = 0x04020303;
    uint64_t TYPE_100_21_A_1  = 0x05020101;
    uint64_t TYPE_100_21_A_7  = 0x05020102;
    uint64_t TYPE_130_21_A_1  = 0x06020101;
    uint64_t TYPE_130_21_A_7  = 0x06020102;
};

class FSAActuatorDirection {
public:
    int DIRECTION_NORMAL  = 1;
    int DIRECTION_REVERSE = -1;
};

class FSAActuatorReductionRatio {
public:
    int REDUCTION_RATIO_7   = 7;
    int REDUCTION_RATIO_30  = 30;
    int REDUCTION_RATIO_50  = 50;
    int REDUCTION_RATIO_70  = 70;
    int REDUCTION_RATIO_100 = 100;
    int REDUCTION_RATIO_120 = 120;
};

class FSAMotorType {
public:
    int TYPE_MOTOR_NULL = 0;
    int TYPE_JIOALONG   = 1;
    int FSA130_20V0     = 2;
    int FSA100_15V0     = 3;
    int FSA80_10V0      = 4;
    int FSA60_08V0      = 5;
    int FSA36_08V0      = 6;
    int FSA25_08V0      = 7;
    int FSA36_10V0      = 8;
};

class FSAHardwareType {
public:
    int TYPE_HARDWARE_NULL = 0;
    int TYPE_ODrive_V36    = 1;
    int TYPE_H95V104       = 2;
    int TYPE_H66V104       = 3;
    int TYPE_H46V104       = 4;
    int TYPE_H30V303       = 5;
};

class FSAMotorVBUS {
public:
    int VBUS_36V = 36;
    int VBUS_48V = 48;
};

class FSAMotorDirection {
public:
    int ABC = 1;
    int ACB = -1;
};

class FSAMotorPolePairs {
public:
    int POLE_PAIRS_7  = 7;
    int POLE_PAIRS_10 = 10;
    int POLE_PAIRS_21 = 21;
};

class FSAMotorMaxSpeed {
public:
    int MAX_SPEED_1000 = 1000;  // rpm
    int MAX_SPEED_2000 = 2000;  // rpm
    int MAX_SPEED_3000 = 3000;  // rpm
};

class FSAMotorMaxAcceleration {
public:
    int MAX_ACCELERATION_6000  = 6000;   // rpm/s
    int MAX_ACCELERATION_10000 = 10000;  // rpm/s
    int MAX_ACCELERATION_30000 = 30000;  // rpm/s
    int MAX_ACCELERATION_60000 = 60000;  // rpm/s
};

class FSAEncoderDirection {
public:
    int DIRECTION_CW  = -1;
    int DIRECTION_CCW = 1;
};

class FSAFlagState {
public:
    int CLEAR = 0;
    int SET   = 1;
};

class FsaModeOfOperation {
public:
    int NONE                       = 0;
    int CURRENT_CLOSE_LOOP_CONTROL = 4;
    int VELOCITY_CONTROL           = 3;
    int POSITION_CONTROL           = 1;
    int TRAPEZOIDAL_CONTROL        = 5;
};

/****************************user interface***********************************/
std::string int_array_to_ip( const std::vector< int >& int_array );  //

int broadcast( char* ip_ser, int& ser_num );
int broadcast( char* ip_ser, int& ser_num, const char* type );

int clear_fault( char* ip );
int comm_config_get( char* ip, rapidjson::Document* config_buffer );
int comm_config_set( char* ip, std::string name, bool dhcp_enable, std::string ssid, std::string password, uint8_t static_ip );

int control_param_set( char* ip, float vel, float acc, float cur );
int control_param_get( char* ip, float& vel, float& acc, float& cur );
int control_param_imm_get( char* ip, float& vel, float& acc, float& cur );
int control_param_imm_set( char* ip, float vel, float acc, float cur );

int set_current_control( char* ip, double current_val );
int set_position_control( char* ip, double pos, double vel, double acc );
int set_velocity_control( char* ip, double vel, double cur_ff );
int set_mode_of_operation( char* ip, uint8_t mode );

int ctrl_config_get( char* ip, rapidjson::Document* config_buffer );
int ctrl_config_set( char* ip, uint64_t act_type, int dir, int redTat, int motor_type, int hardware_type, int vbus, int motor_dir, int pole_pairs, int max_speed, int max_acc, int encoder_dir );
int ctrl_config_save( char* ip );

int enable_set( char* ip );
int disable_set( char* ip );

int flag_of_operation_set( char* ip, int act_val, int motor_val, int encoder_val, int pid_val );
int flag_of_operation_get( char* ip, bool& act_val, bool& motor_val, bool& encoder_val, bool& pid_val );

int get_abs_encoder_value( char* ip, double& angle );
int get_measured( char* ip, double& pos, double& vel, double& cur );

int get_pvc( char* ip, double& pos, double& vel, double& cur );
int get_pvcc( char* ip, double& pos, double& vel, double& cur );
int get_pvcccc( char* ip, double& pos, double& vel, double& cur );
int get_state( char* ip, int& state_val );

int home_offset_get( char* ip, double& angle );
int home_offset_set( char* ip, double angle );
int home_position_set( char* ip );

int lookup( char* ip_ser, int& ser_num );
int lookup_abs_encoder( char* ip_ser, int& ser_num );
int lookup_ctrlbox( char* ip_ser, int& ser_num );
int lookup_actuator( char* ip_ser, int& ser_num );

int ota( char* ip );
int ota_test( char* ip );
int ota_devel( char* ip );
int ota_cloud( char* ip );

int ota_driver( char* ip );
int ota_driver_test( char* ip );
int ota_driver_devel( char* ip );
int ota_driver_cloud( char* ip );

int pid_param_get( char* ip, rapidjson::Document* config_buffer );
int pid_param_set( char* ip, double pos_kp, double vel_kp, double vel_ki, double cur_kp, double cur_ki );
int pid_param_imm_get( char* ip, rapidjson::Document* config_buffer );
int pid_param_imm_set( char* ip, double pos_imm_kp, double vel_imm_kp, double vel_imm_ki, double cur_imm_kp, double cur_imm_ki );

int reboot( char* ip );
int reboot_actuator( char* ip );

int set_calibrate_encoder( char* ip );
int set_encrypt( char* ip, const char* username, const char* passwold );

}  // namespace Actuator

#endif  // !__IF_FSA_H__
