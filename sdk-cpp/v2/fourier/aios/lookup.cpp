#include "actuator.h"
// #include "log.h"

using namespace std;

// 对比依据
// 1.大小不同，则不同；
// 2.大小相同，遍历map1，查找map2中是否有相同Key，没有则不同；
// 3.有相同键，比较Value，Value不同则不同。
template <typename K, typename V>
bool CompareMap(const std::map<K, V> &map1, const std::map<K, V> &map2) {
  if (map1.size() != map2.size()) {
    DEBUG("unequal error: map1.size(): " + map1.size());
    DEBUG("unequal error: map2.size(): " + map2.size());
    return false;
  }

  for (auto it1 : map1) {
    auto it2 = map2.find(it1.first);
    if (it2 == map2.end()) {
      DEBUG("unequal error: left ip: " + it1.first);
      DEBUG("unequal error: right not found.");
      return false;
    } else {
      std::string strLeft = it1.second;
      std::string strRight = it2->second;
      if (strLeft.substr(strLeft.find("serial_number\":\""), 12) !=
          strRight.substr(strRight.find("serial_number\":\""), 12)) {
        return false;
      }
    }
  }

  return true;
}

void broadcast(FourierLookupPtr lookupPtr) {
  float freq = 0.0f;
  strMapPtr tmp = std::make_shared<std::map<std::string, std::string>>();
  while (!lookupPtr->exitLookupThreadFlag) {
    freq = Fourier::Broadcast::frequency_;
    // 控制精度要求不高，使用延时代替，降低cpu使用率
    if (freq == 0) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    // StartTimeChrono(testAll)

    // auto tm = intervalTimeStart();
    lookupPtr->boradcastVecMu.lock();
    lookupPtr->readWriteMu.lock();
    if (freq != 0) tmp->clear();
    for (const auto &boradcast : lookupPtr->vBroadcast) {
      if (freq == 0) {
        break;
      }
      // StartTimeChrono(broadcast_func);
      std::cout << "boradcast->broadcast_func" << std::endl;
      boradcast->broadcast_func(tmp);
      // EndTimeChrono(broadcast_func);
    }
    std::cout << "broadcast end" << std::endl;
    if (!CompareMap(*tmp, *lookupPtr->ipAndActuatorInfo)) {
      DEBUG("tmp->size: " + std::to_string(tmp->size()));
      DEBUG("lookupPtr->ipAndActuatorInfo->size: " +
            std::to_string(lookupPtr->ipAndActuatorInfo->size()));
      lookupPtr->ipAndActuatorInfo->clear();
      for (auto elem : *tmp) {
        lookupPtr->ipAndActuatorInfo->insert(elem);
      }
      // 将flag置为true，使下次执行器获取反馈时增加Motion controller parameters
      // 及Motor Config parameters
    }

    lookupPtr->boradcastVecMu.unlock();
    lookupPtr->readWriteMu.unlock();

    std::this_thread::sleep_for(
        std::chrono::milliseconds(int((1.0 / freq) * 1000)));
    // EndTimeChrono(testAll);
  }
}

void constructionBroadcast(FourierLookupPtr lookup, const char *addr,
                           const size_t addr_len) {
  std::string ip = "";
  std::vector<std::string> vIp;
  if (addr == nullptr || addr_len == 0) {
    DEBUG("======= Use the default IP for broadcasting.");
  } else {
#ifdef WIN32
    char *tmp = new char[addr_len + 1];
#else
    char tmp[addr_len];
#endif  // WIN32

    strncpy(tmp, addr, addr_len + 1);
    const char *delim = ",";
    char *p = strtok(tmp, delim);
    while (p) {
      ip = p;
      // 删除字符串中空格
      auto itor = remove_if(ip.begin(), ip.end(), ::isspace);
      ip.erase(itor, ip.end());

      // 删除ip中无用的0，例：删除前010.010.010.255，删除后10.10.10.255
      int i = 0;
      for (; i < 2; ++i) {
        if (ip[i] != '0') {
          break;
        }
      }
      ip = ip.substr(i);
      ip = std::regex_replace(ip, std::regex("(\\.00)|(\\.0)"), ".");

      vIp.emplace_back(ip);
      p = strtok(NULL, delim);
    }
#ifdef WIN32
    delete[] tmp;
#endif  // WIN32
  }

  std::lock_guard<std::mutex> lock_guard(lookup->boradcastVecMu);
  lookup->vBroadcast.clear();
  for (auto elem : vIp) {
    DEBUG("Broadcast: " + elem);
    lookup->vBroadcast.push_back(std::make_unique<Fourier::Broadcast_>(elem));
  }
  for (const auto &boradcast : lookup->vBroadcast) {
    boradcast->constructionSocket();
  }
}

FourierLookupPtr fourierLookupCreate(const char *addr, size_t addr_len) {
  FourierLookupPtr lookupPtr = new FourierLookup_();

  constructionBroadcast(lookupPtr, addr, addr_len);

  lookupPtr->ipAndActuatorInfo =
      std::make_shared<std::map<std::string, std::string>>();

  std::thread t(broadcast, lookupPtr);
  t.detach();

  return lookupPtr;
}

void fourierLookupRelease(FourierLookupPtr lookup) {
  lookup->exitLookupThreadFlag = true;

  // 等待lookup线程退出
  std::this_thread::sleep_for(std::chrono::milliseconds(
      (int((1.0 / Fourier::Broadcast::frequency_) * 2) * 1000)));
  delete lookup;
}

void fourierLookupSetNetworks(FourierLookupPtr lookup, const char *addr) {
  constructionBroadcast(lookup, addr, strlen(addr) + 1);
}

std::string fourierLookupGetCtrlBoxIP(FourierLookupPtr lookup) {
  lookup->readWriteMu.lock();
  auto ipAndActuaorInfo = *(lookup->ipAndActuatorInfo);
  lookup->readWriteMu.unlock();

  int i = 0;
  for (auto elem : ipAndActuaorInfo) {
    // CtrlBox控制盒
    if (elem.second.find("deviceType") != std::string::npos) {
      return elem.first;
    }
  }
  return string("");
}

FourierStatusCode fourierLookupSetLookupFrequencyHz(FourierLookupPtr lookup,
                                                    const float frequency) {
  std::lock_guard<std::mutex> lock_guard(lookup->readWriteMu);
  Fourier::Broadcast::frequency_ = frequency;
  return FourierStatusSuccess;
}

const float fourierLookupGetLookupFrequencyHz(FourierLookupPtr lookup) {
  std::lock_guard<std::mutex> lock_guard(lookup->readWriteMu);
  return Fourier::Broadcast::frequency_;
}

FourierStatusCode fourierLookupEntryListGetName(
    FourierLookupEntryListPtr lookup_list, size_t index, char *name,
    size_t *length) {
  if (index >= lookup_list->entryInfo.size()) {
    SPDERROR("Index Out of Bounds.");
    return FourierStatusFailure;
  }
  auto str = lookup_list->entryInfo.at(index).name.c_str();
  *length = strlen(str);
  if (name == nullptr) {
    return FourierStatusSuccess;
  }
  strncpy(name, str, *length);
  return FourierStatusSuccess;
}

FourierStatusCode fourierLookupEntryListGetFamily(
    FourierLookupEntryListPtr lookup_list, size_t index, char *family,
    size_t *length) {
  if (index >= lookup_list->entryInfo.size()) {
    SPDERROR("Index Out of Bounds.");
    return FourierStatusFailure;
  }
  auto str = lookup_list->entryInfo.at(index).family.c_str();
  *length = strlen(str);
  if (family == nullptr) {
    return FourierStatusSuccess;
  }
  strncpy(family, str, *length);
  return FourierStatusSuccess;
}

FourierStatusCode fourierLookupEntryListGetSerialNumber(
    FourierLookupEntryListPtr lookup_list, size_t index, char *serialNumber,
    size_t *length) {
  if (index >= lookup_list->entryInfo.size()) {
    SPDERROR("Index Out of Bounds.");
    return FourierStatusFailure;
  }
  auto str = lookup_list->entryInfo.at(index).serial_number.c_str();
  *length = strlen(str);
  if (serialNumber == nullptr) {
    return FourierStatusSuccess;
  }
  strncpy(serialNumber, str, *length);
  return FourierStatusSuccess;
}

void fourierLookupEntryListRelease(FourierLookupEntryListPtr lookup_list) {
  delete lookup_list;
  lookup_list = nullptr;
}

FourierLookupEntryListPtr fourierCreateLookupEntryList(
    FourierLookupPtr lookup) {
  lookup->lookupEntryListPtr = new FourierLookupEntryList_();

  lookup->readWriteMu.lock();
  auto ipAndActuaorInfo = *(lookup->ipAndActuatorInfo);
  lookup->readWriteMu.unlock();

  int i = 0;
  for (auto elem : ipAndActuaorInfo) {
    EntryInfo info;
    info.ip = elem.first;
    string tmp = "";
    std::size_t pos = elem.second.find("serial_number");
    if (pos != string::npos) {
      // 16表示serial_number起始位置与到值的长度，12表示值的长度
      tmp = elem.second.substr(pos + 16, 12);
    }
    info.serial_number = tmp;

    pos = elem.second.find("mac_address");
    if (pos != string::npos) {
      // 14表示mac_address起始位置与到值的长度，17表示值的长度
      tmp = elem.second.substr(pos + 14, 17);
    }
    info.mac_address = tmp;

    pos = elem.second.find("Fw_version");
    if (pos != string::npos) {
      // 14表示mac_address起始位置与到值的长度，17表示值的长度
      tmp = elem.second.substr(pos + 13, 5);
    }
    info.fw_version = tmp;

    info.name = "Default" + std::to_string(i);
    info.family = "Default";

    // CtrlBox控制盒
    if (elem.second.find("deviceType") != std::string::npos) {
      info.name = "CtrlBox" + std::to_string(i);
      info.family = "CtrlBox";
    }
    // for (auto groupCfg : *(lookup->aios->getCfgInfo())) {
    //     for (auto cfg : groupCfg.second) {
    //         if (cfg.serial_number != info.serial_number) {
    //             continue;
    //         }
    //         info.name = cfg.name;
    //         info.family = groupCfg.first;
    //     }
    // }

    lookup->lookupEntryListPtr->entryInfo.push_back(info);
    ++i;
  }

  // 根据name字段按照升序排序
  // std::sort(lookup->lookupEntryListPtr->entryInfo.begin(),
  //           lookup->lookupEntryListPtr->entryInfo.end());
  return lookup->lookupEntryListPtr;
}

size_t fourierLookupEntryListGetSize(FourierLookupEntryListPtr lookup_list) {
  return lookup_list->entryInfo.size();
}
