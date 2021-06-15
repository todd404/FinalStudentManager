#pragma once
#include "sqlite3.h"
#include "char_conversion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_FILE "database.db"

typedef int (*sqlite3_callback)(void*, int, char**, char**);

typedef struct {
	sqlite3* db;
} Sql;

void OpenDB(Sql* sql);
void gtou(char* dest, int len, char* src);
void utog(char* dest, int len, char* src);