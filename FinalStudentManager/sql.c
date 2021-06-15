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

void gtou(char* dest, int len, char* src)
{
	gb2312ToUtf8(dest, strlen(src), src, strlen(src));
}

void utog(char* dest, int len, char* src)
{
	utf8ToGb2312(dest, strlen(src), src, strlen(src));
}
