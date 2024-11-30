#include "Fsa.h"

using namespace FSA_CONNECT::FSAConfig;

int main() {
    const string     ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;

    fsa.init( ip );

    constexpr set_pvc_timeout_protect_config_t set_pvc_config{
        // 保护模式使能
        .protect_enable = 1,
        // 多长时间没有再发 pvc 之后触发保护
        .timeout_ms = 100,
        // 设置进入保护状态时使用三环位置控制还是PD控制
        .protect_mode_of_operation = FSA_CONNECT::Status::POSITION_CONTROL,
        // 如果关闭该功能时正在保护状态，是否恢复到进入保护状态前的操作模式, 0: no, 1: yes
        .close_back_last_mode_of_operation = 0,
        // 进入保护模式后是否禁止修改 mode_of_operation, 0: no, 1: yes
        .ignore_set_mode_of_operation = 1,
    };

    ack_ret_t ret = {};

    constexpr uint32_t tx_count = 1;

    fsa.SetPVCTimeoutProtect( tx_count, set_pvc_config, ret );

    std::cout << "Count: " << ret.count << "\t" << "Return: " << ret.res << std::endl;

    fsa.ClearSetPVCTimeoutProtect( tx_count, 0, ret );

    std::cout << "Count: " << ret.count << "\t" << "Return: " << ret.res << std::endl;

    return 0;
}