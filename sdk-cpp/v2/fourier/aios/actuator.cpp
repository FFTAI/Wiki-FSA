#include "actuator.h"

#include <fstream>

#include "log.h"

using namespace std;

float Fourier::Broadcast::frequency_ = 5.0f;

LOG mylog;

namespace Fourier {

uv_loop_t *g_loop = uv_default_loop();

static void alloc_cb(uv_handle_t *handle, size_t suggested_size,
                     uv_buf_t *buf) {
  static char slab[65536];
  buf->base = slab;
  buf->len = sizeof(slab);
}
static void close_cb(uv_handle_t *handle) { uv_is_closing(handle); }

static void cl_recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf,
                       const struct sockaddr *addr, unsigned flags) {
  DEBUG("nread: {}", nread);
  if (nread == 0) {
    return;
  }
  static char sender[17] = {0};
  uv_ip4_name((const struct sockaddr_in *)addr, sender, 16);
  std::string cur_ip = std::string(sender);
  std::string content = std::string(buf->base);
  DEBUG("receive from {} : {}", cur_ip, content);

  auto ipAndActuatorInfo = (std::map<std::string, std::string> *)handle->data;
  // 广播获取的电压不是一个确定值，故将其删除，因为ipAndActuatorInfo将与上次的结果进行比较
#ifdef WIN32
  std::regex pattern("\"vbus_voltage\":[\d|.]*,");
#else
  std::regex pattern("\"vbus_voltage\":[\\d|.]*,");
#endif
  std::regex pattern_brace("[{|}]");
  content = std::regex_replace(content, pattern, "");
  content = std::regex_replace(content, pattern_brace, "");
  ipAndActuatorInfo->insert(make_pair(cur_ip, content));
}

void timecb(uv_timer_t *req) {
  DEBUG("********** timeout *********");
  uv_udp_t *handle = (uv_udp_t *)req->data;
  uv_udp_recv_stop(handle);

  uv_timer_stop(req);
}

Broadcast_::~Broadcast_() {
  uv_close((uv_handle_t *)&send_socket_, close_cb);
  uv_close((uv_handle_t *)&recv_socket_, close_cb);
  uv_close((uv_handle_t *)&timer_, NULL);
};

int Broadcast_::constructionSocket() {
  int res = 0;
  res = uv_udp_init(g_loop, &send_socket_);
  if (res != 0) {
    SPDERROR("uv_udp_init error: {}", uv_strerror(res));
    return -1;
  }

  res = uv_ip4_addr(network_.c_str(), PORT_SER, &send_addr_);
  if (res != 0) {
    SPDERROR("uv_ip4_addr error: {}", uv_strerror(res));
    return -1;
  }

  res = uv_udp_bind(&send_socket_, (const struct sockaddr *)&send_addr_,
                    UV_UDP_REUSEADDR);
  if (res != 0) {
    SPDERROR("uv_udp_connect error: {}", uv_strerror(res));
    return -1;
  }

  res = uv_udp_set_broadcast(&send_socket_, 1);
  if (res != 0) {
    SPDERROR("uv_udp_set_broadcast error: {}", uv_strerror(res));
    return -1;
  }

  res = uv_timer_init(g_loop, &timer_);
  if (res != 0) {
    SPDERROR("uv_timer_init error: {}", uv_strerror(res));
    return -1;
  }

  res = uv_ip4_addr("0.0.0.0", PORT_SER, &recv_addr_);
  if (res != 0) {
    SPDERROR("uv_ip4_addr error: {}", uv_strerror(res));
    return -1;
  }
  res = uv_udp_init(g_loop, &recv_socket_);
  if (res != 0) {
    SPDERROR("uv_udp_init error: {}", uv_strerror(res));
    return -1;
  }
  res = uv_udp_bind(&recv_socket_, (const struct sockaddr *)&recv_addr_,
                    UV_UDP_REUSEADDR);
  if (res != 0) {
    SPDERROR("uv_udp_bind error: {}", uv_strerror(res));
    return -1;
  }

  timer_.data = &recv_socket_;
  return 0;
};

int Broadcast_::broadcast_func(strMapPtr ipAndActuatorInfo, int timeout) {
  recv_socket_.data = ipAndActuatorInfo.get();
  char msg[] = "Is any AIOS server here?";
  int res = 0;
  uv_buf_t buf = uv_buf_init(msg, sizeof(msg));
  uv_udp_send_t req;

  res = uv_udp_send(&req, &send_socket_, &buf, 1,
                    (const struct sockaddr *)&send_addr_, NULL);
  if (res != 0) {
    SPDERROR("uv_udp_send error: {}", uv_strerror(res));
    return -1;
  }
  res = uv_udp_recv_start(&recv_socket_, alloc_cb, cl_recv_cb);
  if (res != 0) {
    SPDERROR("uv_udp_recv_start error: {}", uv_strerror(res));
    return -1;
  }
  res = uv_timer_start(&timer_, timecb, timeout, 0);
  if (res != 0) {
    SPDERROR("uv_timer_start error: {}", uv_strerror(res));
    return -1;
  }

  uv_run(g_loop, UV_RUN_DEFAULT);
};

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp) {
  time_t clock;
  struct tm tm;
  SYSTEMTIME wtm;
  GetLocalTime(&wtm);
  tm.tm_year = wtm.wYear - 1900;
  tm.tm_mon = wtm.wMonth - 1;
  tm.tm_mday = wtm.wDay;
  tm.tm_hour = wtm.wHour;
  tm.tm_min = wtm.wMinute;
  tm.tm_sec = wtm.wSecond;
  tm.tm_isdst = -1;
  clock = mktime(&tm);
  tp->tv_sec = clock;
  tp->tv_usec = wtm.wMilliseconds * 1000;
  return (0);
}
#endif

int Broadcast::constructionSocket(int timeout)  // ms
{
#ifdef WIN32
  WSADATA wsaData;
  WORD sockVersion = MAKEWORD(2, 2);
  if (WSAStartup(sockVersion, &wsaData) != 0) {
    SPDERROR("Socket initialization failed.");
    return FourierStatusFailure;
  }
#endif

  // create socket
  sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
  sockfd_serv_ = socket(AF_INET, SOCK_DGRAM, 0);
  sockfd_rt_ = socket(AF_INET, SOCK_DGRAM, 0);
  sockfd_pt_ = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef WIN32
  if (sockfd_ == INVALID_SOCKET) {
    SPDERROR("Failed to socket create socket.");
    return FourierStatusFailure;
  }
#else
  if (sockfd_ < 0) {
    SPDERROR("Failed to socket create socket.");
    return FourierStatusFailure;
  }
#endif

  // Time out
#ifdef WIN32
  int recv_timeout = timeout;  // ms
  if (SOCKET_ERROR == setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO,
                                 (char *)&recv_timeout, sizeof(int))) {
    SPDERROR("setsockopt failed.");
    closesocket(sockfd_);
    return FourierStatusFailure;
  };
#else
  struct timeval recv_timeout;
  recv_timeout.tv_sec = std::trunc(timeout / 1000);
  recv_timeout.tv_usec = (timeout % 1000) * 1000;  //  us
  if (setsockopt(sockfd_, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout,
                 sizeof(recv_timeout)) == FourierStatusFailure) {
    SPDERROR("setsockopt failed.");
    close(sockfd_serv_);
    return FourierStatusFailure;
  };
  if (setsockopt(sockfd_serv_, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout,
                 sizeof(recv_timeout)) == FourierStatusFailure) {
    SPDERROR("setsockopt failed.");
    close(sockfd_serv_);
    return FourierStatusFailure;
  };
  if (setsockopt(sockfd_rt_, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout,
                 sizeof(recv_timeout)) == FourierStatusFailure) {
    SPDERROR("setsockopt failed.");
    close(sockfd_serv_);
    return FourierStatusFailure;
  };
  if (setsockopt(sockfd_pt_, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout,
                 sizeof(recv_timeout)) == FourierStatusFailure) {
    SPDERROR("setsockopt failed.");
    close(sockfd_serv_);
    return FourierStatusFailure;
  };
#endif

  // set SO_BROADCAST
#ifdef WIN32
  bool optval = true;
  if (SOCKET_ERROR == setsockopt(sockfd_, SOL_SOCKET, SO_BROADCAST,
                                 (char *)&optval, sizeof(optval))) {
    printf("setsockopt SO_BROADCAST error: %d, %s\n", errno, strerror(errno));
    closesocket(sockfd_);
    return FourierStatusFailure;
  }
#else
  if (network_ == "10.10.10.255") {
    int optval = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int)) ==
        FourierStatusFailure) {
      printf("setsockopt SO_BROADCAST error: %d, %s\n", errno, strerror(errno));
      close(sockfd_);
      return FourierStatusFailure;
    }
  }
#endif

  setSockaddr(network_);

  return FourierStatusSuccess;
};

int Broadcast::setSockaddr(const std::string ip) {
  // set ip and port
  sockaddr_len_ = sizeof(struct sockaddr_in);
  struct in_addr p;
  inet_pton(AF_INET, ip.c_str(), &p);

  memset(&sockaddr_broadcast_, 0, sizeof(sockaddr_broadcast_));
  sockaddr_broadcast_.sin_family = AF_INET;  // Use IPV4
  sockaddr_broadcast_.sin_port = htons(PORT_srv);
  sockaddr_broadcast_.sin_addr = p;

  memset(&sockaddr_serv_, 0, sizeof(sockaddr_serv_));
  sockaddr_serv_.sin_family = AF_INET;  // Use IPV4
  sockaddr_serv_.sin_port = htons(PORT_srv);
  sockaddr_serv_.sin_addr = p;

  if (ip != "10.10.10.255") {
    int ret = connect(sockfd_serv_, (struct sockaddr *)&sockaddr_serv_,
                      sizeof(sockaddr_serv_));
    if (ret < 0) {
      perror("bind failed");
      SPDERROR("sockaddr_serv bind failed");
      exit(EXIT_FAILURE);
    }
  }

  memset(&sockaddr_rt_, 0, sizeof(sockaddr_rt_));
  sockaddr_rt_.sin_family = AF_INET;  // Use IPV4
  sockaddr_rt_.sin_port = htons(PORT_rt);
  sockaddr_rt_.sin_addr = p;
  if (ip != "10.10.10.255") {
    int ret = connect(sockfd_rt_, (struct sockaddr *)&sockaddr_rt_,
                      sizeof(sockaddr_rt_));
    if (ret < 0) {
      perror("bind failed");
      SPDERROR("sockfd_rt_ bind failed");
      exit(EXIT_FAILURE);
    }
  }
  memset(&sockaddr_pt_, 0, sizeof(sockaddr_pt_));
  sockaddr_pt_.sin_family = AF_INET;  // Use IPV4
  sockaddr_pt_.sin_port = htons(PORT_pt);
  sockaddr_pt_.sin_addr = p;
  if (ip != "10.10.10.255") {
    int ret = connect(sockfd_pt_, (struct sockaddr *)&sockaddr_pt_,
                      sizeof(sockaddr_pt_));
    if (ret < 0) {
      perror("bind failed");
      SPDERROR("sockfd_pt_ bind failed");
      exit(EXIT_FAILURE);
    }
  }
  return FourierStatusSuccess;
};

FourierStatusCode Broadcast::broadcast_func(strMapPtr ipAndActuatorInfo) {
  bool found_server_flag = false;
  std::string send_buf = "Is any AIOS server here?";

  setSockaddr(network_);
  sendBroadcastData(send_buf);

  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, recv_buf_len_);
  int recv_num;
  std::string cur_ip;
  while (true) {
#ifdef WIN32
    recv_num = recvfrom(sockfd_, recv_buf, sizeof(recv_buf), 0,
                        (struct sockaddr *)&sockaddr_serv_, &sockaddr_len_);
#else
    recv_num = recvfrom(sockfd_, recv_buf, sizeof(recv_buf), 0,
                        (struct sockaddr *)&sockaddr_broadcast_,
                        (socklen_t *)&sockaddr_len_);
#endif
    if (recv_num < 0) {
      if (found_server_flag) {
        DEBUG("found servers");
        INFO("lookup Finished.");
        break;
      } else {
        INFO("[Timeout] [{}] Do not have any server!", network_);
        return FourierStatusFailure;
      }
    }

    char p[inet_buf_len_];
    cur_ip =
        inet_ntop(AF_INET, &sockaddr_broadcast_.sin_addr, p, inet_buf_len_);
    DEBUG("server receive from " + cur_ip + ":" + recv_buf);

    // 广播获取的电压不是一个确定值，故将其删除，因为ipAndActuatorInfo将与上次的结果进行比较
    std::string tmp(recv_buf);
#ifdef WIN32
    std::regex pattern("\"vbus_voltage\":[\d|.]*,");
#else
    std::regex pattern("\"vbus_voltage\":[\\d|.]*,");
#endif
    std::regex pattern_brace("[{|}]");
    tmp = std::regex_replace(tmp, pattern, "");
    tmp = std::regex_replace(tmp, pattern_brace, "");
    ipAndActuatorInfo->insert(make_pair(std::string(cur_ip), tmp));
    found_server_flag = true;
  }
  return FourierStatusSuccess;
}
FourierStatusCode Broadcast::sendData(const std::string &send_buf,
                                      sockaddr_in sockaddr, bool print_flag) {
  if (print_flag) DEBUG("Send JSON Obj:" + send_buf);

  int send_num = sendto(sockfd_, send_buf.c_str(), send_buf.size(), 0,
                        (struct sockaddr *)&sockaddr, sockaddr_len_);
  if (send_num < 0) {
    SPDERROR("sendto error: " + send_buf);
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}
FourierStatusCode Broadcast::sendData(const unsigned char *send_buf,
                                      const int str_len, sockaddr_in sockaddr,
                                      const bool print_flag) {
  if (print_flag) DEBUG("Send JSON Obj:" + *send_buf);
  int send_num = sendto(sockfd_, (const char *)send_buf, str_len, 0,
                        (struct sockaddr *)&sockaddr, sockaddr_len_);
  if (send_num < 0) {
    SPDERROR("sendto error: " + *send_buf);
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
};

FourierStatusCode Broadcast::sendBroadcastData(const std::string &send_buf,
                                               bool print_flag) {
  if (print_flag) DEBUG("Send JSON Obj:" + send_buf);

  int send_num = sendto(sockfd_, send_buf.c_str(), send_buf.size(), 0,
                        (struct sockaddr *)&sockaddr_broadcast_,
                        sizeof(sockaddr_broadcast_));
  if (send_num < 0) {
    SPDERROR("sendto error: " + send_buf);
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Broadcast::sendServData(const std::string &send_buf,
                                          bool print_flag) {
  if (print_flag) DEBUG("Send JSON Obj:" + send_buf);

  int send_num = send(sockfd_serv_, send_buf.c_str(), send_buf.size(), 0);
  if (send_num < 0) {
    SPDERROR("sendto error: " + send_buf);
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Broadcast::sendRtData(const std::string &send_buf,
                                        bool print_flag) {
  if (print_flag) DEBUG("Send JSON Obj:" + send_buf);

  int send_num = send(sockfd_rt_, send_buf.c_str(), send_buf.size(), 0);
  if (send_num < 0) {
    SPDERROR("sendto error: " + send_buf);
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Broadcast::sendPtData(const std::string &send_buf,
                                        bool print_flag) {
  if (print_flag) DEBUG("Send JSON Obj:" + send_buf);
  std::cout << "send_pt_data" << std::endl;

  int send_num = send(sockfd_pt_, send_buf.c_str(), send_buf.size(), 0);
  if (send_num < 0) {
    SPDERROR("sendto error: " + send_buf);
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Broadcast::sendPtData(const unsigned char *send_buf,
                                        int str_len, bool print_flag) {
  std::string send_buf_str((char *)send_buf, str_len);
  return sendPtData(send_buf_str, print_flag);
}

FourierStatusCode Broadcast::recvServData(char *recv_buf, bool print_flag) {
  memset(recv_buf, 0, recv_buf_len_);
#ifdef WIN32
  int recv_num = recvfrom(sockfd_, recv_buf, recv_buf_len_, 0,
                          (struct sockaddr *)&sockaddr_serv_, &sockaddr_len_);
#else
  // int recv_num =
  //     recvfrom(sockfd_, recv_buf, recv_buf_len_, 0,
  //              (struct sockaddr *)&sockaddr_serv_, (socklen_t
  //              *)&sockaddr_len_);
  int recv_num = recv(sockfd_serv_, recv_buf, recv_buf_len_, 0);
#endif  // WIN32

  if (recv_num < 0) {
    SPDERROR("[Timeout] [{}] Didn't receive anymore data!", network_);
    return FourierStatusFailure;
  }
  if (print_flag) {
    char p[inet_buf_len_];
    std::string ip =
        inet_ntop(AF_INET, &sockaddr_serv_.sin_addr, p, inet_buf_len_);
    DEBUG("server receive from " + ip + ":" + recv_buf);
  }
  return FourierStatusSuccess;
}

FourierStatusCode Broadcast::recvRtData(char *recv_buf, bool print_flag) {
  int recv_num = -1;
  char *recv_buf_ptr = recv_buf;
  if (recv_buf == nullptr) {
    recv_buf_ptr = recv_rt_buf_;
  }
  memset(recv_buf_ptr, 0, recv_buf_len_);
#ifdef WIN32
  recv_num = recvfrom(sockfd_, recv_buf_ptr, recv_buf_len_, 0,
                      (struct sockaddr *)&sockaddr_rt_, &sockaddr_len_);
#else
  // recv_num =
  //     recvfrom(sockfd_, recv_buf_ptr, recv_buf_len_, 0,
  //              (struct sockaddr *)&sockaddr_rt_, (socklen_t
  //              *)&sockaddr_len_);
  recv_num = recv(sockfd_rt_, recv_buf_ptr, recv_buf_len_, 0);
#endif  // WIN32

  if (recv_num < 0) {
    SPDERROR("[Timeout] [{}] Didn't receive anymore data!", network_);
    return FourierStatusFailure;
  }
  if (print_flag) {
    char p[inet_buf_len_];
    std::string ip =
        inet_ntop(AF_INET, &sockaddr_rt_.sin_addr, p, inet_buf_len_);
    DEBUG("server receive from " + ip + ":" + recv_buf_ptr);
  }

  return FourierStatusSuccess;
}

FourierStatusCode Broadcast::recvPtData(char *recv_buf, bool print_flag,
                                        std::string *recv_ip) {
  // fw2.0
  int recv_num = -1;
  char *recv_buf_ptr = recv_buf;
  if (recv_buf == nullptr) {
    recv_buf_ptr = recv_pt_buf_;
  }

  memset(recv_buf_ptr, 0, recv_buf_pt_len_);
#ifdef WIN32
  recv_num = recvfrom(sockfd_, recv_buf_ptr, recv_buf_pt_len_, 0,
                      (struct sockaddr *)&sockaddr_pt_, &sockaddr_len_);
#else
  // recv_num =
  //     recvfrom(sockfd_, recv_buf_ptr, recv_buf_pt_len_, 0,
  //              (struct sockaddr *)&sockaddr_pt_, (socklen_t
  //              *)&sockaddr_len_);
  recv_num = recv(sockfd_pt_, recv_buf_ptr, recv_buf_pt_len_, 0);
#endif  // WIN32

  if (recv_num < 0) {
    SPDERROR("[Timeout] [{}] Didn't receive anymore data!", network_);
    return FourierStatusFailure;
  }
  char p[inet_buf_len_];
  std::string ip = inet_ntop(AF_INET, &sockaddr_pt_.sin_addr, p, inet_buf_len_);
  if (recv_ip != nullptr) *recv_ip = ip;

  if (print_flag) {
    Fourier::CvpPtFeedback pos_pt_info;
    memcpy((void *)&pos_pt_info, recv_buf_ptr + 1,
           sizeof(Fourier::CvpPtFeedback));
    DEBUG("server receive val:" + std::to_string(pos_pt_info.pos) + " " +
          std::to_string(pos_pt_info.vel) + " " +
          std::to_string(pos_pt_info.torque));
  }
  return FourierStatusSuccess;
}

FourierStatusCode Broadcast::recvPtData(
    std::shared_ptr<CvpPtFeedback> cvp_pt_feedback) {
  // fw2.0
  char recv[recv_buf_pt_len_];
  if (recvPtData(recv) != FourierStatusSuccess) {
    return FourierStatusFailure;
  };

  memcpy((void *)cvp_pt_feedback.get(), (void *)(recv + 1),
         sizeof(CvpPtFeedback));
  return FourierStatusSuccess;
}

FourierStatusCode Aios::getRoot(char *recv_buf, bool print_flag) {
  nlohmann::json data = {{"method", "GET"}, {"reqTarget", "/"}};

  char tmp[recv_buf_len_];
  if (recv_buf == nullptr) {
    recv_buf = tmp;
    sendServData(data.dump(), print_flag);
    return FourierStatusSuccess;
  }
  memset(recv_buf, 0, recv_buf_len_);
  if (runServSocket(data, recv_buf, print_flag) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
};

Aios::Aios(std::string ip) : Fourier::Broadcast(ip) {
  nlohmann::json j;
  // TODO:
  // ifstream
  // jfile("/home/fourier/Workspace/AIOS-GUI/AiosGUI/aios_cpp/kits/arm/arm.json");
  // jfile >> j;
  // for(auto it = j.begin(); it != j.end(); ++it) {
  //     g_cfg->insert(make_pair(it.key(), vector<cfgStc>()));
  //     for (auto elem : *it) {
  //         cfgStc tmp;
  //         tmp.name = elem.at("name");
  //         tmp.serial_number = elem.at("serial_number");
  //         auto res = g_cfg->find(std::string(it.key()));
  //         res->second.push_back(tmp);
  //     }
  // }
  // jfile.close();
}

Aios::~Aios() {}

bool Aios::isMotorNumberInVec(const int motor_number) {
  auto it = find(motorNumbers.begin(), motorNumbers.end(), motor_number);
  if (it == motorNumbers.end()) {
    SPDERROR("There is no motor with that number.");
    return false;
  }
  return true;
}

void Aios::printPVC(const char *buf) {
  std::string buf_str = buf;
  auto json_obj = nlohmann::json::parse(buf_str);
  DEBUG("Position = {}, Velocity = {}, Current = {}", json_obj["position"],
        json_obj["velocity"], json_obj["current"]);
}

FourierStatusCode Broadcast::runServSocket(const nlohmann::json &data,
                                           char *recv_buf, bool print_flag) {
  std::string json_str = data.dump();
  if (sendServData(json_str) < 0) {
    return FourierStatusFailure;
  }

  if (recvServData(recv_buf) < 0) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Broadcast::runRtSocket(const nlohmann::json &data,
                                         char *recv_buf, bool print_flag) {
  std::string json_str = data.dump();
  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }

  if (recvRtData(recv_buf) < 0) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::enable(const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/m0/requested_state"},
      {"property", AXIS_STATE_ENABLE},
  };
  std::string motor_str = "/m" + to_string(motor_number) + "/requested_state";
  data["reqTarget"] = motor_str;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  std::string buf = recv_buf;
  auto json_obj = nlohmann::json::parse(buf);
  if (json_obj["status"] != "OK") {
    SPDERROR("Recv Data Error !");
    SPDERROR(buf);
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::disable(const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/m0/requested_state"},
      {"property", AXIS_STATE_IDLE},
  };
  std::string motor_str = "/m" + to_string(motor_number) + "/requested_state";
  data["reqTarget"] = motor_str;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::ctrlBoxEnable() {
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/"},
      {"property", "EN_BTS50085"},
      {"EN_BTS50085", "1"},
  };
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runRtSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  std::string buf = recv_buf;
  auto json_obj = nlohmann::json::parse(buf);
  if (json_obj["status"] != "OK") {
    SPDERROR("Recv Data Error !");
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::ctrlBoxDisable() {
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/"},
      {"property", "EN_BTS50085"},
      {"EN_BTS50085", "0"},
  };
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runRtSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::getState(AxisState *axis_state, const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "GET"},
                         {"reqTarget", "/m0/requested_state"}};
  std::string motor_str = "/m" + to_string(motor_number) + "/requested_state";
  data["reqTarget"] = motor_str;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  std::string buf = recv_buf;
  auto json_obj = nlohmann::json::parse(buf);
  if (json_obj["status"] != "OK") {
    SPDERROR("Recv Data Error !");
    SPDERROR(buf);
    return FourierStatusFailure;
  }
  try {
    if (json_obj["current_state"] == 1)
      *axis_state = AxisState::AXIS_STATE_IDLE;
    else if (json_obj["current_state"] == 8)
      *axis_state = AxisState::AXIS_STATE_ENABLE;
  } catch (std::exception) {
    SPDERROR("Return value parsing error in getState");
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

int Aios::getRootConfig() {
  nlohmann::json data = {{"method", "GET"}, {"reqTarget", "/config"}};
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::setRootConfig(const nlohmann::json &configParam) {
  nlohmann::json data = {{"method", "GET"},
                         {"reqTarget", "/config"},
                         {"dc_bus_overvoltage_trip_level", 30},
                         {"dc_bus_undervoltage_trip_level", 10}};
  data.update(configParam);

  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::saveConfig() {
  nlohmann::json data = {
      {"method", "SET"}, {"reqTarget", "/"}, {"property", "save_config"}};
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::eraseConfig() {
  nlohmann::json data = {
      {"method", "SET"}, {"reqTarget", "/"}, {"property", "erase_config"}};
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::reboot() {
  nlohmann::json data = {
      {"method", "SET"}, {"reqTarget", "/"}, {"property", "reboot"}};
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::rebootMotorDrive() {
  nlohmann::json data = {{"method", "SET"},
                         {"reqTarget", "/"},
                         {"property", "reboot_motor_drive"}};
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::OTAupdate() {
  nlohmann::json data = {
      {"method", "SET"}, {"reqTarget", "/"}, {"property", "OTA_update"}};
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::getError(char *recv_buf, const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "GET"}, {"reqTarget", "/m0/error"}};
  std::string motor_str = "/m" + to_string(motor_number) + "/error";
  data["reqTarget"] = motor_str;
  if (recv_buf != nullptr) {
    if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
      return FourierStatusFailure;
    }
  } else {
    char recv[recv_buf_len_];
    memset(recv, 0, sizeof(recv));
    if (runServSocket(data, recv) != FourierStatusSuccess) {
      return FourierStatusFailure;
    }
  }
  return FourierStatusSuccess;
}

int Aios::clearError(const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "SET"}, {"reqTarget", "/m0/error"}};
  std::string motor_str = "/m" + to_string(motor_number) + "/error";
  data["reqTarget"] = motor_str;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

int Aios::getCVP(
    std::shared_ptr<std::map<std::string, std::vector<double>>> pvcMap,
    bool print_flag, const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "GET"}, {"reqTarget", "/m0/CVP"}};
  std::string motor_str = "/m" + to_string(motor_number) + "/CVP";
  data["reqTarget"] = motor_str;
  std::string json_str = data.dump();

  if (sendRtData(json_str, print_flag) < 0) {
    return FourierStatusFailure;
  }
  if (!print_flag) {
    return FourierStatusSuccess;
  }
  if (pvcMap == nullptr) {
    SPDERROR("ERROR: pvcMap must be passed a valid pointer");
  }

  return recvCVPData(pvcMap);
}

int Aios::recvCVPData(std::shared_ptr<std::vector<double>> vPVC) {
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (recvRtData(recv_buf, false) < 0) {
    return FourierStatusFailure;
  }
  std::string buf = recv_buf;
  auto json_obj = nlohmann::json::parse(buf);
  if (json_obj["status"] != "OK") {
    SPDERROR("Recv Data Error !");
    return FourierStatusFailure;
  }
  vPVC->clear();
  try {
    vPVC->push_back(double(json_obj["position"]));
    vPVC->push_back(double(json_obj["velocity"]));
    vPVC->push_back(double(json_obj["current"]));
  } catch (std::exception) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

int Aios::recvCVPData(
    std::shared_ptr<std::map<std::string, std::vector<double>>> pvcMap) {
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (recvRtData(recv_buf, false) < 0) {
    return FourierStatusFailure;
  }
  std::string buf = recv_buf;
  auto json_obj = nlohmann::json::parse(buf);
  if (json_obj["status"] != "OK") {
    SPDERROR("Recv Data Error !");
    return FourierStatusFailure;
  }
  char p[inet_buf_len_];
  std::string ip =
      inet_ntop(AF_INET, &sockaddr_serv_.sin_addr, p, inet_buf_len_);
  auto pvc = pvcMap->find(ip);
  if (pvc == pvcMap->end()) {
    SPDERROR("recv CVP data!");
    return FourierStatusFailure;
  }
  pvc->second.clear();
  pvc->second.push_back(json_obj["position"]);
  pvc->second.push_back(json_obj["velocity"]);
  pvc->second.push_back(json_obj["current"]);
  return FourierStatusSuccess;
}

FourierStatusCode Aios::encoderIsReady(const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "GET"},
                         {"reqTarget", "/m0/encoder/is_ready"}};
  std::string motor_str = "/m" + to_string(motor_number) + "/encoder/is_ready";
  data["reqTarget"] = motor_str;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  std::string buf = recv_buf;
  auto json_obj = nlohmann::json::parse(buf);
  if (json_obj["status"] != "OK") {
    SPDERROR("Recv Data Error !");
    SPDERROR(buf);
    return FourierStatusFailure;
  }
  try {
    if (!json_obj["property"]) {
      return FourierStatusFailure;
    }
  } catch (std::exception) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Aios::controlMode(ControlMode ctrlMode,
                                    const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "SET"},
                         {"reqTarget", "/m0/controller/config"},
                         {"control_mode", POSITION_CONTROL}};
  std::string motor_str = "/m" + to_string(motor_number) + "/controller/config";
  data["reqTarget"] = motor_str;
  data["control_mode"] = ctrlMode;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Aios::inputMode(InputMode inMode, const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "SET"},
                         {"reqTarget", "/m0/controller/config"},
                         {"input_mode", INPUT_MODE_PASSTHROUGH}};
  std::string motor_str = "/m" + to_string(motor_number) + "/controller/config";
  data["reqTarget"] = motor_str;
  data["input_mode"] = inMode;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Aios::setLinearCount(int linearCount,
                                       const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {
      {"method", "SET"}, {"reqTarget", "/m0/encoder"}, {"set_linear_count", 0}};
  std::string motor_str = "/m" + to_string(motor_number) + "/encoder";
  data["reqTarget"] = motor_str;
  data["set_linear_count"] = linearCount;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Aios::getMotionCtrlConfig(char *recv_buf,
                                            const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "GET"},
                         {"reqTarget", "/m0/controller/config"}};
  std::string motor_str = "/m" + to_string(motor_number) + "/controller/config";
  data["reqTarget"] = motor_str;

  char tmp[recv_buf_len_];
  if (recv_buf == nullptr) {
    recv_buf = tmp;
    sendServData(data.dump());
    return FourierStatusSuccess;
  }
  memset(recv_buf, 0, recv_buf_len_);
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Aios::setMotionCtrlConfig(const nlohmann::json &param,
                                            const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/m0/controller/config"},
      {"pos_gain", 20},
      {"vel_gain", 0.0005},
      {"vel_integrator_gain", 0.0002},
      {"vel_limit", 40000},
      {"vel_limit_tolerance", 1.2},
  };
  std::string motor_str = "/m" + to_string(motor_number) + "/controller/config";
  data["reqTarget"] = motor_str;
  data.update(param);
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Aios::getMotorConfig(char *recv_buf, const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "GET"}, {"reqTarget", "/m1/motor/config"}};
  std::string motor_str = "/m" + to_string(motor_number) + "/motor/config";
  data["reqTarget"] = motor_str;

  char tmp[recv_buf_len_];
  if (recv_buf == nullptr) {
    recv_buf = tmp;
    sendServData(data.dump());
    return FourierStatusSuccess;
  }
  memset(recv_buf, 0, recv_buf_len_);
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setMotorConfig(const nlohmann::json &param,
                                       const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/m1/motor/config"},
      {"current_lim", 15},
      {"current_lim_margin", 5},
      {"inverter_temp_limit_lower", 100},
      {"inverter_temp_limit_upper", 120},
      {"requested_current_range", 30},
      {"current_control_bandwidth", 1000},
  };
  std::string motor_str = "/m" + to_string(motor_number) + "/motor/config";
  data["reqTarget"] = motor_str;
  data.update(param);
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::getTrapTraj(char *recv_buf, const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "GET"}, {"reqTarget", "/m0/trap_traj"}};
  std::string motor_str = "/m" + to_string(motor_number) + "/trap_traj";
  data["reqTarget"] = motor_str;

  char tmp[recv_buf_len_];
  if (recv_buf == nullptr) {
    recv_buf = tmp;
    sendServData(data.dump());
    return FourierStatusSuccess;
  }
  memset(recv_buf, 0, recv_buf_len_);
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  std::string buf = recv_buf;
  auto json_obj = nlohmann::json::parse(buf);
  if (json_obj["status"] == "OK") {
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setTrapTraj(const nlohmann::json &param,
                                    const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "SET"},
                         {"reqTarget", "/m0/trap_traj"},
                         {"accel_limit", 320000},
                         {"decel_limit", 320000},
                         {"vel_limit", 200000}};
  std::string motor_str = "/m" + to_string(motor_number) + "/trap_traj";
  data["reqTarget"] = motor_str;
  data.update(param);
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::velRampEnable(bool enable, const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "SET"},
                         {"reqTarget", "/m0/controller"},
                         {"vel_ramp_enable", false}};
  std::string motor_str = "/m" + to_string(motor_number) + "/controller";
  data["reqTarget"] = motor_str;
  data["vel_ramp_enable"] = enable;
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::velRampTarget(const int target,
                                      const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "SET"},
                         {"reqTarget", "/m0/controller"},
                         {"vel_ramp_target", 0}};
  std::string motor_str = "/m" + to_string(motor_number) + "/controller";
  data["reqTarget"] = motor_str;
  data["vel_ramp_target"] = target;
  std::string json_str = data.dump();
  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }

  // char recv_buf[recv_buf_len_];
  // memset(recv_buf, 0, sizeof(recv_buf));
  // if (runRtSocket(data, recv_buf) !=  FourierStatusSuccess) {
  //     return FourierStatusFailure;
  // }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::trapezoidalMove(const int position, bool reply_enable,
                                        const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {{"method", "SET"},
                         {"reqTarget", "/m0/trapezoidalMove"},
                         {"property", 0},
                         {"reply_enable", true}};
  std::string motor_str = "/m" + to_string(motor_number) + "/trapezoidalMove";
  data["reqTarget"] = motor_str;
  data["property"] = position;
  data["reply_enable"] = reply_enable;
  std::string json_str = data.dump();

  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setPosition(const int pos, const int velocity_ff,
                                    const int current_ff, bool reply_enable,
                                    const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {
      {"method", "SET"},      {"reqTarget", "/m0/setPosition"},
      {"reply_enable", true}, {"position", 0},
      {"velocity_ff", 0},     {"current_ff", 0}};
  std::string motor_str = "/m" + to_string(motor_number) + "/setPosition";
  data["reqTarget"] = motor_str;
  data["reply_enable"] = reply_enable;
  data["position"] = pos;
  data["velocity_ff"] = velocity_ff;
  data["current_ff"] = current_ff;
  std::string json_str = data.dump();

  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setVelocity(const int velocity, const int current_ff,
                                    bool reply_enable, const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/m0/setVelocity"},
      {"velocity", 0},
      {"current_ff", 0},
  };
  std::string motor_str = "/m" + to_string(motor_number) + "/setVelocity";
  data["reqTarget"] = motor_str;
  data["reply_enable"] = reply_enable;
  data["velocity"] = velocity;
  data["current_ff"] = current_ff;
  std::string json_str = data.dump();

  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }

  if (!reply_enable) {
    return FourierStatusSuccess;
  }
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (recvRtData(recv_buf, false) < 0) {
    return FourierStatusFailure;
  }
  printPVC(recv_buf);
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setCurrent(const int current, bool reply_enable,
                                   const int motor_number) {
  if (!isMotorNumberInVec(motor_number)) {
    return FourierStatusFailure;
  };
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/m0/setCurrent"},
      {"current", 0},
  };
  std::string motor_str = "/m" + to_string(motor_number) + "/setCurrent";
  data["reqTarget"] = motor_str;
  data["reply_enable"] = reply_enable;
  data["current"] = current;
  std::string json_str = data.dump();

  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }

  // if (!reply_enable) {
  //     return FourierStatusSuccess;
  // }
  // char recv_buf[recv_buf_len_];
  // memset(recv_buf, 0, sizeof(recv_buf));
  // if (recvRtData(recv_buf, false) < 0) {
  //     return FourierStatusFailure;
  // }
  // printPVC(recv_buf);
  return FourierStatusSuccess;
}

int Aios::dumFunc() {
  nlohmann::json data = {
      {"method", "XET"},
      {"reqTarget", "/"},
  };
  std::string json_str = data.dump();

  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setIOState(const nlohmann::json &param,
                                   bool reply_enable) {
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/IO_State"},
      {"reply_enable", true},
  };
  data["reply_enable"] = reply_enable;
  data.update(param);
  std::string json_str = data.dump();

  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }

  if (!reply_enable) {
    return FourierStatusSuccess;
  }
  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (recvRtData(recv_buf) < 0) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::getIOState() {
  nlohmann::json data = {
      {"method", "GET"},
      {"reqTarget", "/IO_State"},
  };
  std::string json_str = data.dump();

  if (sendRtData(json_str) < 0) {
    return FourierStatusFailure;
  }

  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (recvRtData(recv_buf) < 0) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::getNetworkSetting(char *recv_buf) {
  nlohmann::json data = {
      {"method", "GET"},
      {"reqTarget", "/network_setting"},
  };

  char tmp[recv_buf_len_];
  if (recv_buf == nullptr) {
    recv_buf = tmp;
    sendServData(data.dump());
    return FourierStatusSuccess;
  }
  memset(recv_buf, 0, recv_buf_len_);
  if (runServSocket(data, recv_buf) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }

  return FourierStatusSuccess;
}

FourierStatusCode Aios::setNetworkSetting(const nlohmann::json &param) {
  nlohmann::json data = {
      {"method", "SET"},
      {"reqTarget", "/network_setting"},
      {"DHCP_enable", true},
  };
  data.update(param);
  std::string json_str = data.dump();
  std::regex e1("\"\\[");
  std::regex e2("\\]\"");
  json_str = std::regex_replace(json_str, e1, "[");
  json_str = std::regex_replace(json_str, e2, "]");

  if (sendServData(json_str) < 0) {
    return FourierStatusFailure;
  }

  char recv_buf[recv_buf_len_];
  memset(recv_buf, 0, sizeof(recv_buf));
  if (recvServData(recv_buf) < 0) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

/******* fw2.0 *******/
FourierStatusCode Aios::setInputPositionPt(const PosPtInfoActual &pos_pt_info) {
  unsigned char tx_messages[sizeof(PosPtInfoActual) + 1] = {0};
  tx_messages[0] = 0x0c;
  memcpy((void *)(tx_messages + 1), (void *)&pos_pt_info,
         sizeof(PosPtInfoActual));
  // for (int i = 0; i < sizeof(PosPtInfoActual) + 1; i++)
  //   printf("c1:%x\n", tx_messages[i]);
  if (sendPtData(tx_messages, sizeof(tx_messages)) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setInputPositionPt(
    const PosPtInfoActual &pos_pt_info,
    std::shared_ptr<CvpPtFeedback> cvp_pt_feedback) {
  if (setInputPositionPt(pos_pt_info) != FourierStatusSuccess) {
    return FourierStatusFailure;
  };

  if (recvPtData(cvp_pt_feedback) != FourierStatusSuccess) {
    return FourierStatusFailure;
  };
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setInputVelocityPt(const VelPtInfo &vel_pt_info) {
  unsigned char tx_messages[sizeof(VelPtInfo) + 1] = {0};
  tx_messages[0] = 0x0d;
  memcpy((void *)(tx_messages + 1), (void *)&vel_pt_info, sizeof(VelPtInfo));
  if (sendPtData(tx_messages, sizeof(tx_messages)) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setInputVelocityPt(
    const VelPtInfo &vel_pt_info,
    std::shared_ptr<CvpPtFeedback> cvp_pt_feedback) {
  if (setInputVelocityPt(vel_pt_info) != FourierStatusSuccess) {
    return FourierStatusFailure;
  };

  if (recvPtData(cvp_pt_feedback) != FourierStatusSuccess) {
    return FourierStatusFailure;
  };
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setInputTorquePt(const TorquePtInfo &torque_pt_info) {
  unsigned char tx_messages[sizeof(TorquePtInfo) + 1] = {0};
  tx_messages[0] = 0x0e;
  memcpy((void *)(tx_messages + 1), (void *)&torque_pt_info,
         sizeof(TorquePtInfo));
  if (sendPtData(tx_messages, sizeof(tx_messages)) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::setInputTorquePt(
    const TorquePtInfo &torque_pt_info,
    std::shared_ptr<CvpPtFeedback> cvp_pt_feedback) {
  if (setInputTorquePt(torque_pt_info) != FourierStatusSuccess) {
    return FourierStatusFailure;
  };

  if (recvPtData(cvp_pt_feedback) != FourierStatusSuccess) {
    return FourierStatusFailure;
  };
  return FourierStatusSuccess;
}

FourierStatusCode Aios::getCvpPt() {
  char tx_messages[2] = {0};
  tx_messages[0] = 0x1a;
  if (sendPtData(tx_messages, sizeof(tx_messages)) != FourierStatusSuccess) {
    return FourierStatusFailure;
  }
  return FourierStatusSuccess;
}

FourierStatusCode Aios::getCvpPt(
    std::shared_ptr<CvpPtFeedback> cvp_pt_feedback) {
  if (getCvpPt() != FourierStatusSuccess) {
    return FourierStatusFailure;
  };
  if (recvPtData(cvp_pt_feedback) != FourierStatusSuccess) {
    return FourierStatusFailure;
  };
  return FourierStatusSuccess;
}
/******* fw2.0 *******/

void Delay(const double delayTime) {
  long double uDelayTime = delayTime * 1000000;
  timeval tvLast;
  gettimeofday(&tvLast, NULL);
  while (true) {
    timeval nowtime;
    gettimeofday(&nowtime, NULL);

    if ((nowtime.tv_sec - tvLast.tv_sec) * 1000000 +
            (nowtime.tv_usec - tvLast.tv_usec) >=
        uDelayTime) {
      break;
    }
  }
}

void DelaySelect(const double delayTime) {
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = delayTime * 1000 * 1000;
  int ret = select(0, NULL, NULL, NULL, &tv);
  if (-1 == ret) {
    fprintf(stderr, " select error . errno=%d [%s]/n", errno, strerror(errno));
  }
}

// 秒级延时
int Delay(const double delayTime, const std::shared_ptr<Aios> aios,
          const std::vector<double> &posList) {
  // TODO
  return FourierStatusSuccess;
}

const timeval intervalTimeStart() {
  timeval tv_last;
  gettimeofday(&tv_last, NULL);
  return tv_last;
}

double intervalTimeEnd(const timeval &tv_last, const float &frequency) {
  float interval_time;
  if (frequency == 0) {
    interval_time = 0;
  } else {
    interval_time = 1.0 / frequency;
  }
  long double delay_time = interval_time * 1000000;
  long double time = 0;
  timeval now_time;
  while (true) {
    gettimeofday(&now_time, NULL);
    time = ((now_time.tv_sec - tv_last.tv_sec) * 1000000 +
            (now_time.tv_usec - tv_last.tv_usec));
    if (time >= delay_time) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
  return time / 1000;
}

}  // namespace Fourier

void to_json(nlohmann::json &j, const MotionControllerConfig *cfg) {
  j = nlohmann::json{
      {"pos_gain", cfg->pos_gain},
      {"vel_gain", cfg->vel_gain},
      {"vel_integrator_gain", cfg->vel_integrator_gain},
      {"vel_limit", cfg->vel_limit},
      {"vel_limit_tolerance", cfg->vel_limit_tolerance},
  };
}
void from_json(const nlohmann::json &j, MotionControllerConfig *cfg) {
  j.at("pos_gain").get_to(cfg->pos_gain);
  j.at("vel_gain").get_to(cfg->vel_gain);
  j.at("vel_integrator_gain").get_to(cfg->vel_integrator_gain);
  j.at("vel_limit").get_to(cfg->vel_limit);
  j.at("vel_limit_tolerance").get_to(cfg->vel_limit_tolerance);
}

void to_json(nlohmann::json &j, const MotorConfig *cfg) {
  j = nlohmann::json{
      {"current_lim", cfg->current_lim},
      {"current_lim_margin", cfg->current_lim_margin},
      {"inverter_temp_limit_lower", cfg->inverter_temp_limit_lower},
      {"inverter_temp_limit_upper", cfg->inverter_temp_limit_upper},
      {"requested_current_range", cfg->requested_current_range},
      {"current_control_bandwidth", cfg->current_control_bandwidth},
  };
}
void from_json(const nlohmann::json &j, MotorConfig *cfg) {
  j.at("current_lim").get_to(cfg->current_lim);
  j.at("current_lim_margin").get_to(cfg->current_lim_margin);
  j.at("inverter_temp_limit_lower").get_to(cfg->inverter_temp_limit_lower);
  j.at("inverter_temp_limit_upper").get_to(cfg->inverter_temp_limit_upper);
  j.at("requested_current_range").get_to(cfg->requested_current_range);
  j.at("current_control_bandwidth").get_to(cfg->current_control_bandwidth);
}

void to_json(nlohmann::json &j, const TrapTraj *cfg) {
  j = nlohmann::json{
      {"accel_limit", cfg->accel_limit},
      {"decel_limit", cfg->decel_limit},
      {"vel_limit", cfg->vel_limit},
  };
}
void from_json(const nlohmann::json &j, TrapTraj *cfg) {
  j.at("accel_limit").get_to(cfg->accel_limit);
  j.at("decel_limit").get_to(cfg->decel_limit);
  j.at("vel_limit").get_to(cfg->vel_limit);
}

void to_json(nlohmann::json &j, const NetworkSetting *cfg) {
  j = nlohmann::json{
      {"DHCP_enable", cfg->dhcp_enable},
      {"SSID", cfg->SSID},
      {"password", cfg->password},
      {"name", cfg->name},
      {"staticIP", cfg->staticIP},
      {"gateway", cfg->gateway},
      {"subnet", cfg->subnet},
      {"dns_1", cfg->dns_1},
      {"dns_2", cfg->dns_2},
  };
}

void from_json(const nlohmann::json &j, NetworkSetting *cfg) {
  j.at("DHCP_enable").get_to(cfg->dhcp_enable);
  j.at("SSID").get_to(cfg->SSID);
  j.at("password").get_to(cfg->password);
  j.at("name").get_to(cfg->name);
  j.at("staticIP").get_to(cfg->staticIP);
  j.at("gateway").get_to(cfg->gateway);
  j.at("subnet").get_to(cfg->subnet);
  j.at("dns_1").get_to(cfg->dns_1);
  j.at("dns_2").get_to(cfg->dns_2);
}

FourierStatusCode fourierSetLogLevel(const char *mode) {
  if (strcmp(mode, "DEBUG") == 0) {
    mylog.setLevel(spdlog::level::level_enum::debug);
  } else if (strcmp(mode, "INFO") == 0) {
    mylog.setLevel(spdlog::level::level_enum::info);
  } else if (strcmp(mode, "WARN") == 0) {
    mylog.setLevel(spdlog::level::level_enum::warn);
  } else if (strcmp(mode, "ERROR") == 0) {
    mylog.setLevel(spdlog::level::level_enum::err);
  } else {
    SPDERROR("The log level is unknown.");
  }

  return FourierStatusSuccess;
}
