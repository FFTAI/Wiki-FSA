from fi_fsa import fi_fsa_v2
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for j in range(10000):
            # start_time = time.time()

            for i in range(len(server_ip_list)):
                rx_cnt, res = fi_fsa_v2.fast_set_ack_disable(server_ip_list[i], j)
                print("cnt = %u, res = %d" % (rx_cnt, res))

            # end_time = time.time()
            #
            # # print the latency of the pvc function
            # latency = end_time - start_time
            # print(latency * 1000, "ms")

            time.sleep(0.2)  # sleep 1 second


if __name__ == "__main__":
    main()