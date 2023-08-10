import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # auto run one turn to set the encoder of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.clear_fault(server_ip_list[i])

        print('\n')
        time.sleep(1)


if __name__ == '__main__':
    main()
