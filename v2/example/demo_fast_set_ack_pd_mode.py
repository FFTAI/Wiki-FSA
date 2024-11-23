from fi_fsa import fi_fsa_v2
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        tx_cnt = 0
        for i in range(len(server_ip_list)):
            tx_cnt = tx_cnt + 1
            rx_cnt, res = fi_fsa_v2.fast_set_ack_mode_of_operation(
                server_ip_list[i], tx_cnt, fi_fsa_v2.FSAModeOfOperation.POSITION_CONTROL_PD
            )
            print("cnt = %u, res = %d" % (rx_cnt, res))

        time.sleep(1)  # sleep 1 second


if __name__ == "__main__":
    main()
