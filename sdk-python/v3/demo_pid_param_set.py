import fi_fsa
import time

server_ip_list = []
server_ip_list = ["192.168.137.164"]
class SpeedParamList :
    #         相对电流环分频系数| 前馈增益 |   KP   |   KI       |  Wc   |   Wo   |   B0   |0PI/1LADRC |SPD_FC|带宽  #
    MOTOR_NULL = [     2,          0.0,    0.1   ,  0.01      ,   0   ,   0    ,    0    ,   0      , 5000 , 600  ]
    JioaLong   = [     2,          0.0,    0.005 ,  0.0001    ,   0   ,   0    ,    0    ,	  0     , 5000 , 600  ]
    FSA13020V0 = [     2,          0.0,    0.055 ,  0.00005   ,  100  ,  200   ,   10000 ,   0      , 500  , 400  ]
    FSA10015V0 = [     2,          0.0,    0.05  ,  0.001     ,  100  ,  200   ,   10000 ,   0      , 1000 , 600  ]
    FSA8010V0  = [     2,          0.0,    0.005 ,  0.00001   ,  100  ,  200   ,   10000 ,   0      , 500  , 400  ]
    FSA6008V0  = [     2,          0.0,    0.04  ,  0.0001    ,   20  ,  100   ,   50000 ,   1      , 5000 , 600  ]
    FSA3608V0  = [     2,          0.0,    0.0025,  0.00001   ,  100  ,  200   ,   10000 ,   0      , 5000 , 600  ]
    FSA3610V0  = [     2,          0.0,    0.007 ,  0.00001   ,  100  ,  200   ,   10000 ,   0      , 5000 , 600  ]
    FSA2508V0  = [     2,          0.0,    0.05  ,  0.0001    ,  200  ,  800   ,    5000 ,   0      , 5000 , 600  ]


def main():
    # server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.get_pid_param(server_ip_list[i])

        print('\n')
        time.sleep(1)
        FSA_Type = SpeedParamList.FSA3608V0
        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            dict = { #36
                'control_position_kp': 0.5,
                'control_velocity_kp': FSA_Type[2],
                'control_velocity_ki': FSA_Type[3],
                'control_current_kp': 0.0,  # not work for now
                'control_current_ki': 0.0,  # not work for now
            }
            # dict = { #36
            #     'control_position_kp': 0.5,
            #     'control_velocity_kp': 0.0025,
            #     'control_velocity_ki': 0.00001,
            #     'control_current_kp': 0.0,  # not work for now
            #     'control_current_ki': 0.0,  # not work for now
            # }
            # dict = { #60
            #     'control_position_kp': 0.08,
            #     'control_velocity_kp': 0.04,
            #     'control_velocity_ki': 0.0001,
            #     'control_current_kp': 0.0,  # not work for now
            #     'control_current_ki': 0.0,  # not work for now
            # }
            print(dict)
            fi_fsa.set_pid_param(server_ip_list[i], dict)

        # print('\n')
        # time.sleep(1)

        # # get the communication configuration of all FAS
        # for i in range(len(server_ip_list)):
        #     fi_fsa.get_config(server_ip_list[i])

        # print('\n')

        # reboot all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.reboot(server_ip_list[i])


if __name__ == '__main__':
    main()
