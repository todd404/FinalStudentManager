#include "AdminFunction.h"
#include "InputFunction.h"
#include "ToolFunction.h"
#include "EditHelper.h"

Sql* SQL;

typedef void (*AdminFunction)();

void SignupNewAccount();
void CreateCourse();
void AdminEditStudentInfo();
void AdminExit() { exit(0); }

AdminFunction ADMIN_FUN_TABLE[5] = { SignupNewAccount, SignupNewAccount, AdminEditStudentInfo, CreateCourse, AdminExit};

void PrintAdminMenu()
{
	printf("======= Welcome Administrator =======\n");
	printf("\t1. ע�����û�\n");
	printf("\t2. ɾ���û�\n");
	printf("\t3. �޸��û�\n");
	printf("\t4. �����γ�\n");
	printf("\t5. �˳�\n");
	printf("=====================================\n");
	printf("������ѡ��: ");
}

void ShowAdminUi() 
{
	while (1){
		ClearScreen();
		PrintAdminMenu();

		int section = GetSection();
		if (section < 1 || section > 5)
			continue;

		ADMIN_FUN_TABLE[section - 1]();
	}
}

void AdminProcess(long long id, Sql* sql)
{
	SQL = sql;
	ShowAdminUi();
}

//�����γ����
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
	printf("======= �����γ� =======\n");
	printf("�γ̴���: ");
	char courseId[512];
	GetInput(courseId, 511, NO_COVER, Number);
	if (IsCourseExists(courseId)) {
		MsgBox("�γ̴����Ѵ��ڣ�");
		goto loop;
	}

	printf("�γ�����: ");
	char courseName[512];
	gets_s(courseName, 512);
	
	printf("�γ�ѧ��: ");
	char credit[512];
	GetInput(credit, 2, NO_COVER, Number);
	
	InsertNewCourse(courseId, courseName, credit);
}

//�޸�ѧ����Ϣ���
const char* EDIT_COL_TITLE[9] = { "ѧ��", "����", "����", "�Ա�", "����", "�ֻ�����", "ѧԺ", "רҵ", "��ͥ��ַ" };
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
	printf("===============����Ա�޸���Ϣ==============\n");
	for (int i = 1; i <= 9; i++) {
		printf("%d. %s: %s\n", i, EDIT_COL_TITLE[i - 1], EDIT_COL_VAL[i - 1]);
	}
	printf("=========================================\n");
	printf("��ѡ��Ҫ�޸ĵ���Ϣ: ");
}

long long GetEditStuId()
{
loop:
	ClearScreen();
	printf("������Ҫ�޸ĵ�ѧ��ID: ");
	char stuId[512];
	GetInput(stuId, 512, NO_COVER, Number);
	if (!IsStudentIdInDB(SQL, stuId)) {
		MsgBox("ѧ�Ų�����! ���������룡");
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
		MsgBox("��ֱ���޸����գ�");
		goto loop;
	}
	if (section < 1 && section > 9) {
		MsgBox("��������ȷѡ�");
		goto loop;
	}

	char newVal[512];
section_loop:
	printf("�������µ� %s :", EDIT_COL_TITLE[section - 1]);
	switch (section)
	{
	case 1:	//��ȡid
		GetInput(newVal, 512, NO_COVER, Number); break;
	case 4: //��ȡ�Ա�
		printf("���л�Ů��");
		gets_s(newVal, 512);
		if (!IsGender(newVal)) {
			MsgBox("�������Ա�");
			goto section_loop;
		}
		break;
	case 5: //��ȡ����
		printf("(YYYY-MM-DD) ");
		GetInput(newVal, 10, NO_COVER, Date);
		if (!IsDate(newVal)) {
			MsgBox("��������ȷ���ڣ�");
			goto section_loop;
		}
		break;
	case 6:
		PhoneNumberInput(newVal);
		if (strlen(newVal) != 11) {
			MsgBox("��������ȷ���ֻ����룿");
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

//ע�����û����
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
	printf("====== ������ѧ����Ϣ ======\n");
	char newStuInfo[9][512] = {""};
	for (int i = 0; i < 9; i++) {
		if (i == 2) continue;
		printf("%s: ", EDIT_COL_TITLE[i]);
section_loop:
		switch (i + 1)
		{
		case 1:get_id:	//��ȡid
			GetInput(newStuInfo[i], 512, NO_COVER, Number);
			if (IsStudentIdInDB(SQL, newStuInfo[i])) {
				MsgBox("��ѧ���Ѵ��ڣ����������룡");
				printf("ѧ��: ");
				goto get_id;
			}
			break;
		case 4: //��ȡ�Ա�
			printf("���л�Ů��");
			gets_s(newStuInfo[i], 512);
			if (!IsGender(newStuInfo[i])) {
				MsgBox("�������Ա�");
				goto section_loop;
			}
			break;
		case 5: //��ȡ����
			printf("(YYYY-MM-DD) ");
			GetInput(newStuInfo[i], 10, NO_COVER, Date);
			if (!IsDate(newStuInfo[i])) {
				MsgBox("��������ȷ���ڣ�");
				goto section_loop;
			}
			break;
		case 6:
			PhoneNumberInput(newStuInfo[i]);
			if (strlen(newStuInfo[i]) != 11) {
				MsgBox("��������ȷ���ֻ����룿");
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

void InsertTeacherInfoToDB(char teacherInfo[][512]) 
{

}

long long InsertNewTeacherInfo() 
{
	ClearScreen();
	printf("====== �������ʦ��Ϣ ======\n");
	char newTeacherInfo[2][512] = { "" };

get_id:
	printf("�����빤��: ");
	GetInput(newTeacherInfo[0], 512, NO_COVER, Number);
	if (IsTeacherIdInDB(SQL, newTeacherInfo[0])) {
		MsgBox("��ѧ���Ѵ��ڣ����������룡");
		goto get_id;
	}
	printf("������������");
	gets_s(newTeacherInfo[1], 512);

	InsertTeacherInfoToDB(newTeacherInfo);
	return atoll(newTeacherInfo[0]);
}

void SignupNewAccount()
{
	char user_name[512] = "", password[512] = "";
	int user_group;
	printf("====== ע�����û� ======\n");
	printf("�û���: ");
	UserNameInput(user_name);
	printf("����: ");
	PasswordInput(password);
get_user_group:
	printf("�û���(1. ѧ��, 2.��ʦ, 3.����Ա): ");
	user_group = GetSection();
	if (user_group < 1 || user_group > 3) {
		MsgBox("��������ȷ�û���");
		goto get_user_group;
	}
	long long id;
	if (user_group == 1)
		id = InsertNewStuInfo();
	if (user_group == 2)
		id = InsertNewTeacherInfo();
}