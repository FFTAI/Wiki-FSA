from fi_fsa import fi_fsa_v2
import time
import socket
import struct

server_ip_list = []
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_socket.bind(("", 0))
addr_not_use, port = udp_socket.getsockname()
print("Socket is bound to port:", port)


def main():
    server_ip_list = fi_fsa_v2.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        config = {
            "port": port,
            "enable": 1,
            "position": 1,
            "velocity": 1,
            "current": 1,
            "torque": 1,
            "error": 1,
            "error_ext2": 1,
            "error_ext3": 1,
            "error_ext4": 1,
            "error_ext5": 1,
            "error_ext6": 1,
            "error_ext7": 1,
            "error_ext8": 1,
        }

        for i in range(len(server_ip_list)):
            fi_fsa_v2.set_subscribe(server_ip_list[i], config)

        if config["enable"]:
            while True:
                data, addr = udp_socket.recvfrom(2048)
                print(": Server received from {}:{}".format(addr, data))
                print(len(data))
                pointer = 0
                while pointer < len(data):
                    if data[pointer] == 0x02:
                        value = struct.unpack(">f", data[pointer + 1: pointer + 5])
                        print("index = 0x02, position = %f" % value)
                    elif data[pointer] == 0x03:
                        value = struct.unpack(">f", data[pointer + 1: pointer + 5])
                        print("index = 0x03, velocity = %f" % value)
                    elif data[pointer] == 0x04:
                        value = struct.unpack(">f", data[pointer + 1: pointer + 5])
                        print("index = 0x04, current = %f" % value)
                    elif data[pointer] == 0x05:
                        value = struct.unpack(">f", data[pointer + 1: pointer + 5])
                        print("index = 0x05, torque = %f" % value)
                    elif data[pointer] == 0x06:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x06, cnt = %u" % value)
                    elif data[pointer] == 0x07:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x07, error = %u" % value)
                    elif data[pointer] == 0x08:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x08, error_ext2 = %u" % value)
                    elif data[pointer] == 0x09:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x09, error_ext3 = %u" % value)
                    elif data[pointer] == 0x0A:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x0A, error_ext4 = %u" % value)
                    elif data[pointer] == 0x0B:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x0B, error_ext5 = %u" % value)
                    elif data[pointer] == 0x0C:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x0C, error_ext6 = %u" % value)
                    elif data[pointer] == 0x0D:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x0D, error_ext7 = %u" % value)
                    elif data[pointer] == 0x0E:
                        value = struct.unpack(">I", data[pointer + 1: pointer + 5])
                        print("index = 0x0E, error_ext8 = %u" % value)
                    pointer = pointer + 5

        print("\n")
        time.sleep(1)


if __name__ == "__main__":
    main()
