#include "AdminFunction.h"
#include "InputFunction.h"
#include "ToolFunction.h"
#include "EditHelper.h"

Sql* SQL;

typedef void (*AdminFunction)();

void SignupNewAccount();
void DeleteAccount();
void CreateCourse();
void AdminEditStudentInfo();
void CourseManager();
void AdminExit() { exit(0); }

AdminFunction ADMIN_FUN_TABLE[6] = { SignupNewAccount, DeleteAccount, AdminEditStudentInfo, CreateCourse, CourseManager, AdminExit};

void PrintAdminMenu()
{
	printf("======= Welcome Administrator =======\n");
	printf("\t1. 注册新用户\n");
	printf("\t2. 删除用户\n");
	printf("\t3. 修改用户\n");
	printf("\t4. 创建课程\n");
	printf("\t5. 教师课程管理\n");
	printf("\t6. 退出\n");
	printf("=====================================\n");
	printf("请输入选项: ");
}

void ShowAdminUi() 
{
	while (1){
		ClearScreen();
		PrintAdminMenu();

		int section = GetSection();
		if (section < 1 || section > 6)
			continue;

		ADMIN_FUN_TABLE[section - 1]();
	}
}

void AdminProcess(long long id, Sql* sql)
{
	SQL = sql;
	ShowAdminUi();
}

//创建课程相关
int IsCourseExists(const char* courseId)
{
	char codeTestSql[512];
	sprintf_s(codeTestSql, 512, "select course_id from t_course where course_id = %s", courseId);
	int r = GetResultCount(SQL, codeTestSql);
	return r;
}

int InsertNewCourse(const char* course_id, const char* course_name, const char* credit)
{
	char insertSql[512], *errmsg;
	sprintf_s(insertSql, 512, "insert into t_course values (%s, \"%s\", %s);",
		course_id, course_name, credit);
	sqlite3_exec(SQL->db, insertSql, NULL, NULL, &errmsg);
	return 0;
}

void CreateCourse()
{
loop:
	ClearScreen();
	printf("======= 创建课程 =======\n");
	printf("课程代码: ");
	char courseId[512];
	GetInput(courseId, 511, NO_COVER, Number);
	if (IsCourseExists(courseId)) {
		MsgBox("课程代码已存在！");
		goto loop;
	}

	printf("课程名称: ");
	char courseName[512];
	gets_s(courseName, 512);
	
	printf("课程学分: ");
	char credit[512];
	GetInput(credit, 2, NO_COVER, Number);
	
	InsertNewCourse(courseId, courseName, credit);
}

//修改学生信息相关
const char* EDIT_COL_TITLE[9] = { "学号", "姓名", "年龄", "性别", "生日", "手机号码", "学院", "专业", "家庭地址" };
char EDIT_COL_VAL[9][512] = { "" };
char EDIT_COL_NAME[9][512] = { "" };

int AdminQueryStudentInfoCallBack(void* stuInfo, int colNum, char** colVal, char** colName)
{
	for (int i = 0; i < colNum; i++) {
		strcpy_s(EDIT_COL_VAL[i], 512, colVal[i]);
		strcpy_s(EDIT_COL_NAME[i], 512, colName[i]);
	}

	return 0;
}

void AdminQueryStudentInfo(long long id)
{
	int result = 0; char* errmsg;
	char s[512] = "";
	sprintf_s(s, 512, "select * from t_student where stu_id = %lld;", id);

	sqlite3_exec(SQL->db, s, AdminQueryStudentInfoCallBack, NULL, &errmsg);
	return;
}

void AdminPrintEditSection()
{
	printf("===============管理员修改信息==============\n");
	for (int i = 1; i <= 9; i++) {
		printf("%d. %s: %s\n", i, EDIT_COL_TITLE[i - 1], EDIT_COL_VAL[i - 1]);
	}
	printf("=========================================\n");
	printf("请选择要修改的信息: ");
}

long long GetEditStuId()
{
loop:
	ClearScreen();
	printf("请输入要修改的学生ID: ");
	char stuId[512];
	GetInput(stuId, 512, NO_COVER, Number);
	if (!IsStudentIdInDB(SQL, stuId)) {
		MsgBox("学号不存在! 请重新输入！");
		goto loop;
	}
	return atoll(stuId);
}

void UpdateStuInfo(long long id, int section, const char* newVal)
{
	const char* sqlBaseInt = "update t_student set %s = %s where stu_id = %lld;";
	const char* sqlBaseStr = "update t_student set %s = \"%s\" where stu_id = %lld;";
	char sql[512];
	if (section == 1 || section == 3 || section == 6) {
		sprintf_s(sql, 512, sqlBaseInt, EDIT_COL_NAME[section - 1], newVal, id);
	}
	else {
		sprintf_s(sql, 512, sqlBaseStr, EDIT_COL_NAME[section - 1], newVal, id);
	}

	sqlite3_exec(SQL->db, sql, NULL, NULL, NULL);
}

void AdminEditStudentInfo()
{
loop:
	ClearScreen();
	long long id = GetEditStuId();
	AdminQueryStudentInfo(id);
	ClearScreen();
	AdminPrintEditSection();
	int section = GetSection();
	if (section == 3) {
		MsgBox("请直接修改生日！");
		goto loop;
	}
	if (section < 1 && section > 9) {
		MsgBox("请输入正确选项！");
		goto loop;
	}

	char newVal[512];
section_loop:
	printf("请输入新的 %s :", EDIT_COL_TITLE[section - 1]);
	switch (section)
	{
	case 1:	//获取id
		GetInput(newVal, 512, NO_COVER, Number); break;
	case 4: //获取性别
		printf("（男或女）");
		gets_s(newVal, 512);
		if (!IsGender(newVal)) {
			MsgBox("请输入性别！");
			goto section_loop;
		}
		break;
	case 5: //获取生日
		printf("(YYYY-MM-DD) ");
		GetInput(newVal, 10, NO_COVER, Date);
		if (!IsDate(newVal)) {
			MsgBox("请输入正确日期！");
			goto section_loop;
		}
		break;
	case 6:
		PhoneNumberInput(newVal);
		if (strlen(newVal) != 11) {
			MsgBox("请输入正确的手机号码？");
			goto section_loop;
		}
		break;
	default:
		gets_s(newVal, 512);
		break;
	}

	UpdateStuInfo(id, section, newVal);
	if (section == 5) {
		char age[4];
		DateToAge(newVal, age);
		UpdateStuInfo(id, 3, age);
	}
		
}

//注册新用户相关
void InsertStuInfoToDB(char stuInfo[9][512]) 
{
	const char* sqlBase = "insert into t_student values (%s, \"%s\", %s, \"%s\", \"%s\", %s, \"%s\", \"%s\", \"%s\")";
	char sql[512];
	sprintf_s(sql, 512, sqlBase,
		stuInfo[0],
		stuInfo[1],
		stuInfo[2],
		stuInfo[3],
		stuInfo[4],
		stuInfo[5],
		stuInfo[6],
		stuInfo[7],
		stuInfo[8]);
	sqlite3_exec(SQL->db, sql, NULL, NULL, NULL);
}

long long InsertNewStuInfo()
{
	ClearScreen();
	printf("====== 请输入学生信息 ======\n");
	char newStuInfo[9][512] = {""};
	for (int i = 0; i < 9; i++) {
		if (i == 2) continue;
		printf("%s: ", EDIT_COL_TITLE[i]);
section_loop:
		switch (i + 1)
		{
		case 1:get_id:	//获取id
			GetInput(newStuInfo[i], 512, NO_COVER, Number);
			if (IsStudentIdInDB(SQL, newStuInfo[i])) {
				MsgBox("该学号已存在！请重新输入！");
				printf("学号: ");
				goto get_id;
			}
			break;
		case 4: //获取性别
			printf("（男或女）");
			gets_s(newStuInfo[i], 512);
			if (!IsGender(newStuInfo[i])) {
				MsgBox("请输入性别！");
				goto section_loop;
			}
			break;
		case 5: //获取生日
			printf("(YYYY-MM-DD) ");
			GetInput(newStuInfo[i], 10, NO_COVER, Date);
			if (!IsDate(newStuInfo[i])) {
				MsgBox("请输入正确日期！");
				goto section_loop;
			}
			break;
		case 6:
			PhoneNumberInput(newStuInfo[i]);
			if (strlen(newStuInfo[i]) != 11) {
				MsgBox("请输入正确的手机号码？");
				goto section_loop;
			}
			break;
		default:
			gets_s(newStuInfo[i], 512);
			break;
		}
	}
	DateToAge(newStuInfo[4], newStuInfo[2]);
	InsertStuInfoToDB(newStuInfo);
	return atoll(newStuInfo[0]);
}

void InsertTeacherInfoToDB(char teacherInfo[2][512]) 
{
	char sql[512];
	sprintf_s(sql, 512, "insert into t_teacher values (%s, \"%s\");", teacherInfo[0], teacherInfo[1]);
	sqlite3_exec(SQL->db, sql, NULL, NULL, NULL);
}

long long InsertNewTeacherInfo() 
{
	ClearScreen();
	printf("====== 请输入教师信息 ======\n");
	char newTeacherInfo[2][512] = { "" };

get_id:
	printf("请输入工号: ");
	GetInput(newTeacherInfo[0], 512, NO_COVER, Number);
	if (IsTeacherIdInDB(SQL, newTeacherInfo[0])) {
		MsgBox("该工号已存在！请重新输入！");
		goto get_id;
	}
	printf("请输入姓名：");
	gets_s(newTeacherInfo[1], 512);

	InsertTeacherInfoToDB(newTeacherInfo);
	return atoll(newTeacherInfo[0]);
}

void SignupNewAccount()
{
	ClearScreen();
	char user_name[512] = "", password[512] = "";
	int user_group;
	printf("====== 注册新用户 ======\n");
	printf("用户名: ");
	UserNameInput(user_name);
	printf("密码: ");
	PasswordInput(password);
get_user_group:
	printf("用户组(1. 学生, 2.教师, 3.管理员): ");
	user_group = GetSection();
	if (user_group < 1 || user_group > 3) {
		MsgBox("请输入正确用户组");
		goto get_user_group;
	}
	long long id = 0;
	if (user_group == 1)
		id = InsertNewStuInfo();
	if (user_group == 2)
		id = InsertNewTeacherInfo();

	char insertId[512];
	if (id == 0)	strcpy_s(insertId, 512, "null");
	else sprintf_s(insertId, 512, "%lld", id);

	char insertGroup[2];
	_itoa_s(user_group, insertGroup, 2, 10);

	char sql[512];
	char passwordMd5[MD5_STR_LEN];
	md5Encode(password, strlen(password), passwordMd5);
	sprintf_s(sql, 512, "insert into t_login values (\"%s\", \"%s\", %s, %s);", user_name, passwordMd5, insertGroup, insertId);
	sqlite3_exec(SQL->db, sql, NULL, NULL, NULL);
}

//删除用户相关
void DeleteAccount()
{
	ClearScreen();
	printf("===== 删除用户 =====\n");
get_user_name:
	printf("请输入用户名: ");
	char user_name[512];
	UserNameInput(user_name);
	if (!IsAccountUserInDB(SQL, user_name)) {
		MsgBox("用户名不存在！");
		goto get_user_name;
	}

	char sql[512];
	sprintf_s(sql, 512, "delete from t_login where user = \"%s\";", user_name);
	sqlite3_exec(SQL->db, sql, NULL, NULL, NULL);
}

//教师课程管理相关
typedef void (*COURSE_MANAGER_FUNCTION)(const char*);

void PrintTeacherCourseSection()
{
	printf("=======================\n");
	printf("1. 查询执教课程\n");
	printf("2. 添加课程\n");
	printf("3. 删除课程\n");
	printf("4. 退出\n");
	printf("=======================\n");
	printf("请输入选项: ");
}

int QueryTeacherCourseCallBack(void* para, int colNum, char** colVal, char** colName)
{
	printf("课程名称: %s\n", colVal[0]);
	printf("课程代码: %s\n", colVal[1]);
	printf("执教教师: %s\n", colVal[2]);
	printf("=======================\n");
	return 0;
}

void QueryTeacherCourse(const char* teacher_id)
{
	ClearScreen();
	printf("=======================\n");
	char sql[512];
	sprintf_s(sql, 512, "select c.course_name, c.course_id, t.teacher_name from t_teacher as t, t_course as c, t_teacher_course as tc where tc.teacher_id = %s and tc.teacher_id = t.teacher_id and tc.course_id = c.course_id;" ,teacher_id);
	sqlite3_exec(SQL->db, sql, QueryTeacherCourseCallBack, NULL, NULL);
	system("pause");
}

void AddCourse(const char* teacher_id)
{
loop:
	ClearScreen();
	printf("=======================\n");
	printf("请输入课程代码: ");

	char course_id[512];
	GetInput(course_id, 512, NO_COVER, Number);
	if (!IsCourseExists(course_id)) {
		MsgBox("课程不存在！");
		goto loop;
	}
	if (IsTeacherHoldCourse(SQL, teacher_id, course_id)) {
		MsgBox("该教师已执教该课程！");
		return;
	}

	char sql[512];
	sprintf_s(sql, 512, "insert into t_teacher_course values (%s, %s);", teacher_id, course_id);
	sqlite3_exec(SQL->db, sql, NULL, NULL, NULL);
}

void DeleteCourse(const char* teacher_id)
{
loop:
	ClearScreen();
	printf("=======================\n");
	printf("请输入课程代码: ");

	char course_id[512];
	GetInput(course_id, 512, NO_COVER, Number);
	if (!IsCourseExists(course_id)) {
		MsgBox("课程不存在！");
		goto loop;
	}
	if (!IsTeacherHoldCourse(SQL, teacher_id, course_id)) {
		MsgBox("该教师未执教该课程！");
		return;
	}

	char sql[512];
	sprintf_s(sql, 512, "delete from t_teacher_course where teacher_id = %s and course_id = %s;", teacher_id, course_id);
	sqlite3_exec(SQL->db, sql, NULL, NULL, NULL);
}

COURSE_MANAGER_FUNCTION COURSE_FUN_TABLE[3] = {QueryTeacherCourse, AddCourse, DeleteCourse};

int GetTeacherNameCallBack(void* teacher_name, int colNum, char** colVal, char** colName)
{
	char* ptr = (char*)teacher_name;
	strcpy_s(ptr, 512, colVal[0]);
}

void GetTeacherName(char* const teacher_id, char* teacher_name)
{
	char sql[512];
	sprintf_s(sql, 512, "select teacher_name from t_teacher where teacher_id = %s", teacher_id);
	sqlite3_exec(SQL->db, sql, GetTeacherNameCallBack, (void*)teacher_name, NULL);
}

void CourseManager()
{
	ClearScreen();
	printf("===== 教师课程管理 =====\n");
	char teacher_id[512];
get_id:
	printf("请输入工号: ");
	GetInput(teacher_id, 512, NO_COVER, Number);
	if (!IsTeacherIdInDB(SQL, teacher_id)) {
		MsgBox("该工号不存在！请重新输入！");
		goto get_id;
	}

get_section:
	ClearScreen();
	char teacher_name[512];
	GetTeacherName(teacher_id, teacher_name);
	printf("工号: %s, 姓名: %s\n", teacher_id, teacher_name);
	PrintTeacherCourseSection();
	int section = GetSection();

	if (section == 4)
		return;
	if (section < 1 || section > 3)
	{
		MsgBox("请输入正确的选项！");
		goto get_section;
	}
	
	COURSE_FUN_TABLE[section - 1](teacher_id);
	goto get_section;
}
