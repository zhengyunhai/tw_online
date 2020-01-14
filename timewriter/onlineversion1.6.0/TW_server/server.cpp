#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<vector>
#include<direct.h>
#include"..\\include\data_package.h"
#include"..\\include\logsoft.h"
#include"userfunc.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS -1

#pragma comment(lib,"ws2_32.lib")
using namespace std;
//#define BUF_SIZE 1024
//使用前请在安装目录下手动添加user文件夹
vector<SOCKET> g_cl;
char temp[20] = {0};
int main(void)
{
	
	//初始化dll
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	//创建socket
	SOCKET sesock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sesock) { cout << "socket build failed." << endl; }
	//绑定socket
	struct sockaddr_in sesockadd;
	memset(&sesockadd, 0, sizeof(sesockadd));
	sesockadd.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &sesockadd.sin_addr);
	sesockadd.sin_port = htons(1234);
	if (-1 == bind(sesock, (sockaddr*)&sesockadd, sizeof(sesockadd))) { cout << "bind failed" << endl; }
	//监听
	if (INVALID_SOCKET == listen(sesock, 20)) { cout << "listen failed" << endl; }

	timeval tv;
	tv.tv_sec = 8;
	while (1)
	{
		fd_set fdread;
		FD_ZERO(&fdread);
		FD_SET(sesock,&fdread);
		for (int n = (int)g_cl.size() - 1;n >= 0; n--)
		{
			FD_SET(g_cl[n], &fdread);
		}

		int ret=select(sesock + 1, &fdread, NULL, NULL, &tv);
		if (ret < 0) { cout << "select return " << ret << endl; }

		if (FD_ISSET(sesock, &fdread)) 
		{
			FD_CLR(sesock, &fdread);
			//接受请求
			sockaddr_in clsockadd;
			int len = sizeof(clsockadd);
			SOCKET clsock = accept(sesock, (sockaddr*)&clsockadd, &len);
			if (INVALID_SOCKET == clsock) { cout << "accept failed." << endl; }
			g_cl.push_back(clsock);
			//
			inet_ntop(AF_INET, (void*)&clsockadd.sin_addr,temp,sizeof(temp));
			cout << "new client:" << temp << " has been connected." << endl;
		}
		for (size_t n = 0; n < fdread.fd_count; n++)
		{
			//处理信息
			//proc(fdread.fd_array[n]);
			if (-1== proc(fdread.fd_array[n]))//对数据做操作，若返回0,从向量中移除
			{
				auto iter = find(g_cl.begin(), g_cl.end(), fdread.fd_array[n]);//寻找对应向量元素 
				if (iter != g_cl.end()) g_cl.erase(iter);//删除该元素 
			}
		}
	}

	//停用socket
	for (size_t n = g_cl.size() - 1; n >=0; n--)
	{
		closesocket(g_cl[n]);
	}
	closesocket(sesock);

	//停用dll
	WSACleanup();
}
