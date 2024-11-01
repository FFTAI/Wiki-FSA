#include "Fsa.h"

int main() {
    string ip = "192.168.137.101";

    FSA_CONNECT::FSA fsa;

    fsa.init( ip );
    // fsa.SetFrictionCompFlag( 0 );
    fsa.SetFrictionCompFlag( 1 );

    sleep( 1 );

    return 0;
}