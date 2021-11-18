#pragma once
#include <Windows.h>
#include "LsnBuf.h"

/* ����ͨ����
*
*  ����ʵ���˶Դ��ڵĻ�������
*  �����������ָ�����ڵ����ݡ�����ָ�����ݵ�����
*/
class CSerialPort
{
public:
	CSerialPort(void);
	~CSerialPort(void);

public:
	/* ��ʼ�����ں���
	*
	*  @param:  unsigned int portNo ���ڱ��,Ĭ��ֵΪ1,��COM1,ע��,������Ҫ����9
	*  @param:  unsigned int baud   ������,Ĭ��Ϊ9600
	*  @param:  char parity �Ƿ������żУ��,'Y'��ʾ��Ҫ��żУ��,'N'��ʾ����Ҫ��żУ��
	*  @param:  unsigned int databits ����λ�ĸ���,Ĭ��ֵΪ8������λ
	*  @param:  unsigned int stopsbits ֹͣλʹ�ø�ʽ,Ĭ��ֵΪ1
	*  @param:  unsigned long dwCommEvents Ĭ��ΪEV_RXCHAR,��ֻҪ�շ�����һ���ַ�,�����һ���¼�
	*  @return: bool  ��ʼ���Ƿ�ɹ�
	*  @note:   ��ʹ�����������ṩ�ĺ���ǰ,���ȵ��ñ��������д��ڵĳ�ʼ��
	*����������   /n�������ṩ��һЩ���õĴ��ڲ�������,����Ҫ����������ϸ��DCB����,��ʹ�����غ���
	*           /n������������ʱ���Զ��رմ���,�������ִ�йرմ���
	*  @see:
	*/
	bool InitPort(unsigned int portNo = 0, unsigned int baud = CBR_115200, char parity = 'N', unsigned int databits = 8, unsigned int stopsbits = 1, unsigned long dwCommEvents = EV_RXCHAR);

	/* ���������߳�
	*
	*  �������߳���ɶԴ������ݵļ���,�������յ������ݴ�ӡ����Ļ���
	*  @return: bool  �����Ƿ�ɹ�
	*  @note:   ���߳��Ѿ����ڿ���״̬ʱ,����flase
	*  @see:
	*/
	bool OpenListenThread();

	/* �رռ����߳�
	*
	*  @return: bool  �����Ƿ�ɹ�
	*  @note:   ���ñ�������,�������ڵ��߳̽��ᱻ�ر�
	*  @see:
	*/
	bool CloseListenTread();

	/* �򴮿�д����
	*
	*  ���������е�����д�뵽����
	*  @param:  unsigned char * pData ָ����Ҫд�봮�ڵ����ݻ�����
	*  @param:  unsigned int length ��Ҫд������ݳ���
	*  @return: bool  �����Ƿ�ɹ�
	*  @note:   length��Ҫ����pData��ָ�򻺳����Ĵ�С
	*  @see:
	*/
	bool WriteData(unsigned char* pData, unsigned int length);

	/* ��ȡ���ڻ������е��ֽ���
	*
	*  @return: unsigned int  �����Ƿ�ɹ�
	*  @note:   �����ڻ�������������ʱ,����0
	*  @see:
	*/
	unsigned int GetBytesInCOM();

	/* ��ȡ���ڽ��ջ�������һ���ֽڵ�����
	*
	*  @param:  char & cRecved ��Ŷ�ȡ���ݵ��ַ�����
	*  @return: bool  ��ȡ�Ƿ�ɹ�
	*  @note:
	*  @see:
	*/
	bool ReadChar(char& cRecved);

private:
	/* �򿪴���
	*
	*  @param:  unsigned int portNo �����豸��
	*  @return: bool  ���Ƿ�ɹ�
	*  @note:
	*  @see:
	*/
	bool OpenPort(unsigned int portNo);

	/* �رմ���
	*
	*  @return: void  �����Ƿ�ɹ�
	*  @note:
	*  @see:
	*/
	void ClosePort();

	/* ���ڼ����߳�
	*
	*  �������Դ��ڵ����ݺ���Ϣ
	*  @param:  void * pParam �̲߳���
	*  @return: unsigned int WINAPI �̷߳���ֵ
	*  @note:
	*  @see:
	*/
	static unsigned int WINAPI ListenThread(void* pParam);

private:
	HANDLE m_hComm; /* ���ھ�� */
	static bool s_bExit; /* �߳��˳���־���� */
	volatile HANDLE m_hListenThread; /* �߳̾�� */
	CRITICAL_SECTION m_csCommunicationSync; /* ͬ������,�ٽ������� */
};