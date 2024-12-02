#include "Fsa.h"
#include <thread>

using namespace FSA_CONNECT::FSAConfig;

int main() {
    const string     ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;

    fsa.init( ip );

    pvct_errcode_t pvct_errcode{};

    while ( true ) {
        fsa.GetPVCTError( pvct_errcode );

        std::cout << "Position: " << pvct_errcode.position << "\t" << "Velocity: " << pvct_errcode.velocity << "\t" << "Current: " << pvct_errcode.current << "\t" << "Torque: " << pvct_errcode.torque
                  << std::endl;

        // 第二个 err 中的 1 或者 2 不代表发生错误，而代表执行器内置传感器型号，1 代表磁编码器，2 代表线性霍尔
        for ( const auto err : pvct_errcode.error_code ) {
            std::cout << err << "\t";
        }

        std::cout << std::endl;

        this_thread::sleep_for( chrono::microseconds( 1000 ) );
    }

    return 0;
}

/*
# ErrorCode

--------------------- vector.at(0) ---------------------

uint32_t adccalfault : 1; // ADC校准错误
uint32_t CanComTimeOut : 1; // CAN通信超时错误
uint32_t OverCurrnt : 1; // 过流
uint32_t OverVbus : 1; // 过压

uint32_t UnderVbus : 1; // 欠压
uint32_t OverTempTrip : 1; // 芯片过温错误
uint32_t OverTempMos : 1; // MOS过温错误
uint32_t OverPhaseACurrent : 1; // A相过流

uint32_t OverPhaseBCurrent : 1; // B相过流
uint32_t OverPhaseCCurrent : 1; // C相过流
uint32_t OverHardPhaseCurrent : 1; // 硬件过流保护
uint32_t OPDFault : 1; // 缺相

uint32_t EncoderNotCali : 1; // 电角度磁编码未校准
uint32_t EncoderLoss : 1; // 电角度磁编码丢失
uint32_t FlashErr : 1; // Flash错误
uint32_t MotorStall : 1; // 转速异常

uint32_t PositionErr : 1; // 位置保护错误
uint32_t EncoderReversal : 1; // 编码器反转
uint32_t MotorTypeNULL : 1; // 电机类型为空
uint32_t HardwareTypeNULL : 1; // 硬件类型为空

uint32_t EncoderOthersErr : 1; // 其它编码器错误
uint32_t OverTempCoil : 1; // 绕组过温
uint32_t AdcDmaInitErr : 1; // AdcDma初始化错误
uint32_t DoubleEncoderErr : 1;

uint32_t LinerHallNotCali : 1; // 线性霍尔未校准
uint32_t LinerHallReversal : 1; // 线性霍尔反转
uint32_t LinerHallLoss : 1; // 线性霍尔丢失
uint32_t LinerHallWaveformErr : 1; // 线性霍尔波形错误

uint32_t UPhaseloss : 1;
uint32_t VPhaseloss : 1;
uint32_t WPhaseloss : 1;
uint32_t EthComTimeOut : 1;

--------------------- vector.at(0) ---------------------


--------------------- vector.at(1) ---------------------

传感器类型码：2 bit
传感器错误码：30 bit

0x01：磁编码器（传感器类型）(2 bit)
NotCail：磁编码器未校准 (1 bit)
CalibrationErr：磁编码器校准错误 (1 bit)

CommunicationErr：磁编码器通信错误 (1 bit)
PortNull：磁编码器空指针错误 (1 bit)
AmplitudeErr：磁编码器幅值错误 (1 bit)
AngleFullErr：磁编码器最大值错误 (1 bit)

ReversalErr：磁编码器反转错误 (1 bit)
OverMaxPP：磁编码器配置极对数超上限 (1 bit)
EccentricErr：磁编码器偏心校准数据异常 (1 bit)

保留 6 位 (6 bit)

InnerRotorNear：金刚编码器内圈转子过近 (1 bit)
InnerRotorFar：金刚编码器内圈转子过远 (1 bit)
OuterRotorNear：金刚编码器外圈转子过近 (1 bit)
OuterRotorFar：金刚编码器外圈转子过远 (1 bit)

保留 11 位 (11 bit)

------

0x02：线性霍尔（传感器类型）(2 bit)
NotCail：线性霍尔未校准 (1 bit)
AmpErr：线性霍尔最终校准值幅值错误 (1 bit)

MedErr：线性霍尔中值错误 (1 bit)
MinErr：线性霍尔最小值错误 (1 bit)
MaxErr：线性霍尔最大值错误 (1 bit)
AngleOffetErr：线性霍尔电角度偏置错误 (1 bit)

ReversalErr：线性霍尔反转错误 (1 bit)
PortNull：线性霍尔空指针错误 (1 bit)
AmpArrayError：线性霍尔某一个电周期幅值错误 (1 bit)
AngleOffsetAvegErr：线性霍尔校准值与平均校准值偏差过大 (1 bit)

MedAvegErr：线性霍尔中值与平均中值偏差过大 (1 bit)
PhaseDeltaErr：线性霍尔相位差错误 (1 bit)

保留 3 位 (3 bit)

保留 15 位 (15 bit)

--------------------- vector.at(1) ---------------------

--------------------- vector.at(2) ---------------------

其他错误码：32 bit (保留)

--------------------- vector.at(2) ---------------------
 */