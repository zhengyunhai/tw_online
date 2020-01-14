//用户信息处理函数
#include<iostream>
#include<fstream>
#include<iomanip>
#include<cstdio>
#include<cstdlib>
#include<string>
#include<ctime>
#include<direct.h>
#include<windows.h>
#include"..\\include\logsoft.h"
using namespace std;
char registerpath[PATH_LENGTH] = ".\\user\\registerblock";//仅作测试用
//struct wlp* block = NULL;

void GetUsernameFromPath(char*name,char* logpath,char* dirpath)
{
	int len = strlen(dirpath);
	int i = 0;
	while (logpath[len])
	{
		len++;
		name[i] = logpath[len];
		i++;
	}
	cout <<"name="<< name << endl;
}

void OutBlockToFile(char* path, struct wlp* wl)//仅用于保存注册表
{
	ofstream outf;
	struct wlp* wtemp = wl;
	struct wlp* lend;
	struct data* temp = NULL;
	lend = wtemp->next;//第一个节点
	if (wtemp == NULL) { cout << "No data to save!" << endl; }
	else
	{
		cout << "Saving..." << endl;
		outf.open(path);
		while (lend != NULL)
		{
			outf << lend->name << " " << lend->today << " " << lend->b << " " << lend->count << " " << lend->lpdata.time << endl << lend->lpdata.info << endl;
			temp = &(lend->lpdata);
			while (temp->next != NULL)
			{
				temp = temp->next;
				outf << temp->time << endl << temp->info << endl;
			}
			lend = lend->next;
		}
		outf.close();
		cout << "Save finished！" << endl;
	}
}

int SearchNameinBlock(struct wlp* wl, char* username)//仅用于注册时
{
	struct wlp* temp;
	int count = 0;
	temp = wl;
	if (temp == NULL) return -1;
	else
	{
		//temp = findtail(wl);
		while (temp != NULL)
		{
			if (!strcmp(temp->name, username))
			{
				//freed(temp);
				count++;
				//return 2;//用户名与密码均匹配
			}
			temp = temp->next;
		}
		//freed(temp);
		
		return count;//没有匹配项
	}
}

int SearchnNameandPassinBlock(struct wlp* wl, char* username, char* password)//仅用于登录检测
{
	struct wlp* temp;
	int count = 0;
	temp = wl;
	if (temp == NULL) return -1;
	else
	{
		while (temp != NULL)
		{
			if (!strcmp(temp->name, username)&&!strcmp(temp->today,password))
			{
				//freed(temp);
				count++;
				//return 2;//用户名与密码均匹配
			}
			temp = temp->next;
		}
		//freed(temp);

		return count;//没有匹配项
	}
}

int RegistertInBlock(char* username, char* password)//向注册表添加信息
{
	struct wlp* block = NULL;
	loadfile(registerpath, &block);
	struct wlp* book, * temp, * end, * head, * head1;//定义结构体指针 ，局部变量 
	end = block;
	head = block;
	book = NULL;
	temp = NULL;
	head1 = NULL;
	end = end->next;//指向第一个节点

	book = (struct wlp*)malloc(sizeof(struct wlp));//分配空间 ，注意类型转换 
	if (book == NULL) { printf("Memory allocation failed.\n"); exit(1); }
	initnode(&book, 0);
	strcpy_s(book->name, USER_NAME_LENGTH, username);
	strcpy_s(book->today, PASSWORD_LENGTH, password);		
	book->count = 1;
	strcpy_s(book->lpdata.time, TIME_LENGTH,getday());//注册时间（服务器时间）
	
	temp = block->next;
	block->next = book;
	//book->pre = head;
	book->next = temp;
	//if(temp!=NULL)temp->pre = book;
	

	OutBlockToFile(registerpath, block);
	freed(block);
	return 0;
}


bool verif(char* username, char* password)//登录时验证用户身份合法性，合法返回true.非法返回false
{
	struct wlp* block = NULL;
	loadfile(registerpath, &block);
	//搜索用户名
	cout <<"verify:"<< SearchnNameandPassinBlock(block, username, password) << endl;
	if (1 == SearchnNameandPassinBlock(block, username,password))
	{
		//outtofile(registerpath, block);
		freed(block);
		return true;
	}
	else
	{
		//outtofile(registerpath, block);
		freed(block);
		return false;
	}
	
}

bool exist(char *username,char *password)//注册时，遍历注册表，用户存在返回true,不存在返回false
{
	struct wlp* block = NULL;
	loadfile(registerpath, &block);
	if (0 == SearchNameinBlock(block, username))//不存在，可以注册
	{
		outtofile(registerpath, block);
		freed(block);
		return false;
	}
	else
	{
		outtofile(registerpath, block);
		freed(block);
		return true;
	}
}

bool regi()//注册，成功返回ture，失败返回false
{
	return false;
}

void log_rpath(char username)//创建用户文件路径
{

}
void plan_path(char username) 
{

}

bool is_online(char id)//检测用户登录状态,在线返回true//思路，使用链表存储在线用户名，每次调用该函数时，再链表中查询是否有对应用户
{
	return true;
}