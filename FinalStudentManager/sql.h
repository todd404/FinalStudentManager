#pragma once
#include "sqlite3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_FILE "database.db"

typedef int (*sqlite3_callback)(void*, int, char**, char**);

typedef struct {
	sqlite3* db;
} Sql;

void OpenDB(Sql* sql);
int GetResultCount(Sql* sql, const char* s);
int IsStudentIdInDB(Sql* sql, const char* id);
int IsTeacherIdInDB(Sql* sql, const char* id);
int IsAccountUserInDB(Sql* sql, const char* user);
int IsTeacherHoldCourse(Sql* sql, const char* teacher_id, const char* course_id);