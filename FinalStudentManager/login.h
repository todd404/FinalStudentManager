#pragma once
#include "ToolFunction.h"
#include "sql.h"
#include "InputFunction.h"

typedef enum Group { null, student, teacher, admin } UserGroup;

void LoginProccess(Sql* sql);