from fi_fsa import fi_fsa_v2
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # start_time = time.time()
        for i in range(len(server_ip_list)):
            fi_fsa_v2.init(server_ip_list[i])
            fi_fsa_v2.comm(server_ip_list[i])

        pvc = fi_fsa_v2.get_pvc_group(server_ip_list)


        for i in range(len(server_ip_list)):
            print(
                f"Ip = {server_ip_list[i]} Position = {pvc[0][i]}, Velocity = {pvc[1][i]}, Current = {pvc[2][i]}"
            )
        time.sleep(1)  # sleep 1 second


if __name__ == "__main__":
    main()
