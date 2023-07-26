import fi_fsa
import time
import math

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        fsa_state = True
        for i in range(len(server_ip_list)):
            fsa_state = fsa.get_state(server_ip_list[i])
            print("State = %d" % fsa_state)

        print('\n')
        time.sleep(1)

        for i in range(len(server_ip_list)):
            fsa.get_config(server_ip_list[i])

        print('\n')
        time.sleep(1)

        for i in range(len(server_ip_list)):
            pvc = fsa.get_pvc(server_ip_list[i])
            print("Position = %.2f, Velocity = %.0f, Current = %.4f" % (pvc[0], pvc[1], pvc[2]))
            time.sleep(0.01)

        print('\n')
        time.sleep(1)

        # set current control current to 0.0
        for i in range(len(server_ip_list)):
            fsa.set_position_control(server_ip_list[i], 0.0)

        print('\n')
        time.sleep(1)

        # enable all the motors
        for i in range(len(server_ip_list)):
            fsa.set_enable(server_ip_list[i])

        print('\n')
        time.sleep(1)

        # set work at current control mode
        for i in range(len(server_ip_list)):
            fsa.set_mode_of_operation(server_ip_list[i], fsa.FSAModeOfOperation.POSITION_CONTROL)

        print('\n')
        time.sleep(1)

        # ------------------------------------------------------

        for i in range(len(server_ip_list)):
            fsa.set_position_control(server_ip_list[i], 0.0)

        time.sleep(1)

        # move a sin wave
        count_max = round(1000 * 2 * math.pi)
        reduction_ratio = 1.0
        for t in range(0, count_max):
            for i in range(len(server_ip_list)):
                set_position = 360.0 * 3.0 * math.sin(t / 1000.0)  # [deg]
                fsa.set_position_control(server_ip_list[i], set_position)
            time.sleep(0.01)

        time.sleep(1)

        # ------------------------------------------------------

        # for i in range(len(server_ip_list)):
        #     fsa.set_position_control(server_ip_list[i], 0.0)
        #
        # time.sleep(1)

        for i in range(len(server_ip_list)):
            fsa.set_disable(server_ip_list[i])

        time.sleep(1)

        # set work at none control mode
        for i in range(len(server_ip_list)):
            fsa.set_mode_of_operation(server_ip_list[i], fsa.FSAModeOfOperation.POSITION_CONTROL)


if __name__ == '__main__':
    main()
