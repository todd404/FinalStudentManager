#pragma once
#include <stdio.h>

#define COVER 1
#define NO_COVER 0

#define MAX_USER_NAME 512
#define MAX_PASSWORD 512

typedef int (*TestFunc) (char);

void GetInput(char* dest, int length, int isCover, TestFunc test);

void PasswordInput(char* dest);
void UserNameInput(char* dest);
void MenuSectionInput(char* dest);
int GetSection();
void PhoneNumberInput(char* dest);

int AllAc(char c);
int Number(char c);
int Capital(char c);
int Lowercase(char c);
int UserName(char c);
int Date(char c);