#pragma once
#include "StudentFunction.h"
#include "ToolFunction.h"
#include "InputFunction.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
	long long id;
	char name[512];
	int age;
	char gender[7];
	char birthday[11];
	long long phoneNumber;
	char college[512];
	char major[512];
	char address[512];
} StudentInfo;

int QueryStudentInfoCallBack(void* stuInfo, int colNum, char** colVal, char** colName)
{
	StudentInfo* info = (StudentInfo*)stuInfo;
	info->id = atoll(colVal[0]);
	info->age = atoi(colVal[2]);
	info->phoneNumber = atoll(colVal[5]);

	strcpy_s(info->name, 512, colVal[1]);
	strcpy_s(info->gender, 7, colVal[3]);
	strcpy_s(info->birthday, 11, colVal[4]);
	strcpy_s(info->college, 512, colVal[6]);
	strcpy_s(info->major, 512, colVal[7]);
	strcpy_s(info->address, 512, colVal[8]);
}

StudentInfo QueryStudentInfo(long long id, Sql* sql)
{
	int result = 0; char** r; char* errmsg;
	StudentInfo info;
	char s[512] = "";
	sprintf_s(s, 512, "select * from t_student where stu_id = %lld;", id);

	sqlite3_exec(sql->db, s, QueryStudentInfoCallBack, (void*)&info, &errmsg);
	return info;
}

void ShowStudentUi(StudentInfo* info)
{
	printf("======= welcome %s(Student) =======\n", info->name);
	printf("\t1. 查询个人信息\n");
	printf("\t2. 修改个人信息\n");
	printf("\t3. 查询成绩\n");
	printf("===================================\n");
	char section[2] = {""};
	while (section[0] < '1' || section[0] > '3'){
		MenuSectionInput(section);
	}
}

void StudentProcess(long long id, Sql* sql)
{
	StudentInfo s = QueryStudentInfo(id, sql);
	if (s.id == 0) {
		MsgBox("未查询到账户学生信息，请联系管理员");
		exit(511);
	}
	ClearScreen();
	ShowStudentUi(&s);
	return;
}
