import aios
import time
import threading
import numpy as np
import json
import colorama
from colorama import Fore, Back, Style

# Server_IP_list = ['192.168.5.126']
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


            enableSuccess = True

            for i in range(len(Server_IP_list)):
                enableSuccess = aios.enable(Server_IP_list[i], 1)
            print('\n')

            if enableSuccess:

                for i in range(len(Server_IP_list)):
                    aios.trapezoidalMove(0, False, Server_IP_list[i], 1)
                    # aios.passthrough_pt(Server_IP_list[i], 't 1 0\n')
                time.sleep( 2 )

                for j in range(len(Server_IP_list)):
                    aios.inputMode(1, Server_IP_list[j], 1)

                time.sleep( 1 )
                 
                start = 0;
                i = 0;
                while True:
                    if time.time() - start >= 0.002:
                        start = time.time()
                        i += 1
                        if i > 3000:
                            break
                        pos = np.sin(i*0.006*np.pi)*1
                        for j in range(len(Server_IP_list)):
                            # aios.setPosition(pos, 0, 0, True, Server_IP_list[j], 1)
                            feedback = aios.setInputPosition_pt(Server_IP_list[j], pos, 0, 0)
                            print(feedback)
                            # aios.trapezoidalMove(pos, False, Server_IP_list[j], 1)
                        # for j in range(len(Server_IP_list)):
                        #     aios.receive_func()
                        latency = time.time() - start
                        print(latency*1000)
                        if latency > 0.2:
                            print(Fore.RED + Style.BRIGHT + str(latency))

                for j in range(len(Server_IP_list)):
                    aios.inputMode(5, Server_IP_list[j], 1)

                for i in range(len(Server_IP_list)):
                    # aios.passthrough_pt(Server_IP_list[i], 't 1 0.5\n')
                    aios.trapezoidalMove(0, False, Server_IP_list[i], 1)

            time.sleep( 2 )



            for i in range(len(Server_IP_list)):
                aios.disable(Server_IP_list[i], 1)



if __name__ == '__main__':
    main()
