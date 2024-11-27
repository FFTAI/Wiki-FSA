from fi_fsa import fi_fsa_v2
import time

# server_ip_list = ["192.168.137.11"]


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for j in range(10000000):
            # start_time = time.time()

            for i in range(len(server_ip_list)):
                position, velocity, current, torque, error = fi_fsa_v2.fast_get_pvct_error(server_ip_list[i])
                print(
                    "Position = %f, Velocity = %f, Current = %f, Torque = %f"
                    % (position, velocity, current, torque)
                )
                print("Error =" + str(error))

            # end_time = time.time()
            #
            # # print the latency of the pvc function
            # latency = end_time - start_time
            # print(latency * 1000, "ms")

            time.sleep(0.2)  # sleep 1 second


if __name__ == "__main__":
    main()
