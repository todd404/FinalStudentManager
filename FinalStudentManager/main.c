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

    return 0;
}
