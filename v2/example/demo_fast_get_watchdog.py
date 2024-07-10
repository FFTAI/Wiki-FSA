from fi_fsa import fi_fsa_v2
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for j in range(10000):
            # start_time = time.time()

            for i in range(len(server_ip_list)):
                data = fi_fsa_v2.fast_get_watchdog(server_ip_list[i])
                print("received data : {}".format(data))
            time.sleep(0.2)  # sleep 1 second


if __name__ == "__main__":
    main()
