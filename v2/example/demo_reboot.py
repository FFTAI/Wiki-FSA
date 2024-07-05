from fi_fsa import fi_fsa_v2
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v2.broadcast_func()

    if server_ip_list:

        for i in range(len(server_ip_list)):
            fi_fsa_v2.reboot(server_ip_list[i])

        print("\n")
        time.sleep(1)


if __name__ == "__main__":
    main()
