import fsa
import time

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        fsa_state = True
        for i in range(len(server_ip_list)):
            fsa_state = fsa.get_state(server_ip_list[i])
            print("State = %d" % fsa_state)

        time.sleep(1)

        for i in range(len(server_ip_list)):
            fsa.get_config(server_ip_list[i])

        time.sleep(1)

        for i in range(len(server_ip_list)):
            pvc = fsa.get_pvc(server_ip_list[i])
            print("Position = %.2f, Velocity = %.0f, Current = %.4f" % (pvc[0], pvc[1], pvc[2]))
            time.sleep(0.01)

        time.sleep(1)

        # set current control current to 0.0
        for i in range(len(server_ip_list)):
            fsa.set_current_control(server_ip_list[i], 0.0)

        time.sleep(1)

        # set work at current control mode
        for i in range(len(server_ip_list)):
            fsa.set_mode_of_operation(server_ip_list[i], fsa.FSAModeOfOperation.CURRENT_CLOSE_LOOP_CONTROL.value[0])

        time.sleep(1)

        flag_enabled = False

        # enable all the motors
        for i in range(len(server_ip_list)):
            flag_enabled = fsa.set_enable(server_ip_list[i])

        time.sleep(1)

        if flag_enabled:

            for i in range(len(server_ip_list)):
                fsa.set_current_control(server_ip_list[i], 0.0)

            time.sleep(1)

            for i in range(1000):  # last for 10 seconds
                for j in range(len(server_ip_list)):
                    # cmd_current for actuator 36 with 30 reduction ratio should larger than 0.30
                    # cmd_current for actuator 60 should larger than 0.30
                    fsa.set_current_control(server_ip_list[j], 1.0)
                time.sleep(0.01)

            for i in range(len(server_ip_list)):
                fsa.set_current_control(server_ip_list[i], 0.0)

            time.sleep(1)

            for i in range(len(server_ip_list)):
                fsa.set_disable(server_ip_list[i])

            time.sleep(1)

            # set work at none control mode
            for i in range(len(server_ip_list)):
                fsa.set_mode_of_operation(server_ip_list[i], fsa.FSAModeOfOperation.CURRENT_CLOSE_LOOP_CONTROL.value[0])


if __name__ == '__main__':
    main()