#include <process.h>
#include <iostream>
#include <mutex>
#include "SerialPort.h"
#include "LsnBuf.h"

extern LsnBuf lsn_buf;
extern std::mutex mtx;

bool CSerialPort::s_bExit = false;

CSerialPort::CSerialPort() :m_hListenThread(INVALID_HANDLE_VALUE)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;
	InitializeCriticalSection(&m_csCommunicationSync);

	return;
}

CSerialPort::~CSerialPort()
{
	CloseListenTread();
	ClosePort();
	DeleteCriticalSection(&m_csCommunicationSync);

	return;
}

bool CSerialPort::InitPort(unsigned int portNo, unsigned int baud, char parity, unsigned int databits, unsigned int stopsbits, unsigned long dwCommEvents)
{	
	/* ��ʱ����,���ƶ�����ת��Ϊ�ַ�����ʽ,�Թ���DCB�ṹ */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/* ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!OpenPort(portNo))
	{
		return false;
	}

	/* �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/* �Ƿ��д����� */
	bool bIsSuccess = true;

	/* �ڴ˿���������������Ļ�������С,���������,��ϵͳ������Ĭ��ֵ.
	*  �Լ����û�������Сʱ,Ҫע�������Դ�һЩ,���⻺�������
	*/
	/*if (bIsSuccess )
	{
	bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/* ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */
	COMMTIMEOUTS CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;

	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	DCB  dcb;

	if (bIsSuccess)
	{
		// ��ANSI�ַ���ת��ΪUNICODE�ַ���  
		unsigned long dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t* pwText = new wchar_t[dwNum];
		if ( !MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum) )
		{
			bIsSuccess = true;
		}

		/* ��ȡ��ǰ�������ò���,���ҹ��촮��DCB���� */
		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);

		/* ����RTS flow���� */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/* �ͷ��ڴ�ռ� */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/* ʹ��DCB�������ô���״̬ */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}

	/*  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/* �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return (bIsSuccess == true);
}

void CSerialPort::ClosePort()
{
	/* ����д��ڱ��򿪣��ر��� */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}

	return;
}

bool CSerialPort::OpenPort(unsigned int portNo)
{
	/* �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/* �Ѵ��ڵı��ת��Ϊ�豸�� */
	char szPort[50];
	sprintf_s(szPort, "COM%d", portNo);

	/* ��ָ���Ĵ��� */
	m_hComm = CreateFileA(szPort,     /* �豸��,COM1,COM2�� */
		GENERIC_READ | GENERIC_WRITE, /* ����ģʽ,��ͬʱ��д */
		0,                            /* ����ģʽ,0��ʾ������ */
		NULL,                         /* ��ȫ������,һ��ʹ��NULL */
		OPEN_EXISTING,                /* �ò�����ʾ�豸�������,���򴴽�ʧ�� */
		0,
		0);

	/* �����ʧ�ܣ��ͷ���Դ������ */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}

	/* �˳��ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}



bool CSerialPort::OpenListenThread()
{
	/* ����߳��Ƿ��Ѿ������� */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/* �߳��Ѿ����� */
		return false;
	}
	s_bExit = false;

	/* �߳�ID */
	unsigned int threadId;

	/* �����������ݼ����߳� */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}

	/* �����̵߳����ȼ�,������ͨ�߳� */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}

bool CSerialPort::CloseListenTread()
{
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/* ֪ͨ�߳��˳� */
		s_bExit = true;

		/* �ȴ��߳��˳� */
		Sleep(10);

		/* ���߳̾����Ч */
		CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}

	return true;
}

unsigned int CSerialPort::GetBytesInCOM()
{
	unsigned long dwError = 0;  /* ������ */
	COMSTAT  comstat;   /* COMSTAT�ṹ��,��¼ͨ���豸��״̬��Ϣ */
	memset(&comstat, 0, sizeof(COMSTAT));
	unsigned int BytesInQue = 0;

	/* �ڵ���ReadFile��WriteFile֮ǰ,ͨ�������������ǰ�����Ĵ����־ */
	if (ClearCommError(m_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /* ��ȡ�����뻺�����е��ֽ��� */
	}

	return BytesInQue;
}

bool CSerialPort::ReadChar(char& cRecved)
{
	bool  bResult = true;
	unsigned long BytesRead = 0;

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/* �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/* �ӻ�������ȡһ���ֽڵ����� */
	bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);

	if ((!bResult))
	{
		/* ��ȡ������,���Ը��ݸô�����������ԭ�� */
		unsigned long dwError = GetLastError();

		/* ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/* �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return (BytesRead == 1);
}

bool CSerialPort::WriteData(unsigned char* pData, unsigned int length)
{
	bool bResult = true;
	unsigned long BytesToSend = 0;

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/* �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/* �򻺳���д��ָ���������� */
	bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);

	if (!bResult)
	{
		unsigned long dwError = GetLastError();

		/* ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/* �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

unsigned int WINAPI CSerialPort::ListenThread(void* pParam)
{
	/* �õ������ָ�� */
	CSerialPort* pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

	// �߳�ѭ��,��ѯ��ʽ��ȡ��������  
	while (!pSerialPort->s_bExit)
	{
		unsigned int BytesInQue = pSerialPort->GetBytesInCOM();

		/* �������뻺������������ */
		if (BytesInQue == 0)
		{
			continue;
		}

		/* ��ȡ���뻺�����е����ݲ������ʾ */
		char cRecved = 0x00;
		mtx.lock();
		//std::cout << "Rec{ ";
		do
		{
			cRecved = 0x00;
			if (pSerialPort->ReadChar(cRecved))
			{
				lsn_buf.AddChar((unsigned char)cRecved);
				//printf("%02X ", (unsigned char)cRecved);
				continue;
			}
		} while (--BytesInQue);
		//std::cout << "}." << std::endl;
		mtx.unlock();
	}

	return 0;
}
