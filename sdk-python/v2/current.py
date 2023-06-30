import aios
import time
import numpy as np


# pos_list = [1000, 2000, 3000, 5000, 2000, 6000, 10000, 0, 5000, -10000, 15000, 20000]
# delay_list = [0.3, 0.3, 0.3, 0.6, 0.6, 0.6, 1, 1, 1, 2, 2, 2]

pos_list = [1000, 2000, 3000, 5000, 8000, 0, -2000]
delay_list = [0.3, 0.3, 0.3, 0.6, 0.6, 0.6, 0.3]

Server_IP_list = []


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

            print('\n')

            for i in range(20):
                for i in range(len(Server_IP_list)):
                    cvp = aios.getCVP(Server_IP_list[i], 1)
                print("Position = %.2f, Velocity = %.0f, Current = %.4f" %(cvp[0], cvp[1], cvp[2]))
                time.sleep(0.01)

            enableSuccess = True

            for i in range(len(Server_IP_list)):
                enableSuccess = aios.enable(Server_IP_list[i], 1)
            print('\n')

            if enableSuccess:

                # aios.trapezoidalMove(0, Server_IP_1, 1)
                # time.sleep( 10 )
                for i in range(len(Server_IP_list)):
                    aios.setCurrent(0.5, True, Server_IP_list[i], 1)
                time.sleep(1)
                for i in range(len(Server_IP_list)):
                    aios.setCurrent(1, True, Server_IP_list[i], 1)
                time.sleep(1)
                for i in range(len(Server_IP_list)):
                    aios.setCurrent(1.5, True, Server_IP_list[i], 1)
                time.sleep(1)
                for i in range(len(Server_IP_list)):
                    aios.setCurrent(2, True, Server_IP_list[i], 1)
                time.sleep(2)

                # for i in range(400):
                #     start = time.time()
                #     pos = np.sin(i*0.2*np.pi)*2
                #     print(pos)
                #     aios.setCurrent(pos, Server_IP_1, 1)
                #     # aios.trapezoidalMove(pos, Server_IP_1, 1)
                #     print(time.time() - start)
                #     # time.sleep(0.01)

                for i in range(len(Server_IP_list)):
                    aios.setCurrent(0, True, Server_IP_list[i], 1)
                #
                #
                # for i in range(len(pos_list)):
                #     aios.trapezoidalMove(pos_list[i], Server_IP_1, 1)
                #     # aios.trapezoidalMove(pos_list[i], Server_IP_2, 1)
                #     time.sleep( delay_list[i] )


                for i in range(len(Server_IP_list)):
                    aios.disable(Server_IP_list[i], 1)



if __name__ == '__main__':
    main()
