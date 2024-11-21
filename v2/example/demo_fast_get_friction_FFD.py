from fi_fsa import fi_fsa_v2
import time


# server_ip_list = ["192.168.137.101"]

def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:
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
