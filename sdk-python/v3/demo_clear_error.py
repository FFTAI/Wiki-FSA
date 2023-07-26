import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for i in range(len(server_ip_list)):
            error_code = fsa.get_error_code(server_ip_list[i])
            print("Error code = %d" % error_code)

            fsa.clear_error_code(server_ip_list[i])

        print('\n')
        time.sleep(1)


if __name__ == '__main__':
    main()
