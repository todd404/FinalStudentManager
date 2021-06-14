#include "InputFunction.h"
#pragma once

void GetInput(char* dest, int maxLength, int isCover, TestFunc test)
{
	char c; int i = 0;
	for (i = 0; i < maxLength && ((c = getch()) != '\r');)
	{
		if (c != '\b') {
			if (!test(c))
				continue;

			if (isCover)	printf("*");
			else			putchar(c);

			dest[i++] = c;
		}
		else if (i == 0){
			continue;
		}
		else {
			i--;
			printf("\b \b");
			continue;
		}
	}

	if (i == maxLength) {
		while (getch() != '\r');
	}

	printf("\n");
	dest[i] = '\0';
}

void PasswordInput(char* dest)
{
	GetInput(dest, MAX_PASSWORD, COVER, AllAc);
}

void UserNameInput(char* dest)
{
	GetInput(dest, MAX_USER_NAME, NO_COVER, UserName);
}

int AllAc(char c)
{
	return 1;
}

int Number(char c)
{
	return ('0' <= c && '9' >= c);
}

int Capital(char c)
{
	return ('A' <= c && 'Z' >= c);
}

int Lowercase(char c)
{
	return ('a' <= c && 'z' >= c);
}

int UserName(char c)
{
	return (Number(c) || Capital(c) || Lowercase(c) || c == '_');
}
