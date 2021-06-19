#include "sql.h"
#pragma once

void OpenDB(Sql* sql)
{
	int rc = sqlite3_open(DB_FILE, &sql->db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(sql->db));
		exit(510);
	}
}

int GetResultCount(Sql* sql, const char* s)
{
	char * errmsg, ** result; int rowNum = 0, colNum = 0;
	int r = sqlite3_get_table(sql->db, s, &result, &rowNum, &colNum, &errmsg);
	return rowNum;
}

int IsStudentIdInDB(Sql* sql, const char* id)
{
	char testSql[512];
	sprintf_s(testSql, 512, "select stu_id from t_student where stu_id = %s", id);
	return GetResultCount(sql, testSql);
}

int IsTeacherIdInDB(Sql* sql, const char* id)
{
	char testSql[512];
	sprintf_s(testSql, 512, "select teacher_id from t_teacher where teacher_id = %s", id);
	return GetResultCount(sql, testSql);
}

int IsAccountUserInDB(Sql* sql, const char* user)
{
	char testSql[512];
	sprintf_s(testSql, 512, "select user from t_login where user = \"%s\";", user);
	return GetResultCount(sql, testSql);
}

int IsTeacherHoldCourse(Sql* sql, const char* teacher_id, const char* course_id)
{
	char testSql[512];
	sprintf_s(testSql, 512, "select t.teacher_name, c.course_name from t_teacher as t, t_course as c, t_teacher_course as tc where tc.teacher_id = %s and tc.course_id = %s and tc.teacher_id = t.teacher_id and tc.course_id = c.course_id;", teacher_id, course_id);
	return GetResultCount(sql, testSql);
}

int IsCourseIdInDB(Sql* sql, const char* courseId)
{
	char codeTestSql[512];
	sprintf_s(codeTestSql, 512, "select course_id from t_course where course_id = %s", courseId);
	return GetResultCount(sql, codeTestSql);
}

int IsScoreInDb(Sql* sql, char* const student_id, char* const course_id)
{
	char testSql[512];
	sprintf_s(testSql, 512, "select course_id, stu_id, score from t_course_score where stu_id = %s and course_id = %s;", student_id, course_id);
	return GetResultCount(sql, testSql);
}
