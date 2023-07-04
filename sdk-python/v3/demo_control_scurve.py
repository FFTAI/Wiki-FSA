# 本 S 型规划基于 PySCurve 库，该库的源码地址：https://github.com/nameofuser1/py-scurve
# 该库依赖于 matplotlib 库，因此需要先安装 matplotlib 库: pip install matplotlib

import fsa
import time
import math
import numpy as np
import pyscurve

server_ip_list = []


def main():
    # 生成 S 型曲线
    q0 = [0.0]
    q1 = [360.0]
    v0 = [0.0]
    v1 = [0.0]
    v_max = 20.0
    a_max = 15.0
    j_max = 100.0
    T = 10.0

    p = pyscurve.ScurvePlanner()
    traj = p.plan_trajectory(q0, q1, v0, v1, v_max, a_max, j_max, t=T)
    # pyscurve.plot_trajectory(traj, dt=0.01)
    dt = 0.01

    # 获得 S 型曲线的位置、速度、加速度、时间
    dof = traj.dof
    timesteps = int(max(traj.time) / dt)
    time = np.linspace(0, max(traj.time), timesteps)

    p_list = [traj(t) for t in time]
    profiles = np.asarray(p_list)

    r_profiles = np.zeros((dof, 3, timesteps))
    for d in range(dof):
        for p in range(3):
            r_profiles[d, p, :] = profiles[:, d, p]

    print("dof = %s" % dof)

    exit()

    server_ip_list = fsa.broadcast_func()

    if server_ip_list:

        fsa_state = True
        for i in range(len(server_ip_list)):
            fsa_state = fsa.get_state(server_ip_list[i])
            print("State = %d" % fsa_state)

        print('\n')
        time.sleep(1)

        for i in range(len(server_ip_list)):
            fsa.get_config(server_ip_list[i])

        print('\n')
        time.sleep(1)

        for i in range(len(server_ip_list)):
            pvc = fsa.get_pvc(server_ip_list[i])
            print("Position = %.2f, Velocity = %.0f, Current = %.4f" % (pvc[0], pvc[1], pvc[2]))
            time.sleep(0.01)

        print('\n')
        time.sleep(1)

        # set current control current to 0.0
        for i in range(len(server_ip_list)):
            fsa.set_position_control(server_ip_list[i], 0.0)

        print('\n')
        time.sleep(1)

        # enable all the motors
        for i in range(len(server_ip_list)):
            fsa.set_enable(server_ip_list[i])

        print('\n')
        time.sleep(1)

        # set work at current control mode
        for i in range(len(server_ip_list)):
            fsa.set_mode_of_operation(server_ip_list[i], fsa.FSAModeOfOperation.POSITION_CONTROL.value[0])

        print('\n')
        time.sleep(1)

        for i in range(len(server_ip_list)):
            fsa.set_position_control(server_ip_list[i], 0.0)

        time.sleep(1)

        # move a sin wave
        count_max = round(1000 * 2 * math.pi)
        for t in range(0, count_max):
            for i in range(len(server_ip_list)):
                # 梯形规划函数
                set_position = 2 * math.pi * 0.5 * (1 - math.cos(2 * math.pi * t / count_max))

                fsa.set_position_control(server_ip_list[i], set_position)
            time.sleep(0.01)

        time.sleep(1)

        # for i in range(len(server_ip_list)):
        #     fsa.set_position_control(server_ip_list[i], 0.0)
        #
        # time.sleep(1)

        for i in range(len(server_ip_list)):
            fsa.set_disable(server_ip_list[i])

        time.sleep(1)

        # set work at none control mode
        for i in range(len(server_ip_list)):
            fsa.set_mode_of_operation(server_ip_list[i], fsa.FSAModeOfOperation.POSITION_CONTROL.value[0])


if __name__ == '__main__':
    main()
