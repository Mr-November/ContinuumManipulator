#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SerialPort.h"
#include "CtmMpt.h"

using namespace std;

int main(int argc, char* argv[])
{
    //CSerialPort myport;
    //if (!myport.InitPort(8, CBR_115200))
    //{
    //    std::cout << "Failed to initialize port." << std::endl;

    //    return -1;
    //}
    //if (!myport.OpenListenThread())
    //{
    //    std::cout << "Failed to open listen thread." << std::endl;

    //    return -1;
    //}

    //unsigned char set_1[] = { 0x01, 0x10, 0x18, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x02 };
    //unsigned char set_2[] = { 0x01, 0x10, 0x18, 0x02, 0x00, 0x02, 0x04, 0x00, 0x00, 0xff, 0xff };
    //unsigned char set_3[] = { 0x01, 0x10, 0x18, 0x04, 0x00, 0x02, 0x04, 0x00, 0x00, 0x07, 0xd0 };
    //unsigned char set_4[] = { 0x01, 0x10, 0x18, 0x06, 0x00, 0x02, 0x04, 0x00, 0x00, 0x05, 0xdc };
    //unsigned char set_5[] = { 0x01, 0x10, 0x18, 0x08, 0x00, 0x02, 0x04, 0x00, 0x00, 0x05, 0xdc };
    //unsigned char exc_1[] = { 0x02, 0x10, 0x00, 0x7c, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x08 };
    //unsigned char exc_2[] = { 0x02, 0x10, 0x00, 0x7c, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00 };

    //unsigned char fwd_1[] = { 0x09, 0x10, 0x04, 0x80, 0x00, 0x02, 0x04, 0x00, 0x00, 0x75, 0x30 };
    //unsigned char fwd_2[] = { 0x09, 0x06, 0x00, 0x7d, 0x40, 0x00 };
    //unsigned char fwd_3[] = { 0x09, 0x06, 0x00, 0x7d, 0x00, 0x00 };

    //unsigned char sensor[] = { 0x41, 0x54, 0x2b, 0x53, 0x4d, 0x50, 0x52, 0x3d, 0x3f, 0x0d, 0x0a };

    ////if (0)
    ////{
    ////    myport.WriteData(set_1, 11);
    ////    myport.WriteData(set_2, 11);
    ////    myport.WriteData(set_3, 11);
    ////    myport.WriteData(set_4, 11);
    ////    myport.WriteData(set_5, 11);
    ////}
    ////else
    ////{
    ////    myport.WriteData(exc_1, 11);
    ////}
    ////Sleep(2000);

    ////if (1)
    ////{
    ////    myport.WriteData(fwd_1, 11);
    ////    Sleep(10);
    ////    myport.WriteData(fwd_2, 6);
    ////    Sleep(2000);
    ////    myport.WriteData(fwd_3, 6);
    ////}

    //if (1)
    //{
    //    myport.WriteData(sensor, 11);
    //    Sleep(5000);
    //}

    CtmMpt m(0, 0, 5);
    //m.SenRec();
    m.MotInit(3);
    //m.MotFwd(3, true, 5000, 1);
    //m.MotPos(3, 7000);
    Sleep(2 * 1000);
    m.MotRst(3);

    Sleep(1 * 1000);

    return 0;
}