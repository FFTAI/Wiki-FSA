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

        for i in range(10000):
            start_time = time.time()

            for i in range(len(server_ip_list)):
                state = fi_fsa_v1.get_state(server_ip_list[i])
                print("State = %d" % (state))

            end_time = time.time()

            # print the latency of the pvc function
            latency = end_time - start_time
            print(latency * 1000, "ms")

            time.sleep(1)  # sleep 1 second


if __name__ == "__main__":
    main()
