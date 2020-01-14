//logsoft.h

#include"defination.h"



//结构体声明
typedef struct data
{
	char time[TIME_LENGTH];//1.用于日志中可以存储日志时间，2.用于注册表中可以存储注册日期
	char info[DATA_LENGTH];//字数限制200？？？
	data* next;
}data;

typedef struct wlp
{
	char name[DATA_NAME_LENGTH];//存储username
	char today[TIME_LENGTH];//存储password
	int b;//b=0,普通日志,b=1,开发日志
	int count;//计数该分支上节点个数，以确定该分支何时结束
	struct data lpdata;
	wlp* pre;
	wlp* next;
}wlp;

void f0(char* ti);
//时间格式填充0

char* getday();
//获取当前日期，返回格式例20771201

char* gettime();
//获取当前时间，返回格式例[19:01]

struct wlp* findtail(struct wlp* node);
//返回链表的尾节点地址

void initnode(struct wlp** head, int i);
//节点初始化

int loadfile(char* path, struct wlp** wl);
//载入文件

void outtofile(char* path, struct wlp* wl);
//输出到文件

void adddata(struct wlp** library, char* name,char*infodata, int b);
//向链表中添加数据，若第一个节点name，data与参数相同则在此节点添加，否则新建节点添加并作为新的第一个节点

void showinfo(struct wlp* node);
//打印node本节点及其子链表信息

void searchdata(struct wlp* wl, char* name);
//搜索name同名节点

int addinfo(struct wlp* node);
//在node节点子链表上添加信息

void freed(struct wlp* wl);
//释放链表

void freei(struct data* node);
//释放子链表

void outlist();
//输出指令列表

void delnode(struct wlp** node, char* name);
//删除name同名节点

void sampleinfo(struct wlp* node);
//打印简略信息

void exportlog(struct wlp* node, char* name,char* path);
//导出node链表中名为name的节点到同名txt中,tag=1,正序，tag=-1,逆序

void headlist();
//打印软件名版本号等

int is_node(struct wlp* node, char* name);
//返回链表中与name同名的节点个数

void delall(struct wlp* node);
//删除全部节点

int menu(struct wlp** namelink, struct wlp* node,int tag);
//将node链表中节点按顺序保存首次出现的节点名到namelink中,tag=1,正序，tag=-1,逆序

void printmenu(struct wlp* namelink,int tag);
//打印目录，tag=0,打印普通日志，tag=1，打印开发日志

void copynode();
//复制节点到目标位置，暂无

//字符串匹配模板
/*
char command[10],command1[10],command2[10];
char tc;
cin>>command;
tc=getchar();
if (!strcmp(command, "字符串"))
{
	if ('\n' == tc)
	{
		cout << "WrongArgument" << endl;
		goto shuru;
	}
	else
	{
		cin >> command1;
		tc = getchar();
		if ('\n' != tc)
		{
			while ('\n' != tc)tc = getchar();//吃掉多余参数
		}
		else
		{
			if (!strcmp(command1, "字符串1")
			{
				//操作
			}
			else cout<<"WrongArguments"<<endl;
		}

	}
}
*/

