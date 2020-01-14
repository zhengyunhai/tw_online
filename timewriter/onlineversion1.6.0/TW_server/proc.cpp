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
//�����ʱ���޸ģ���¼��ע��ʧ��ʱ�ͻ���ֻ������û������������
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
	bool re_ = false;//�ж��Ƿ��ѱ�ע��
	struct header head;
	recv(clsock, (char*)&head, sizeof(head), 0);
	int lo_ =head.online_tag;//�ж�����״̬
	cout << "cmd:" << head.cmd << " " << "data_length:" << head.datalength << endl;
	switch (head.cmd)
	{
	case CMD_LOGIN:{
		struct Login login;
		struct LoginRes loginres;
		recv(clsock, (char*)&login + sizeof(header), sizeof(login) - sizeof(header), 0);
		if (lo_==STATE_OUTLINE&&true == verif(login.username, login.password))//����״̬���ǺϷ��û�ʱ���Ե�¼
		{
			//�����û�״̬
			lo_ = STATE_ONLINE;
			//�����û��ļ�·��
			strcat_s(logpath, sizeof(logpath), login.username);
			strcat_s(logpath, sizeof(logpath), "\\data");
			strcat_s(logpath, sizeof(logpath), "\\log");//logpath=.E:\\ser\\user\\�û���\\data\\log,�ļ�·��

			strcat_s(planpath, sizeof(planpath), login.username);
			strcat_s(planpath, sizeof(planpath), "\\data");
			strcat_s(planpath, sizeof(planpath), "\\plan");//logpath=.\\user\\�û���\\plan������Ŀ¼
			//�����ʼ���������ļ�(�����ټ���¼��ʱ�������ļ����ھ�����������ʱ�����뼴�ɣ�ͬʱҪ�б��棬�ͷ�����Ĳ���)

			//���ͷ���ֵ��
			loginres.result = RES_SUCC;
			loginres.head.online_tag = lo_;//��������״̬
			strcpy_s(loginres.head.logpath,sizeof(logpath), logpath);//����·����Ϣ��ֻ�����ڵ�¼��ʱ��Ż᷵��·����Ϣ���ǳ�ʱ���·����Ϣ
			strcpy_s(loginres.head.planpath, sizeof(planpath), planpath);
			//loginres.head.planpath = planpath;
			send(clsock, (const char*) & loginres, sizeof(loginres), 0);
			cout << "�û� " << login.username << " ��¼" << endl;
			return 0;
		}
		else//�Ƿ�
		{
			if (lo_ == STATE_ONLINE)//���ȼ�⵱ǰ�Ƿ�����״̬
			{
				loginres.result = ERROR_ONLINE;
				loginres.head.online_tag = lo_;
				send(clsock, (const char*)&loginres, sizeof(loginres), 0);
			}
			else//�ټ���û��Ƿ�Ϸ�
			{
				//���ͷ���ֵ��
				loginres.result = RES_UNEXIST;
				loginres.head.online_tag = lo_;
				send(clsock, (const char*)&loginres, sizeof(loginres), 0);
				//�ͻ���ѯ���Ƿ�ע��
			}		
			return 1;
		}
			
	}break;

	case CMD_LOGOUT: {
		struct Logout logout;
		struct LogoutRes logoutres;
		recv(clsock, (char*)&logout + sizeof(header), sizeof(logout) - sizeof(header), 0);
		if (lo_== STATE_ONLINE)//�жϵ�ǰ����״̬
		{
			lo_ = STATE_OUTLINE;
			logoutres.head.online_tag = lo_;
			strcpy_s(logoutres.head.logpath, sizeof(empty_tag), empty_tag);//����·��
			strcpy_s(logoutres.head.logpath, sizeof(empty_tag), empty_tag);
			logoutres.result = RES_OUTLINE;
			send(clsock, (const char*)&logoutres, sizeof(logoutres), 0);
			cout << "�û��ǳ�" << endl;
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
		if (lo_== STATE_OUTLINE && false == exist(regist.username,regist.password))//���������ظ��û���ʱ����ע��
		{
			//�û���������д��ע���
			RegistertInBlock(regist.username, regist.password);
			//�����ļ�·���������û��ļ�
			strcat_s(logpath, sizeof(logpath), regist.username);
			if (0 != _mkdir(logpath)) { cout << "�û������ļ��д���ʧ��" << endl; };//"E:\\ser\\user\\�û���"
			strcat_s(logpath, sizeof(logpath), "\\data");
			if (0 != _mkdir(logpath)) { cout << "�û������ļ��д���ʧ��" << endl; };//"E:\\ser\\user\\�û���\\data"
			strcat_s(logpath, sizeof(logpath), "\\log");//logpath=.E:\\ser\\user\\�û���\\data\\log,�ļ�·��

			strcpy_s(expdpath, PATH_LENGTH, logpath);
			strcat_s(expdpath, sizeof(expdpath), "expd\\");
			if (0 != _mkdir(expdpath)) { cout << "�û������ļ��д���ʧ��" << endl; };//���������ļ���

			strcat_s(planpath, sizeof(planpath), regist.username);
			strcat_s(planpath, sizeof(planpath), "\\data");
			strcat_s(planpath, sizeof(planpath), "\\plan");//logpath=.\\user\\�û���\\plan������Ŀ¼
			//�����ʼ���������ļ�(�����ټ���¼��ʱ�������ļ����ھ�����������ʱ�����뼴�ɣ�ͬʱҪ�б��棬�ͷ�����Ĳ���)
			outf.open(logpath);//�����ļ�
			outf.close();
			outf.open(planpath);//�����ļ�
			outf.close();
			
			//������Ӧ��Ϣ
			registres.result = RES_SUCC;
			strcpy_s(registres.head.logpath, sizeof(logpath), logpath);//����·����Ϣ��
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
		//�ͻ��˱��������û���������
		//�����û����Ƿ�Ϸ�
		//ɾ���û��ļ���
		//ɾ���û�ע�����Ϣ
		//������Ϣ��
		return 0;
	}break;

	case CMD_LIST_LOG_ZA: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //�Ѿ�����״̬
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.logpath, &block);
			//����menu����������Ŀ¼����
			cct = menu(&namelink, block, 1);//namelinkͷ�ڵ㲻�洢��Ϣ,cctΪnamelink�нڵ����
			cout << "����������־Ŀ¼������" << cct << "��" << endl;
			//����Ŀ¼����
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{
				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//֪ͨ�ͻ���������ܳɹ�����������Ŀ¼
				cout << "���ڷ�����־Ŀ¼..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//���Ͱ�ͷ�����ݣ�˵��list�Ƿ���Ҫ����
					//cout << "��������һ��" << endl;
					temp = temp->next;
				}
				cout << "��־Ŀ¼���ͽ���" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			//cout << "������Ϣһ��" << endl;
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//������ʾ��¼�����ݰ�
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_LOG_AZ: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //�Ѿ�����״̬
		{
			struct ListRes listres;
			listres.result = RES_SUCC;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);//֪ͨ�ͻ���������ܳɹ�����������Ŀ¼
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.logpath, &block);
			//����menu����������Ŀ¼����
			cct = menu(&namelink, block, -1);//namelinkͷ�ڵ㲻�洢��Ϣ,cctΪnamelink�нڵ����
			cout << "����������־Ŀ¼������" << cct << "��" << endl;
			//����Ŀ¼����
			temp = namelink->next;
			if (cct != 0)
			{
				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//֪ͨ�ͻ���������ܳɹ�����������Ŀ¼
				cout << "���ڷ�����־Ŀ¼..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//���Ͱ�ͷ�����ݣ�˵��list�Ƿ���Ҫ����
					temp = temp->next;
				}
				cout << "��־Ŀ¼���ͽ���" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);

			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{		
			//������ʾ��¼�����ݰ�
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_PLAN_AZ: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //�Ѿ�����״̬
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.planpath, &block);
			//����menu����������Ŀ¼����
			cct = menu(&namelink, block, -1);//namelinkͷ�ڵ㲻�洢��Ϣ,cctΪnamelink�нڵ����
			cout << "��������ƻ�Ŀ¼������" << cct << "��" << endl;
			//����Ŀ¼����
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{

				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//֪ͨ�ͻ���������ܳɹ�����������Ŀ¼
				cout << "���ڷ��ͼƻ�Ŀ¼..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//���Ͱ�ͷ�����ݣ�˵��list�Ƿ���Ҫ����
					temp = temp->next;
				}
				cout << "�ƻ�Ŀ¼���ͽ���" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			//cout << "������Ϣһ��" << endl;
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//������ʾ��¼�����ݰ�
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_PLAN_ZA: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //�Ѿ�����״̬
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.planpath, &block);
			//����menu����������Ŀ¼����
			cct = menu(&namelink, block, 1);//namelinkͷ�ڵ㲻�洢��Ϣ,cctΪnamelink�нڵ����
			cout << "��������ƻ�Ŀ¼������" << cct << "��" << endl;
			//����Ŀ¼����
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{

				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//֪ͨ�ͻ���������ܳɹ�����������Ŀ¼
				cout << "���ڷ��ͼƻ�Ŀ¼..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//���Ͱ�ͷ�����ݣ�˵��list�Ƿ���Ҫ����
					temp = temp->next;
				}
				cout << "�ƻ�Ŀ¼���ͽ���" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			//cout << "������Ϣһ��" << endl;
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//������ʾ��¼�����ݰ�
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_PROG_AZ: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //�Ѿ�����״̬
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.logpath, &block);
			//����menu����������Ŀ¼����
			cct = menu(&namelink, block, -1);//namelinkͷ�ڵ㲻�洢��Ϣ,cctΪnamelink�нڵ����
			cout << "����������־Ŀ¼������" << cct << "��" << endl;
			//����Ŀ¼����
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{

				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//֪ͨ�ͻ���������ܳɹ�����������Ŀ¼
				cout << "���ڷ�����־Ŀ¼..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//���Ͱ�ͷ�����ݣ�˵��list�Ƿ���Ҫ����
					temp = temp->next;
				}
				cout << "��־Ŀ¼���ͽ���" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//������ʾ��¼�����ݰ�
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_LIST_PROG_ZA: {
		int cct = 0;
		if (lo_ == STATE_ONLINE) //�Ѿ�����״̬
		{
			struct wlp* namelink, * temp;
			struct wlp* block = NULL;
			loadfile(head.logpath, &block);
			//����menu����������Ŀ¼����
			cct = menu(&namelink, block, 1);//namelinkͷ�ڵ㲻�洢��Ϣ,cctΪnamelink�нڵ����
			cout << "������־Ŀ¼����" << cct << "��" << endl;
			//����Ŀ¼����
			temp = namelink->next;
			struct ListRes listres;
			if (cct != 0)
			{

				listres.result = RES_SUCC;
				listres.head.online_tag = lo_;
				strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&listres, sizeof(listres), 0);//֪ͨ�ͻ���������ܳɹ�����������Ŀ¼
				cout << "���ڷ��Ϳ�����־Ŀ¼..." << endl;
				while (temp != NULL)
				{
					cout << temp->name << " " << temp->today << " " << temp->lpdata.info << endl;
					listres.result = RES_SUCC;
					listres.head.online_tag = lo_;
					strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
					strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
					strcpy_s(listres.node.name, DATA_NAME_LENGTH, temp->name);
					strcpy_s(listres.node.today, TIME_LENGTH, temp->today);
					strcpy_s(listres.node.lpdata.info, DATA_LENGTH, temp->lpdata.info);
					listres.node.b = temp->b;
					listres.node.count = temp->count;
					send(clsock, (const char*)&listres, sizeof(listres), 0);//���Ͱ�ͷ�����ݣ�˵��list�Ƿ���Ҫ����
					temp = temp->next;
				}
				cout << "������־Ŀ¼���ͽ���" << endl;
			}
			listres.result = RES_FAIL;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			//cout << "������Ϣһ��" << endl;
			freed(block);
			freed(namelink);
			return 0;
		}
		else
		{
			//������ʾ��¼�����ݰ�
			struct ListRes listres;
			listres.result = RES_OUTLINE;
			listres.head.online_tag = lo_;
			strcpy_s(listres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(listres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&listres, sizeof(listres), 0);
			return 1;
		}
	}break;

	case CMD_ADD_LOG: {
		if (lo_ == STATE_ONLINE)//δ��¼���ж��ڿͻ������
		{
			struct Log log;
			struct wlp* headnode;
			cout << head.logpath << " " << head.planpath << endl;
			loadfile(head.logpath, &headnode);
			recv(clsock, (char*)&log + sizeof(header), sizeof(log) - sizeof(header), 0);
			//ʹ�ú���������޸�adddata()����;
			adddata(&headnode, log.name, log.data, 0);//�ں����ڲ�һ�����ͬһ���ڶ�������Ϣ������
			//����취���ڿͻ���������־�ı�ʱ���ַ���ƥ�䣬������endw,����CMD_ADD_LOG����İ�ֱ�ӷ��ͣ���ÿһ����Ϣ���������´���һ���µ���־���Ƿ�ͬһ����жϽ���������
			//CMD_INFO�Ͳ�����	
			outtofile(head.logpath, headnode);
			freed(headnode);
			return 0;
		}
	}break;

	case CMD_ADD_PROG: {
	
		if (lo_ == STATE_ONLINE)//δ��¼���ж��ڿͻ������
		{
			struct Log log;
			struct wlp* headnode;
			cout << head.logpath << " " << head.planpath << endl;
			loadfile(head.logpath, &headnode);
			recv(clsock, (char*)&log + sizeof(header), sizeof(log) - sizeof(header), 0);
			//ʹ�ú���������޸�adddata()����;
			adddata(&headnode, log.name, log.data, 1);//�ں����ڲ�һ�����ͬһ���ڶ�������Ϣ������
			//����취���ڿͻ���������־�ı�ʱ���ַ���ƥ�䣬������endw,����CMD_ADD_LOG����İ�ֱ�ӷ��ͣ���ÿһ����Ϣ���������´���һ���µ���־���Ƿ�ͬһ����жϽ���������
			//CMD_INFO�Ͳ�����	
			outtofile(head.logpath, headnode);
			freed(headnode);
			return 0;
		}
	}break;

	case CMD_ADD_PLAN: {
		if (lo_ == STATE_ONLINE)//δ��¼���ж��ڿͻ������
		{
			struct Plan plan;
			struct wlp* headnode;
			cout << head.logpath << " " << head.planpath << endl;
			loadfile(head.planpath, &headnode);
			recv(clsock, (char*)&plan + sizeof(header), sizeof(plan) - sizeof(header), 0);
			//ʹ�ú���������޸�adddata()����;
			adddata(&headnode, plan.name, plan.data, 0);//�ں����ڲ�һ�����ͬһ���ڶ�������Ϣ������
			//����취���ڿͻ���������־�ı�ʱ���ַ���ƥ�䣬������endw,����CMD_ADD_LOG����İ�ֱ�ӷ��ͣ���ÿһ����Ϣ���������´���һ���µ���־���Ƿ�ͬһ����жϽ���������
			//CMD_INFO�Ͳ�����	
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
			cout << "��ѯ�ļ�;" << checklog.name  << ",����" << cc<<"��" << endl;
			if (cc == 0)
			{
				checkres.result = RES_FAIL;
				checkres.head.online_tag = lo_;
				checkres.node.count = 0;
				strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&checkres, sizeof(checkres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				temp = findtail(headnode);
				cout << "���ڴ�������..." << endl;
				while (cc > 0)
				{
					if (!strcmp(temp->name, checklog.name))
					{
						checkres.result = RES_SUCC;
						checkres.head.online_tag = lo_;
						strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
						strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);

						strcpy_s(checkres.node.name, DATA_NAME_LENGTH, temp->name);//��������
						strcpy_s(checkres.node.today, DATA_NAME_LENGTH, temp->today);
						checkres.node.count = temp->count;
						int ct = temp->count;
						send(clsock, (const char*)&checkres, sizeof(checkres), 0);
						tempinfo = &temp->lpdata;
						while (tempinfo != NULL)
						{

							checkres.result = RES_SUCC;
							checkres.head.online_tag = lo_;
							strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
							strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);

							strcpy_s(checkres.node.lpdata.info, DATA_LENGTH, tempinfo->info);//��������
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
				cout << "���ݴ������" << endl;
				checkres.result = RES_FAIL;
				checkres.head.online_tag = lo_;
				checkres.node.count = 0;
				strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			cout << "��ѯ�ļ�;" << checkplan.name << ",����" << cc << "��" << endl;
			if (cc == 0)
			{
				checkres.result = RES_FAIL;
				checkres.head.online_tag = lo_;
				checkres.node.count = 0;
				strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&checkres, sizeof(checkres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				temp = findtail(headnode);
				cout << "���ڴ�������..." << endl;
				while (cc > 0)
				{
					if (!strcmp(temp->name, checkplan.name))
					{
						checkres.result = RES_SUCC;
						checkres.head.online_tag = lo_;
						strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
						strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);

						strcpy_s(checkres.node.name, DATA_NAME_LENGTH, temp->name);//��������
						strcpy_s(checkres.node.today, DATA_NAME_LENGTH, temp->today);
						checkres.node.count = temp->count;
						int ct = temp->count;
						send(clsock, (const char*)&checkres, sizeof(checkres), 0);
						tempinfo = &temp->lpdata;
						while (tempinfo != NULL)
						{

							checkres.result = RES_SUCC;
							checkres.head.online_tag = lo_;
							strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
							strcpy_s(checkres.head.planpath, sizeof(head.planpath), head.planpath);

							strcpy_s(checkres.node.lpdata.info, DATA_LENGTH, tempinfo->info);//��������
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
				cout << "���ݴ������" << endl;
				checkres.result = RES_FAIL;
				checkres.head.online_tag = lo_;
				checkres.node.count = 0;
				strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			strcpy_s(checkres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			cout << "ɾ��Ŀ���ļ�;" << dellog.name << ",����" << cc << "��" << endl;
			if (cc == 0)
			{
				delres.result = RES_FAIL;
				delres.head.online_tag = lo_;
				strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&delres, sizeof(delres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				delres.result = RES_SUCC;
				delres.head.online_tag = lo_;
				strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			cout << "ɾ��Ŀ���ļ�;" << delplan.name << ",����" << cc << "��" << endl;
			if (cc == 0)
			{
				delres.result = RES_FAIL;
				delres.head.online_tag = lo_;
				strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
				strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
				send(clsock, (const char*)&delres, sizeof(delres), 0);
				freed(headnode);
				return 1;
			}
			else
			{
				delres.result = RES_SUCC;
				delres.head.online_tag = lo_;
				strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&delres, sizeof(delres), 0);
			return 1;
		}
	}break;

	case CMD_DEL_ALL_LOG: {
		if (lo_ == STATE_ONLINE)
		{
			cout << "ɾ��Ŀ��·��:" << head.logpath << endl;
			struct DelAllLog delalllog;
			struct DelRes delres;
			struct wlp* headnode;
			loadfile(head.logpath, &headnode);			
			delres.result = RES_SUCC;
			delres.head.online_tag = lo_;
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
			strcpy_s(delres.head.planpath, sizeof(head.planpath), head.planpath);
			send(clsock, (const char*)&delres, sizeof(delres), 0);
			return 1;
		}
	}break;

	case CMD_DEL_ALL_PLAN: {
		if (lo_ == STATE_ONLINE)
		{
			cout << "ɾ��Ŀ��·��:" << head.planpath << endl;
			struct DelAllPlan delallplan;
			struct DelRes delres;
			struct wlp* headnode;
			loadfile(head.planpath, &headnode);			
			delres.result = RES_SUCC;
			delres.head.online_tag = lo_;
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			strcpy_s(delres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
			cout << "�������ļ�;" << expd.name << "����" << cc << "��" << endl;
			if (cc == 0)
			{
				expdres.result = RES_FAIL;
				expdres.head.online_tag = lo_;
				strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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
				cout <<"��������ʱ�ļ���"<< expdpath << endl;
				exportlog(headnode, expd.name,expdpath);
				
				FILE* fp;
				fopen_s(&fp, expdpath, "rb");
				if (fp == NULL)
				{
					cout << "�ļ���ʧ��" << endl;
					expdres.result = RES_FAIL;
					expdres.head.online_tag = lo_;
					strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
					strcpy_s(expdres.head.planpath, sizeof(head.planpath), head.planpath);
					send(clsock, (const char*)&expdres, sizeof(expdres), 0);
					freed(headnode);
					
					return 1;
				}
				else
				{
					cout << "�ļ��򿪳ɹ������ڴ���" << endl;
					char buff[BUF_SIZE] = { 0 };
					int count = 0;
					while ((count = fread(buff, 1, BUF_SIZE, fp)) > 0)
					{
						cout << count << endl;
						expdres.result = RES_SUCC;
						expdres.head.online_tag = lo_;
						strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
						strcpy_s(expdres.head.planpath, sizeof(head.planpath), head.planpath);
						strcpy_s(expdres.buff, BUF_SIZE, buff);
						send(clsock, (const char*)&expdres, sizeof(expdres), 0);						
					}
					expdres.result = RES_FAIL;
					expdres.head.online_tag = lo_;
					strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
					strcpy_s(expdres.head.planpath, sizeof(head.planpath), head.planpath);
					send(clsock, (const char*)&expdres, sizeof(expdres), 0);
					fclose(fp);
					freed(headnode);
					if (0 == remove(expdpath))//������ɺ��ٷ�������ɾ��
					{
						cout << "��ʱ�ļ�ɾ���ɹ�" << endl;
					}
					else
					{
						cout << "��ʱ�ļ�ɾ��ʧ��" << endl;
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
			strcpy_s(expdres.head.logpath, sizeof(head.logpath), head.logpath);//����·����Ϣ��
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