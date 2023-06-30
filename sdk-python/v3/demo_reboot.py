import fsa
import time

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func()

    if server_ip_list:

        for i in range(len(server_ip_list)):
            fsa.reboot(server_ip_list[i])

        print('\n')
        time.sleep(1)


if __name__ == '__main__':
    main()
