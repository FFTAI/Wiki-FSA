import fi_fsa_v1 as fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for j in range(10000):
            # start_time = time.time()

            for i in range(len(server_ip_list)):
                _, error = fi_fsa.fast_get_error(server_ip_list[i])
                print("Error = %d" % error)

            # end_time = time.time()
            #
            # # print the latency of the pvc function
            # latency = end_time - start_time
            # print(latency * 1000, "ms")

            time.sleep(0.2)  # sleep 1 second


if __name__ == '__main__':
    main()