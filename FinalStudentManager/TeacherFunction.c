#pragma once

#include "TeacherFunction.h"
#include "Sql.h"
#include "ToolFunction.h"
#include "InputFunction.h"

Sql* SQL;

char TEACHER_INFO[2][512];

void QueryCourse(char* const teacher_id);
void EntryScore(char* const teacher_id);
void PrintScoreTable(char* const teacher_id);

typedef void (*TEACHER_FUNCTION)(char* const);
const TEACHER_FUNCTION TEACHER_FUNCTION_TABLE[3] = {QueryCourse, EntryScore, PrintScoreTable};

int QueryTeacherInfoCallBack(void* para, int colNum, char** colVal, char** colName)
{
	strcpy_s(TEACHER_INFO[0], 512, colVal[0]);
	strcpy_s(TEACHER_INFO[1], 512, colVal[1]);
	return 0;
}

void QueryTeacherInfo(long long id)
{
	char sql[512];
	sprintf_s(sql, 512, "select * from t_teacher where teacher_id = %lld;", id);
	char errmsg[512];
	sqlite3_exec(SQL->db, sql, QueryTeacherInfoCallBack, NULL, &errmsg);
}

void PrintTeacherMenu()
{
	printf("======= Welcome %s(教师) =======\n", TEACHER_INFO[1]);
	printf("\t1. 查询执教课程\n");
	printf("\t2. 录入课程成绩\n");
	printf("\t3. 获取课程成绩表\n");
	printf("\t4. 退出\n");
	printf("请输入选项: ");
}

void TeacherProcess(long long id, Sql* sql)
{
	char teacher_id[512];
loop:
	sprintf_s(teacher_id, 512, "%lld", id);
	if (!IsTeacherIdInDB(sql, teacher_id)) 
	{
		ErrorMsgBox("未查询到账号相关的教师信息，请联系管理员");
		exit(510);
	}
	SQL = sql;
	QueryTeacherInfo(id);

get_section:
	ClearScreen();
	PrintTeacherMenu();
	int section = GetSection();
	if (section < 1 || section > 4) {
		ErrorMsgBox("请输入正确选项！");
		goto get_section;
	}
	if (section == 4)
		return;

	TEACHER_FUNCTION_TABLE[section - 1](teacher_id);
	goto loop;
}

//查询执教课程相关
int QueryCourseCallBack(void* para, int colNum, char** colVal, char** colName)
{
	printf("课程名称: %s\n", colVal[0]);
	printf("课程代码: %s\n", colVal[1]);
	printf("执教教师: %s\n", colVal[2]);
	printf("=======================\n");
	return 0;
}

void QueryCourse(char* const teacher_id)
{
	ClearScreen();
	printf("=======================\n");
	char sql[512];
	sprintf_s(sql, 512, "select c.course_name, c.course_id, t.teacher_name from t_teacher as t, t_course as c, t_teacher_course as tc where tc.teacher_id = %s and tc.teacher_id = t.teacher_id and tc.course_id = c.course_id;", teacher_id);
	sqlite3_exec(SQL->db, sql, QueryCourseCallBack, NULL, NULL);
	system("pause");
}

//录入成绩相关
void EntryScore(char* const teacher_id)
{
	ClearScreen();
	printf("====== 录入成绩 ======\n");
	
get_student_id:
	printf("请输入学生学号: ");
	char student_id[512];
	GetInput(student_id, 512, NO_COVER, Number);
	if (!IsStudentIdInDB(SQL, student_id))
	{
		ErrorMsgBox("该学号不存在！请重新输入！");
		goto get_student_id;
	}

get_course_id:
	printf("请输入课程代码：");
	char course_id[512];
	GetInput(course_id, 512, NO_COVER, Number);
	if (!IsCourseIdInDB(SQL ,course_id))
	{
		ErrorMsgBox("该课程代码不存在！请重新输入！");
		goto get_course_id;
	}
	if (!IsTeacherHoldCourse(SQL, teacher_id, course_id))
	{
		ErrorMsgBox("你未执教该课程！请重新输入！");
		goto get_course_id;
	}

	int res = 0;
	if (IsScoreInDb(SQL, student_id, course_id)) 
	{
		res = YesNoMsgBox("成绩已录入，是否更新？");
	}
	if (res == IDNO)
		return;

	printf("请输入成绩：");
	char score[4];
	GetInput(score, 3, NO_COVER, Number);

	char insertSql[512] = {""};
	if (res == IDYES){
		char sql1[255];
		char sql2[255];
		sprintf_s(sql1, 255, "delete from t_course_score where course_id = %s and stu_id = %s;", course_id, student_id);
		sprintf_s(sql2, 255, "insert into t_course_score values (%s, %s, %s);", course_id, student_id, score);

		strcat_s(insertSql, 512, sql1);
		strcat_s(insertSql, 512, sql2);
	}
	else if (res == 0) {
		sprintf_s(insertSql, 255, "insert into t_course_score values (%s, %s, %s);", course_id, student_id, score);
	}

	sqlite3_exec(SQL->db, insertSql, NULL, NULL, NULL);
}

//查询成绩表相关
int QueryScoreOrderByScoreCallBack(void* counter, int colNum, char** colVal, char** colName)
{
	int* c = (int*)counter;
	char* const COL_TILE[5] = { "学号", "名字", "课程名", "课程分数", "课程学分" };
	if ((*c)++ == 0) {
		for (int i = 0; i < 5; i++) {
			printf("|%-15s|", COL_TILE[i]);
		}
		printf("\n");
		for (int i = 0; i < 85; i++) {
			printf("-");
		}
		printf("\n");
	}

	for (int i = 0; i < 5; i++) {
		printf("|%-15s|", colVal[i]);
	}
	printf("\n");
	for (int i = 0; i < 85; i++) {
		printf("-");
	}
	printf("\n");

	return 0;
}

void QueryScoreOrderByScore(char* const teacher_id, char* const course_id)
{
	ClearScreen();
	char sql[512];
	sprintf_s(sql, 512, "select s.stu_id, s.name, c.course_name, cs.score, c.credit from t_student as s, t_teacher as t, t_course as c, t_course_score as cs, t_teacher_course as tc where t.teacher_id = %s and c.course_id = %s and tc.teacher_id = t.teacher_id and c.course_id = tc.course_id and tc.course_id = cs.course_id and s.stu_id = cs.stu_id order by cs.score desc;", teacher_id, course_id);

	int counter = 0;
	for (int i = 0; i < 85; i++) {
		printf("-");
	}
	printf("\n");
	sqlite3_exec(SQL->db, sql, QueryScoreOrderByScoreCallBack, (void*)&counter, NULL);
	system("pause");
}

int QueryScoreOrderByIdCallBack(void* counter, int colNum, char** colVal, char** colName)
{
	int* c = (int*)counter;
	char* const COL_TILE[5] = {"学号", "名字", "课程名", "课程分数", "课程学分"};
	if ((*c)++ == 0){
		for (int i = 0; i < 5; i++) {
			printf("|%-15s|", COL_TILE[i]);
		}
		printf("\n");
		for (int i = 0; i < 85; i++) {
			printf("-");
		}
		printf("\n");
	}

	for (int i = 0; i < 5; i++) {
		printf("|%-15s|", colVal[i]);
	}
	printf("\n");
	for (int i = 0; i < 85; i++) {
		printf("-");
	}
	printf("\n");

	return 0;
}

void QueryScoreOrderById(char* const teacher_id, char* const course_id)
{
	ClearScreen();
	char sql[512];
	sprintf_s(sql, 512, "select s.stu_id, s.name, c.course_name, cs.score, c.credit from t_student as s, t_teacher as t, t_course as c, t_course_score as cs, t_teacher_course as tc where t.teacher_id = %s and c.course_id = %s and tc.teacher_id = t.teacher_id and c.course_id = tc.course_id and tc.course_id = cs.course_id and s.stu_id = cs.stu_id order by s.stu_id;", teacher_id, course_id);

	int counter = 0;
	for (int i = 0; i < 85; i++) {
		printf("-");
	}
	printf("\n");
	sqlite3_exec(SQL->db, sql, QueryScoreOrderByIdCallBack, (void*)&counter, NULL);
	system("pause");
}

void PrintScoreTable(char* const teacher_id)
{
get_course_id:
	ClearScreen();
	printf("请输入想要查询的课程代码：");
	char course_id[512];
	GetInput(course_id, 512, NO_COVER, Number);
	if (!IsCourseIdInDB(SQL, course_id)) {
		ErrorMsgBox("课程不存在");
		goto get_course_id;
	}
	if (!IsTeacherHoldCourse(SQL, teacher_id, course_id)) {
		ErrorMsgBox("你未执教该课程");
		goto get_course_id;
	}

loop:
	ClearScreen();
	printf("======= 成绩表查询 =======\n");
	printf("\t1. 按学号排序\n");
	printf("\t2. 按成绩排序\n");
	printf("\t3. 退出\n");
	printf("请输入选项: ");

	int section = GetSection();
	if (section == 3)	return;
	if (section < 1 || section > 2) {
		ErrorMsgBox("请输入正确选项！");
		goto loop;
	}

	switch (section)
	{
	case 1:
		QueryScoreOrderById(teacher_id, course_id);
		break;
	case 2:
		QueryScoreOrderByScore(teacher_id, course_id);
		break;
	}
}