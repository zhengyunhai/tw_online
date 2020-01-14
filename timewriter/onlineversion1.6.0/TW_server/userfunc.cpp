//�û���Ϣ������
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
char registerpath[PATH_LENGTH] = ".\\user\\registerblock";//����������
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

void OutBlockToFile(char* path, struct wlp* wl)//�����ڱ���ע���
{
	ofstream outf;
	struct wlp* wtemp = wl;
	struct wlp* lend;
	struct data* temp = NULL;
	lend = wtemp->next;//��һ���ڵ�
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
		cout << "Save finished��" << endl;
	}
}

int SearchNameinBlock(struct wlp* wl, char* username)//������ע��ʱ
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
				//return 2;//�û����������ƥ��
			}
			temp = temp->next;
		}
		//freed(temp);
		
		return count;//û��ƥ����
	}
}

int SearchnNameandPassinBlock(struct wlp* wl, char* username, char* password)//�����ڵ�¼���
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
				//return 2;//�û����������ƥ��
			}
			temp = temp->next;
		}
		//freed(temp);

		return count;//û��ƥ����
	}
}

int RegistertInBlock(char* username, char* password)//��ע��������Ϣ
{
	struct wlp* block = NULL;
	loadfile(registerpath, &block);
	struct wlp* book, * temp, * end, * head, * head1;//����ṹ��ָ�� ���ֲ����� 
	end = block;
	head = block;
	book = NULL;
	temp = NULL;
	head1 = NULL;
	end = end->next;//ָ���һ���ڵ�

	book = (struct wlp*)malloc(sizeof(struct wlp));//����ռ� ��ע������ת�� 
	if (book == NULL) { printf("Memory allocation failed.\n"); exit(1); }
	initnode(&book, 0);
	strcpy_s(book->name, USER_NAME_LENGTH, username);
	strcpy_s(book->today, PASSWORD_LENGTH, password);		
	book->count = 1;
	strcpy_s(book->lpdata.time, TIME_LENGTH,getday());//ע��ʱ�䣨������ʱ�䣩
	
	temp = block->next;
	block->next = book;
	//book->pre = head;
	book->next = temp;
	//if(temp!=NULL)temp->pre = book;
	

	OutBlockToFile(registerpath, block);
	freed(block);
	return 0;
}


bool verif(char* username, char* password)//��¼ʱ��֤�û���ݺϷ��ԣ��Ϸ�����true.�Ƿ�����false
{
	struct wlp* block = NULL;
	loadfile(registerpath, &block);
	//�����û���
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

bool exist(char *username,char *password)//ע��ʱ������ע����û����ڷ���true,�����ڷ���false
{
	struct wlp* block = NULL;
	loadfile(registerpath, &block);
	if (0 == SearchNameinBlock(block, username))//�����ڣ�����ע��
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

bool regi()//ע�ᣬ�ɹ�����ture��ʧ�ܷ���false
{
	return false;
}

void log_rpath(char username)//�����û��ļ�·��
{

}
void plan_path(char username) 
{

}

bool is_online(char id)//����û���¼״̬,���߷���true//˼·��ʹ������洢�����û�����ÿ�ε��øú���ʱ���������в�ѯ�Ƿ��ж�Ӧ�û�
{
	return true;
}