#include<iostream>
#include<fstream>
#include<WinSock2.h>
#include<direct.h>
#include"..\\include\\logsoft.h"
#include"..\\include\data_package.h"
#include"userfunc.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;
char empty_tag[] = "EMPTY_PATH";
//char registerpath[PATH_LENGTH] = "E:\\ser\\user\\registerblock";
//char registerpath[PATH_LENGTH]= ".\\user\\registerblock";
//最后打包时，修改：登录或注销失败时客户端只输出“用户名或密码错误”
struct wlp* regbloc = NULL;

typedef struct ListRes {
	struct header head = { CMD_LIST_RES ,sizeof(ListRes) };
	int result;
	struct wlp node;
}ListRes;

typedef struct CheckRes {
	struct header head = { CMD_CHECK_RES ,sizeof(CheckRes) };
	int result;
	struct wlp node;
}CheckRes;


int proc(SOCKET clsock)
{
	ofstream outf;
	char dirpath[PATH_LENGTH] = ".\\user\\";
	char logpath[PATH_LENGTH] = { 0 };
	char planpath[PATH_LENGTH] = { 0 };
	char expdpath[PATH_LENGTH] = { 0 };
	strcpy_s(logpath, PATH_LENGTH, dirpath);
	strcpy_s(planpath, PATH_LENGTH, dirpath);
	
	/*
	char dirpath[PATH_LENGTH] = ".\\user\\";
	char planpath[PATH_LENGTH] = ".\\user\\";
	*/
	struct wlp* wl = NULL;
	struct wlp* wp = NULL;
	bool re_ = false;//判断是否已被注册
	struct header head;
	recv(clsock, (char*)&head, sizeof(head), 0);
	int lo_ =head.online_tag;//判断在线状态
	cout << "cmd:" << head.cmd << " " << "data_length:" << head.datalength << endl;
	switch (head.cmd)
	{
	case CMD_LOGIN:{
		struct Login login;
		struct LoginRes loginres;
		recv(clsock, (char*)&login + sizeof(header), sizeof(login) - sizeof(header), 0);
		if (lo_==STATE_OUTLINE&&true == verif(login.username, login.password))//离线状态且是合法用户时可以登录
		{
			//更新用户状态
			lo_ = STATE_ONLINE;
			//处理用户文件路径
			strcat_s(logpath, sizeof(logpath), login.username);
			strcat_s(logpath, sizeof(logpath), "\\data");
			strcat_s(logpath, sizeof(logpath), "\\log");//logpath=.E:\\ser\\user\\用户名\\data\\log,文件路径

			strcat_s(planpath, sizeof(planpath), login.username);
			strcat_s(planpath, sizeof(planpath), "\\data");
			strcat_s(planpath, sizeof(planpath), "\\plan");//logpath=.\\user\\用户名\\plan，创建目录
			//链表初始化，载入文件(不用再检测登录的时候载入文件，在具体操作命令的时候载入即可，同时要有保存，释放链表的操作)

			//发送返回值包
			loginres.result = RES_SUCC;
			loginres.head.online_tag = lo_;//更新在线状态
			strcpy_s(loginres.head.logpath,sizeof(logpath), logpath);//更新路径信息。只有在在登录的时候才会返回路径信息，登出时清空路径信息
			strcpy_s(loginres.head.planpath, sizeof(planpath), planpath);
			//loginres.head.planpath = planpath;
			send(clsock, (const char*) & loginres, sizeof(loginres), 0);
			cout << "用户 " << login.username << " 登录" << endl;
			return 0;
		}
		else//非法
		{
			if (lo_ == STATE_ONLINE)//首先检测当前是否在线状态
			{
				loginres.result = ERROR_ONLINE;
				loginres.head.online_tag = lo_;
				send(clsock, (const char*)&loginres, sizeof(loginres), 0);
			}
			else//再检测用户是否合法
			{
				//发送返回值包
				loginres.result = RES_UNEXIST;
				loginres.head.online_tag = lo_;
				send(clsock, (const char*)&loginres, sizeof(loginres), 0);
				//客户端询问是否注册
			}		
			return 1;
		}
			
	}break;

	case CMD_LOGOUT: {
		struct Logout logout;
		struct LogoutRes logoutres;
		recv(clsock, (char*)&logout + sizeof(header), sizeof(logout) - sizeof(header), 0);
		if (lo_== STATE_ONLINE)//判断当前在线状态
		{
			lo_ = STATE_OUTLINE;
			logoutres.head.online_tag = lo_;
			strcpy_s(logoutres.head.logpath, sizeof(empty_tag), empty_tag);//重置路径
			strcpy_s(logoutres.head.logpath, sizeof(empty_tag), empty_tag);
			logoutres.result = RES_OUTLINE;
			send(clsock, (const char*)&logoutres, sizeof(logoutres), 0);
			cout << "用户登出" << endl;
			return 0;
		}
		else
		{
			logoutres.result = ERROR_OUTLINE;
			send(clsock, (const char*)&logoutres, sizeof(logoutres), 0);
			return 1;
		}	
	}break;

	case CMD_REGISTER: {
		struct Register regist;
		struct RegisterRes registres;
		recv(clsock, (char*)&regist +sizeof(header), sizeof(Register)-sizeof(header),0);
		if (lo_== STATE_OUTLINE && false == exist(regist.username,regist.password))//离线且无重复用户名时可以注册
		{
			//用户名与密码写入注册表
			RegistertInBlock(regist.username, regist.password);
			//生成文件路径，创建用户文件
			strcat_s(logpath, sizeof(logpath), regist.username);
			if (0 != _mkdir(logpath)) { cout << "用户个人文件夹创建失败" << endl; };//"E:\\ser\\user\\用户名"
			strcat_s(logpath, sizeof(logpath), "\\data");
			if (0 != _mkdir(logpath)) { cout << "用户数据文件夹创建失败" << endl; };//"E:\\ser\\user\\用户名\\data"
			strcat_s(logpath, sizeof(logpath), "\\log");//logpath=.E:\\ser\\user\\用户名\\data\\log,文件路径

			strcpy_s(expdpath, PATH_LENGTH, logpath);
			strcat_s(expdpath, sizeof(expdpath), "expd\\");
			if (0 != _mkdir(expdpath)) { cout << "用户导出文件夹创建失败" << endl; };//创建导出文件夹

			strcat_s(planpath, sizeof(planpath), regist.username);
			strcat_s(planpath, sizeof(planpath), "\\data");
			strcat_s(planpath, sizeof(planpath), "\\plan");//logpath=.\\user\\用户名\\plan，创建目录
			//链表初始化，载入文件(不用再检测登录的时候载入文件，在具体操作命令的时候载入即可，同时要有保存，释放链表的操作)
			outf.open(logpath);//创建文件
			outf.close();
			outf.open(planpath);//创建文件
			outf.close();
			
			//返回相应信息
			registres.result = RES_SUCC;
			strcpy_s(registres.head.logpath, sizeof(logpath), logpath);//更新路径信息。
			strcpy_s(registres.head.planpath, sizeof(planpath), planpath);
			send(clsock, (const char*)&registres, sizeof(registres), 0);
			return 0;
		}
		else
		{
			if (lo_ == STATE_ONLINE)
			{
				registres.result = ERROR_ONLINE;
				registres.head.online_tag = lo_;
				send(clsock, (const char*)&registres, sizeof(registres), 0);
				return 1;
			}
			else
			{
				registres.result = RES_FAIL;
				send(clsock, (const char*)&registres, sizeof(registres), 0);
				return 1;
			}
		}
	}break;

	case CMD_CANCEL: {
		//客户端必须输入用户名与密码
		//查找用户名是否合法
		//删除用户文件夹
		//删除用户注册表信息
		//返回信息包
		return 0;
	}break;

	case CMD_LIST_LOG_ZA: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //已经在线状态
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.logpath, &block);
			//调用menu函数，生成目录链表
			cct = menu(&namelink, block, 1);//namelink头节点不存储信息,cct为namelink中节点个数
			cout << "正在请求日志目录，共有" << cct << "项" << endl;
			//返回目录链表
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{
				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//通知客户端命令接受成功，正在生成目录
				cout << "正在发送日志目录..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//发送包头及数据，说明list是否需要结束
					//cout << "返回数据一次" << endl;
					temp = temp->next;
				}
				cout << "日志目录发送结束" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			//cout << "返回信息一次" << endl;
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//发送提示登录的数据包
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_LOG_AZ: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //已经在线状态
		{
			struct ListRes listres;
			listres.result = RES_SUCC;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);//通知客户端命令接受成功，正在生成目录
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.logpath, &block);
			//调用menu函数，生成目录链表
			cct = menu(&namelink, block, -1);//namelink头节点不存储信息,cct为namelink中节点个数
			cout << "正在请求日志目录，共有" << cct << "项" << endl;
			//返回目录链表
			temp = namelink->next;
			if (cct != 0)
			{
				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//通知客户端命令接受成功，正在生成目录
				cout << "正在发送日志目录..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//发送包头及数据，说明list是否需要结束
					temp = temp->next;
				}
				cout << "日志目录发送结束" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);

			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{		
			//发送提示登录的数据包
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_PLAN_AZ: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //已经在线状态
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.planpath, &block);
			//调用menu函数，生成目录链表
			cct = menu(&namelink, block, -1);//namelink头节点不存储信息,cct为namelink中节点个数
			cout << "正在请求计划目录，共有" << cct << "项" << endl;
			//返回目录链表
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{

				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//通知客户端命令接受成功，正在生成目录
				cout << "正在发送计划目录..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//发送包头及数据，说明list是否需要结束
					temp = temp->next;
				}
				cout << "计划目录发送结束" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			//cout << "返回信息一次" << endl;
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//发送提示登录的数据包
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_PLAN_ZA: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //已经在线状态
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.planpath, &block);
			//调用menu函数，生成目录链表
			cct = menu(&namelink, block, 1);//namelink头节点不存储信息,cct为namelink中节点个数
			cout << "正在请求计划目录，共有" << cct << "项" << endl;
			//返回目录链表
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{

				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//通知客户端命令接受成功，正在生成目录
				cout << "正在发送计划目录..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//发送包头及数据，说明list是否需要结束
					temp = temp->next;
				}
				cout << "计划目录发送结束" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			//cout << "返回信息一次" << endl;
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//发送提示登录的数据包
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_PROG_AZ: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //已经在线状态
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.logpath, &block);
			//调用menu函数，生成目录链表
			cct = menu(&namelink, block, -1);//namelink头节点不存储信息,cct为namelink中节点个数
			cout << "正在请求日志目录，共有" << cct << "项" << endl;
			//返回目录链表
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{

				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//通知客户端命令接受成功，正在生成目录
				cout << "正在发送日志目录..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//发送包头及数据，说明list是否需要结束
					temp = temp->next;
				}
				cout << "日志目录发送结束" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//发送提示登录的数据包
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_PROG_ZA: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //已经在线状态
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.logpath, &block);
			//调用menu函数，生成目录链表
			cct = menu(&namelink, block, 1);//namelink头节点不存储信息,cct为namelink中节点个数
			cout << "开发日志目录共有" << cct << "项" << endl;
			//返回目录链表
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{

				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//通知客户端命令接受成功，正在生成目录
				cout << "正在发送开发日志目录..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//发送包头及数据，说明list是否需要结束
					temp = temp->next;
				}
				cout << "开发日志目录发送结束" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			//cout << "返回信息一次" << endl;
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//发送提示登录的数据包
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_ADD_LOG: {
		if (lo_ == STATE_ONLINE)//未登录的判断在客户端完成
		{
			struct Log log;
			struct wlp* headnode;
			cout << head.logpath << " " << head.planpath << endl;
			loadfile(head.logpath, &headnode);
			recv(clsock, (char*)&log + sizeof(header), sizeof(log) - sizeof(header), 0);
			//使用函数解决，修改adddata()函数;
			adddata(&headnode, log.name, log.data, 0);//在函数内部一并解决同一天内多次添加信息的问题
			//解决办法：在客户端输入日志文本时，字符串匹配，若不是endw,则当作CMD_ADD_LOG命令的包直接发送，即每一条信息都当作重新大宋一条新的日志，是否同一天的判断交给服务器
			//CMD_INFO就不用了	
			outtofile(head.logpath, headnode);
			freed(headnode);
			return 0;
		}
	}break;

	case CMD_ADD_PROG: {
	
		if (lo_ == STATE_ONLINE)//未登录的判断在客户端完成
		{
			struct Log log;
			struct wlp* headnode;
			cout << head.logpath << " " << head.planpath << endl;
			loadfile(head.logpath, &headnode);
			recv(clsock, (char*)&log + sizeof(header), sizeof(log) - sizeof(header), 0);
			//使用函数解决，修改adddata()函数;
			adddata(&headnode, log.name, log.data, 1);//在函数内部一并解决同一天内多次添加信息的问题
			//解决办法：在客户端输入日志文本时，字符串匹配，若不是endw,则当作CMD_ADD_LOG命令的包直接发送，即每一条信息都当作重新大宋一条新的日志，是否同一天的判断交给服务器
			//CMD_INFO就不用了	
			outtofile(head.logpath, headnode);
			freed(headnode);
			return 0;
		}
	}break;

	case CMD_ADD_PLAN: {
		if (lo_ == STATE_ONLINE)//未登录的判断在客户端完成
		{
			struct Plan plan;
			struct wlp* headnode;
			cout << head.logpath << " " << head.planpath << endl;
			loadfile(head.planpath, &headnode);
			recv(clsock, (char*)&plan + sizeof(header), sizeof(plan) - sizeof(header), 0);
			//使用函数解决，修改adddata()函数;
			adddata(&headnode, plan.name, plan.data, 0);//在函数内部一并解决同一天内多次添加信息的问题
			//解决办法：在客户端输入日志文本时，字符串匹配，若不是endw,则当作CMD_ADD_LOG命令的包直接发送，即每一条信息都当作重新大宋一条新的日志，是否同一天的判断交给服务器
			//CMD_INFO就不用了	
			outtofile(head.planpath, headnode);
			freed(headnode);
			return 0;
		}
	}break;

	case CMD_CHECK_LOG: {
		if (lo_ == STATE_ONLINE)
		{
			struct CheckLog checklog;
			struct CheckRes checkres;
			struct wlp* headnode, * temp;
			struct data* tempinfo;
			loadfile(head.logpath, &headnode);
			recv(clsock, (char*)&checklog + sizeof(header), sizeof(checklog) - sizeof(header), 0);
			int cc = is_node(headnode, checklog.name);
			cout << "查询文件;" << checklog.name  << ",共有" << cc<<"项" << endl;
			if (cc == 0)
			{
				checkres.result = RES_FAIL;
				checkres.head.online_tag = lo_;
				checkres.node.count = 0;
				strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&checkres, sizeof(checkres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				temp = findtail(headnode);
				cout << "正在传输数据..." << endl;
				while (cc > 0)
				{
					if (!strcmp(temp->name, checklog.name))
					{
						checkres.result = RES_SUCC;
						checkres.head.online_tag = lo_;
						strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
						strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);

						strcpy_s(checkres.node.name, DATA_NAME_LENGTH, temp->name);//复制数据
						strcpy_s(checkres.node.today, DATA_NAME_LENGTH, temp->today);
						checkres.node.count = temp->count;
						int ct = temp->count;
						send(clsock, (const char*)&checkres, sizeof(checkres), 0);
						tempinfo = &temp->lpdata;
						while (tempinfo != NULL)
						{

							checkres.result = RES_SUCC;
							checkres.head.online_tag = lo_;
							strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
							strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);

							strcpy_s(checkres.node.lpdata.info, DATA_LENGTH, tempinfo->info);//复制数据
							strcpy_s(checkres.node.lpdata.time, TIME_LENGTH, tempinfo->time);
							checkres.node.count = ct;
							ct--;
							send(clsock, (const char*)&checkres, sizeof(checkres), 0);
							tempinfo = tempinfo->next;
						}
						cc--;
					}
					temp = temp->pre;
					if (temp == NULL)cout << "ERROR" << endl;
				}
				cout << "数据传输结束" << endl;
				checkres.result = RES_FAIL;
				checkres.head.online_tag = lo_;
				checkres.node.count = 0;
				strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&checkres, sizeof(checkres), 0);
				freed(headnode);
				return 0;
			}
		}
		else
		{
			struct CheckRes checkres;
			checkres.result = RES_OUTLINE;
			checkres.head.online_tag = lo_;
			strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&checkres, sizeof(checkres), 0);
			return 1;
		}
	}break;

	case CMD_CHECK_PLAN: {
		if (lo_ == STATE_ONLINE)
		{
			struct CheckPlan checkplan;
			struct CheckRes checkres;
			struct wlp* headnode, * temp;
			struct data* tempinfo;
			loadfile(head.planpath, &headnode);
			recv(clsock, (char*)&checkplan + sizeof(header), sizeof(checkplan) - sizeof(header), 0);
			int cc = is_node(headnode, checkplan.name);
			cout << "查询文件;" << checkplan.name << ",共有" << cc << "项" << endl;
			if (cc == 0)
			{
				checkres.result = RES_FAIL;
				checkres.head.online_tag = lo_;
				checkres.node.count = 0;
				strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&checkres, sizeof(checkres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				temp = findtail(headnode);
				cout << "正在传输数据..." << endl;
				while (cc > 0)
				{
					if (!strcmp(temp->name, checkplan.name))
					{
						checkres.result = RES_SUCC;
						checkres.head.online_tag = lo_;
						strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
						strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);

						strcpy_s(checkres.node.name, DATA_NAME_LENGTH, temp->name);//复制数据
						strcpy_s(checkres.node.today, DATA_NAME_LENGTH, temp->today);
						checkres.node.count = temp->count;
						int ct = temp->count;
						send(clsock, (const char*)&checkres, sizeof(checkres), 0);
						tempinfo = &temp->lpdata;
						while (tempinfo != NULL)
						{

							checkres.result = RES_SUCC;
							checkres.head.online_tag = lo_;
							strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
							strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);

							strcpy_s(checkres.node.lpdata.info, DATA_LENGTH, tempinfo->info);//复制数据
							strcpy_s(checkres.node.lpdata.time, TIME_LENGTH, tempinfo->time);
							checkres.node.count = ct;
							ct--;
							send(clsock, (const char*)&checkres, sizeof(checkres), 0);
							tempinfo = tempinfo->next;
						}
						cc--;
					}
					temp = temp->pre;
					if (temp == NULL)cout << "ERROR" << endl;
				}
				cout << "数据传输结束" << endl;
				checkres.result = RES_FAIL;
				checkres.head.online_tag = lo_;
				checkres.node.count = 0;
				strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&checkres, sizeof(checkres), 0);
				freed(headnode);
				return 0;
			}
		}
		else
		{
			struct CheckRes checkres;
			checkres.result = RES_OUTLINE;
			checkres.head.online_tag = lo_;
			strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&checkres, sizeof(checkres), 0);
			return 1;
		}
	}break;

	case CMD_DEL_LOG: {
		if (lo_ == STATE_ONLINE)
		{
			struct DelLog dellog;
			struct DelRes delres;
			struct wlp* headnode;
			loadfile(head.logpath, &headnode);
			recv(clsock, (char*)&dellog + sizeof(header), sizeof(dellog) - sizeof(header), 0);
			int cc = is_node(headnode, dellog.name);
			cout << "删除目标文件;" << dellog.name << ",共有" << cc << "项" << endl;
			if (cc == 0)
			{
				delres.result = RES_FAIL;
				delres.head.online_tag = lo_;
				strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&delres, sizeof(delres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				delres.result = RES_SUCC;
				delres.head.online_tag = lo_;
				strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
				delnode(&headnode, dellog.name);
				send(clsock, (const char*)&delres, sizeof(delres), 0);
				outtofile(head.logpath, headnode);
				freed(headnode);
				return 0;
			}
		}
		else
		{
			struct DelRes delres;
			delres.result = RES_OUTLINE;
			delres.head.online_tag = lo_;
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&delres, sizeof(delres), 0);
			return 1;
		}
	}break;

	case CMD_DEL_PLAN: {
		if (lo_ == STATE_ONLINE)
		{
			struct DelPlan delplan;
			struct DelRes delres;
			struct wlp* headnode;
			loadfile(head.planpath, &headnode);
			recv(clsock, (char*)&delplan + sizeof(header), sizeof(delplan) - sizeof(header), 0);
			int cc = is_node(headnode, delplan.name);
			cout << "删除目标文件;" << delplan.name << ",共有" << cc << "项" << endl;
			if (cc == 0)
			{
				delres.result = RES_FAIL;
				delres.head.online_tag = lo_;
				strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&delres, sizeof(delres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				delres.result = RES_SUCC;
				delres.head.online_tag = lo_;
				strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
				delnode(&headnode, delplan.name);
				send(clsock, (const char*)&delres, sizeof(delres), 0);
				outtofile(head.planpath, headnode);
				freed(headnode);
				return 0;
			}
		}
		else
		{
			struct DelRes delres;
			delres.result = RES_OUTLINE;
			delres.head.online_tag = lo_;
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&delres, sizeof(delres), 0);
			return 1;
		}
	}break;

	case CMD_DEL_ALL_LOG: {
		if (lo_ == STATE_ONLINE)
		{
			cout << "删除目标路径:" << head.logpath << endl;
			struct DelAllLog delalllog;
			struct DelRes delres;
			struct wlp* headnode;
			loadfile(head.logpath, &headnode);			
			delres.result = RES_SUCC;
			delres.head.online_tag = lo_;
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
			delall(headnode);
			send(clsock, (const char*)&delres, sizeof(delres), 0);
			outtofile(head.logpath, headnode);
			freed(headnode);
			return 0;
		}
		else
		{
			struct DelRes delres;
			delres.result = RES_OUTLINE;
			delres.head.online_tag = lo_;
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&delres, sizeof(delres), 0);
			return 1;
		}
	}break;

	case CMD_DEL_ALL_PLAN: {
		if (lo_ == STATE_ONLINE)
		{
			cout << "删除目标路径:" << head.planpath << endl;
			struct DelAllPlan delallplan;
			struct DelRes delres;
			struct wlp* headnode;
			loadfile(head.planpath, &headnode);			
			delres.result = RES_SUCC;
			delres.head.online_tag = lo_;
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
			delall(headnode);
			send(clsock, (const char*)&delres, sizeof(delres), 0);
			outtofile(head.planpath, headnode);
			freed(headnode);
			return 0;
		}
		else
		{
			struct DelRes delres;
			delres.result = RES_OUTLINE;
			delres.head.online_tag = lo_;
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&delres, sizeof(delres), 0);
			return 1;
		}
	}break;

	case CMD_EXPD: {
		if (lo_ == STATE_ONLINE)
		{
			//char expdpath[PATH_LENGTH] = "E:\\ser\\user\\";
			struct Expd expd;
			struct ExpdRes expdres;
			struct wlp* headnode;
			loadfile(head.logpath, &headnode);
			recv(clsock, (char*)&expd + sizeof(header), sizeof(expd) - sizeof(header), 0);
			int cc = is_node(headnode, expd.name);
			cout << "待导出文件;" << expd.name << "共有" << cc << "项" << endl;
			if (cc == 0)
			{
				expdres.result = RES_FAIL;
				expdres.head.online_tag = lo_;
				strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
				strcpy_s(expdres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&expdres, sizeof(expdres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				strcpy_s(expdpath, PATH_LENGTH, head.logpath);
				strcat_s(expdpath, sizeof(expdpath), "expd\\");
				strcat_s(expdpath, PATH_LENGTH, expd.name);
				strcat_s(expdpath, PATH_LENGTH, ".txt");
				cout <<"创建按临时文件："<< expdpath << endl;
				exportlog(headnode, expd.name,expdpath);
				
				FILE* fp;
				fopen_s(&fp, expdpath, "rb");
				if (fp == NULL)
				{
					cout << "文件打开失败" << endl;
					expdres.result = RES_FAIL;
					expdres.head.online_tag = lo_;
					strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
					strcpy_s(expdres.head.planpath, sizeof(head.planpath), head.planpath);
					send(clsock, (const char*)&expdres, sizeof(expdres), 0);
					freed(headnode);
					
					return 1;
				}
				else
				{
					cout << "文件打开成功，正在传输" << endl;
					char buff[BUF_SIZE] = { 0 };
					int count = 0;
					while ((count = fread(buff, 1, BUF_SIZE, fp)) > 0)
					{
						cout << count << endl;
						expdres.result = RES_SUCC;
						expdres.head.online_tag = lo_;
						strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
						strcpy_s(expdres.head.planpath, sizeof(head.planpath), head.planpath);
						strcpy_s(expdres.buff, BUF_SIZE, buff);
						send(clsock, (const char*)&expdres, sizeof(expdres), 0);						
					}
					expdres.result = RES_FAIL;
					expdres.head.online_tag = lo_;
					strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
					strcpy_s(expdres.head.planpath, sizeof(head.planpath), head.planpath);
					send(clsock, (const char*)&expdres, sizeof(expdres), 0);
					fclose(fp);
					freed(headnode);
					if (0 == remove(expdpath))//传输完成后再服务器上删除
					{
						cout << "临时文件删除成功" << endl;
					}
					else
					{
						cout << "临时文件删除失败" << endl;
					}
					return 0;
				}
			}				
		}
		else
		{
			struct ExpdRes expdres;
			expdres.result = RES_OUTLINE;
			expdres.head.online_tag = lo_;
			strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//更新路径信息。
			strcpy_s(expdres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&expdres, sizeof(expdres), 0);
			return 1;
		}
	}break;

	case CMD_SAVE: {
		return 0;
	}break;
	case CMD_SAVE_LOG: {
		return 0;
	}break;
	case CMD_SAVE_PLAN: {
		return 0;
	}break;
	case CMD_EXIT: {
		struct ExitRes exitres;
		exitres.result = 1;
		send(clsock, (const char*)&exitres, sizeof(exitres), 0);
		return -1;
	}break;
	default:
		break;
	}

	
}