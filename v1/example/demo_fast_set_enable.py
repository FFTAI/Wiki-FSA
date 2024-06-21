import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for i in range(len(server_ip_list)):
            (_,) = fi_fsa.fast_set_enable(server_ip_list[i])

        time.sleep(0.2)  # sleep 1 second


if __name__ == "__main__":
    main()
