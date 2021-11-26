#include <stdio.h>
#include <iostream>
#include <string>
#include "SerialPort.h"
#include "CtmMpt.h"
#include "LsnBuf.h"

LsnBuf lsn_buf;

CtmMpt::CtmMpt(unsigned int sen_1_port_no, unsigned int sen_2_port_no, unsigned int mot_port_no)
	: sen_1_buf(), sen_2_buf, mot_buf(), sen_1_port(sen_1_buf), sen_2_port(sen_2_buf), mot_port(mot_buf)
{
	bool is_opened = true;
	int i = 0;

	for (i = 0; i < CtmMpt::MOT_NO; i++)
	{
		;
	}

	if ( !this->sen_1_port.InitPort(sen_1_port_no))
	{
		is_opened = false;
		std::cout << "ERR >>> Failed to initialize sensor group 1 port." << std::endl;
	}
	if (!this->sen_1_port.OpenListenThread())
	{
		is_opened = false;
		std::cout << "ERR >>> Failed to open sensor group 1 listen thread." << std::endl;
	}

	if (!this->sen_2_port.InitPort(sen_2_port_no))
	{
		is_opened = false;
		std::cout << "ERR >>> Failed to initialize sensor group 2 port." << std::endl;
	}
	if (!this->sen_2_port.OpenListenThread())
	{
		is_opened = false;
		std::cout << "ERR >>> Failed to open sensor group 2 listen thread." << std::endl;
	}

	if (!this->mot_port.InitPort(mot_port_no))
	{
		is_opened = false;
		std::cout << "ERR >>> Failed to initialize motor port." << std::endl;
	}
	if (!this->mot_port.OpenListenThread())
	{
		is_opened = false;
		std::cout << "ERR >>> Failed to open motor listen thread." << std::endl;
	}

	if (is_opened)
	{
		std::cout << ">>> Opened." << std::endl;
	}

	return;
}

CtmMpt::~CtmMpt()
{
	return;
}

bool CtmMpt::MotInit(int id)
{
	bool output = true;
	unsigned char cmd_reso[] = { 0x00, 0x10, 0x03, 0x80, 0x00, 0x04, 0x08,
									0x00, 0x00, 0x00, 0x01, // Electronic gear A = 1.
									0x00, 0x00, 0x00, 0x01 }; // Electronic gear B = 1, resolution = 30,000Hz.
	unsigned char cmd_rst_paras[] = { 0x00, 0x10, 0x02, 0xb0, 0x00, 0x06, 0x0c,
										0x00, 0x00, 0x1f, 0x40, // Operation speed = 8000Hz.
										0x00, 0x00, 0x07, 0xd0, // Acceleration = 2000Hz.
										0x00, 0x00, 0x00, 0x00 }; // Starting speed = 0Hz.

	lsn_buf.Clear();

	cmd_reso[0] = (unsigned char)id;
	cmd_rst_paras[0] = (unsigned char)id;

	output &= this->MotWrt(cmd_reso, 15);
	output &= this->MotWrt(cmd_rst_paras, 19);

	output &= this->MotRst(id);

	return output;
}

bool CtmMpt::MotInit(int* id)
{
	return false;
}

bool CtmMpt::MotRst(int id)
{
	bool output = true;
	unsigned char cmd_rst[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x10 };

	cmd_rst[0] = (unsigned char)id;
	output &= this->MotWrt(cmd_rst, 6);

	while (!this->MotHmEnd(id));
	std::cout << ">>> At home." << std::endl;

	return output;
}

bool CtmMpt::MotRst(int* id)
{
	return false;
}

bool CtmMpt::MotPos(int id, int pos, int vel, int k_i, int k_f)
{
	bool output = true;
	int i = 0;
	unsigned char cmd_pos_paras[] = { 0x00, 0x10, 0x18, 0x00, 0x00, 0x0a, 0x14,
								0x00, 0x00, 0x00, 0x02, // Incremental positioning (based on command position).
								0x00, 0x00, 0x00, 0x00, // Index 11-14, position.
								0x00, 0x00, 0x00, 0x00, // Index 15-18, speed.
								0x00, 0x00, 0x00, 0x00, // Index 19-22, starting rate.
								0x00, 0x00, 0x00, 0x00 }; // Index 23-26, stopping deceleration.
	unsigned char cmd_pos_on[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x08 };
	unsigned char cmd_pos_off[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x00 };

	cmd_pos_paras[0] = (unsigned char)id;
	cmd_pos_on[0] = (unsigned char)id;
	cmd_pos_off[0] = (unsigned char)id;

	for (i = 0; i < 4; i++)
	{
		*(cmd_pos_paras + 11 + i) = *((unsigned char*)&pos + 3 - i);
	}
	for (i = 0; i < 4; i++)
	{
		*(cmd_pos_paras + 15 + i) = *((unsigned char*)&vel + 3 - i);
	}
	for (i = 0; i < 4; i++)
	{
		*(cmd_pos_paras + 19 + i) = *((unsigned char*)&k_i + 3 - i);
	}
	for (i = 0; i < 4; i++)
	{
		*(cmd_pos_paras + 23 + i) = *((unsigned char*)&k_f + 3 - i);
	}

	output &= this->MotWrt(cmd_pos_paras, 27);
	output &= this->MotWrt(cmd_pos_on, 6);
	output &= this->MotWrt(cmd_pos_off, 6);

	while (!this->MotInPos(id));
	std::cout << ">>> Arrived." << std::endl;

	return output;
}

bool CtmMpt::MotVel(int id, int vel, float dur, int k_i, int k_f)
{
	bool output = true;
	unsigned char cmd_vel_paras[] = { 0x00, 0x10, 0x18, 0x00, 0x00, 0x0a, 0x14,
										0x00, 0x00, 0x00, 0x10, // Continuous (speed control).
										0x00, 0x00, 0x00, 0x00, // Do not change.
										0x00, 0x00, 0x00, 0x00, // Index 15-18, speed.
										0x00, 0x00, 0x00, 0x00, // Index 19-22, starting rate.
										0x00, 0x00, 0x00, 0x00 }; // Index 23-26, stopping deceleration.
	unsigned char cmd_vel_on[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x08 };
	unsigned char cmd_vel_off[] = { 0x00, 0x06, 0x00, 0x7d, 0x00, 0x20 };
	int i = 0;

	cmd_vel_paras[0] = (unsigned char)id;
	cmd_vel_on[0] = (unsigned char)id;
	cmd_vel_off[0] = (unsigned char)id;

	for (i = 0; i < 4; i++)
	{
		*(cmd_vel_paras + 15 + i) = *((unsigned char*)&vel + 3 - i);
	}
	for (i = 0; i < 4; i++)
	{
		*(cmd_vel_paras + 19 + i) = *((unsigned char*)&k_i + 3 - i);
	}
	for (i = 0; i < 4; i++)
	{
		*(cmd_vel_paras + 23 + i) = *((unsigned char*)&k_f + 3 - i);
	}

	output &= this->MotWrt(cmd_vel_paras, 27);
	output &= this->MotWrt(cmd_vel_on, 6);
	Sleep(dur * 1000);
	output &= this->MotWrt(cmd_vel_off, 6);

	return output;
}

bool CtmMpt::SenInit()
{
	bool output = true;
	std::string cmd_uart_paras("AT+UARTCFG=115200,8,1.00,N");
	std::string cmd_smpf("AT+SMPF=80");

	//41 54 2b 47 4f 44 0d 0a AT+GOD\r\n
	//AA 55 00 1B 00 00 C1 48 F2 C1 71 71 2F C2 9D E1 94 C1 78 A9 6B C1 70 57 E5 C1 9B 8C E3 C1 E7
	//41 54 2b 47 53 44 0d 0a AT+GSD\r\n
	//41 54 2b 47 53 44 3d 53 54 4f 50 0d 0a AT+GSD=STOP\r\n

	output &= this->SenWrt(cmd_uart_paras);
	output &= this->SenWrt(cmd_smpf);

	return output;
}

bool CtmMpt::SenRec()
{
	bool output = true;
	std::string cmd_rec("AT+GOD");

	output &= this->SenWrt(cmd_rec);

	this->SenRecAnalyse();

	return output;
}

bool CtmMpt::MotWrt(const unsigned char* p_buf, const unsigned int len)
{
	bool output = true;
	unsigned char* p_new_buf = new unsigned char[len + 2]; // Create a new array.
	unsigned int i = 0, j = 0;
	unsigned int crc = 0xffff; // CRC16-MODBUS.

	//Add check.
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
	if (p_new_buf[1] != 0x03) // If the command is not to read the motor register.
	{
		std::cout << "Wrt{ ";
		for (i = 0; i < len + 2; i++)
		{
			printf("%02X ", p_new_buf[i]);
		}
		std::cout << "} to motors." << std::endl;
	}
	
	output &= this->mot_port.WriteData(p_new_buf, len + 2);

	if (p_new_buf[1] != 0x03)
	{
		while (lsn_buf.GetLen() != 8);
		lsn_buf.DispMot();
		lsn_buf.Clear();
	}

	// Delete the new array.
	delete[] p_new_buf;

	return output;
}

bool CtmMpt::SenWrt(const std::string& str)
{
	bool output = true;
	int len = str.length();
	unsigned char* cmd = new unsigned char[len + 2]; // Create a new array.
	int i = 0;

	for (i = 0; i < len; i++)
	{
		cmd[i] = (unsigned char)str.at(i);
	}

	// Add "\r\n".
	cmd[len] = 0x0d;
	cmd[len + 1] = 0x0a;

	// Write to sensors.
	std::cout << "Wrt{ ";
	for (i = 0; i < len; i++)
	{
		printf("%c", cmd[i]);
	}
	std::cout << "\\r\\n } to sensors." << std::endl;
	output &= this->sen_1_port.WriteData(cmd, len + 2); // To sensor group 1.
	if (str != "AT+GOD" && str != "AT+GSD")
	{
		while (lsn_buf.GetEndChar() != 0x0a);
		lsn_buf.DispSen(1);
		lsn_buf.Clear();
	}
	output &= this->sen_2_port.WriteData(cmd, len + 2); // To sensor group 2.
	if (str != "AT+GOD" && str != "AT+GSD")
	{
		while (lsn_buf.GetEndChar() != 0x0a);
		lsn_buf.DispSen(2);
		lsn_buf.Clear();
	}

	// Delete the new array.
	delete[] cmd;

	return output;
}

bool CtmMpt::MotInPos(int id)
{
	bool in_pos = false;
	unsigned char BIT_IN_POS = 0x40;
	unsigned char cmd_rec[] = { 0x00, 0x03, 0x00, 0x7f, 0x00, 0x01 };

	cmd_rec[0] = (unsigned char)id;

	this->MotWrt(cmd_rec, 6);

	while (lsn_buf.GetLen() != 7);
	in_pos = ((lsn_buf.GetChar(3) & BIT_IN_POS) == BIT_IN_POS);
	lsn_buf.Clear();

	return in_pos;
}

bool CtmMpt::MotHmEnd(int id)
{
	bool hm_end = false;
	unsigned char BIT_HM_END = 0x10;
	unsigned char cmd_rec[] = { 0x00, 0x03, 0x00, 0x7f, 0x00, 0x01 };

	cmd_rec[0] = (unsigned char)id;

	this->MotWrt(cmd_rec, 6);

	while (lsn_buf.GetLen() != 7);
	hm_end = ((lsn_buf.GetChar(4) & BIT_HM_END) == BIT_HM_END);
	lsn_buf.Clear();

	return hm_end;
}

bool CtmMpt::MotInPosAll()
{
	return false;
}

bool CtmMpt::MotHmEndAll()
{
	return false;
}

void CtmMpt::SenRecAnalyse()
{
	return;
}
