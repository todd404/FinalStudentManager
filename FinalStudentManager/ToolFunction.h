#pragma once
#define MD5_STR_LEN 33

void ClearScreen();
void md5Encode(unsigned char* dest_str, unsigned int dest_len, char* md5_str);
void ClearInputBuf();