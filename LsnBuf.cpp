#include <mutex>
#include <stdio.h>
#include <iostream>
#include "LsnBuf.h"

std::mutex mtx;

LsnBuf::LsnBuf()
{
	this->p_buf = new unsigned char[512]();
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

void LsnBuf::Disp()
{
	int i = 0;

	mtx.lock();

	std::cout << "Buf{ ";
	for (i = 0; i < this->buf_len; i++)
	{
		printf("%02X ", this->p_buf[i]);
	}
	std::cout << "}." << std::endl;

	mtx.unlock();

	return;
}
