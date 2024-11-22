from fi_fsa import fi_fsa_v2
import time


# server_ip_list = ["192.168.137.101"]

def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:
        friction_FFD_SET = 0
        friction_FFD_DeadSpeedK = 0
        friction_FFD_DeadSpeed = 0
        friction_FFD_Fs = 0
        friction_FFD_Fc = 0
        friction_FFD_B = 0
        friction_FFD_percent = 1
        for i in range(len(server_ip_list)):
            ret = fi_fsa_v2.fast_set_friction_FFD(
                server_ip_list[i], 
                friction_FFD_SET,
                friction_FFD_DeadSpeedK,
                friction_FFD_DeadSpeed,
                friction_FFD_Fs,
                friction_FFD_Fc,
                friction_FFD_B,
                friction_FFD_percent
            )
            print(ret)

        time.sleep(1)


if __name__ == "__main__":
    main()
