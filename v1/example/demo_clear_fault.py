from fi_fsa import fi_fsa_v1
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v1.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        match_version = "2.14.2"
        for i in range(len(server_ip_list)):
            firmware_version = fi_fsa_v1.get_comm_firmware_version(server_ip_list[i])
            if fi_fsa_v1.version_compare(firmware_version, match_version) < 0:
                print("the firmware version of %s is %s, less than the match version %s" % (
                    server_ip_list[i], firmware_version, match_version))
                return

        for i in range(len(server_ip_list)):
            error_code = fi_fsa_v1.get_error_code(server_ip_list[i])
            print("Error code = %d" % error_code)

        # auto run one turn to set the encoder of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa_v1.clear_fault(server_ip_list[i])

        print("\n")
        time.sleep(1)


if __name__ == "__main__":
    main()
