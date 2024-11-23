from fi_fsa import fi_fsa_v2
import time


# server_ip_list = ["192.168.137.101"]


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:
        position_loop_freq_div = 1
        velocity_loop_freq_div = 1
        PD_loop_freq_div = 1
        tx_cnt = 0
        for i in range(len(server_ip_list)):
            tx_cnt = tx_cnt + 1
            rx_cnt, res = fi_fsa_v2.fast_set_ack_control_loop_freq_div(
                server_ip_list[i], tx_cnt, position_loop_freq_div, velocity_loop_freq_div, PD_loop_freq_div)
            print("cnt = %u, res = %d" % (rx_cnt, res))

        time.sleep(1)


if __name__ == "__main__":
    main()
