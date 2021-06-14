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
