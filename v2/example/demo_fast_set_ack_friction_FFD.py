from fi_fsa import fi_fsa_v2
import time


# server_ip_list = ["192.168.137.101"]

def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        match_version = "0.3.12.7"
        for i in range(len(server_ip_list)):
            firmware_version = fi_fsa_v2.get_comm_firmware_version(server_ip_list[i])
            if fi_fsa_v2.version_compare(firmware_version, match_version) < 0:
                print("the firmware version of %s is %s, less than the match version %s" % (
                    server_ip_list[i], firmware_version, match_version))
                return

        friction_FFD_SET = 0
        friction_FFD_DeadSpeedK = 0
        friction_FFD_DeadSpeed = 0
        friction_FFD_Fs = 0
        friction_FFD_Fc = 0
        friction_FFD_B = 0
        friction_FFD_percent = 1
        tx_cnt = 0
        for i in range(len(server_ip_list)):
            tx_cnt = tx_cnt + 1
            rx_cnt, res = fi_fsa_v2.fast_set_ack_friction_FFD(
                server_ip_list[i],
                tx_cnt,
                friction_FFD_SET,
                friction_FFD_DeadSpeedK,
                friction_FFD_DeadSpeed,
                friction_FFD_Fs,
                friction_FFD_Fc,
                friction_FFD_B,
                friction_FFD_percent
            )
            print("cnt = %u, res = %d" % (rx_cnt, res))

        time.sleep(1)


if __name__ == "__main__":
    main()
