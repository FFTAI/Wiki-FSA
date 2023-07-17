#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace FSA_CONNECT {
namespace Transmit {
class UDPSocket {
  public:
    UDPSocket(std::string ip, uint16_t remote_port);
    ~UDPSocket();
    int SendData(const std::string &send_data);
    int SendData(const unsigned char *send_data, uint16_t data_length);
    int SendData(const char *send_data, uint16_t data_length);
    int ReceiveData_rt(std::string &receive);
    int ReceiveData_nrt(std::string &receive);
    int ReceiveData(unsigned char *rec_ptr);

  private:
    int sfd;
    char *ip_ = new char[16];
    struct sockaddr_in ser_addr_;
};
} // namespace Transmit
} // namespace FSA_CONNECT
