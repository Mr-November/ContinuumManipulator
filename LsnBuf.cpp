#include <stdio.h>
#include <iostream>
#include <mutex>
#include "LsnBuf.h"

std::mutex mtx;

LsnBuf::LsnBuf()
{
	this->p_buf = new unsigned char[64]();
	if (this->p_buf == NULL)
	{
		std::cout << "ERR >>> Failed to allocate memory." << std::endl;
	}
	this->buf_len = 0;
}

LsnBuf::~LsnBuf()
{
	delete[] this->p_buf;
}

void LsnBuf::AddChar(unsigned char c)
{
	this->p_buf[this->buf_len++] = c;

	return;
}

unsigned char LsnBuf::GetChar(int idx)
{
	return this->p_buf[idx];
}

unsigned char LsnBuf::GetEndChar()
{
	return this->p_buf[this->buf_len - 1];
}

int LsnBuf::GetLen()
{
	return (this->buf_len);
}

void LsnBuf::Clear()
{
	mtx.lock();

	while (this->buf_len > 0)
	{
		this->p_buf[--this->buf_len] = 0x00;
	}

	mtx.unlock();

	return;
}

void LsnBuf::DispMot()
{
	int i = 0;

	mtx.lock();

	std::cout << "Buf{ ";
	for (i = 0; i < this->buf_len; i++)
	{
		printf("%02X ", this->p_buf[i]);
	}
	std::cout << "} from motors." << std::endl;

	mtx.unlock();

	return;
}

void LsnBuf::DispSen(int group_no)
{
	int i = 0;

	mtx.lock();

	std::cout << "Buf{ ";
	for (i = 0; i < this->buf_len - 2; i++)
	{
		printf("%c", this->p_buf[i]);
	}
	if (group_no == 1)
	{
		std::cout << "\\r\\n } from sensor group 1." << std::endl;
	}
	else if (group_no == 2)
	{
		std::cout << "\\r\\n } from sensor group 2." << std::endl;
	}
	else
	{
		std::cout << "\\r\\n } from which sensor group?" << std::endl;
	}

	mtx.unlock();

	return;
}
