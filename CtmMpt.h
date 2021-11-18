#pragma once
#include <iostream>
#include "SerialPort.h"

class CtmMpt
{
public:
	CtmMpt(unsigned int sen_1_port_no, unsigned int sen_2_port_no, unsigned int mot_port_no);
	~CtmMpt();

public:
	bool MotInit(int id);
	bool MotInit(int* id);
	bool MotRst(int id);
	bool MotRst(int* id);

	bool MotFwd(int id, bool rvs, int freq, float dur);

	bool MotPos(int id, int pos, int vel = 5000, int k_i = 1500, int k_f = 1500);
	bool MotPos(int* id, float* pos);

	bool MotVel(int id, float vel);
	bool MotVel(int* id, float* vel);

	bool SenRec();

private:
	CSerialPort sen_1_port;
	CSerialPort sen_2_port;
	CSerialPort mot_port;

private:
	bool MotWrt(unsigned char* p_buf, unsigned int len);
};