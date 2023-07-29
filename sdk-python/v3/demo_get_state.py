import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for i in range(10000):
            start_time = time.time()

            for i in range(len(server_ip_list)):
                state = fi_fsa.get_state(server_ip_list[i])
                print("State = %d" % (state))

            end_time = time.time()

            # print the latency of the pvc function
            latency = end_time - start_time
            print(latency * 1000, "ms")

            time.sleep(1)  # sleep 1 second


if __name__ == '__main__':
    main()
