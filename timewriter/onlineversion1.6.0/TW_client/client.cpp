#include<iostream>
#include<iomanip>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include"..\\include\data_package.h"
#include"..\\include\logsoft.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;
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

int main(void)
{
	//初始化dll
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	//创建socket
	SOCKET clsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == clsock) { cout << "socket build failed." << endl; exit(0); }
	//绑定socket
	struct sockaddr_in clsockadd;
	memset(&clsockadd, 0, sizeof(clsockadd));
	clsockadd.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &clsockadd.sin_addr);
	clsockadd.sin_port = htons(1234);
	if (-1 == connect(clsock, (sockaddr*)&clsockadd, sizeof(clsockadd))) { cout << "connect failed" << endl; exit(0); }
	headlist();
	char* td = getday();
	char* tt = gettime();
	cout << "[" << td << "] " << tt <<endl;
	struct header head;
	head.online_tag = STATE_OUTLINE;

	shuru:
	{
		char command[CMD_LENGTH] = { 0 };
		char command1[CMD_LENGTH] = { 0 };
		char command2[CMD_LENGTH] = { 0 };
		char command3[CMD_LENGTH] = { 0 };
		cout <<endl<< "TimeWriter>> ";
		cin >> command;
		
		struct wlp* namelink = NULL;
		char tc[1] = { 0 };
		tc[0] = getchar();
		//帮助信息
		if (!strcmp(command, "-help"))
		{
			if (tc[0] == '\n')
			{
				outlist();

			}
			else
			{
				while (tc[0] != '\n')//吃掉多余的参数
				{
					tc[0] = getchar();
				}
				cout << "Wrong argument!" << endl;

			}
			goto shuru;
		}
		//用户信息处理
		if (!strcmp(command, "login"))
		{
			struct Login login;
			strcpy_s(login.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
			strcpy_s(login.head.planpath, sizeof(head.planpath), head.planpath);
			login.head.online_tag = head.online_tag;
			//head={CMD_LOGIN,sizeof(data)};
			cout << "UserName: ";
			cin >> login.username;
			cout << "PassWord: ";
			cin >> login.password;
			//send(clsock,(const char*)&head,sizeof(head),0);
			send(clsock, (const char*)&login, sizeof(login), 0);
			struct LoginRes loginresult;
			//recv(clsock,(char*)&head,sizeof(head),0);
			recv(clsock, (char*)&loginresult, sizeof(loginresult), 0);
			head.online_tag = loginresult.head.online_tag;//更新head 
			strcpy_s(head.logpath, sizeof(loginresult.head.logpath), loginresult.head.logpath);
			strcpy_s(head.planpath, sizeof(loginresult.head.planpath), loginresult.head.planpath);
			if (loginresult.result == RES_SUCC)cout << "ServerReturn: 登录成功！" <<endl;
			if (loginresult.result == RES_UNEXIST)cout << "ServerReturn: 用户名或密码错误！" << endl;
			if (loginresult.result == ERROR_ONLINE)cout << "ServerReturn: 用户已经在线，请勿重复登录！" << endl;
			goto shuru;
		}

		if (!strcmp(command, "logout"))
		{
			struct Logout logout;
			struct LogoutRes logoutres;
			logout.head.online_tag = head.online_tag;//更新在线状态
			send(clsock, (const char*)&logout, sizeof(logout), 0);
			recv(clsock, (char*)&logoutres, sizeof(logoutres), 0);
			head.online_tag = logoutres.head.online_tag;//更新head 
			strcpy_s(head.logpath, sizeof(head.logpath), logoutres.head.logpath);
			strcpy_s(head.planpath, sizeof(head.planpath), logoutres.head.planpath);
			if (logoutres.result == RES_OUTLINE) cout << "ServerReturn: 登出成功！" <<endl;
			if (logoutres.result == ERROR_OUTLINE)cout << "ServerReturn: 离线状态无法登出，请先登录！" << endl;
			goto shuru;
		}

		if (!strcmp(command, "register"))
		{
			struct Register regist;
			struct RegisterRes registres;
			regist.head.online_tag = head.online_tag;//更新在线状态
			cout << "UserName: ";
			cin >> regist.username;
			cout << "PassWord: ";
			cin >> regist.password; 
			send(clsock, (const char*)&regist, sizeof(regist), 0);
			recv(clsock, (char*)&registres, sizeof(registres), 0);
			if (registres.result == RES_SUCC)cout << "ServerReturn: 注册成功！" <<endl;
			if (registres.result == ERROR_ONLINE)cout << "ServerReturn: 登录状态下无法注册，请先登出！" << endl;
			if (registres.result == RES_FAIL)cout << "ServerReturn: 用户名已存在！" <<endl;
			goto  shuru;
		}
		//数据处理
		if (!strcmp(command, "list"))//列出项目名,示例：list log -az
		{
			struct ListLogAz listlogaz;
			struct ListPlanAz listplanaz;
			struct ListProgAz listprogaz;
			struct ListLogZa listlogza;
			struct ListPlanZa listplanza;
			struct ListProgZa listprogza;
			struct ListRes listres;
			if (tc[0] == '\n')//list
			{
				cout << "Need more arguments!" << endl;
				goto shuru;
			}
			else//list log(plan)
			{
				cin >> command1;

				if (!strcmp(command1, "log"))//列出普通日志目录
				{
					tc[0] = getchar();
					if (tc[0] == '\n')//za
					{					
						//获取menu并输出：
						//发送命令包
						strcpy_s(listlogza.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
						strcpy_s(listlogza.head.planpath, sizeof(head.planpath), head.planpath);
						listlogza.head.online_tag = head.online_tag;
						send(clsock, (const char*)&listlogza, sizeof(listlogza), 0);//发送命令										
						recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
						head.online_tag = listres.head.online_tag;//更新head 
						strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
						strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

						if (listres.result == RES_OUTLINE)
						{
							cout << "ServerReturn: 请先登录！" <<endl; goto shuru;
						}
						else if (listres.result == RES_FAIL)
						{
							cout << "ServerReturn: 没有数据！" <<endl; goto shuru;
						}
						else if (listres.result == RES_SUCC)
						{
							cout << "输出目录:" <<endl;
							cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
							while (listres.result != RES_FAIL)
							{
								//struct wlp temp;
								recv(clsock, (char*)&listres, sizeof(listres), 0);//
								head.online_tag = listres.head.online_tag;//更新head
								strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
								strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
									if (listres.result != RES_FAIL&&listres.node.b == 0)
									{
										cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
										sampleinfo(&listres.node);
									}
							}
							cout <<endl<< "输出完毕" <<endl;
							goto shuru;
						}
						
					}
					else
					{
						cin >> command2;
						if (!strcmp(command2, "-za"))//za
						{
							//获取menu并输出：
						//发送命令包
							strcpy_s(listlogza.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
							strcpy_s(listlogza.head.planpath, sizeof(head.planpath), head.planpath);
							listlogza.head.online_tag = head.online_tag;
							send(clsock, (const char*)&listlogza, sizeof(listlogza), 0);//发送命令										
							recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
							head.online_tag = listres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
							strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

							if (listres.result == RES_OUTLINE)
							{
								cout << "ServerReturn: 请先登录！" <<endl; goto shuru;
							}
							else if (listres.result == RES_FAIL)
							{
								cout << "ServerReturn: 没有数据！" <<endl; goto shuru;
							}
							else if (listres.result == RES_SUCC)
							{
								cout << "输出目录:" <<endl;
								cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
								while (listres.result != RES_FAIL)
								{
									//struct wlp temp;
									recv(clsock, (char*)&listres, sizeof(listres), 0);//
									head.online_tag = listres.head.online_tag;//更新head
									strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
									strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
									if (listres.result != RES_FAIL && listres.node.b == 0)
									{
										cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
										sampleinfo(&listres.node);
									}
								}
								cout <<endl<< "输出完毕" <<endl;
								goto shuru;
							}
						}
						else if (!strcmp(command2, "-az"))//az
						{
							//获取menu并输出：
						//发送命令包
							strcpy_s(listlogaz.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
							strcpy_s(listlogaz.head.planpath, sizeof(head.planpath), head.planpath);
							listlogaz.head.online_tag = head.online_tag;
							send(clsock, (const char*)&listlogaz, sizeof(listlogaz), 0);//发送命令										
							recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
							head.online_tag = listres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
							strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

							if (listres.result == RES_OUTLINE)
							{
								cout << "ServerReturn: 请先登录！" <<endl; goto shuru;
							}
							else if (listres.result == RES_FAIL)
							{
								cout << "ServerReturn: 没有数据！" <<endl; goto shuru;
							}
							else if (listres.result == RES_SUCC)
							{
								cout << "输出目录" <<endl;
								cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
								while (listres.result != RES_FAIL)
								{
									//struct wlp temp;
									recv(clsock, (char*)&listres, sizeof(listres), 0);//
									head.online_tag = listres.head.online_tag;//更新head
									strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
									strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
									if (listres.result != RES_FAIL && listres.node.b == 0)
									{
										cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
										sampleinfo(&listres.node);
									}
								}
								cout <<endl<< "输出完毕" << endl;
								goto shuru;
							}
						}
						else
						{
							cout << "Wrong Arguments!" << endl;
							goto shuru;
						}
					}

				}
				if (!strcmp(command1, "prog"))//列出开发日志目录
				{
					tc[0] = getchar();
					if (tc[0] == '\n')//za
					{
						//获取menu并输出：
						//发送命令包
						strcpy_s(listprogza.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
						strcpy_s(listprogza.head.planpath, sizeof(head.planpath), head.planpath);
						listprogza.head.online_tag = head.online_tag;
						send(clsock, (const char*)&listprogza, sizeof(listprogza), 0);//发送命令										
						recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
						head.online_tag = listres.head.online_tag;//更新head 
						strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
						strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

						if (listres.result == RES_OUTLINE)
						{
							cout << "ServerReturn: 请先登录" <<endl; goto shuru;
						}
						else if (listres.result == RES_FAIL)
						{
							cout << "ServerReturn: 没有数据" <<endl; goto shuru;
						}
						else if (listres.result == RES_SUCC)
						{
							cout << "输出目录" <<endl;
							cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
							while (listres.result != RES_FAIL)
							{
								//struct wlp temp;
								recv(clsock, (char*)&listres, sizeof(listres), 0);//
								head.online_tag = listres.head.online_tag;//更新head
								strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
								strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
								if (listres.result != RES_FAIL && listres.node.b == 1)
								{
									cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
									sampleinfo(&listres.node);
								}
							}
							cout << endl<<"输出完毕" <<endl;
							goto shuru;
						}
					}
						
						else
						{
							cin >> command2;
							if (!strcmp(command2, "-za"))
							{
								//获取menu并输出：
						//发送命令包
								strcpy_s(listprogza.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
								strcpy_s(listprogza.head.planpath, sizeof(head.planpath), head.planpath);
								listprogza.head.online_tag = head.online_tag;
								send(clsock, (const char*)&listprogza, sizeof(listprogza), 0);//发送命令										
								recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
								head.online_tag = listres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
								strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

								if (listres.result == RES_OUTLINE)
								{
									cout << "ServerReturn: 请先登录" <<endl; goto shuru;
								}
								else if (listres.result == RES_FAIL)
								{
									cout << "ServerReturn: 没有数据" <<endl; goto shuru;
								}
								else if (listres.result == RES_SUCC)
								{
									cout << "输出目录" << endl;
									cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
									while (listres.result != RES_FAIL)
									{
										//struct wlp temp;
										recv(clsock, (char*)&listres, sizeof(listres), 0);//
										head.online_tag = listres.head.online_tag;//更新head
										strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
										strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
										if (listres.result != RES_FAIL && listres.node.b == 1)
										{
											cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
											sampleinfo(&listres.node);
										}
									}
									cout <<endl<< "输出完毕" <<endl;
									goto shuru;
								}
							}
							else if (!strcmp(command2, "-az"))
							{
								//获取menu并输出：
						//发送命令包
								strcpy_s(listprogaz.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
								strcpy_s(listprogaz.head.planpath, sizeof(head.planpath), head.planpath);
								listprogaz.head.online_tag = head.online_tag;
								send(clsock, (const char*)&listprogaz, sizeof(listprogaz), 0);//发送命令										
								recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
								head.online_tag = listres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
								strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

								if (listres.result == RES_OUTLINE)
								{
									cout << "ServerReturn: 请先登录" <<endl; goto shuru;
								}
								else if (listres.result == RES_FAIL)
								{
									cout << "ServerReturn: 没有数据" <<endl; goto shuru;
								}
								else if (listres.result == RES_SUCC)
								{
									cout << "输出目录" << endl;
									cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
									while (listres.result != RES_FAIL)
									{
										//struct wlp temp;
										recv(clsock, (char*)&listres, sizeof(listres), 0);//
										head.online_tag = listres.head.online_tag;//更新head
										strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
										strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
										if (listres.result != RES_FAIL && listres.node.b == 1)
										{
											cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
											sampleinfo(&listres.node);
										}
									}
									cout << endl<<"输出完毕" <<endl;
									goto shuru;
								}
							}
							else cout << "Wrong Arguments!" <<endl;
						}
						freed(namelink);
						goto shuru;
					}
				if (!strcmp(command1, "plan"))//列出普通日志目录
					{
						tc[0] = getchar();
						if (tc[0] == '\n')
						{
							// 获取menu并输出：
							//发送命令包
							strcpy_s(listplanza.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
							strcpy_s(listplanza.head.planpath, sizeof(head.planpath), head.planpath);
							listplanza.head.online_tag = head.online_tag;
							send(clsock, (const char*)&listplanza, sizeof(listplanza), 0);//发送命令										
							recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
							head.online_tag = listres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
							strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

							if (listres.result == RES_OUTLINE)
							{
								cout << "ServerReturn: 请先登录" <<endl; goto shuru;
							}
							else if (listres.result == RES_FAIL)
							{
								cout << "ServerReturn: 没有数据" <<endl; goto shuru;
							}
							else if (listres.result == RES_SUCC)
							{
								cout << "输出目录" <<endl;
								cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
								while (listres.result != RES_FAIL)
								{
									//struct wlp temp;
									recv(clsock, (char*)&listres, sizeof(listres), 0);//
									head.online_tag = listres.head.online_tag;//更新head
									strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
									strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
									if (listres.result != RES_FAIL && listres.node.b == 0)
									{
										cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
										sampleinfo(&listres.node);
									}
								}
								cout <<endl<< "输出完毕" <<endl;
								goto shuru;
							}
						}
						else
						{
							cin >> command2;
							if (!strcmp(command2, "-za"))
							{
								// 获取menu并输出：
							//发送命令包
								strcpy_s(listplanza.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
								strcpy_s(listplanza.head.planpath, sizeof(head.planpath), head.planpath);
								listplanza.head.online_tag = head.online_tag;
								send(clsock, (const char*)&listplanza, sizeof(listplanza), 0);//发送命令										
								recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
								head.online_tag = listres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
								strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

								if (listres.result == RES_OUTLINE)
								{
									cout << "ServerReturn: 请先登录" <<endl; goto shuru;
								}
								else if (listres.result == RES_FAIL)
								{
									cout << "ServerReturn: 没有数据" <<endl; goto shuru;
								}
								else if (listres.result == RES_SUCC)
								{
									cout << "输出目录" <<endl;
									cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
									while (listres.result != RES_FAIL)
									{
										//struct wlp temp;
										recv(clsock, (char*)&listres, sizeof(listres), 0);//
										head.online_tag = listres.head.online_tag;//更新head
										strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
										strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
										if (listres.result != RES_FAIL && listres.node.b == 0)
										{
											cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
											sampleinfo(&listres.node);
										}
									}
									cout <<endl<< "输出完毕" <<endl;
									goto shuru;
								}
							}
							else if (!strcmp(command2, "-az"))
							{
								// 获取menu并输出：
								//发送命令包
								strcpy_s(listplanaz.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
								strcpy_s(listplanaz.head.planpath, sizeof(head.planpath), head.planpath);
								listplanaz.head.online_tag = head.online_tag;
								send(clsock, (const char*)&listplanaz, sizeof(listplanaz), 0);//发送命令										
								recv(clsock, (char*)&listres, sizeof(listres), 0);//接受一次返回信息确定受否有数据						
								head.online_tag = listres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);
								strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);

								if (listres.result == RES_OUTLINE)
								{
									cout << "ServerReturn: 请先登录" <<endl; goto shuru;
								}
								else if (listres.result == RES_FAIL)
								{
									cout << "ServerReturn: 没有数据" <<endl; goto shuru;
								}
								else if (listres.result == RES_SUCC)
								{
									cout << "输出目录" <<endl;
									cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" <<endl;
									while (listres.result != RES_FAIL)
									{
										//struct wlp temp;
										recv(clsock, (char*)&listres, sizeof(listres), 0);//
										head.online_tag = listres.head.online_tag;//更新head
										strcpy_s(head.logpath, sizeof(listres.head.logpath), listres.head.logpath);//客户端需要根据路径读取数据，故每次都要更新head
										strcpy_s(head.planpath, sizeof(listres.head.planpath), listres.head.planpath);
										if (listres.result != RES_FAIL && listres.node.b == 0)
										{
											cout << "  " << setw(20) << setiosflags(ios::left) << listres.node.name; cout << " " << "[" << listres.node.today << "]" << "   ";
											sampleinfo(&listres.node);
										}
									}
									cout <<endl<< "输出完毕" <<endl;
									goto shuru;
								}
							}
							else cout << "Wrong Arguments!" <<endl;
						}
						freed(namelink);
						goto shuru;
					}

					else {
						cout << "Wrong argument!" <<endl;
						goto shuru;
					}
				}
			
		}

		if (!strcmp(command, "cl"))//check logs in date
		{
			if (tc[0] == '\n')
			{
				struct CheckLog checklog;
				struct CheckRes checkres;
				struct wlp* temp = (struct wlp*)malloc(sizeof(struct wlp));
				struct data* temp1 = (struct data*)malloc(sizeof(struct data));
				strcpy_s(checklog.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
				strcpy_s(checklog.head.planpath, sizeof(head.planpath), head.planpath);
				checklog.head.online_tag = head.online_tag;
				strcpy_s(checklog.name, TIME_LENGTH, td);
				send(clsock, (const char*)&checklog, sizeof(checklog), 0);
				recv(clsock, (char*)&checkres, sizeof(checkres), 0);
				head.online_tag = checkres.head.online_tag;//更新head 
				strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
				strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
				if (checkres.result == RES_OUTLINE)
				{
					cout << "ServerReturn: 请先登录" <<endl; goto shuru;
				}
				else
				{
					while (checkres.result == RES_SUCC)
					{
						cout << checkres.node.name << "   [" << checkres.node.today << "]" << endl; cout << endl;
						recv(clsock, (char*)&checkres, sizeof(checkres), 0);
						head.online_tag = checkres.head.online_tag;//更新head 
						strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
						strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
						while (checkres.node.count > 0)
						{
							cout << " "<<checkres.node.lpdata.time << "   " << checkres.node.lpdata.info << endl;
							recv(clsock, (char*)&checkres, sizeof(checkres), 0);
							head.online_tag = checkres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
							strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
						}
						head.online_tag = checkres.head.online_tag;//更新head 
						strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
						strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
					}
					//if (checkres.result == RES_FAIL)
					{
						cout << "输出结束" << endl;
						goto shuru;
					}
				}			
			}	
			else
			{
				cin >> command1;
				tc[0] = getchar();
				if (tc[0] != '\n')
				{
					while (tc[0] != '\n')//吃掉多余的参数
					{
						tc[0] = getchar();
					}
					cout << "Wrong argument!" << endl;
					goto shuru;
				}
				else
				{
					
					struct CheckLog checklog;
					struct CheckRes checkres;
					struct wlp* temp = (struct wlp*)malloc(sizeof(struct wlp));
					struct data* temp1 = (struct data*)malloc(sizeof(struct data));
					strcpy_s(checklog.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
					strcpy_s(checklog.head.planpath, sizeof(head.planpath), head.planpath);
					checklog.head.online_tag = head.online_tag;
					strcpy_s(checklog.name, TIME_LENGTH, command1);
					send(clsock, (const char*)&checklog, sizeof(checklog), 0);
					recv(clsock, (char*)&checkres, sizeof(checkres), 0);
					head.online_tag = checkres.head.online_tag;//更新head 
					strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
					strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
					if (checkres.result == RES_OUTLINE)
					{
						cout << "ServerReturn: 请先登录" <<endl; goto shuru;
					}
					else
					{
						while (checkres.result == RES_SUCC)
						{
							cout << checkres.node.name << "   ["<< checkres.node.today<<"]" <<endl; cout << endl;
							recv(clsock, (char*)&checkres, sizeof(checkres), 0);
							head.online_tag = checkres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
							strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
							while (checkres.node.count > 0)
							{
								cout << checkres.node.lpdata.time << "   " << checkres.node.lpdata.info << endl;
								recv(clsock, (char*)&checkres, sizeof(checkres), 0);
								head.online_tag = checkres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
								strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
							}
							head.online_tag = checkres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
							strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
						}
						//if (checkres.result == RES_FAIL)
						{
							cout << "输出结束" <<endl;
							goto shuru;
						}
					}
				}
			}
		}

		if (!strcmp(command, "cp"))//check logs in date
		{
			if (tc[0] == '\n')
			{
				struct CheckPlan checkplan;
				struct CheckRes checkres;
				struct wlp* temp = (struct wlp*)malloc(sizeof(struct wlp));
				struct data* temp1 = (struct data*)malloc(sizeof(struct data));
				strcpy_s(checkplan.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
				strcpy_s(checkplan.head.planpath, sizeof(head.planpath), head.planpath);
				checkplan.head.online_tag = head.online_tag;
				strcpy_s(checkplan.name, TIME_LENGTH, td);
				send(clsock, (const char*)&checkplan, sizeof(checkplan), 0);
				recv(clsock, (char*)&checkres, sizeof(checkres), 0);
				head.online_tag = checkres.head.online_tag;//更新head 
				strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
				strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
				if (checkres.result == RES_OUTLINE)
				{
					cout << "请先登录" << endl; goto shuru;
				}
				else
				{
					while (checkres.result == RES_SUCC)
					{
						cout << checkres.node.name << "   [" << checkres.node.today<<"]" << endl; cout << endl;
						recv(clsock, (char*)&checkres, sizeof(checkres), 0);
						head.online_tag = checkres.head.online_tag;//更新head 
						strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
						strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
						while (checkres.node.count > 0)
						{
							cout << checkres.node.lpdata.time << "   " << checkres.node.lpdata.info << endl;
							recv(clsock, (char*)&checkres, sizeof(checkres), 0);
							head.online_tag = checkres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
							strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
						}
						head.online_tag = checkres.head.online_tag;//更新head 
						strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
						strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
					}
					//if (checkres.result == RES_FAIL)
					{
						cout << "输出结束" << endl;
						goto shuru;
					}
				}
			}
			else
			{
				cin >> command1;
				tc[0] = getchar();
				if (tc[0] != '\n')
				{
					while (tc[0] != '\n')//吃掉多余的参数
					{
						tc[0] = getchar();
					}
					cout << "Wrong argument!" <<endl;
					goto shuru;
				}
				else
				{

					struct CheckPlan checkplan;
					struct CheckRes checkres;
					struct wlp* temp = (struct wlp*)malloc(sizeof(struct wlp));
					struct data* temp1 = (struct data*)malloc(sizeof(struct data));
					strcpy_s(checkplan.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
					strcpy_s(checkplan.head.planpath, sizeof(head.planpath), head.planpath);
					checkplan.head.online_tag = head.online_tag;
					strcpy_s(checkplan.name, TIME_LENGTH, command1);
					send(clsock, (const char*)&checkplan, sizeof(checkplan), 0);
					recv(clsock, (char*)&checkres, sizeof(checkres), 0);
					head.online_tag = checkres.head.online_tag;//更新head 
					strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
					strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
					if (checkres.result == RES_OUTLINE)
					{
						cout << "请先登录" <<endl; goto shuru;
					}
					else
					{
						while (checkres.result == RES_SUCC)
						{
							cout << checkres.node.name << "   [" << checkres.node.today<<"]" <<endl; cout << endl;
							recv(clsock, (char*)&checkres, sizeof(checkres), 0);
							head.online_tag = checkres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
							strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
							while (checkres.node.count > 0)
							{
								cout << checkres.node.lpdata.time << "   " << checkres.node.lpdata.info << endl;
								recv(clsock, (char*)&checkres, sizeof(checkres), 0);
								head.online_tag = checkres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
								strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
							}
							head.online_tag = checkres.head.online_tag;//更新head 
							strcpy_s(head.logpath, sizeof(checkres.head.logpath), checkres.head.logpath);
							strcpy_s(head.planpath, sizeof(checkres.head.planpath), checkres.head.planpath);
						}
						//if (checkres.result == RES_FAIL)
						{
							cout << "输出结束" <<endl;
							goto shuru;
						}
					}
				}
			}
		}

		if (!strcmp(command, "log"))//写日志
		{
			struct Log log;
			if (tc[0] == '\n')
			{
				if (head.online_tag == STATE_OUTLINE) { cout << "ServerReturn: 请先登录" << endl; goto shuru; }
				else
				{
					cout << "AddData>> ";
					cin >> log.data; cout << endl;
					while (strcmp(log.data, "endw"))
					{
						strcpy_s(log.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
						strcpy_s(log.head.planpath, sizeof(head.planpath), head.planpath);
						log.head.online_tag = head.online_tag;
						strcpy_s(log.name, TIME_LENGTH, td);
						send(clsock, (const char*)&log, sizeof(log), 0);
						cout << "AddData>> ";
						cin >> log.data; cout << endl;
					}
					goto shuru;
				}
			}
			else
			{
				while (tc[0] != '\n')//吃掉多余的参数
				{
					tc[0] = getchar();
				}
				cout << "Wrong argument!" << endl;
				goto shuru;
			}
		}

		if (!strcmp(command, "prog"))
		{
			struct Prog prog;
			if (tc[0] == '\n')
			{
				cout << "No argument!" << endl;
				goto shuru;
			}
			else
			{
				cin >> command1;
				tc[0] = getchar();
				if (tc[0] != '\n')
				{
					while (tc[0] != '\n')//吃掉多余的参数
					{
						tc[0] = getchar();
					}
					cout << "Wrong argument!" << endl;
					goto shuru;
				}
				else
				{
					if (head.online_tag == STATE_OUTLINE) { cout << "ServerReturn: 请先登录" << endl; goto shuru; }
					else
					{
						cout << "AddData>> " ;
						cin >> prog.data; cout << endl;
						while (strcmp(prog.data, "endw"))
						{
							strcpy_s(prog.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
							strcpy_s(prog.head.planpath, sizeof(head.planpath), head.planpath);
							prog.head.online_tag = head.online_tag;
							strcpy_s(prog.name, TIME_LENGTH, command1);
							send(clsock, (const char*)&prog, sizeof(prog), 0);
							cout << "AddData>> ";
							cin >> prog.data; cout << endl;
						}
						goto shuru;
					}
				}
			}
		}

		if (!strcmp(command, "plan"))//写计划
		{
			struct Plan plan;
			if (tc[0] == '\n')//plan
			{
				if (head.online_tag == STATE_OUTLINE) { cout << "ServerReturn: 请先登录" << endl; goto shuru; }
				else
				{
					cout << "AddData>> " ;
					cin >> plan.data; cout << endl;
					while (strcmp(plan.data, "endw"))
					{
						strcpy_s(plan.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
						strcpy_s(plan.head.planpath, sizeof(head.planpath), head.planpath);
						plan.head.online_tag = head.online_tag;
						strcpy_s(plan.name, TIME_LENGTH, td);
						send(clsock, (const char*)&plan, sizeof(plan), 0);
						cout << "AddData>> ";
						cin >> plan.data; cout << endl;
					}
					goto shuru;
				}
			}
			else//plan [date]
			{
				cin >> command1;
				tc[0] = getchar();
				if (tc[0] != '\n')
				{
					while (tc[0] != '\n')//吃掉多余的参数
					{
						tc[0] = getchar();
					}
					cout << "Wrong argument!" << endl;
					goto shuru;
				}
				else
				{
					if (head.online_tag == STATE_OUTLINE) { cout << "ServerReturn: 请先登录" << endl; goto shuru; }
					else
					{
						cout << "AddData>> ";
						cin >> plan.data; cout << endl;
						while (strcmp(plan.data, "endw"))
						{
							strcpy_s(plan.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
							strcpy_s(plan.head.planpath, sizeof(head.planpath), head.planpath);
							plan.head.online_tag = head.online_tag;
							strcpy_s(plan.name, TIME_LENGTH, command1);
							send(clsock, (const char*)&plan, sizeof(plan), 0);
							cout << "AddData>> ";
							cin >> plan.data; cout << endl;
						}
						goto shuru;
					}
				}
			}
		}

		if (!strcmp(command, "del"))//删除功能
		{
			if (tc[0] == '\n')
			{
				cout << "No argument!" << endl;
				goto shuru;
			}
			else
			{
				cin >> command1;
				tc[0] = getchar();
				if (tc[0] == '\n')
				{
					cout << "Need more arguments!" << endl;
					goto shuru;
				}
				else
				{
					cin >> command2;
					tc[0] = getchar();
					if (tc[0] != '\n')
					{
						cout << "Too many argument!" << endl;
						while (tc[0] != '\n')//吃掉多余的参数
						{
							tc[0] = getchar();
						}
						goto shuru;
					}
					else
					{
						cout << command1 << " in " << command2 << " will be deleted.Countinue?(Y/N):";
						cin >> command3;
						if (!strcmp(command3, "y") || !strcmp(command3, "Y"))
						{
							if (!strcmp(command1, "log") || !strcmp(command1, "prog"))
							{
								//delnode(&wl, command2);
								struct DelLog dellog;
								struct DelRes delres;
								strcpy_s(dellog.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
								strcpy_s(dellog.head.planpath, sizeof(head.planpath), head.planpath);
								dellog.head.online_tag = head.online_tag;
								strcpy_s(dellog.name, DATA_NAME_LENGTH, command2);
								send(clsock, (const char*)&dellog, sizeof(dellog), 0);

								recv(clsock, (char*)&delres, sizeof(delres), 0);
								head.online_tag = delres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(delres.head.logpath), delres.head.logpath);
								strcpy_s(head.planpath, sizeof(delres.head.planpath), delres.head.planpath);
								if (delres.result == RES_OUTLINE)
								{
									cout << "ServerReturn: 请先登录！" <<endl; goto shuru;
								}
								else if (delres.result == RES_FAIL)
								{
									cout << "ServerReturn: 没有找到指定数据！" <<endl;
								}
								else if (delres.result == RES_SUCC)
								{
									cout << "ServerReturn: 删除成功！" <<endl;
								}
							}
							else if (!strcmp(command1, "plan"))
							{
								//delnode(&wp, command2);
								struct DelPlan delplan;
								struct DelRes delres;
								strcpy_s(delplan.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
								strcpy_s(delplan.head.planpath, sizeof(head.planpath), head.planpath);
								delplan.head.online_tag = head.online_tag;
								strcpy_s(delplan.name, DATA_NAME_LENGTH, command2);
								send(clsock, (const char*)&delplan, sizeof(delplan), 0);

								recv(clsock, (char*)&delres, sizeof(delres), 0);
								head.online_tag = delres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(delres.head.logpath), delres.head.logpath);
								strcpy_s(head.planpath, sizeof(delres.head.planpath), delres.head.planpath);
								if (delres.result == RES_OUTLINE)
								{
									cout << "ServerReturn: 请先登录！" <<endl; goto shuru;
								}
								else if (delres.result == RES_FAIL)
								{
									cout << "ServerReturn: 没有找到指定数据！" <<endl;
								}
								else if (delres.result == RES_SUCC)
								{
									cout << "ServerReturn: 删除成功！" <<endl;
								}
							}
							else cout << "Wrong argument!" << endl;
							goto shuru;
						}
						else goto shuru;

					}

				}
			}
		}

		if (!strcmp(command, "delall"))
		{
			if (tc[0] == '\n')
			{
				cout << "No argument!" << endl;
				goto shuru;
			}
			else
			{
				cin >> command1;
				tc[0] = getchar();
				if (tc[0] != '\n')
				{
					cout << "Too many argument!" << endl;
					while (tc[0] != '\n')//吃掉多余的参数
					{
						tc[0] = getchar();
					}
					goto shuru;
				}
				else
				{
					cout << "ALL " << command1 << " WILL BE DELETED.COUNTINUE?(Y/N)";
					cin >> command2;
					if (!strcmp(command2, "y") || !strcmp(command2, "Y"))
					{
						if (!strcmp(command1, "log")) 
						{ 
							//delall(wl); 
							struct DelAllLog delalllog;
							struct DelRes delres;
							strcpy_s(delalllog.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
							strcpy_s(delalllog.head.planpath, sizeof(head.planpath), head.planpath);
							delalllog.head.online_tag = head.online_tag;
							send(clsock, (const char*)&delalllog, sizeof(delalllog), 0);
							recv(clsock, (char*)&delres, sizeof(delres), 0);
							if (delres.result == RES_OUTLINE)
							{
								cout << "ServerReturn: 请先登录！" <<endl; goto shuru;
							}
							else if (delres.result == RES_SUCC)
							{
								cout << "ServerReturn: ALL " << command1 << " have been DELETED." <<endl;
							}
						}
						else if (!strcmp(command1, "plan")) 
						{ 
							struct DelAllPlan delallplan;
							struct DelRes delres;
							strcpy_s(delallplan.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
							strcpy_s(delallplan.head.planpath, sizeof(head.planpath), head.planpath);
							delallplan.head.online_tag = head.online_tag;
							send(clsock, (const char*)&delallplan, sizeof(delallplan), 0);
							recv(clsock, (char*)&delres, sizeof(delres), 0);
							if (delres.result == RES_OUTLINE)
							{
								cout << "ServerReturn: 请先登录！" <<endl; goto shuru;
							}
							else if (delres.result == RES_SUCC)
							{
								cout << "ServerReturn: ALL " << command1 << " have been DELETED." <<endl;
							}
						}
						else cout << "Wrong Argument!" << endl;
					}
					goto shuru;
				}
			}
		}

		if (!strcmp(command, "expd"))//暂时只用于导出开发日志
		{
			if (tc[0] == '\n')
			{
				cout << "No argument." << endl;
				goto shuru;
			}
			else
			{
				cin >> command1;
				tc[0] = getchar();
				if (tc[0] != '\n')
				{
					while (tc[0] != '\n')//吃掉多余的参数
					{
						tc[0] = getchar();
					}
					cout << "Wrong argument!" << endl;
					goto shuru;
				}
				else
				{
					//exportlog(wl, command1);
					struct Expd expd;
					struct ExpdRes expdres;
					struct wlp* temp = (struct wlp*)malloc(sizeof(struct wlp));
					struct data* temp1 = (struct data*)malloc(sizeof(struct data));
					strcpy_s(expd.head.logpath, sizeof(head.logpath), head.logpath);//向服务器发包前用head更新状态和路径 
					strcpy_s(expd.head.planpath, sizeof(head.planpath), head.planpath);
					expd.head.online_tag = head.online_tag;
					strcpy_s(expd.name, TIME_LENGTH, command1);
					send(clsock, (const char*)&expd, sizeof(expd), 0);
					recv(clsock, (char*)&expdres, sizeof(expdres), 0);
					head.online_tag = expdres.head.online_tag;//更新head 
					strcpy_s(head.logpath, sizeof(expdres.head.logpath), expdres.head.logpath);
					strcpy_s(head.planpath, sizeof(expdres.head.planpath), expdres.head.planpath);
					if (expdres.result == RES_OUTLINE)
					{
						cout << "ServerReturn: 请先登录！" <<endl; goto shuru;
					}
					else if (expdres.result == RES_FAIL)
					{
						cout << "导出失败" << endl; goto shuru;
					}
					else 
					{
						char expdpath[PATH_LENGTH] = { 0 };
						strcpy_s(expdpath, PATH_LENGTH, command1);
						strcat_s(expdpath, PATH_LENGTH, ".txt");
						FILE* fp;
						fopen_s(&fp, expdpath, "wb");
						if (fp == NULL) {
							cout << "文件建立失败" << endl;
							goto shuru;
						}
						else
						{
							cout << "接收中..." << endl;
							while (expdres.result == RES_SUCC)
							{
								char buff[BUF_SIZE] = { 0 };								
								int n = fwrite(expdres.buff, 1, sizeof(expdres.buff), fp);
								recv(clsock, (char*)&expdres, sizeof(expdres), 0);
								head.online_tag = expdres.head.online_tag;//更新head 
								strcpy_s(head.logpath, sizeof(expdres.head.logpath), expdres.head.logpath);
								strcpy_s(head.planpath, sizeof(expdres.head.planpath), expdres.head.planpath);
							}
							cout << "导出完成！" << endl;
							
							fclose(fp);
							goto shuru;
						}
						
					}		
				}
			}
		}

		if (!strcmp(command, "exit"))
		{
			struct Exit exit;
			struct ExitRes exitres;
			send(clsock, (const char*)&exit, sizeof(exit), 0);
			recv(clsock, (char*)&exitres, sizeof(exitres), 0);
			if (exitres.result == 1) goto finish;
			else cout << "ServerReturn: 退出时服务器错误！" <<endl;
		}
		else
		{
			tc[0] = getchar();
			cout << "Can't find command such command in command list." << endl;
			while (tc[0] != '\n')//吃掉多余的参数
			{
				tc[0] = getchar();
			}
			goto shuru;
		}
	}
	
finish://停用socket

	closesocket(clsock);

	//停用dll
	WSACleanup();
}