//logsoft.h

#include"defination.h"



//�ṹ������
typedef struct data
{
	char time[TIME_LENGTH];//1.������־�п��Դ洢��־ʱ�䣬2.����ע����п��Դ洢ע������
	char info[DATA_LENGTH];//��������200������
	data* next;
}data;

typedef struct wlp
{
	char name[DATA_NAME_LENGTH];//�洢username
	char today[TIME_LENGTH];//�洢password
	int b;//b=0,��ͨ��־,b=1,������־
	int count;//�����÷�֧�Ͻڵ��������ȷ���÷�֧��ʱ����
	struct data lpdata;
	wlp* pre;
	wlp* next;
}wlp;

void f0(char* ti);
//ʱ���ʽ���0

char* getday();
//��ȡ��ǰ���ڣ����ظ�ʽ��20771201

char* gettime();
//��ȡ��ǰʱ�䣬���ظ�ʽ��[19:01]

struct wlp* findtail(struct wlp* node);
//���������β�ڵ��ַ

void initnode(struct wlp** head, int i);
//�ڵ��ʼ��

int loadfile(char* path, struct wlp** wl);
//�����ļ�

void outtofile(char* path, struct wlp* wl);
//������ļ�

void adddata(struct wlp** library, char* name,char*infodata, int b);
//��������������ݣ�����һ���ڵ�name��data�������ͬ���ڴ˽ڵ���ӣ������½��ڵ���Ӳ���Ϊ�µĵ�һ���ڵ�

void showinfo(struct wlp* node);
//��ӡnode���ڵ㼰����������Ϣ

void searchdata(struct wlp* wl, char* name);
//����nameͬ���ڵ�

int addinfo(struct wlp* node);
//��node�ڵ��������������Ϣ

void freed(struct wlp* wl);
//�ͷ�����

void freei(struct data* node);
//�ͷ�������

void outlist();
//���ָ���б�

void delnode(struct wlp** node, char* name);
//ɾ��nameͬ���ڵ�

void sampleinfo(struct wlp* node);
//��ӡ������Ϣ

void exportlog(struct wlp* node, char* name,char* path);
//����node��������Ϊname�Ľڵ㵽ͬ��txt��,tag=1,����tag=-1,����

void headlist();
//��ӡ������汾�ŵ�

int is_node(struct wlp* node, char* name);
//������������nameͬ���Ľڵ����

void delall(struct wlp* node);
//ɾ��ȫ���ڵ�

int menu(struct wlp** namelink, struct wlp* node,int tag);
//��node�����нڵ㰴˳�򱣴��״γ��ֵĽڵ�����namelink��,tag=1,����tag=-1,����

void printmenu(struct wlp* namelink,int tag);
//��ӡĿ¼��tag=0,��ӡ��ͨ��־��tag=1����ӡ������־

void copynode();
//���ƽڵ㵽Ŀ��λ�ã�����

//�ַ���ƥ��ģ��
/*
char command[10],command1[10],command2[10];
char tc;
cin>>command;
tc=getchar();
if (!strcmp(command, "�ַ���"))
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
			while ('\n' != tc)tc = getchar();//�Ե��������
		}
		else
		{
			if (!strcmp(command1, "�ַ���1")
			{
				//����
			}
			else cout<<"WrongArguments"<<endl;
		}

	}
}
*/

