#include "login.h"
#include <stdio.h>
#include <string.h>
#pragma once

typedef struct {
	char user[MAX_USER_NAME];
	char passwordMD5[MD5_STR_LEN];
	UserGroup group;
} LoginInfo;

void LoginInfoInit(LoginInfo* info)
{
	info->group = null;
	strcpy_s(info->user, MAX_USER_NAME, "");
	strcpy_s(info->passwordMD5, MD5_STR_LEN, "");
}

void ShowLoginUi()
{
	printf("========== login ==========\n");
}

void GetUser(LoginInfo* info)
{
	printf("user: ");
	UserNameInput(info->user);
}

void GetPassWord(LoginInfo* info)
{
	printf("password: ");
	char c, password[MAX_PASSWORD] = {""};
	PasswordInput(password);

	char md5[MD5_STR_LEN];
	md5Encode(password, strlen(password), info->passwordMD5);
}

int QueryLoginInfoCallBack(void* loginInfo, int col, char** col_val, char** col_name)
{
	LoginInfo* temp = (LoginInfo*)loginInfo;
	strcpy_s(temp->user, MAX_USER_NAME, col_val[0]);
	strcpy_s(temp->passwordMD5, MD5_STR_LEN, col_val[1]);

	UserGroup g = atoi(col_val[2]);
	temp->group = g;

	return 0;
}

//如果查询到返回真，并且将信息放入第二个参数中
//否则返回假
int QueryLoginInfo(char* user, LoginInfo* info, Sql* sql)
{
	char s[512] = "";
	sprintf_s(s, 512, "select * from login where user = \"%s\";", user);
	char* errmsg = NULL;
	LoginInfoInit(info);

	sqlite3_exec(sql->db, s, QueryLoginInfoCallBack, (void*)info, &errmsg);

	return (!(info->group == null));
}

//验证成功返回用户组
//否则返回0
int confirm(LoginInfo* info, Sql* sql)
{
	LoginInfo temp;
	if (!QueryLoginInfo(info->user, &temp, sql))
		return 0;

	if (!strcmp(info->passwordMD5, temp.passwordMD5))
		return temp.group;
	
	return 0;
}

void LoginProccess(Sql* sql)
{
	LoginInfo info;
	LoginInfoInit(&info);

	int count = 0;
	
	while (!info.group) {
		ClearScreen();
		if (count >= 3) {
			printf("Bye!\n");
			exit(1203);
		}
		if (count > 0)
			printf("Wrong User Name Or PassWord\n");

		ShowLoginUi();
		GetUser(&info);
		GetPassWord(&info);
		info.group = confirm(&info, sql);
		count++;
	}
}
