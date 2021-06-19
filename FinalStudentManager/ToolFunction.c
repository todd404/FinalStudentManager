#include "ToolFunction.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "md5.h"
#pragma once

#define MD5_SIZE 16

void ClearScreen()
{
	system("cls");
}

void md5Encode(unsigned char* dest_str, unsigned int dest_len, char* md5_str)
{
	int i;
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;

	// init md5
	MD5Init(&md5);

	MD5Update(&md5, dest_str, dest_len);

	MD5Final(&md5, md5_value);

	// convert md5 value to md5 string
	for (i = 0; i < MD5_SIZE; i++)
	{
		_snprintf_s(md5_str + i * 2, 2 + 1, MD5_STR_LEN, "%02x", md5_value[i]);
	}

	return 0;
}

void ClearInputBuf()
{
	fflush(stdin);
}

void ErrorMsgBox(char* msg)
{
	MessageBoxA(0, msg, 0, 0);
}

int YesNoMsgBox(char* msg)
{
	return MessageBoxA(NULL, msg, "È·¶¨£¿", MB_YESNO);
}

void SetConsoleSize(int cols, int lines)
{
	char command[512] = "";
	sprintf_s(command, 512, "mode con: cols=%d lines=%d", cols, lines);
	system(command);
}
