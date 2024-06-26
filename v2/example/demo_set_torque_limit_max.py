import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for i in range(len(server_ip_list)):
            torque_limit_max = 30
            ret = fi_fsa.set_torque_limit_max(server_ip_list[i], torque_limit_max)
            print(ret)

        time.sleep(1)  # sleep 1 second


if __name__ == "__main__":
    main()
