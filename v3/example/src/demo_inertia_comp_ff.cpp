#include "Fsa.h"

int main()
{
    string ip = "192.168.137.101";

    FSA_CONNECT::FSA fsa1;

    fsa1.init(ip);
    fsa1.SetTorqueLimitMax(1.0);

    sleep(1);

    return 0;
}