import time
import socket
import threading


udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


def udp_log_enable():
    remote_addr = ('192.168.137.51', 6666)
    while True:
        udp_socket.sendto('Hello, UDP!'.encode(), remote_addr)
        time.sleep(0.01)


def udp_log_recv():
    while True:
        data, addr = udp_socket.recvfrom(2048)
        print(f'Received data from {addr}: {data.decode()}')


def main():
    enable_thread = threading.Thread(target=udp_log_enable, name="enable_thread")
    enable_thread.start()

    recv_thread = threading.Thread(target=udp_log_recv, name="recv_thread")
    recv_thread.start()


if __name__ == '__main__':
    main()
