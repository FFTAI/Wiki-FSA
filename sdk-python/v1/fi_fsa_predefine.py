from enum import Enum


class FSAFunctionResult:
    SUCCESS = 0
    FAIL = -1
    RUNNING = 1
    PREPARE = 2
    EXECUTE = 3
    NOT_EXECUTE = 4
    TIMEOUT = 5


class FSAFlagState:
    CLEAR = 0
    SET = 1


class FSAActuatorType:
    TYPE_DEFAULT = 0x00000001


class FSAActuatorDirection:
    DIRECTION_NORMAL = 1
    DIRECTION_REVERSE = -1


class FSAActuatorReductionRatio:
    REDUCTION_RATIO_DEFAULT = 1
    REDUCTION_RATIO_1 = 1
    REDUCTION_RATIO_7 = 7
    REDUCTION_RATIO_9_75 = 9.75
    REDUCTION_RATIO_13_7 = 13.7
    REDUCTION_RATIO_28_8 = 28.8
    REDUCTION_RATIO_30 = 30
    REDUCTION_RATIO_31 = 31
    REDUCTION_RATIO_36 = 36
    REDUCTION_RATIO_50 = 50
    REDUCTION_RATIO_51 = 51
    REDUCTION_RATIO_80 = 80
    REDUCTION_RATIO_81 = 81
    REDUCTION_RATIO_100 = 100
    REDUCTION_RATIO_101 = 101
    REDUCTION_RATIO_120 = 120
    REDUCTION_RATIO_121 = 121


class FSAActuatorCommHardwareType:
    TYPE_NULL = 0
    TYPE_V1 = 1
    TYPE_V2 = 2


class FSAControlWord:
    NONE = 0
    CALIBRATE_PREPARE = 0xA0
    CALIBRATE_ADC = 0xA1
    CALIBRATE_MOTOR = 0xA2
    CALIBRATE_ENCODER = 0xA3
    CALIBRATE_DIRECTION = 0xA4
    CALIBRATE_OFFSET = 0xA5
    OPEN_LOOP_CONTORL = 0x04
    SERVO_OFF = 0x06
    SERVO_ON = 0x0F
    MOTION_ABS = 0x103F
    CLEAR_FAULT = 0x86


class FSAModeOfOperation:
    NONE = 0
    TORQUE_CONTROL = 6
    CURRENT_CONTROL = 4
    VELOCITY_CONTROL = 3
    POSITION_CONTROL = 1
    CURRENT_CLOSE_LOOP_CONTROL = 4
    POSITION_CONTROL_PD = 5


class FSAState:
    IDLE = 0
    INIT = 1
    NORMAL = 2
    FAULT = 3
    ENCODER_CAIL = 4


class FSAErrorCode(Enum):
    ERROR_NONE = 0x0
    ERROR_ADC_CAL_FAULT = 0x01
    ERROR_CAN_COM_TIMEOUT = 0x02
    ERROR_OVER_CURRENT = 0x04
    ERROR_OVER_VBUS = 0x08

    ERROR_UNDER_VBUS = 0x10
    ERROR_OVER_TEMP_TRIP = 0x20
    ERROR_OVER_TEMP_MOSFET = 0x40
    ERROR_OVER_PHASE_A_CURRENT = 0x80

    ERROR_OVER_PHASE_B_CURRENT = 0x100
    ERROR_OVER_PHASE_C_CURRENT = 0x200
    ERROR_OVER_HARD_PHASE_CURRENT = 0x400
    ERROR_OPD_FAULT = 0x800

    ERROR_ENCODER_NOT_CALIBRATED = 0x1000
    ERROR_ENCODER_LOSS = 0x2000
    ERROR_FLASH_ERROR = 0x4000
    ERROR_MOTOR_STALL = 0x8000

    ERROR_POSITION_LIMIT_ERROR = 0x10000
    ERROR_ENCODER_REVERSAL = 0x20000
    ERROR_MOTOR_TYPE_NULL = 0x40000
    ERROR_HARDWARE_TYPE_NULL = 0x80000


class FSAInputMode:
    INPUT_MODE_INACTIVE = 0
    INPUT_MODE_PASSTHROUGH = 1
    INPUT_MODE_VEL_RAMP = 2
    INPUT_MODE_POS_FILTER = 3
    INPUT_MODE_TRAP_TRAJ = 5
    INPUT_MODE_TORQUE_RAMP = 6


class FSAMotorType:
    TYPE_MOTOR_NULL = 0
    TYPE_JIOALONG = 1
    FSA130_20V0 = 2
    FSA100_15V0 = 3
    FSA80_10V0 = 4
    FSA60_08V0 = 5
    FSA36_08V0 = 6
    FSA25_08V0 = 7
    FSA36_10V0 = 8
    FSA130_20V1 = 9
    FSA100_15V1 = 10
    FSA80_10V1 = 11
    FSA60_08V1 = 12
    FSA45_15V1 = 13


class FSAHardwareType:
    TYPE_HARDWARE_NULL = 0
    TYPE_ODrive_V36 = 1
    TYPE_H95V104 = 2
    TYPE_H66V104 = 3
    TYPE_H46V104 = 4
    TYPE_H30V303 = 5


class FSAMotorDirection:
    ABC = 1
    ACB = -1


class FSAMotorVBUS:
    VBUS_DEFAULT = 48
    VBUS_36V = 36
    VBUS_48V = 48


class FSAMotorPolePairs:
    POLE_PAIRS_7 = 7
    POLE_PAIRS_10 = 10
    POLE_PAIRS_21 = 21


class FSAMotorMaxSpeed:
    MAX_SPEED_1000 = 1000  # rpm
    MAX_SPEED_1600 = 1600  # rpm
    MAX_SPEED_2000 = 2000  # rpm
    MAX_SPEED_3000 = 3000  # rpm
    MAX_SPEED_3400 = 3400  # rpm
    MAX_SPEED_4800 = 4800  # rpm
    MAX_SPEED_7000 = 7000  # rpm


class FSAMotorMaxAcceleration:
    MAX_ACCELERATION_6000 = 6000  # rpm/s
    MAX_ACCELERATION_10000 = 10000  # rpm/s
    MAX_ACCELERATION_30000 = 30000  # rpm/s
    MAX_ACCELERATION_60000 = 60000  # rpm/s


class FSAMotorVIBCADCRatio:
    _ADC_PRECISION = 4096.0
    _ADC_SHUNT_RESISTANCE_2mR = 0.002
    _ADC_SHUNT_RESISTANCE_05mR = 0.0005
    _ADC_REF_V = 3.3
    _ADC_AMPLIFIER = 20
    VIBC_ADC_RATIO_2mR = (_ADC_REF_V / _ADC_PRECISION / _ADC_SHUNT_RESISTANCE_2mR / _ADC_AMPLIFIER)
    VIBC_ADC_RATIO_05mR = (_ADC_REF_V / _ADC_PRECISION / _ADC_SHUNT_RESISTANCE_05mR / _ADC_AMPLIFIER)


class FSAMotorVBUSADCRatio:
    _ADC_PRECISION = 4096.0
    _UP_SHUNT_RESISTANCE = 18000
    _DOWN_SHUNT_RESISTANCE = 1000
    _ADC_REF_V = 3.3
    VBUS_ADC_RATIO = (
            _ADC_REF_V / _ADC_PRECISION * (_UP_SHUNT_RESISTANCE + _DOWN_SHUNT_RESISTANCE) / _DOWN_SHUNT_RESISTANCE)


class FSAEncoderDirection:
    DIRECTION_CW = -1
    DIRECTION_CCW = 1


class FSAEncoderResolution:
    RESOLUTION_4000 = 4000  # 4000 pulse/rev
