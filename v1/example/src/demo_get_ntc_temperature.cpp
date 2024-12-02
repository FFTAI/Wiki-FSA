#include "Fsa.h"
#include <iostream>

int main( int argc, const char** argv ) {
    string           ip = "192.168.137.101";
    FSA_CONNECT::FSA fsa;

    float mos_temperature      = 0;
    float armature_temperature = 0;

    fsa.init( ip );

    while ( true ) {
        fsa.GetNtcTemperature( mos_temperature, armature_temperature );

        std::cout << "mos_temperature: " << mos_temperature << "\t"
                  << "armature_tempature: " << armature_temperature << std::endl;

        sleep( 1 );
    }

    return 0;
}