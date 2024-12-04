#include "Fsa.h"
#include <iostream>

int main( int argc, const char** argv ) {
    string           ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;

    float vbus = 0;

    fsa.init( ip );

    while ( true ) {
        fsa.GetVBus( vbus );

        std::cout << "VBus: " << vbus << std::endl;

        sleep( 1 );
    }

    return 0;
}