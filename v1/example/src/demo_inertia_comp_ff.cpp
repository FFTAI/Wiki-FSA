#include "Fsa.h"

int main()
{
    string ip = "192.168.137.101";

    FSA_CONNECT::FSA fsa;

    fsa.init(ip);
    fsa.SetInertiaCompensation(1.0);

    sleep(1);

    return 0;
}