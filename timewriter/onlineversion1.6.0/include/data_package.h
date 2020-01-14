//package
#include"defination.h"
//#include "logsoft.h"





enum RESULT_TAG {
	RES_SUCC,
	RES_FAIL,
	RES_UNEXIST,//������
	RES_ONLINE,//����
	RES_OUTLINE,//�û�����
	ERROR_ONLINE,//�û��Ѿ����ߣ����ڼ���ظ���¼��������״̬��ע��
	ERROR_OUTLINE,//�û��Ѿ����ߣ����ڼ���ظ��ǳ�
	STATE_ONLINE,
	STATE_OUTLINE
};



enum CMD {
	CMD_LOGIN,
	CMD_LOGINRES,

	CMD_LOGOUT,
	CMD_LOGOUTRES,

	CMD_REGISTER,
	CMD_REGISTERRES,
	CMD_CANCEL,//ע��

	CMD_LIST_LOG_AZ,
	CMD_LIST_LOG_ZA,
	CMD_LIST_PLAN_AZ,
	CMD_LIST_PLAN_ZA,
	CMD_LIST_PROG_AZ,
	CMD_LIST_PROG_ZA,
	CMD_LIST_RES,

	CMD_ADD_LOG, CMD_ADD_PROG, CMD_ADD_PLAN, CMD_ADD_INFO,
	CMD_ENDW,

	CMD_CHECK_LOG, CMD_CHECK_PLAN, CMD_CHECK_INFO,
	CMD_CHECK_RES,

	CMD_DEL_LOG,
	CMD_DEL_PLAN,
	CMD_DEL_ALL_LOG,
	CMD_DEL_ALL_PLAN,
	CMD_DEL_RES,

	CMD_EXPD,
	CMD_EXPD_RES,

	CMD_SAVE,
	CMD_SAVE_LOG,
	CMD_SAVE_PLAN,
	CMD_SAVE_RES,

	CMD_EXIT,
	CMD_EXITRES
};

typedef struct header {
	int cmd;
	int datalength;
	int online_tag = STATE_OUTLINE;//��ʼ��Ϊ����״̬
	char logpath[PATH_LENGTH] = "EMPTY_PATH";//�洢�û��ļ���·��
	char planpath[PATH_LENGTH] = "EMPTY_PATH";
}header;

typedef struct Login {
	struct header head = { CMD_LOGIN,sizeof(Login) };
	char username[USER_NAME_LENGTH];
	char password[PASSWORD_LENGTH];
}Login;

typedef struct LoginRes {
	struct header head = { CMD_LOGINRES ,sizeof(LoginRes) };
	int result;//����RES_SUCCʱΪ�ɹ���RES_FAILΪʧ��
}LoginRes;

typedef struct Logout {
	struct header head = { CMD_LOGOUT,sizeof(Logout) };
	char username[USER_NAME_LENGTH];
}Logout;

typedef struct LogoutRes {
	struct header head = { CMD_LOGOUTRES ,sizeof(LogoutRes) };
	int result;
}LogoutRes;

typedef struct Register {
	struct header head = { CMD_REGISTER ,sizeof(Register) };
	char username[USER_NAME_LENGTH];
	char password[PASSWORD_LENGTH];
}Register;

typedef struct RegisterRes {
	struct header head = { CMD_REGISTERRES ,sizeof(RegisterRes) };
	int result;
}RegisterRes;

typedef struct ListLogAz {
	struct header head = { CMD_LIST_LOG_AZ ,sizeof(ListLogAz) };
}ListLogAz;

typedef struct ListPlanAz {
	struct header head = { CMD_LIST_PLAN_AZ ,sizeof(ListPlanAz) };
}ListPlanAz;

typedef struct ListProgAz {
	struct header head = { CMD_LIST_PROG_AZ ,sizeof(ListProgAz) };
}ListProgAz;

typedef struct ListLogZa {
	struct header head = { CMD_LIST_LOG_ZA ,sizeof(ListLogZa) };
}ListLogZa;

typedef struct ListPlanZa {
	struct header head = { CMD_LIST_PLAN_ZA ,sizeof(ListPlanZa) };
}ListPlanZa;

typedef struct ListProgZa {
	struct header head = { CMD_LIST_PROG_ZA ,sizeof(ListProgZa) };
}ListProgZa;


typedef struct Log {
	struct header head = { CMD_ADD_LOG ,sizeof(Log) };
	char name[DATA_NAME_LENGTH];
	char data[DATA_LENGTH];
}Log;

typedef struct Prog {
	struct header head = { CMD_ADD_PROG ,sizeof(Prog) };
	char name[DATA_NAME_LENGTH];
	char data[DATA_LENGTH];
}Prog;

typedef struct Plan {
	struct header head = { CMD_ADD_PLAN ,sizeof(Plan) };
	char name[DATA_NAME_LENGTH];
	char data[DATA_LENGTH];
}Plan;

typedef struct Addinfo {
	struct header head = { CMD_ADD_INFO ,sizeof(Addinfo) };
	char info[DATA_LENGTH];
}Addinfo;

typedef struct Endw {
	struct header head = { CMD_ENDW ,sizeof(Endw) };
}Endw;

typedef struct CheckLog {
	struct header head = { CMD_CHECK_LOG ,sizeof(CheckLog) };
	char name[DATA_NAME_LENGTH];
}CheckLog;

typedef struct CheckPlan {
	struct header head = { CMD_CHECK_PLAN ,sizeof(CheckPlan) };
	char name[DATA_NAME_LENGTH];
}CheckPlan;

typedef struct CheckInfo {
	struct header head = { CMD_CHECK_INFO ,sizeof(CheckInfo) };
	char name[DATA_NAME_LENGTH];
}CheckInfo;


typedef struct DelLog {
	struct header head = { CMD_DEL_LOG ,sizeof(DelLog) };
	char name[DATA_NAME_LENGTH];
}DelLog;

typedef struct DelPlan {
	struct header head = { CMD_DEL_PLAN ,sizeof(DelPlan) };
	char name[DATA_NAME_LENGTH];
}DelPlan;

typedef struct DelAllLog {
	struct header head = { CMD_DEL_ALL_LOG ,sizeof(DelAllLog) };
}DelAllLog;

typedef struct DelAllPlan {
	struct header head = { CMD_DEL_ALL_PLAN ,sizeof(DelAllPlan) };
}DelAllPlan;

typedef struct DelRes {
	struct header head = { CMD_DEL_RES ,sizeof(DelRes) };
	int result;
}DelRes;

typedef struct Expd {
	struct header head = { CMD_EXPD ,sizeof(Expd) };
	char name[DATA_NAME_LENGTH];
}Expd;

typedef struct ExpdRes {
	struct header head = { CMD_EXPD_RES ,sizeof(ExpdRes) };
	int result;
	char buff[BUF_SIZE] = { 0 };
}ExpdRes;

typedef struct Save {
	struct header head = { CMD_SAVE ,sizeof(Save) };
}Save;

typedef struct SaveLog {
	struct header head = { CMD_SAVE_LOG ,sizeof(SaveLog) };
}SaveLog;

typedef struct SavePlan {
	struct header head = { CMD_SAVE_PLAN ,sizeof(SavePlan) };
}SavePlan;

typedef struct SaveRes {
	struct header head = { CMD_SAVE_RES ,sizeof(SaveRes) };
	int result;
}SaveRes;

typedef struct Exit{
	struct header head = { CMD_EXIT ,sizeof(Exit) };
}Exit;

typedef struct ExitRes {
	struct header head = { CMD_EXITRES ,sizeof(ExitRes) };
	int result;
}ExitRes;