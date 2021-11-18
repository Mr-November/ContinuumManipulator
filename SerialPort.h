#pragma once
#include <Windows.h>
#include "LsnBuf.h"

/* 串口通信类
*
*  本类实现了对串口的基本操作
*  例如监听发到指定串口的数据、发送指定数据到串口
*/
class CSerialPort
{
public:
	CSerialPort(void);
	~CSerialPort(void);

public:
	/* 初始化串口函数
	*
	*  @param:  unsigned int portNo 串口编号,默认值为1,即COM1,注意,尽量不要大于9
	*  @param:  unsigned int baud   波特率,默认为9600
	*  @param:  char parity 是否进行奇偶校验,'Y'表示需要奇偶校验,'N'表示不需要奇偶校验
	*  @param:  unsigned int databits 数据位的个数,默认值为8个数据位
	*  @param:  unsigned int stopsbits 停止位使用格式,默认值为1
	*  @param:  unsigned long dwCommEvents 默认为EV_RXCHAR,即只要收发任意一个字符,则产生一个事件
	*  @return: bool  初始化是否成功
	*  @note:   在使用其他本类提供的函数前,请先调用本函数进行串口的初始化
	*　　　　　   /n本函数提供了一些常用的串口参数设置,若需要自行设置详细的DCB参数,可使用重载函数
	*           /n本串口类析构时会自动关闭串口,无需额外执行关闭串口
	*  @see:
	*/
	bool InitPort(unsigned int portNo = 0, unsigned int baud = CBR_115200, char parity = 'N', unsigned int databits = 8, unsigned int stopsbits = 1, unsigned long dwCommEvents = EV_RXCHAR);

	/* 开启监听线程
	*
	*  本监听线程完成对串口数据的监听,并将接收到的数据打印到屏幕输出
	*  @return: bool  操作是否成功
	*  @note:   当线程已经处于开启状态时,返回flase
	*  @see:
	*/
	bool OpenListenThread();

	/* 关闭监听线程
	*
	*  @return: bool  操作是否成功
	*  @note:   调用本函数后,监听串口的线程将会被关闭
	*  @see:
	*/
	bool CloseListenTread();

	/* 向串口写数据
	*
	*  将缓冲区中的数据写入到串口
	*  @param:  unsigned char * pData 指向需要写入串口的数据缓冲区
	*  @param:  unsigned int length 需要写入的数据长度
	*  @return: bool  操作是否成功
	*  @note:   length不要大于pData所指向缓冲区的大小
	*  @see:
	*/
	bool WriteData(unsigned char* pData, unsigned int length);

	/* 获取串口缓冲区中的字节数
	*
	*  @return: unsigned int  操作是否成功
	*  @note:   当串口缓冲区中无数据时,返回0
	*  @see:
	*/
	unsigned int GetBytesInCOM();

	/* 读取串口接收缓冲区中一个字节的数据
	*
	*  @param:  char & cRecved 存放读取数据的字符变量
	*  @return: bool  读取是否成功
	*  @note:
	*  @see:
	*/
	bool ReadChar(char& cRecved);

private:
	/* 打开串口
	*
	*  @param:  unsigned int portNo 串口设备号
	*  @return: bool  打开是否成功
	*  @note:
	*  @see:
	*/
	bool OpenPort(unsigned int portNo);

	/* 关闭串口
	*
	*  @return: void  操作是否成功
	*  @note:
	*  @see:
	*/
	void ClosePort();

	/* 串口监听线程
	*
	*  监听来自串口的数据和信息
	*  @param:  void * pParam 线程参数
	*  @return: unsigned int WINAPI 线程返回值
	*  @note:
	*  @see:
	*/
	static unsigned int WINAPI ListenThread(void* pParam);

private:
	HANDLE m_hComm; /* 串口句柄 */
	static bool s_bExit; /* 线程退出标志变量 */
	volatile HANDLE m_hListenThread; /* 线程句柄 */
	CRITICAL_SECTION m_csCommunicationSync; /* 同步互斥,临界区保护 */
};