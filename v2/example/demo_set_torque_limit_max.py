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

        for i in range(len(server_ip_list)):
            torque_limit_max = 30
            ret = fi_fsa_v2.set_torque_limit_max(server_ip_list[i], torque_limit_max)
            print(ret)

        time.sleep(1)  # sleep 1 second


if __name__ == "__main__":
    main()
