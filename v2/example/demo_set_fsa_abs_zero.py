import fi_fsa
import time

# server_ip_list = ["192.168.137.101"]


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for i in range(len(server_ip_list)):
            print("now is setting: " + server_ip_list[i])
            position = fi_fsa.get_fsa_abs_position(server_ip_list[i])
            print(position)
            ret = fi_fsa.set_fsa_abs_offset(server_ip_list[i], position)
            print(ret)
            time.sleep(1)

        time.sleep(3)

        for i in range(len(server_ip_list)):
            fi_fsa.reboot(server_ip_list[i])


if __name__ == "__main__":
    main()
