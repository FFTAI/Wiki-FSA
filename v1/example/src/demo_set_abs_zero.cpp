#include "Fsa.h"
#include <iostream>

int main( int argc, const char** argv ) {
    string           ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;
    float            abs_pos;

    fsa.init( ip );

    fsa.SetABSZero();

    sleep( 1 );

    // 设置零位后必须重启执行器
    fsa.Reboot();

    fsa.GetABSPosition( abs_pos );

    std::cout << "ABS Position: " << abs_pos << std::endl;

    return 0;
}