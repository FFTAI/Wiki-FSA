import fi_fsa_v1 as fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # auto run one turn to set the encoder of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.set_calibrate_encoder(server_ip_list[i])

        time.sleep(1)
        
        state = []
        for i in range(len(server_ip_list)):
            state.append(0)
        for count in range(5):
            for i in range(len(server_ip_list)):
                if state[i] != 4:
                    state[i] = fi_fsa.get_state(server_ip_list[i])
            for i in range(len(server_ip_list)):
                if state[i] == 4:
                    fi_fsa.set_disable(server_ip_list[i])
            time.sleep(1)  # sleep 1 second
        for i in range(len(server_ip_list)):
            fi_fsa.set_disable(server_ip_list[i])


if __name__ == '__main__':
    main()
