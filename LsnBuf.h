#pragma once
#include <stdio.h>
#include <iostream>
#include <string>

class LsnBuf
{
public:
	LsnBuf();
	~LsnBuf();

public:
	void AddChar(unsigned char c);
	unsigned char GetChar(int idx);
	unsigned char GetEndChar();
	int GetLen();
	void Clear();
	void DispMot();
	void DispSen(int group_no);
	void Copy(unsigned char* dst, int begin, int len);
	void Paste(unsigned char* src, int begin, int len);

private:
	unsigned char* p_buf;
	int buf_len;
};