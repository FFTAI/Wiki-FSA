import fi_fsa_v2 as fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for i in range(len(server_ip_list)):
            is_enable = 1
            ret = fi_fsa.set_double_encoder_enable(server_ip_list[i],is_enable)
            print(ret)

        # end_time = time.time()
        #
        # # print the latency of the pvc function
        # latency = end_time - start_time
        # print(latency * 1000, "ms")

        time.sleep(0.2)  # sleep 1 second


if __name__ == '__main__':
    main()
