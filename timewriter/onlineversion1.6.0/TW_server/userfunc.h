#include<WinSock2.h>

int proc(SOCKET clsock);
//���׽�������Ӧ����

bool verif(char* username, char* password);
//��֤�û����

bool exist(char username);
//����ע��ʱ����������ظ��û���

bool regi();
//ע�ᣬ�ɹ�����ture��ʧ�ܷ���false

bool is_online(char id);
//����û���¼״̬,���߷���true

bool exist(char* username, char* password);
//��������ע������û�������ƥ���û�������

int RegistertInBlock(char* username, char* password);
//��ע��������Ϣ

char* GetUsernameFromPath(char* logpath, char* dirpath);
//�����û���