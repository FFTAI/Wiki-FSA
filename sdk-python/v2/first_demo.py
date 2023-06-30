import aios
import time
import threading
import numpy as np
import json

Server_IP_list = []

pos_list_1 = [0.25, 0.5, 0.75, 1.25, 0.5, 1.5, 2.5, 0, 0.75, -2.5, 3.75, 5, 0]
delay_list_1 = [0.3, 0.3, 0.3, 0.6, 0.6, 0.6, 1, 1, 1, 2, 2, 2, 2]

pos_list_2 = [1, 2, 3, 4, 0]
delay_list_2 = [0, 0, 0, 0, 1]


def main():

    Server_IP_list = aios.broadcast_func()
    
    if Server_IP_list:


        # for i in range(5):
        #     for i in range(len(Server_IP_list)):
        #         cvp = aios.getCVP(Server_IP_list[i], 1)
        #         print("Position = %.2f, Velocity = %.0f, Current = %.4f" %(cvp[0], cvp[1], cvp[2]))

        #     time.sleep(0.02)
        # print('\n')

        encoderIsReady = True
        for i in range(len(Server_IP_list)):
            if (not aios.encoderIsReady(Server_IP_list[i], 1)):
                encoderIsReady = False

        print('\n')

        if encoderIsReady:
            for i in range(len(Server_IP_list)):
                aios.getRoot(Server_IP_list[i])

            print('\n')

            for i in range(len(Server_IP_list)):
                enableSuccess = aios.enable(Server_IP_list[i], 1)
            print('\n')

            if enableSuccess:

                for i in range(len(Server_IP_list)):
                    aios.trapezoidalMove(0, True, Server_IP_list[i], 1)
                time.sleep( 2 )


                for i in range(len(pos_list_1)):
                    start = time.time()
                    for j in range(len(Server_IP_list)):
                        aios.trapezoidalMove(pos_list_1[i], True, Server_IP_list[j], 1)
                    print((time.time() - start)*1)
                    time.sleep( delay_list_1[i] )


                for i in range(len(pos_list_2)):
                    start = time.time()
                    for j in range(len(Server_IP_list)):
                        aios.trapezoidalMove(pos_list_2[i], True, Server_IP_list[j], 1)
                        time.sleep( 0.2 )
                    print((time.time() - start)*1)
                    time.sleep( delay_list_2[i] )

                for i in range(800):
                    start = time.time()
                    pos = np.sin(i*0.04*np.pi)*0.5
                    print(pos)
                    for j in range(len(Server_IP_list)):
                        # aios.setPosition(pos, 0, 0, True, Server_IP_list[j], 1)
                        aios.trapezoidalMove(pos, False, Server_IP_list[j], 1)
                    print(time.time() - start)
                    time.sleep(0.01)


                for i in range(len(Server_IP_list)):
                    aios.trapezoidalMove(2, True, Server_IP_list[i], 1)
                time.sleep( 1 )



                for i in range(len(Server_IP_list)):
                    aios.disable(Server_IP_list[i], 1)



if __name__ == '__main__':
    main()
