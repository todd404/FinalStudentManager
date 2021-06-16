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
