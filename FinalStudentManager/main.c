#include <stdio.h>
#include "ToolFunction.h"
#include "login.h"
#include "md5.h"
#include "sql.h"
#include "InputFunction.h"

int main(int argc, char* argv[])
{
    Sql sql;
    OpenDB(&sql);
    LoginProccess(&sql);
    //char* errmsg;
    //sqlite3_exec(sql.db, "insert into t_student values (20034530115,\"����\",20,\"male\",\"2001-06-14\",13048768440,\"�����ѧԺ\",\"�������ѧ�뼼��\",\"����\");", null, null, &errmsg);
    return 0;
}
