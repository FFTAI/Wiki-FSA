import random

from fi_fsa import fi_fsa_v2
import time


# server_ip_list = ["192.168.137.101"]


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:
        # 0: disable, 1: enable
        protect_enable = 1

        # timeout interval(ms)
        timeout_ms = 100  # ms

        # 设置进入保护状态时使用三环位置控制还是PD控制
        protect_mode_of_operation = fi_fsa_v2.FSAModeOfOperation.POSITION_CONTROL
        # protect_mode_of_operation = fi_fsa_v2.FSAModeOfOperation.POSITION_CONTROL_PD

        # 如果关闭该功能时正在保护状态，是否恢复到进入保护状态前的操作模式,0: no, 1: yes
        close_back_last_mode_of_operation = 0

        # 进入保护模式后是否禁止修改mode_of_operation,0: no, 1: yes
        ignore_set_mode_of_operation = 1

        tx_cnt = 0
        for i in range(len(server_ip_list)):
            tx_cnt = tx_cnt + 1
            print(tx_cnt)
            rx_cnt, res = fi_fsa_v2.fast_set_ack_setpvc_timeout_protect(server_ip_list[i],
                                                                        tx_cnt,
                                                                        protect_enable,
                                                                        timeout_ms,
                                                                        protect_mode_of_operation,
                                                                        close_back_last_mode_of_operation,
                                                                        ignore_set_mode_of_operation)
            print("cnt = %u, res = %d" % (rx_cnt, res))

        time.sleep(1)


if __name__ == "__main__":
    main()
