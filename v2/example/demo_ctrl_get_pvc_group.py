from fi_fsa import fi_fsa_v2
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        match_version = "0.3.9.9"
        for i in range(len(server_ip_list)):
            firmware_version = fi_fsa_v2.get_comm_firmware_version(server_ip_list[i])
            if fi_fsa_v2.version_compare(firmware_version, match_version) < 0:
                print("the firmware version of %s is %s, less than the match version %s" % (
                    server_ip_list[i], firmware_version, match_version))
                return

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
