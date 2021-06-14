#pragma once
#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>

#define DB_FILE "database.db"

typedef int (*sqlite3_callback)(void*, int, char**, char**);

typedef struct {
	sqlite3* db;
} Sql;

void OpenDB(Sql* sql);