#include "Fsa.h"

#include <thread>

using namespace FSA_CONNECT::FSAConfig;

int main() {
    const string     ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;

    fsa.init( ip );

    FSAPIDParams pidparams{ 0 };
    double       pvc[ 3 ] = {};

    pidparams.control_position_kp = 0.1;
    pidparams.control_velocity_kp = 0.01;
    pidparams.control_velocity_ki = 0;

    fsa.SetPIDParams( pidparams );
    fsa.GetPIDParams( pidparams );

    while ( true ) {
        fsa.GetPVC( pvc[ 0 ], pvc[ 1 ], pvc[ 2 ] );

        std::cout << pvc[ 0 ] << " " << pvc[ 1 ] << " " << pvc[ 2 ] << std::endl;

        this_thread::sleep_for( chrono::microseconds( 1000 ) );
    }

    return 0;
}