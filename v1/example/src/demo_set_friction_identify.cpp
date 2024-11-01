#include "Fsa.h"
#include <iostream>
#include <unistd.h>

int main( int argc, const char** argv ) {
    string           ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;

    fsa.init( ip );
    fsa.SetFrictionIdentifyMode();

    sleep( 1 );

    return 0;
}