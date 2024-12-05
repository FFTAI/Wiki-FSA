from fi_fsa import fi_fsa_v2
import time

# server_ip_list = ["192.168.137.101"]


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        match_version = "0.3.12.1"
        for i in range(len(server_ip_list)):
            firmware_version = fi_fsa_v2.get_comm_firmware_version(server_ip_list[i])
            if fi_fsa_v2.version_compare(firmware_version, match_version) < 0:
                print("the firmware version of %s is %s, less than the match version %s" % (
                    server_ip_list[i], firmware_version, match_version))
                return

        # 0: disable, 1: enable
        protect_enable = 1
        # timeout interval(ms)
        timeout_interval = 1000  # ms
        for i in range(len(server_ip_list)):

            ret = fi_fsa_v2.set_eth_timeout_protect(server_ip_list[i], protect_enable, timeout_interval)
            print(ret)

        time.sleep(1)


if __name__ == "__main__":
    main()
