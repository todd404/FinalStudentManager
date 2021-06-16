#include "EditHelper.h"
#include <stdio.h>
#include <windows.h>

int IsDate(const char* str)
{
	int year = 0, month = 0, day = 0;
	sscanf_s(str, "%d-%d-%d", &year, &month, &day);
	if(!year || !month || !day)
		return 0;
	SYSTEMTIME time;
	GetLocalTime(&time);
	if (year < 1990 || year > time.wYear) return 0;
	if (month < 1 || month > 12) return 0;
	if (day < 1 || day > 31) return 0;
	return 1;
}

int IsGender(const char* str)
{
	return (!strcmp(str, "ÄÐ") || !strcmp(str, "Å®"));
}

void DateToAge(const char* date, char* age)
{
	if (IsDate(date)) {
		int year = 0, month = 0, day = 0;
		sscanf_s(date, "%d-%d-%d", &year, &month, &day);
		SYSTEMTIME time;
		GetLocalTime(&time);
		int a = 0;
		a = time.wYear - year;
		if (month > time.wMonth) a--;
		if (month == time.wMonth && day > time.wDay)	a--;
		_itoa_s(a, age, 4, 10);
	}
}
