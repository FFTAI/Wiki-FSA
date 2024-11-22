from fi_fsa import fi_fsa_v2
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:
        position_kp = 0
        velocity_kp = 0
        velocity_ki = 0
        for i in range(len(server_ip_list)):
            _ = fi_fsa_v2.fast_set_pid_imm(server_ip_list[i], position_kp, velocity_kp, velocity_ki)

        time.sleep(0.2)  # sleep 1 second


if __name__ == "__main__":
    main()
