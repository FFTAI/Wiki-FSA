#pragma once
#include <stddef.h>
#include <stdint.h>

#include <string>

/* Use C linkage when compiling this C library header from C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * API call status codes
 */
typedef enum FourierStatusCode {
  FourierStatusFailure = -1,  // Failure
  FourierStatusSuccess = 0,   // Success
} FourierStatusCode;

/**
 * motor number
 */
typedef enum MotorNumber {
  MOTOR_NUMBER_0 = 0,  // reserved
  MOTOR_NUMBER_1 = 1,  // in use
} MotorNumber;

/**
 * Initialize the actuator state
 */
typedef enum AxisState {
  AXIS_STATE_UNKNOW = -1,
  AXIS_STATE_IDLE = 1,    // idle state
  AXIS_STATE_ENABLE = 8,  // enabling state
} AxisState;

/**
 * Actuator control mode
 */
typedef enum ControlMode {
  VOLTAGE_CONTROL = 0,     //
  CURRENT_CONTROL = 1,     //
  VELOCITY_CONTROL = 2,    //
  POSITION_CONTROL = 3,    //
  TRAJECTORY_CONTROL = 4,  //
} ControlMode;

/**
 * Actuator input mode
 */
typedef enum InputMode {
  INPUT_MODE_INACTIVE = 0,
  INPUT_MODE_PASSTHROUGH = 1,
  INPUT_MODE_VEL_RAMP = 2,
  INPUT_MODE_POS_FILTER = 3,
  INPUT_MODE_TRAP_TRAJ = 5,
  INPUT_MODE_TORQUE_RAMP = 6,
} InputMode;

////////////////////////////////////////////////////////////////////////////////
/// Command Enums
////////////////////////////////////////////////////////////////////////////////
typedef enum FourierCommandEnum {
  UNKNOW = -1,
  // float
  FourierCommandCurrent = CURRENT_CONTROL,
  FourierCommandVelocity = VELOCITY_CONTROL,
  FourierCommandPosition = POSITION_CONTROL,
  FourierCommandVelocityRamp,
  FourierCommandTrapezoidal,
  FourierCommandTorquePt,    // fw2.0
  FourierCommandVelocityPt,  // fw2.0
  FourierCommandPositionPt,  // fw2.0
  FourierCommandEnable,

  // bool
  FourierCommandReboot,
  FourierCommandGetError,
  FourierCommandClearError,

  FourierCommandResetLinearCount,        // Reset Linear Count
  FourierCommandMotionControllerConfig,  // Motion Controller Config
  FourierCommandMotorConfig,             // Motor Config
  FourierCommandTrapTraj,                // TRAP-TRAJ
  FourierCommandSaveConfig,              // Save Config

  FourierCommandNetworkSetting,  // network setting

  FourierCommandLatencyTest,  // For Latency Test

  FourierCommandCtrlBoxPower,  // For CtrlBox power status
} FourierCommandEnum;

////////////////////////////////////////////////////////////////////////////////
/// Feedback Enums
////////////////////////////////////////////////////////////////////////////////
typedef enum FourierFeedbackCode {
  FourierFeedbackAll = 1,  //
  FourierFeedbackCVP = 2,  // Used for ovtaining actuator position,velocity and
                           // current at high speed
} FourierFeedbackCode;

////////////////////////////////////////////////////////////////////////////////
/// Typedefs
////////////////////////////////////////////////////////////////////////////////
/**
 * To return group objects, only one Lookup object is required per application
 */
typedef struct FourierLookup_ *FourierLookupPtr;

/**
 * The entry generated after the lookup
 */
typedef struct FourierLookupEntryList_ *FourierLookupEntryListPtr;

/**
 * Represents a connection to a group of modules. Sends commands to and receives
 * feedback from the group.
 */
typedef struct FourierGroup_ *FourierGroupPtr;

/**
 * Encapsulates feedback received from a module
 */
typedef struct FourierGroupFeedback_ *FourierGroupFeedbackPtr;

/**
 * Encapsulates data to be sent to a module
 */
typedef struct FourierGroupCommand_ *FourierGroupCommandPtr;

typedef void (*GroupFeedbackHandlerFunction)(FourierGroupFeedbackPtr fbk,
                                             void *user_data);

////////////////////////////////////////////////////////////////////////////////
/// Structures
////////////////////////////////////////////////////////////////////////////////
typedef struct FeedbackError_ {
  std::string axis;
  std::string motor;
  std::string encoder;
} FeedbackError;

typedef struct FeedbackError_ *FourierFeedbackErrorPtr;

typedef struct MotionControllerConfig_ {
  float pos_gain;
  float vel_gain;
  float vel_integrator_gain;
  float vel_limit;
  float vel_limit_tolerance;
} MotionControllerConfig;

typedef struct MotorConfig_ {
  int current_lim;
  int current_lim_margin;
  int inverter_temp_limit_lower;
  int inverter_temp_limit_upper;
  int requested_current_range;
  int current_control_bandwidth;
} MotorConfig;

typedef struct TrapTraj_ {
  int accel_limit;
  int decel_limit;
  int vel_limit;
} TrapTraj;

typedef struct NetworkSetting_ {
  bool dhcp_enable;
  std::string SSID;
  std::string password;
  std::string name;
  std::string staticIP;
  std::string gateway;
  std::string subnet;
  std::string dns_1;
  std::string dns_2;
} NetworkSetting;

typedef struct Feedback_ {
  float position;
  float velocity;
  float current;
  int error_code;
  float voltage;
  float motor_temp_m0;  // reserved
  float motor_temp_m1;
  float inverter_temp_m0;  // reserved
  float inverter_temp_m1;
  bool drive_status;
  std::string ip;
  std::string serial_number;
  std::string connect_mode;
  std::string model;
  std::string mac_address;
  std::string hw_version;
  std::string fw_version;

  MotionControllerConfig motion_ctrl_config;
  MotorConfig motor_config;
  TrapTraj trap_traj;

  NetworkSetting network_setting;
} Feedback;

typedef struct Feedback_ *FourierFeedbackPtr;

typedef struct PosPtInfo_ {
  float pos;
  float vel;
  float torque;
} PosPtInfo;

typedef struct Command_ {
  float float_fields_;
  bool bool_fields_;

  PosPtInfo pos_pt_info_;

  MotionControllerConfig *motion_ctrl_config;
  MotorConfig *motor_config;
  TrapTraj *trap_traj;

  NetworkSetting *network_setting;
  FourierCommandEnum commandEnum;
} Command;

typedef struct Command_ *FourierCommandPtr;

////////////////////////////////////////////////////////////////////////////////
/// Lookup API
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Create a Lookup instance.
 * Lookup created by this function must be released with 'fourierLookupRelease'
 * when no longer needed.
 *
 * Note that this call invokes a background thread to query the network for
 * modules at regular intervals.
 *
 * @param addr A string pointer to a broadcast address.
 *             eg: "192.168.100.255,192.168.101.255"
 *
 * @param addr_len the length of the buffer `addr`. This must be zero
 *                 if `addr` is null.
 */
FourierLookupPtr fourierLookupCreate(const char *addr, size_t addr_len);

/**
 * @brief Frees resources created by the lookup object.
 *
 * Lookup object should no longer be used after this function is called!
 * Note that background query thread is stopped by this function.
 *
 * @param lookup Object created by `fourierLookupCreate`.
 */
void fourierLookupRelease(FourierLookupPtr lookup);

/**
 * @brief Sets the broadcast address for the lookup.
 *
 * @param lookup Object created by `fourierLookupCreate`.
 * @param addr A string pointer to a broadcast address.
 *             eg: "192.168.100.255,192.168.101.255"
 */
void fourierLookupSetNetworks(FourierLookupPtr lookup, const char *addr);

std::string fourierLookupGetCtrlBoxIP(FourierLookupPtr lookup);

/**
 * @brief sets the lookup request rate [Hz]
 *
 * @param lookup Object created by `fourierLookupCreate`.
 * @param frequency The recommended range is 0 to 100
 *
 * @returns FourierStatusSuccess on success, FourierStatusInvalidArgument on
 *          negative or non-finite frequency
 */
FourierStatusCode fourierLookupSetLookupFrequencyHz(FourierLookupPtr lookup,
                                                    const float frequency);

/**
 * @brief gets the lookup request rate [Hz]
 */
const float fourierLookupGetLookupFrequencyHz(FourierLookupPtr lookup);

/**
 * @brief Write the found object to FourierLookupEntryListPtr and return it.
 *
 * @param lookup Object created by `fourierLookupCreate`.
 */
FourierLookupEntryListPtr fourierCreateLookupEntryList(FourierLookupPtr lookup);

/**
 * @brief Gets the number of entries in the lookup entry list.
 *
 * @param lookup_list Object created by `fourierCreateLookupEntryList`.
 */
size_t fourierLookupEntryListGetSize(FourierLookupEntryListPtr lookup_list);

/**
 * @brief Gets the name of the given entry in the lookup entry list. Must be a
 *        valid index.
 *
 * @param lookup_list Object created by `fourierCreateLookupEntryList`.
 * @param index The entry index that is being queried.
 * @param name An allocated buffer of length 'length'
 * @param length the length of the provided buffer. After calling this function,
 *               the value dereferenced will be updated with the length of the
 *               string plus the null character. This argument must not be NULL.
 *
 * @returns FourierStatusSuccess on success，FourierStatusFailure on failed.
 */
FourierStatusCode fourierLookupEntryListGetName(
    FourierLookupEntryListPtr lookup_list, size_t index, char *name,
    size_t *length);

/**
 * @brief Gets the family of the given entry in the lookup entry list. Must be a
 *        valid index.
 *
 * @param lookup_list Object created by `fourierCreateLookupEntryList`.
 * @param index The entry index that is being queried.
 * @param family An allocated buffer of length 'length'
 * @param length the length of the provided buffer. After calling this function,
 *               the value dereferenced will be updated with the length of the
 *               string plus the null character. This argument must not be NULL.
 *
 * @returns FourierStatusSuccess on success，FourierStatusFailure on failed.
 */
FourierStatusCode fourierLookupEntryListGetFamily(
    FourierLookupEntryListPtr lookup_list, size_t index, char *family,
    size_t *length);

/**
 * @brief Gets the serialNumber of the given entry in the lookup entry list.
 * Must be a valid index.
 *
 * @param lookup_list Object created by `fourierCreateLookupEntryList`.
 * @param index The entry index that is being queried.
 * @param serialNumber An allocated buffer of length 'length'
 * @param length the length of the provided buffer. After calling this function,
 *               the value dereferenced will be updated with the length of the
 *               string plus the null character. This argument must not be NULL.
 *
 * @returns FourierStatusSuccess on success，FourierStatusFailure on failed.
 */
FourierStatusCode fourierLookupEntryListGetSerialNumber(
    FourierLookupEntryListPtr lookup_list, size_t index, char *serialNumber,
    size_t *length);

/**
 * @brief Release resources for a given lookup entry list; list should not be
 * used after this call.
 * @param lookup_list Object created by `fourierCreateLookupEntryList`.
 */
void fourierLookupEntryListRelease(FourierLookupEntryListPtr lookup_list);

////////////////////////////////////////////////////////////////////////////////
/// Feedback API
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Creates a GroupFeedback for a group with the specified number of
 * modules.
 *
 * @param size The number of modules in the group.
 *
 * @returns A pointer to a new GroupFeedback object. This must be released
 * with \c fourierGroupFeedbackRelease(FourierGroupFeedbackPtr).
 */
FourierGroupFeedbackPtr fourierGroupFeedbackCreate(size_t size);

/**
 * @brief Return the number of modules in this group Feedback.
 *
 * @returns The number of module feedbacks in this group feedback.
 */
size_t fourierGroupFeedbackGetSize(FourierGroupFeedbackPtr feedback);

/**
 * @brief Get an individual feedback for a particular module at index
 * \c module_index.
 *
 * @param module_index The index to retrieve the module feedback; must be
 * from 0 to the size - 1, inclusive, or results in undefined behavior.
 *
 * @returns The feedback corresponding to the module at index \c module_index.
 */
FourierFeedbackPtr fourierGroupFeedbackGetModuleFeedback(
    FourierGroupFeedbackPtr feedback, size_t module_index);

/**
 * @brief Frees resources created by the GroupFeedback object.
 *
 * The GroupFeedbackPtr must not be used after this function is called.
 */
void fourierGroupFeedbackRelease(FourierGroupFeedbackPtr feedback);

////////////////////////////////////////////////////////////////////////////////
/// Group API
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Create a group with all modules known to the lookup with the given
 * family.
 *
 * Group contains all modules with the given family, regardless of name.
 *
 * @param lookup Object created by `fourierLookupCreate`.
 * @param family The given family of the modules, as viewable in the
 * FourierStudio Must be a null-terminated string, and must not be NULL.
 * @param timeout_ms Timeout in milliseconds.  A value of -1 blocks until
 * a module is found, and a value of 0 returns immediately if no module with
 * that address is currently known by the Lookup class.
 *
 * @returns NULL if matching group not found in allotted time; pointer to newly
 * allocated group object otherwise.
 */
FourierGroupPtr fourierGroupCreateFromFamily(FourierLookupPtr lookup,
                                             const char *family,
                                             int32_t timeout_ms);

/**
 * @brief Returns the number of modules in a group.
 *
 * @param group The group to send this command to.
 *
 * @returns the number of modules in the group.
 */
size_t fourierGroupGetSize(FourierGroupPtr group);

/**
 * @brief Sends a command to the given group without requesting an
 * acknowledgement.
 *
 * Appropriate for high-frequency applications.
 *
 * @param group The group to send this command to.
 * @param command The FourierGroupCommand object containing information to be
 * sent to the group.
 *
 * @returns FourierStatusSuccess if the command was successfully sent, otherwise
 * a failure code.
 */
FourierStatusCode fourierGroupSendCommand(FourierGroupPtr group,
                                          FourierGroupCommandPtr command);

/**
 * @brief Returns the current command lifetime, in milliseconds.
 *
 * @param group Which group is being queried.
 *
 * @returns The current command lifetime, in milliseconds. A value of '0'
 * indicates that commands remain active until the next command is received.
 */
FourierStatusCode fourierGroupSetCommandLifetime(FourierGroupPtr group,
                                                 int32_t lifetime_ms);

/**
 * @brief Sets the feedback request loop frequency (in Hz).
 *
 * // TODO:
 * The group is queried for feedback in a background thread at this frequency,
 * and any added callbacks are called from this background thread.
 *
 * @param group Which group this frequency set is for.
 * @param frequency The feedback request loop frequency (in Hz). A value of '0'
 * is the default, and disables the feedback request thread.
 *
 * @returns FourierStatusSuccess on success，FourierStatusFailure on failed.
 */
FourierStatusCode fourierGroupSetFeedbackFrequencyHz(FourierGroupPtr group,
                                                     const float frequency);

/**
 * @brief Returns the current feedback request loop frequency (in Hz).
 *
 * @param group Which group is being queried.
 *
 * @returns The current feedback request loop frequency (in Hz).
 */
float fourierGroupGetFeedbackFrequencyHz(FourierGroupPtr group);

/**
 * @brief Requests feedback from the group.
 *
 * Sends a background request to the modules in the group; if/when all modules
 * return feedback, any associated handler functions are called. This returned
 * feedback is also stored to be returned by the next call to
 * fourierGroupGetNextFeedback (any previously returned data is discarded).
 *
 * @param group The group to return feedback from.
 *
 * @returns FourierStatusSuccess on success，FourierStatusFailure on failed.
 */
FourierStatusCode fourierGroupSendFeedbackRequest(
    FourierGroupPtr group,
    FourierFeedbackCode feedbackCode = FourierFeedbackAll);

/**
 * @brief Returns the most recently stored feedback from a sent feedback
 * request, or returns the next one received (up to the requested timeout).
 *
 * Note that a feedback request can be sent either with the
 * fourierGroupSendFeedbackRequest function, or by setting a background feedback
 * frequency with fourierGroupSetFeedbackFrequencyHz.
 *
 * Warning: other data in the provided 'Feedback' object is erased!
 *
 * @param group The group to return feedback from.
 * @param feedback On success, the feedback read from the group are written
 * into this structure.
 * @param timeout_ms Indicates how many milliseconds to wait for feedback.
 * For typical networks, '15' ms is a value that can be reasonably expected to
 * allow for a round trip transmission after the last 'send feedback request'
 * call.
 *
 * @returns FourierStatusSuccess on success，FourierStatusFailure on failed.
 */
FourierStatusCode fourierGroupGetNextFeedback(FourierGroupPtr group,
                                              FourierGroupFeedbackPtr feedback,
                                              int32_t timeout_ms);

// TODO:
FourierStatusCode fourierGroupRegisterFeedbackHandler(
    FourierGroupPtr group, GroupFeedbackHandlerFunction handler,
    void *user_data);

// TODO:
void fourierGroupClearFeedbackHandlers(FourierGroupPtr group);

/**
 * @brief Release resources for a given group; group should not be used after
 * this call.
 *
 * @param group A valid FourierGroup object.
 */
void fourierGroupRelease(FourierGroupPtr group);

/**
 * @brief Creates a GroupCommand for a group with the specified number of
 * modules.
 *
 * @param size The number of modules in the group.
 *
 * @returns A pointer to a new GroupCommand object. This must be released
 * with \c fourierGroupCommandRelease(FourierGroupCommandPtr).
 */
FourierGroupCommandPtr fourierGroupCommandCreate(size_t size);

/**
 * @brief Return the number of modules in this group Command.
 *
 * @returns The number of module commands in this group command.
 */
size_t fourierGroupCommandGetSize(FourierGroupCommandPtr command);

/**
 * @brief Get an individual command for a particular module at index
 * \c module_index.
 *
 * @param module_index The index to retrieve the module command; must be
 * from 0 to the size - 1, inclusive, or results in undefined behavior.
 *
 * @returns The command corresponding to the module at index \c module_index.
 */
FourierCommandPtr fourierGroupCommandGetModuleCommand(
    FourierGroupCommandPtr command, size_t module_index);

/**
 * @brief Sets the type of a set of commands
 *
 * @param command_enum Any value in FourierCommandEnum
 *
 * @returns The command corresponding to the module at index \c module_index.
 */
bool fourierGroupCommandSetType(FourierGroupCommandPtr command,
                                FourierCommandEnum command_enum);
/**
 * @brief Clears all data in the GroupCommand object.
 */
void fourierGroupCommandClear(FourierGroupCommandPtr command);

/**
 * @brief Frees resources created by the GroupCommand object.
 *
 * The GroupCommandPtr must not be used after this function is called.
 */
void fourierGroupCommandRelease(FourierGroupCommandPtr command);

/**
 * @brief Gets the actuator error message
 */
FourierFeedbackErrorPtr fourierGroupFeedbackError(FourierGroupPtr group,
                                                  int idx);

/**
 * @brief Set the log print level.
 *
 * @param mode "DEBUG" "INFO" "WARN" "ERROR"
 * @return FourierStatusCode
 */
FourierStatusCode fourierSetLogLevel(const char *mode);

/**
 * \brief Get the version of the library
 *
 * All parameters must not be NULL.
 *
 * \return FourierStatusSuccess on success, otherwise
 * FourierStatusInvalidArgument if a parameter is NULL.
 */
FourierStatusCode fourierGetLibraryVersion(int32_t *major, int32_t *minor,
                                           int32_t *revision);

#ifdef __cplusplus
}  // extern "C"
#endif
