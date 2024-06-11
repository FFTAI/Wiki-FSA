import fi_fsa_v2 as fi_fsa
import time
import fi_fsa_predefine

server_ip_list = ['192.168.137.51']

FSA_cfg = {'FSA130-9E': {'actuator_type': fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
                         'actuator_direction': fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
                         'actuator_reduction_ratio': fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_9_75,
                         'motor_type': fi_fsa_predefine.FSAMotorType.FSA130_20V1,
                         'motor_hardware_type': fi_fsa_predefine.FSAHardwareType.TYPE_H142V100,
                         "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
                         'motor_direction': -1,
                         "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_1600,
                         "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                         'motor_max_current': 100,
                         "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_V2},
           'FSA130-14E': {'actuator_type': fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
                          'actuator_direction': fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
                          'actuator_reduction_ratio': fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_13_7,
                          'motor_type': fi_fsa_predefine.FSAMotorType.FSA130_20V1,
                          'motor_hardware_type': fi_fsa_predefine.FSAHardwareType.TYPE_H142V102,
                          "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
                          'motor_direction': -1,
                          "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_1600,
                          "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                          'motor_max_current': 100,
                          "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_V2},
           'FSA80-29E': {'actuator_type': fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
                         'actuator_direction': fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
                         'actuator_reduction_ratio': fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_28_8,
                         'motor_type': fi_fsa_predefine.FSAMotorType.FSA80_10V1,
                         'motor_hardware_type': fi_fsa_predefine.FSAHardwareType.TYPE_H88V100,
                         "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
                         'motor_direction': 1,
                         "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_2000,
                         "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                         'motor_max_current': 100,
                         "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_V2},
           'FSA60-17-50Z V2.0.0': {'actuator_type': fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
                                   'actuator_direction': fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
                                   'actuator_reduction_ratio': fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_51,
                                   'motor_type': fi_fsa_predefine.FSAMotorType.FSA60_08V1,
                                   'motor_hardware_type': fi_fsa_predefine.FSAHardwareType.TYPE_H66V106,
                                   "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
                                   'motor_direction': -1,
                                   "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_3400,
                                   "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                                   'motor_max_current': 100,
                                   "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_V2},
           'FSA60-17-50Z V2.1.0': {'actuator_type': fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
                                   'actuator_direction': fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
                                   'actuator_reduction_ratio': fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_51,
                                   'motor_type': fi_fsa_predefine.FSAMotorType.FSA60_08V1,
                                   'motor_hardware_type': fi_fsa_predefine.FSAHardwareType.TYPE_H66V202,
                                   "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
                                   'motor_direction': -1,
                                   "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_3400,
                                   "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                                   'motor_max_current': 100,
                                   "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_V2},
           'FSA60-17-80Z V2.1.0': {'actuator_type': fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
                                   'actuator_direction': fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
                                   'actuator_reduction_ratio': fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_81,
                                   'motor_type': fi_fsa_predefine.FSAMotorType.FSA60_08V1,
                                   'motor_hardware_type': fi_fsa_predefine.FSAHardwareType.TYPE_H66V202,
                                   "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
                                   'motor_direction': -1,
                                   "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_3400,
                                   "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                                   'motor_max_current': 100,
                                   "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_V2},
           'FSA36B-14-80': {'actuator_type': fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
                            'actuator_direction': fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
                            'actuator_reduction_ratio': fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_81,
                            'motor_type': fi_fsa_predefine.FSAMotorType.FSA36_08V0,
                            'motor_hardware_type': fi_fsa_predefine.FSAHardwareType.TYPE_H46V104,
                            "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
                            'motor_direction': 1,
                            "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_3000,
                            "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                            'motor_max_current': 3.3,
                            "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_V2}
           }


def main():
    # server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            cfg_dict = FSA_cfg['FSA60-17-50Z V2.1.0']

            # cfg_dict = {
            #     'actuator_type': fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
            #     'actuator_direction': fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
            #     'actuator_reduction_ratio': fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_7,
            #     'motor_type': fi_fsa_predefine.FSAMotorType.FSA130_20V0,
            #     'motor_hardware_type': fi_fsa_predefine.FSAHardwareType.TYPE_H95V104,
            #     "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
            #     'motor_direction': fi_fsa_predefine.FSAMotorDirection.ABC,
            #     "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_3000,
            #     "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
            #     'motor_max_current': 60,
            #     "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_NULL,
            #     "actuator_double_encoder_enable": 0
            # }
            fi_fsa.set_config(server_ip_list[i], cfg_dict)

        print('\n')
        time.sleep(1)

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.get_config(server_ip_list[i])

        print('\n')

        # reboot all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.reboot(server_ip_list[i])


if __name__ == '__main__':
    main()
