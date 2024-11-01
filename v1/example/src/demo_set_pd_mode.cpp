#include "Fsa.h"
#include "FsaConfig.h"
#include "FsaStatus.h"
#include <iostream>
#include <thread>
#include <unistd.h>

int main( int argc, const char** argv ) {
    string                                  ip = "192.168.137.101";
    FSA_CONNECT::FSA                        fsa;
    FSA_CONNECT::Status::FSAModeOfOperation pd_mode    = FSA_CONNECT::Status::FSAModeOfOperation::PD_CONTROL;
    FSA_CONNECT::FSAConfig::FSAPIDParams    pid_params = { 0 };
    double                                  p, v, c;
    int                                     ret = 0;

    pid_params.control_pd_kp = 50;
    pid_params.control_pd_kd = 2;

    fsa.init( ip );
    fsa.SetPIDParams( pid_params );
    fsa.SetControlMode( pd_mode );

    sleep( 1 );

    do {
        ret = fsa.GetPVC( p, v, c );
        sleep( 1 );
    } while ( ret != FSA_CONNECT::ResultCode::SUCCESS );

    std::cout << "p: " << p << "\t"
              << "v: " << v << "\t"
              << "c: " << c << std::endl;

    fsa.SetPosition( p, 0, 0 );
    fsa.Enable();
    sleep( 1 );

    // 正反运动 20 度
    fsa.SetPosition( p + 20, 0, 0 );
    sleep( 1 );
    fsa.SetPosition( p, 0, 0 );

    sleep( 1 );
    return 0;
}