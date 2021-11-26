#pragma once
#include <iostream>
#include <string>
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

	bool MotPos(int id, int pos, int vel = 5000, int k_i = 1500, int k_f = 1500);
	bool MotPos(int* id, int* pos, int* vel, int* k_i, int* k_f);

	bool MotVel(int id, int vel, float dur, int k_i = 5000, int k_f = 5000);
	bool MotVel(int* id, int* vel, float* dur, int* k_i, int* k_f);

	bool SenInit();
	bool SenRec();

private:
	CSerialPort sen_1_port;
	CSerialPort sen_2_port;
	CSerialPort mot_port;
	//static const int MOT_NO = 9;
	//int ID[MOT_NO];
	//int POS[MOT_NO];

private:
	bool MotWrt(const unsigned char* p_buf, const unsigned int len);
	bool SenWrt(const std::string& str);

	bool MotInPos(int id);
	bool MotHmEnd(int id);
	bool MotInPosAll();
	bool MotHmEndAll();

	void SenRecAnalyse();
};