import aios
import time

Server_IP_list = []

vel_list = [2.5, 10, 15, 20, -20, -6, 0]
delay_list = [3, 3, 3, 3, 4, 5, 1]


def main():
    Server_IP_list = aios.broadcast_func()

    if Server_IP_list:

        encoderIsReady = True
        for i in range(len(Server_IP_list)):
            if (not aios.encoderIsReady(Server_IP_list[i], 1)):
                encoderIsReady = False

        print('\n')

        if encoderIsReady:
            for i in range(len(Server_IP_list)):
                aios.getRoot(Server_IP_list[i])

            i = 0;
            enableSuccess = aios.enable(Server_IP_list[i], 1)
            print('\n')

            if enableSuccess:

                # aios.trapezoidalMove(0, False, Server_IP_list[i], 1)
                # time.sleep( 4 )

                aios.inputMode(2, Server_IP_list[i], 1)

                for j in range(len(vel_list)):
                    aios.setVelocity(vel_list[j], 0, True, Server_IP_list[i], 1)
                    time.sleep(delay_list[j])

                aios.disable(Server_IP_list[i], 1)


if __name__ == '__main__':
    main()
