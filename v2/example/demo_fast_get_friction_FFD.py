from fi_fsa import fi_fsa_v2
import time


# server_ip_list = ["192.168.137.101"]

def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        match_version = "0.3.12.5"
        for i in range(len(server_ip_list)):
            firmware_version = fi_fsa_v2.get_comm_firmware_version(server_ip_list[i])
            if fi_fsa_v2.version_compare(firmware_version, match_version) < 0:
                print("the firmware version of %s is %s, less than the match version %s" % (
                    server_ip_list[i], firmware_version, match_version))
                return

        for i in range(len(server_ip_list)):
            SET, DeadSpeedK, DeadSpeed, Fs, Fc, B, percent = fi_fsa_v2.fast_get_friction_FFD(server_ip_list[i])
            print(
                "friction_FFD_SET=%d, friction_FFD_DeadSpeedK=%f, friction_FFD_DeadSpeed=%f, friction_FFD_Fs=%f,"
                "friction_FFD_Fc=%f, friction_FFD_B=%f, friction_FFD_percent=%f"
                % (SET, DeadSpeedK, DeadSpeed, Fs, Fc, B, percent)
            )

        time.sleep(1)


if __name__ == "__main__":
    main()
