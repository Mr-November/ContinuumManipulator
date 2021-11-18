#include <stdio.h>
#include <iostream>
#include "SerialPort.h"
#include "CtmMpt.h"
#include "LsnBuf.h"

LsnBuf lsn_buf;

CtmMpt::CtmMpt(unsigned int sen_1_port_no, unsigned int sen_2_port_no, unsigned int mot_port_no)
{
	if ( !this->sen_1_port.InitPort(sen_1_port_no))
	{
		std::cout << "ERR >>> Failed to initialize sensor group 1 port." << std::endl;
	}
	if (!this->sen_1_port.OpenListenThread())
	{
		std::cout << "ERR >>> Failed to open sensor group 1 listen thread." << std::endl;
	}

	if (!this->sen_2_port.InitPort(sen_2_port_no))
	{
		std::cout << "ERR >>> Failed to initialize sensor group 2 port." << std::endl;
	}
	if (!this->sen_2_port.OpenListenThread())
	{
		std::cout << "ERR >>> Failed to open sensor group 2 listen thread." << std::endl;
	}

	if (!this->mot_port.InitPort(mot_port_no))
	{
		std::cout << "ERR >>> Failed to initialize motor port." << std::endl;
	}
	if (!this->mot_port.OpenListenThread())
	{
		std::cout << "ERR >>> Failed to open motor listen thread." << std::endl;
	}
}

CtmMpt::~CtmMpt()
{
	;
}

bool CtmMpt::MotInit(int id)
{
	bool output = true;
	unsigned char reso[] = { 0x00, 0x10, 0x03, 0x80, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01 };

	reso[0] = (unsigned char)id;
	output &= this->MotWrt(reso, 15);

	return output;
}

bool CtmMpt::MotInit(int* id)
{
	return false;
}

bool CtmMpt::MotRst(int id)
{
	bool output = true;
	unsigned char rst_vel[] = {0x00, 0x10, 0x02, 0xb0, 0x00, 0x02, 0x04, 0x00, 0x00, 0x13, 0x88};
	unsigned char rst[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x10 };

	rst_vel[0] = (unsigned char)id;
	rst[0] = (unsigned char)id;
	output &= this->MotWrt(rst_vel, 11);
	output &= this->MotWrt(rst, 6);

	return output;
}

bool CtmMpt::MotRst(int* id)
{
	return false;
}

bool CtmMpt::MotFwd(int id, bool rvs, int freq, float dur)
{
	bool output = true;
	unsigned char fwd_1[] = { 0x00, 0x10, 0x04, 0x80, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00 };
	unsigned char fwd_2[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x00 };
	unsigned char fwd_3[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x00 };
	int i = 0;

	fwd_1[0] = (unsigned char)id;
	fwd_2[0] = (unsigned char)id;
	fwd_3[0] = (unsigned char)id;

	if (rvs)
	{
		fwd_2[4] = 0x80;
	}
	else
	{
		fwd_2[4] = 0x40;
	}

	for (i = 0; i < 4; i++)
	{
		*(fwd_1 + 7 + i) = *((unsigned char*)&freq + 3 - i);
	}

	output &= this->MotWrt(fwd_1, 11);
	output &= this->MotWrt(fwd_2, 6);
	Sleep(dur * 1000);
	output &= this->MotWrt(fwd_3, 6);

	return output;
}

bool CtmMpt::MotPos(int id, int pos, int vel, int k_i, int k_f)
{
	bool output = true;
	int i = 0;
	unsigned char cmd_pos[] = { 0x00, 0x10, 0x18, 0x00, 0x00, 0x0a, 0x014,
								0x00, 0x00, 0x00, 0x02, // Mode 2.
								0x00, 0x00, 0x00, 0x00, // Index 11-14, position.
								0x00, 0x00, 0x00, 0x00, // Index 15-18, velocity.
								0x00, 0x00, 0x00, 0x00, // Index 19-22, start slope.
								0x00, 0x00, 0x00, 0x00 }; // Index 23-26, stop slope.
	unsigned char cmd_stt_on[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x08 };
	unsigned char cmd_stt_off[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x00 };

	cmd_pos[0] = (unsigned char)id;
	cmd_stt_on[0] = (unsigned char)id;
	cmd_stt_off[0] = (unsigned char)id;

	for (i = 0; i < 4; i++)
	{
		*(cmd_pos + 11 + i) = *((unsigned char*)&pos + 3 - i);
	}
	for (i = 0; i < 4; i++)
	{
		*(cmd_pos + 15 + i) = *((unsigned char*)&vel + 3 - i);
	}
	for (i = 0; i < 4; i++)
	{
		*(cmd_pos + 19 + i) = *((unsigned char*)&k_i + 3 - i);
	}
	for (i = 0; i < 4; i++)
	{
		*(cmd_pos + 23 + i) = *((unsigned char*)&k_f + 3 - i);
	}

	output &= this->MotWrt(cmd_pos, 27);
	output &= this->MotWrt(cmd_stt_on, 6);
	output &= this->MotWrt(cmd_stt_off, 6);

	return output;
}

bool CtmMpt::MotPos(int* id, float* pos)
{
	bool output = true;



	return output;
}

bool CtmMpt::MotVel(int id, float vel)
{
	bool output = true;



	return output;
}

bool CtmMpt::MotVel(int* id, float* vel)
{
	bool output = true;



	return output;
}

bool CtmMpt::SenRec()
{
	bool output = true;
	int i = 0;
	unsigned char cmd[] = { 0x41, 0x54, 0x2b, 0x47, 0x4f, 0x44, 0x0d, 0x0a }; // cmd: AT+GOD\r\n.

	// Write to sensor group 1.
	std::cout << "Wrt{ ";
	for (i = 0; i < 8; i++)
	{
		printf("%02X ", cmd[i]);
	}
	std::cout << "} to sensor group 1." << std::endl;
	output &= this->sen_1_port.WriteData(cmd, 8);

	// Write to sensor group 2.
	std::cout << "Wrt{ ";
	for (i = 0; i < 8; i++)
	{
		printf("%02X ", cmd[i]);
	}
	std::cout << "} to sensor group 2." << std::endl;
	output &= this->sen_2_port.WriteData(cmd, 8);

	return output;
}

bool CtmMpt::MotWrt(unsigned char* p_buf, unsigned int len)
{
	bool output = true;
	unsigned char* p_new_buf = new unsigned char[len + 2]; // Create a new array.
	unsigned int i = 0, j = 0;
	unsigned int crc = 0xffff; // CRC16-MODBUS

	//Adding check
	for (i = 0; i < len; i++)
	{
		p_new_buf[i] = p_buf[i];
	}
	for (i = 0; i < len; i++)
	{
		crc ^= (p_buf[i] & 0x00ff);
		for (j = 0; j < 8; j++)
		{
			if (crc & 0x01)
			{
				crc >>= 1;
				crc ^= 0xa001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	p_new_buf[len] = crc;
	p_new_buf[len + 1] = crc >> 8;

	// Write to motors.
	std::cout << "Wrt{ ";
	for (i = 0; i < len + 2; i++)
	{
		printf("%02X ", p_new_buf[i]);
	}
	std::cout << "} to motors." << std::endl;
	output &= this->mot_port.WriteData(p_new_buf, len + 2);
	Sleep(10);
	lsn_buf.Disp();

	// Delete the new array.
	delete[] p_new_buf;

	return output;
}
