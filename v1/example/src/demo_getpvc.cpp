#include "Fsa.h"

#include <thread>

using namespace FSA_CONNECT::FSAConfig;

int main() {
    const string     ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;

    fsa.init( ip );

    FSAPIDParams pidparams;
    double       pvc[ 3 ] = {};

    while ( true ) {
        fsa.GetPIDParams( pidparams );
        fsa.GetPVC( pvc[ 0 ], pvc[ 1 ], pvc[ 2 ] );

        std::cout << pvc[ 0 ] << " " << pvc[ 1 ] << " " << pvc[ 2 ] << std::endl;

        this_thread::sleep_for( chrono::microseconds( 1000 ) );
    }

    return 0;
}