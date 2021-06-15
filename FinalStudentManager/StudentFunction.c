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
typedef void (*SectionFunction)(StudentInfo*);

const char* COL_TITLE[9] = {"ѧ��", "����", "����", "�Ա�", "����", "�ֻ�����", "ѧԺ", "רҵ", "��ͥ��ַ"};
char COL_VAL[9][512] = {""};
char COL_NAME[9][512] = { "" };

Sql* SQL;

StudentInfo QueryStudentInfo(long long id);
int GetSection();
void UpdateStudentInfo(int section, const char* newVal, StudentInfo* info);

void ShowStudentUi(StudentInfo* info);

void PrintStudentInfo(StudentInfo* info);
void EditStudentInfo(StudentInfo* info);
void Exit(StudentInfo* info) { exit(0); };

const SectionFunction FUNCTION_TABLE[4] = { PrintStudentInfo, EditStudentInfo, NULL, Exit };

int QueryStudentInfoCallBack(void* stuInfo, int colNum, char** colVal, char** colName)
{
	StudentInfo* info = (StudentInfo*)stuInfo;

	for(int i = 0; i< colNum; i++){
		strcpy_s(COL_VAL[i], 512, colVal[i]);
		strcpy_s(COL_NAME[i], 512, colName[i]);
	}
	
	info->id = atoll(colVal[0]);
	info->age = atoi(colVal[2]);
	info->phoneNumber = atoll(colVal[5]);

	strcpy_s(info->name, 512, colVal[1]);
	strcpy_s(info->gender, 7, colVal[3]);
	strcpy_s(info->birthday, 11, colVal[4]);
	strcpy_s(info->college, 512, colVal[6]);
	strcpy_s(info->major, 512, colVal[7]);
	strcpy_s(info->address, 512, colVal[8]);

	return 0;
}

StudentInfo QueryStudentInfo(long long id)
{
	int result = 0; char* errmsg;
	StudentInfo info;
	char s[512] = "";
	sprintf_s(s, 512, "select * from t_student where stu_id = %lld;", id);

	sqlite3_exec(SQL->db, s, QueryStudentInfoCallBack, (void*)&info, &errmsg);
	return info;
}

void PrintMenu(const char* name)
{
	printf("======= welcome %s(Student) =======\n", name);
	printf("\t1. ��ѯ������Ϣ\n");
	printf("\t2. �޸ĸ�����Ϣ\n");
	printf("\t3. ��ѯ�ɼ�\n");
	printf("\t4. �˳�\n");
	printf("===================================\n");
	printf("������ѡ��: ");
}

int GetSection()
{
	char section[2] = { "" };
	MenuSectionInput(section);
	return atoi(section);
}

void ShowStudentUi(StudentInfo* info)
{
	while (1) {
		ClearScreen();
		PrintMenu(info->name);
		int section = GetSection();

		if (section < 1 || section > 4)
			continue;

		FUNCTION_TABLE[section - 1](info);
	}
}

void PrintStudentInfo(StudentInfo* info) 
{
	ClearScreen();
	for (int i = 0; i < 9; i++) {
		printf("%s: %s\n\n", COL_TITLE[i], COL_VAL[i]);
	}
	system("pause");
}

void PrintEditSection()
{
	printf("===============�޸ĸ�����Ϣ==============\n");
	for (int i = 1; i <= 9; i++) {
		printf("%d. %s: %s\n", i, COL_TITLE[i - 1], COL_VAL[i - 1]);
	}
	printf("========================================\n");
	printf("��ѡ��Ҫ�޸ĵ���Ϣ: ");
}

void EditStudentInfo(StudentInfo* info)
{
loop:
	ClearScreen();
	PrintEditSection();
	int section = GetSection();
	if (section != 6 && section != 9){
		MsgBox("���˻������޸������Ϣ������ϵ����Ա�޸�");
		goto loop;
	}

get_phone_number:
	printf("�������µ� %s :", COL_TITLE[section - 1]);
	char newVal[512];

	if (section == 6) {
		PhoneNumberInput(newVal);
		if (strlen(newVal) != 11) {
			MsgBox("��������ȷ���ֻ����룿");
			goto get_phone_number;
		}
	}
	if (section == 9) {
		gets_s(newVal, 512);
	}

	UpdateStudentInfo(section, newVal, info);
end:
	return;
}

void UpdateStudentInfo(int section, const char* newVal, StudentInfo* info)
{
	char sql[512]; char* errmsg;
	if(section == 6)
		sprintf_s(sql, 512, "update t_student set %s=%s where stu_id=%lld;", COL_NAME[section - 1], newVal, info->id);
	if(section == 9)
		sprintf_s(sql, 512, "update t_student set %s=\"%s\" where stu_id=%lld;", COL_NAME[section - 1], newVal, info->id);

	sqlite3_exec(SQL->db, sql, NULL, NULL, &errmsg);
	*info = QueryStudentInfo(info->id);
}

void StudentProcess(long long id, Sql* sql)
{
	SQL = sql;
	StudentInfo s = QueryStudentInfo(id);
	if (s.id == 0) {
		MsgBox("δ��ѯ���˻�ѧ����Ϣ������ϵ����Ա");
		exit(511);
	}
	ClearScreen();
	ShowStudentUi(&s);
	return;
}