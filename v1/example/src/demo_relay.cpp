#include "Fsa.h"

int main( int argc, const char** argv ) {
    string           ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;

    fsa.init( ip );

    fsa.OpenRelay();

    sleep( 5 );

    fsa.CloseRelay();

    return 0;
}