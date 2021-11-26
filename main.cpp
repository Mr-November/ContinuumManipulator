#include <stdio.h>
#include <iostream>
#include <string>
#include "SerialPort.h"
#include "CtmMpt.h"

int main(int argc, char* argv[])
{
    //if (1)
    //{
    //    myport.WriteData(sensor, 11);
    //    Sleep(5000);
    //}

    CtmMpt m(9, 8, 5);
    int i = 0, j = 1;

    //for (i = 1; i < 10; i++) { m.MotInit(i); }
    //m.MotInit(j);
    //m.MotPos(j, -2400000, 50000, 30000, 30000);
    //m.MotRst(j);
    //m.MotVel(j, -50000, 5, 30000, 30000);
    //for (i = 1; i < 10; i++)
    //{
    //    m.MotPos(i, -60000, 15000, 5000, 5000);
    //    m.MotVel(i, -20000, 2, 8000, 8000);
    //}

    //const char* str = "AT+UARTCFG=115200,8,1.00,N\r\n";
    //std::cout << str;
    //unsigned char cmd[64] = { 0x00 };
    //for (i = 0; i < 30; i++)
    //{
    //    cmd[i] = *(str + i);
    //}
    //for (i = 0; i < 64; i++)
    //{
    //    printf("0x%02x, ", cmd[i]);
    //}

    m.SenInit();
    m.SenRec();

    return 0;
}