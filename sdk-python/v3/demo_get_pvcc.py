import fsa
import time

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func()

    if server_ip_list:

        for i in range(10000):
            # start_time = time.time()

            for i in range(len(server_ip_list)):
                pvcc = fsa.get_pvcc(server_ip_list[i])
                print("Position = %f, Velocity = %f, Current_iq = %.4f, Current_id = %.4f" % (pvcc[0], pvcc[1], pvcc[2], pvcc[3]))

            # end_time = time.time()
            #
            # # print the latency of the pvc function
            # latency = end_time - start_time
            # print(latency * 1000, "ms")

            time.sleep(0.2)  # sleep 1 second


if __name__ == '__main__':
    main()
