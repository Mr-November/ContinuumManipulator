#pragma once
#include <Windows.h>
#include "LsnBuf.h"

class CSerialPort
{
public:
	CSerialPort();
	~CSerialPort();

public:
	bool InitPort(unsigned int portNo,
					unsigned int baud = CBR_115200,
					char parity = 'N',
					unsigned int databits = 8,
					unsigned int stopsbits = 1,
					unsigned long dwCommEvents = EV_RXCHAR);
	bool OpenListenThread();
	bool CloseListenTread();
	bool WriteData(unsigned char* pData, unsigned int length);
	unsigned int GetBytesInCOM();
	bool ReadChar(char& cRecved);

private:
	bool OpenPort(unsigned int portNo);
	void ClosePort();
	static unsigned int WINAPI ListenThread(void* pParam);

private:
	HANDLE m_hComm; // Serial port handle.
	static bool s_bExit; // Thread exit flag.
	volatile HANDLE m_hListenThread; // Thread handle.
	CRITICAL_SECTION m_csCommunicationSync;
};