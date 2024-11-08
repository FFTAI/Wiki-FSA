from fi_fsa import fi_fsa_v2
import time

# server_ip_list = ["192.168.137.10"]


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for j in range(10000):
            # start_time = time.time()

            for i in range(len(server_ip_list)):
                position, velocity, current = fi_fsa_v2.fast_get_foc_target_pvc(server_ip_list[i])
                print(
                    "Position = %f, Velocity = %f, Current = %.4f"
                    % (position, velocity, current)
                )

            time.sleep(0.2)


if __name__ == "__main__":
    main()
