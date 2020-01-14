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

const char* VER = "1.6.0";
using namespace std;

void f0(char* ti)//填充0
{
	char temp[8] = "0";
	strcat_s(temp, sizeof(temp), ti);
	strcpy_s(ti, sizeof(temp), temp);
}

int addinfo(struct wlp* node,char* infodata)
{
	char* tt=gettime();
	struct wlp* temphead = node;
	struct data* temp;
	struct data* endn;
	char temp2[DATA_LENGTH] = { 0 };
	temphead = temphead->next;//链表第一个节点
	endn = &(temphead->lpdata);
	strcpy_s(temp2, DATA_LENGTH, infodata);
	
		temp = (struct data*)malloc(sizeof(struct data));
		if (temp == NULL) { cout << "Memory allocation failed." << endl; exit(1); }
		//tt = gettime();
		strcpy_s(temp->time, TIME_LENGTH, tt);
		temp->next = NULL;
		strcpy_s(temp->info, strlen(temp2) + 1, temp2);

		while (endn->next != NULL)
		{
			endn = endn->next;
		}
		endn->next = temp;
		temphead->count++;
		
	
	return 0;
}

char* getday()//获取当前日期
{
	char temp1[TIME_LENGTH] = { 0 };
	char temp2[8] = { 0 };
	char temp3[8] = { 0 };
	char* r;
	r = (char*)malloc(sizeof(temp1));
	if (r == NULL) { cout << "Memory allocation failed." << endl; exit(1); }
	time_t sec = time(0);
	struct tm now;
	localtime_s(&now, &sec);
	_itoa_s(now.tm_year + 1900, temp1, 10);
	_itoa_s(now.tm_mon + 1, temp2, 10);
	if (now.tm_mon < 10) f0(temp2);
	_itoa_s(now.tm_mday, temp3, 10);
	if (now.tm_mday < 10) f0(temp3);
	strcat_s(temp1, sizeof(temp1), temp2);
	strcat_s(temp1, sizeof(temp1), temp3);//temp1返回日期
	strcpy_s(r, TIME_LENGTH, temp1);
	return r;
}

char* gettime()//获取当前时间
{
	char temp1[TIME_LENGTH] = { 0 };
	char temp2[8] = { 0 };
	char temp3[8] = { 0 };
	char temp4[TIME_LENGTH] = "[";
	char* r;
	r = (char*)malloc(sizeof(temp1));
	if (r == NULL) { cout << "Memory allocation failed." << endl; exit(1); }
	time_t sec = time(0);
	struct tm now;
	localtime_s(&now, &sec);
	_itoa_s(now.tm_hour, temp1, 10);
	if (now.tm_hour < 10) f0(temp1);
	_itoa_s(now.tm_min, temp2, 10);
	if (now.tm_min < 10) f0(temp2);
	strcat_s(temp1, sizeof(temp1), ":");
	strcat_s(temp1, sizeof(temp1), temp2);
	strcat_s(temp4, sizeof(temp4), temp1);
	strcat_s(temp4, sizeof(temp4), "]");
	strcpy_s(r, TIME_LENGTH, temp4);
	return r;
}

struct wlp* findtail(struct wlp* node)
{
	//struct wlp* end = NULL;
	struct wlp* head = node;

		while (head->next != NULL)
		{
			head = head->next;
		}
	
	return head;

}

void initnode(struct wlp** head, int i)//未完成，i=1时？
{
	struct wlp* r = *head;
	if (i == 0)
	{
		strcpy_s(r->name, DATA_NAME_LENGTH, "LOGNAME");
		strcpy_s(r->today, TIME_LENGTH, "LOGHEAD");
		r->count = 1;
		r->b = 0;
		strcpy_s(r->lpdata.time, TIME_LENGTH, "LOGTIME");
		strcpy_s(r->lpdata.info, TIME_LENGTH, "LOGINFO");
		r->pre = NULL;
		r->next = NULL;
		r->lpdata.next = NULL;
	}
	if (i == 1)
	{
		strcpy_s(r->name, 20, "LOGNAME");
		strcpy_s(r->today, 13, "LOGHEAD");
		r->count = 1;
		r->b = 0;
		strcpy_s(r->lpdata.time, 13, "LOGTIME");
		strcpy_s(r->lpdata.info, 13, "LOGINFO");
		r->next = NULL;
		r->lpdata.next = NULL;
	}
}

int loadfile(char* path, struct wlp** wl)//假定文件存在
{
	ifstream inf;
	ofstream outf;
	char temp1[DATA_NAME_LENGTH];//缓存最先读入的name
	struct wlp* lend, * head;
	lend = NULL;
	head = (struct wlp*)malloc(sizeof(struct wlp));
	if (head == NULL) { cout << "Memory allocation failed." << endl; exit(1); }
	initnode(&head, 0);
	*wl = head;
	lend = head;
	
	inf.open(path);
	if (!inf.is_open()) { return 2; }
	else
	{
		while (inf >> temp1)//读入链表
		{
			struct wlp* temp2;
			struct data* tempend1, * nn;
			int i = 2;
			tempend1 = NULL;
			nn = NULL;
			temp2 = (struct wlp*)malloc(sizeof(struct wlp));
			if (temp2 == NULL) { cout << "Memory allocation failed." << endl; exit(1); }
			initnode(&temp2, 0);
			strcpy_s(temp2->name, strlen(temp1) + 1, temp1);
			inf >> temp2->today;
			inf >> temp2->b;
			inf >> temp2->count;
			inf >> temp2->lpdata.time;
			inf >> temp2->lpdata.info;
			temp2->lpdata.next = NULL;
			temp2->next = NULL;
			lend->next = temp2;
			temp2->pre = lend;
			lend = temp2;
			tempend1 = &(temp2->lpdata);
			while (i <= temp2->count)
			{
				nn = (struct data*)malloc(sizeof(struct data));
				if (nn == NULL) { cout << "Memory allocation failed." << endl; exit(1); }
				inf >> nn->time;
				inf >> nn->info;
				nn->next = NULL;
				tempend1->next = nn;
				tempend1 = nn;
				i++;
			}
		}
	}
	inf.close();
	return 0;
}

void outtofile(char* path, struct wlp* wl)
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
			outf << lend->name << " " << lend->today << " " << lend->b << " " << lend->count << endl << lend->lpdata.time << endl << lend->lpdata.info << endl;
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

void adddata(struct wlp** library, char* name, char*infodata, int b)//new version//可以继续优化
											  //原理过程说明：查看第一个节点是否为目标节点，是则在其上添加数据，否则新建节点并插入，即只检测第一个节点，并非遍历整个链表
	//b用来标记当前添加的节点类型
{
	char* td = getday();
	char* tt ;
	struct wlp* book, * temp, * end, * head, * head1;//定义结构体指针 ，局部变量 
	end = *library;
	head = *library;
	book = NULL;
	temp = NULL;
	head1 = NULL;
	//搜索节点（只看第一个节点）（头节点后的第一个节点）	
	if (*library == NULL)//空链表 ，只有空的头指针//由于loadfile()的存在，不可能为空链表，至少有一个头节点
	{
		book = (struct wlp*)malloc(sizeof(struct wlp));//分配空间 ，注意类型转换 
		if (book == NULL)//分配失败 
		{
			printf("Memory allocation failed.\n");
			exit(1);
		}
		initnode(&book, 0);
		strcpy_s(book->name, DATA_NAME_LENGTH, name);
		strcpy_s(book->today, TIME_LENGTH, td);
		book->b = b;
		book->count = 1;
		strcpy_s(book->lpdata.info, DATA_LENGTH,infodata);
		if (!strcmp(book->lpdata.info, "endw"))
		{
			free(book);
		}
		else
		{
			tt = gettime();
			strcpy_s(book->lpdata.time, TIME_LENGTH, tt);
			book->lpdata.next = NULL;
			book->next = NULL;
			addinfo(book,td);
			head1 = (struct wlp*)malloc(sizeof(struct wlp));
			if (head1 == NULL) { cout << "Memory allocation failed." << endl; exit(1); }
			initnode(&head1, 0);
			*library = head1;//头节点
			head1->next = book;//注意book是指针，这里是传递地址 
		}
	}
	else//非空链表 ,有头节点，头插法
	{
		end = end->next;//指向第一个节点
		if (end == NULL)//第一个节点为空，只有一个头节点
		{
			book = (struct wlp*)malloc(sizeof(struct wlp));//分配空间 ，注意类型转换 
			if (book == NULL) { printf("Memory allocation failed.\n"); exit(1); }
			initnode(&book, 0);
			strcpy_s(book->name, DATA_NAME_LENGTH, name);
			strcpy_s(book->today, TIME_LENGTH, td);
			book->b = b;
			book->count = 1;
			strcpy_s(book->lpdata.info, DATA_LENGTH, infodata);
			if (!strcmp(book->lpdata.info, "endw"))
			{
				free(book);
			}
			else
			{
				tt = gettime();
				strcpy_s(book->lpdata.time, TIME_LENGTH, tt);
				book->lpdata.next = NULL;
				book->next = NULL;
				//addinfo(book,td);
				temp = head->next;
				head->next = book;
				book->pre = head;
				book->next = temp;
				if (temp != NULL)temp->pre = book;
			}
		}
		else if (!strcmp(end->name, name) && !strcmp(end->today, td))//目标节点已存在,即第一个节点为目标节点，日志本身是以日期为名，故可以用name判断，但开发日志以项目为名，同时以日期进行判断
		{
			addinfo(*library,infodata);
		}
		else//目标节点不存在，即第一个节点不是目标节点
		{
			book = (struct wlp*)malloc(sizeof(struct wlp));//分配空间 ，注意类型转换 
			if (book == NULL)//分配失败 
			{
				printf("Memory allocation failed.\n");
				exit(1);
			}
			initnode(&book, 0);
			strcpy_s(book->name, DATA_NAME_LENGTH, name);
			strcpy_s(book->today, TIME_LENGTH, td);
			book->b = b;
			book->count = 1;
			strcpy_s(book->lpdata.info, DATA_LENGTH, infodata);
			if (!strcmp(book->lpdata.info, "endw"))
			{
				free(book);
			}
			else
			{
				tt = gettime();
				strcpy_s(book->lpdata.time, TIME_LENGTH, tt);
				book->lpdata.next = NULL;
				book->next = NULL;
				//addinfo(book,infodata);
				temp = head->next;
				head->next = book;
				book->pre = head;
				book->next = temp;
				temp->pre = book;
			}
		}
	}

}

void showinfo(struct wlp* node)
{
	struct data* end;
	cout <<" "<< node->today << endl<<endl;
	end = &(node->lpdata);
	while (end != NULL)
	{
		cout << " " << end->time << " " << end->info << endl;
		end = end->next;
	}
	cout << endl;
}

void searchdata(struct wlp** namelink, struct wlp* wl, char* name)
{
	struct wlp* temp,*temp2;
	//*namelink = (struct wlp*)malloc(sizeof(struct wlp));
	int count = 0;
	temp = wl;
	if (temp == NULL) cout << "Empty Data." << endl;
	else
	{	
		temp = findtail(wl);
		while (temp != NULL)
		{
			if (!strcmp(temp->name, name))
			{
				temp2= (struct wlp*)malloc(sizeof(struct wlp));
				initnode(&temp2,0);
				{
					
				}
				count++;
			}
			temp = temp->pre;
		}
		if (count == 0) cout << "There's no data for '" << name << "'" << endl;
	}
}

void freei(struct data* node)
{
	struct data* temp = NULL;
	while (node != NULL)
	{
		temp = node;
		node = node->next;
		free(temp);
	}
}

void freed(struct wlp* wl)
{
	struct wlp* temp = NULL;
	struct data* temp2 = NULL;
	while (wl != NULL)
	{
		temp = wl;
		temp2 = wl->lpdata.next;
		wl = wl->next;
		freei(temp2);
		free(temp);
	}
}

void delnode(struct wlp** node, char* name)//注意这会删除与name同名的所有节点
{
	struct wlp* temp;
	struct wlp* head;
	int i = 0;
	head = *node;
	temp = NULL;

	if (*node == NULL) {
		cout << "No data for '" << name << "'" << endl;
	}
	else
	{
		temp = head;
		head = head->next;
		if (head == NULL) {//第一个节点为空
			cout << "No data for '" << name << "'" << endl;
		}
		else
		{
			while (head != NULL)
			{
				if (strcmp(head->name, name))
				{
					temp = head;
					head = head->next;
				}
				else
				{
					temp->next = head->next;
					freei(head->lpdata.next);
					free(head);
					i = i + 1;
					head = temp->next;
				}

			}

		}
	}
	//shuchu:
	if (i == 1)cout << i << " Data node '" << name << "' has been deleted!" << endl;
	else if (i > 1)cout << i << " Data nodes '" << name << "' have been deleted!" << endl;
	else cout << "No such data '" << name << "'" << endl;
}

void delall(struct wlp* node)
{
	struct wlp* temp;
	temp = node->next;
	node->next = NULL;
	freed(temp);
}

void sampleinfo(struct wlp* node)
{
	if (node->count != 0)
	{
		int i = 0;
		int N;
		char temp[21] = { 0 };
		if (strlen(node->lpdata.info) >= 12)
		{
			N = 12;
		}
		else N = strlen(node->lpdata.info);

		for (i = 0; i < N; i++)
		{
			temp[i] = node->lpdata.info[i];
		}
		if (node->count > 1 || N == 12)strcat_s(temp, sizeof(temp), " ... ...");

		cout << temp;
	}
	cout << endl;
}

int is_node(struct wlp* node, char* name)
{
	struct wlp* temp;
	temp = node;
	int i = 0;
	while (temp != NULL)
	{
		if (!strcmp(temp->name, name))
		{
			i++;
		}
		temp = temp->next;
	}
	return i;
}

void exportlog(struct wlp* node, char* name,char* path)
{
	
	
	ofstream outf;
	if (!is_node(node, name))
	{
		cout << "No data can be exported." << endl;
	}
	else
	{
		outf.open(path);
		struct wlp* temp;
		struct data* end;
		//temp = node->next;
		temp = findtail(node);
		while (temp != NULL)
		{
			if (!strcmp(temp->name, name))
			{
				outf << temp->today << endl;
				end = &(temp->lpdata);
				while (end != NULL)
				{
					outf << end->time << " " << end->info << endl;
					end = end->next;
				}
			}
			temp = temp->pre;
		}
		outf.close();
		cout << "Data '" << name << "' has been exported in TXT.'" << endl;
	}
}

void outlist()
{
	cout << "  " << "                                         " << getday() << " " << gettime() << endl;
	cout << "  " << "—————————————————————————————" << endl
		<< "  " << "|                    * Command list *                    |" << endl
		<< "  " << "—————————————————————————————" << endl

		<< "  " << "—————————————————————————————" << endl
		<< "  " << "|     -help                   -show command list         |" << endl
		<< "  " << "—————————————————————————————" << endl
		<< "  " << "|     login                   -log into your account     |" << endl
		<< "  " << "|     logout                  -logout account            |" << endl
		<< "  " << "|     register                -Registered accounts       |" << endl
		<< "  " << "—————————————————————————————" << endl
		<< "  " << "|     list [log/prog/plan]    -show logmenu              |" << endl
		<< "  " << "|               -az           -order by last time        |" << endl
		<< "  " << "|               -za           -order by created time     |" << endl
		<< "  " << "—————————————————————————————" << endl
		<< "  " << "|     log                     -to start writing a log    |" << endl
		<< "  " << "|     prog [progname]         -to start writing a proglog|" << endl
		<< "  " << "|     endw                    -to finish writing the log |" << endl
		<< "  " << "|     cl [name]               -check log/prog            |" << endl
		<< "  " << "—————————————————————————————" << endl
		<< "  " << "|     plan [name]             -to start writing a plan   |" << endl
		<< "  " << "|     endw                    -to finish writing the plan|" << endl
		<< "  " << "|     cp [date/name]          -check plan in date        |" << endl
		<< "  " << "—————————————————————————————" << endl
		<< "  " << "|     del [log/plan] [name]   -delete data               |" << endl
		<< "  " << "|     delall [log/plan]       -delete data               |" << endl
		<< "  " << "—————————————————————————————" << endl
		<< "  " << "|     expd [name]             -export log data           |" << endl
		//<< "  " << "|     save                    -save data                 |" << endl
		//<< "  " << "|     save [log/plan]         -save data                 |" << endl
		<< "  " << "|     exit                    -save and exit             |" << endl
		<< "  " << "—————————————————————————————" << endl
		<< "  " << "*         DO USE 'exit' TO FINISH THIS PROGRAM ,         *" << endl
		<< "  " << "*         OTHERWISE YOUR DATA WILL NOT BE SAVED.         *" << endl
		<< "  " << "—————————————————————————————" << endl;
	/*	<< "  " << "*           DO NOT use 'exit' before you really          *" << endl
		<< "  " << "*               want to FINISH this program.             *" << endl
		<< "  " << "—————————————————————————————" << endl;
	*/
}

void headlist()
{
	cout << "  " << "———————————————————————————" << endl
		<< "  " << "|                     Time Writer                    |" << endl
		<< "  " << "———————————————————————————" << endl
		<< "  " << "|                    Version " << VER << "                   |" << endl
		<< "  " << "———————————————————————————" << endl
		<< "  " << "|     https://github.com/zhengyunhai/tw_online.git   |" << endl
		<< "  " << "———————————————————————————" << endl;
}

int menu(struct wlp** namelink, struct wlp* node,int tag)
{
	int count = 0;
	struct wlp* head, * temp, * temp1;
	head = node->next;
	*namelink = (struct wlp*)malloc(sizeof(struct wlp));
	initnode(namelink, 0);
	temp = *namelink;//头节点不存信息
	if (1==tag)//从前到后，即按照更新的顺序从新到旧输出
	{
		while (head != NULL)
		{
			if (!is_node(*namelink, head->name))//若namelink中没有head->name，则将其加入
			{
				temp1 = (struct wlp*)malloc(sizeof(struct wlp));
				initnode(&temp1, 0);
				strcpy_s(temp1->name, sizeof(head->name), head->name);//名称
				temp1->b = head->b;//日志类型
				strcpy_s(temp1->today, sizeof(head->today), head->today);//日期
				temp1->count = head->count;//子节点计数
				strcpy_s(temp1->lpdata.info, sizeof(head->lpdata.info), head->lpdata.info);//数据
				temp->next = temp1;
				temp = temp1;
				count++;
			}
			head = head->next;
		}
	}
	if (-1 == tag)//按照从旧到新输出
	{
		
		head = findtail(node);
		while (head != NULL)
		{
			if (!is_node(*namelink, head->name))
			{
				temp1 = (struct wlp*)malloc(sizeof(struct wlp));
				initnode(&temp1, 0);
				strcpy_s(temp1->name, sizeof(head->name), head->name);
				temp1->b = head->b;
				strcpy_s(temp1->today, sizeof(head->today), head->today);
				temp1->count = head->count;
				strcpy_s(temp1->lpdata.info, sizeof(head->lpdata.info), head->lpdata.info);
				temp->next = temp1;
				temp = temp1;
				count++;
			}
			head = head->pre;
		}
	}
	return count;
}

void printmenu(struct wlp* namelink,int tag)
{
	struct wlp* temp;

			temp = namelink->next;
			if (temp == NULL)cout << "No Logs!" << endl;
			else
			{
				cout << "  " << setw(20) << setiosflags(ios::left) << "Name"; cout << "  " << setw(10) << "Date" << "  " << "SampleInfo" << endl << endl;
				while (temp != NULL)
				{
					if (temp->b == tag)
					{
						cout << "  " << setw(20) << setiosflags(ios::left) << temp->name; cout << " " << "[" << temp->today << "]" << "   ";
						sampleinfo(temp);
					}
					temp = temp->next;
				}
			}	
}

void copynode()
{

}
