#pragma once
#include <stdio.h>
#include <iostream>

class LsnBuf
{
public:
	LsnBuf();
	~LsnBuf();

public:
	void AddChar(unsigned char c);
	unsigned char GetChar(int idx);
	int GetLen();
	void Clear();
	void Disp();

private:
	unsigned char* p_buf;
	int buf_len;
};