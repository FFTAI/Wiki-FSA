/**
 *  2023.7.18
 * Gedit by fourier intelligent
 *
 */

#pragma once
#include "FsaConfig.h"
#include "FsaJsonData.h"
// #include "FsaLog.h"
#include "FsaResult.h"
#include "FsaSocket.h"
#include "FsaStatus.h"
// #include "peripheral/abs_encoder/abs_encoder.h"
// #include "udp_socket/include/udp_socket.h"
#include <chrono>
#include <deque>
#include <memory>
#include <sys/socket.h>
#define MAX_UDP_RECV_DATA_NUM_PER_CYCLE 20

using namespace std;
/**
 * @brief this namespace for connect to physical motor, and allows Command,
 * Feedback, and Info objects to be sent to and recieved from the actuator.
 */
namespace FSA_CONNECT {
/**
 * @brief all command in this class ,only need ip of motor
 */
class FSA {
public:
    // constructor

    void init( const string& ip );
    // FSA &operator=(const FSA &otherAIOS);
    /**
     * @fn GetRootConfig
     * @brief get motor attribute ,which saved in motor drives, you can get some root config such as hardware version
     * @details recommend use python sdk
     * @return return result code, find in "FsaResult.h"
     */
    int GetRootConfig();

    /**
     * @fn GetState
     * @brief get motor state
     * @details sate = 0 when normal conditions
     * @return return result code, find in "FsaResult.h"
     */
    int GetState();
    /**
     * @fn Enable
     * @brief servo on
     * @details must enable before control motor
     * @return return result code, find in "FsaResult.h"
     */
    int Enable();
    /**
     * @fn Disable
     * @brief servo off
     * @details use this func best when end program
     * @return return result code, find in "FsaResult.h"
     */
    int Disable();
    // int SetConfig(const FSAConfig::FSAMotionConfig &motion_controller_config);

    bool is_enabled = false;

    std::shared_ptr< Transmit::UDPSocket > ctrl_udp_socket;
    std::shared_ptr< Transmit::UDPSocket > root_udp_socket;
    std::shared_ptr< Transmit::UDPSocket > fast_udp_socket;
    /**
     * @fn SetControlConfig
     * @brief set motor config
     * @details motor drive has default config, do not use if not necessary
     * @return return result code, find in "FsaResult.h"
     */
    int SetControlConfig( const FSAConfig::FSAControlConfig& config );
    /**
     * @fn SetPIDParams
     * @brief set PID paraments
     * @details motor drive has default pid ,if you want use your pid, must use this func every time, because motor deive can not remember your pid
     * @return return result code, find in "FsaResult.h"
     */
    int SetPIDParams( FSAConfig::FSAPIDParams& pidparams );
    /**
     * @fn GetPIDParams
     * @brief read pid params now
     * @details recommend use python sdk
     * @return return result code, find in "FsaResult.h"
     */

    int SetPDParams( FSAConfig::FSAPIDParams& pidparams );

    int GetPIDParams( FSAConfig::FSAPIDParams& pidparams );
    /**
     * @fn GetControlConfig
     * @brief read control config now
     * @details recommend use python sdk
     * @return return result code, find in "FsaResult.h"
     */
    int GetControlConfig();
    /**
     * @fn SetPosition
     * @brief send ideal position order
     * @details use in realtime loop
     * @return return result code, find in "FsaResult.h"
     */
    int SetPosition( const double& pos, const double& vel_ff, const double& cur_ff );
    /**
     * @fn SetVelocity
     * @brief send ideal velocity order
     * @details use in realtime loop
     * @return return result code, find in "FsaResult.h"
     */
    int SetVelocity( const double& vel, const double& cur_ff );
    /**
     * @fn SetCurrent
     * @brief send ideal current order
     * @details use in realtime loop
     * @return return result code, find in "FsaResult.h"
     */
    int SetCurrent( const double& cur );
    /**
     * @fn GetPVC
     * @brief read actual position, velocity, current from motor drive
     * @details use in realtime loop
     * @return return result code, find in "FsaResult.h"
     */
    int GetPVC( double& pos, double& vel, double& cur );
    /**
     * @fn EnablePosControl
     * @brief set operation of mode : POSITION CONTROL
     * @details must enable this func before use "setpositon"
     * @return return result code, find in "FsaResult.h"
     */
    int EnablePosControl();
    /**
     * @fn EnableVelControl
     * @brief set operation of mode : VELOCITY CONTROL
     * @details must enable this func before use "setvelocity"
     * @return return result code, find in "FsaResult.h"
     */
    int EnableVelControl();
    /**
     * @fn EnableCurControl
     * @brief set operation of mode : CURRENT_CLOSE_LOOP_CONTROL
     * @details must enable this func before use "setcurrent"
     * @return return result code, find in "FsaResult.h"
     */
    int EnableCurControl();

    int EnablePDControl();

    /**
     * @fn SetTorqueLimitMax
     * @brief Torque limiter parameter configuration
     * @details Set the maximum torque limit of the actuator
     * @return return result code, find in "FsaResult.h"
     */
    int SetTorqueLimitMax( const double& torque_limit_max );

    /**
     * @fn SetInertiaCompensation
     * @brief Inertia compensation coefficient configuration
     * @details Set the inertia compensation coefficient of the actuator
     * @return return result code, find in "FsaResult.h"
     */
    int SetInertiaCompensation( const double& inertia_ff );

    int GetNtcTemperature( float& out_mos_temperature, float& out_armature_temperature );

    int FastGetNtcTemperature( float& out_mos_temperature, float& out_armature_temperature );

    int SetControlWord( FSA_CONNECT::Status::FSAControlWord& control_word );

    int SetReturnZeroMode( void );

    int SetFrictionIdentifyMode( void );

    int SetFrictionCompFlag( uint8_t friction_comp_flag );

    int SetControlMode( FSA_CONNECT::Status::FSAModeOfOperation& control_mode );

    int SetPVCTimeoutProtect( uint32_t count, const FSA_CONNECT::FSAConfig::set_pvc_timeout_protect_config_t& config, FSAConfig::ack_ret_t& set_pvc_timeout_protect_ret );

    int ClearSetPVCTimeoutProtect( uint32_t count, uint32_t clear_back_last_mode_of_operation, FSAConfig::ack_ret_t& clear_set_pvc_timeout_protect_ret );

    int GetPVCTError( FSAConfig::pvct_errcode_t& pvct_errcode );

    int SetABSZero();

    int Reboot();

    int GetABSPosition( float& abs_pos );

    int GetVBus( float& VBus );

    int OpenRelay();

    int CloseRelay();

    // std::shared_ptr<Transmit::UDPSocket> pt_udp_socket;
    std::string ip_;

private:
    int get_root_config                 = 0;
    int get_state                       = 0;
    int poweron_state                   = 0;
    int poweroff_state                  = 0;
    int set_ctrlcfg_state               = 0;
    int get_ctrlcfg_state               = 0;
    int set_pid_state                   = 0;
    int set_pd_state                    = 0;
    int get_pid_state                   = 0;
    int set_pos_state                   = 0;
    int set_vel_state                   = 0;
    int set_cur_state                   = 0;
    int get_pvc_state                   = 0;
    int control_state                   = 0;
    int set_torque_limit_max_state      = 0;
    int set_inertia_ff_state            = 0;
    int get_ntc_temperature_state       = 0;
    int set_pvc_timeout_protect_state   = 0;
    int clear_pvc_timeout_protect_state = 0;
    int get_pvct_errorcode_state        = 0;
    int set_abs_zero_state              = 0;
    int reboot_state                    = 0;
    int get_abs_pos_state               = 0;
    int get_vbus_state                  = 0;
    int open_relay_state                = 0;
    int close_relay_state               = 0;

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::milliseconds             int_ms;
    bool                                  motor_drive_ready_;
};

}  // namespace FSA_CONNECT