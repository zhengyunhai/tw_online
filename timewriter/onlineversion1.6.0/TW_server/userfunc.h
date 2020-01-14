#include<WinSock2.h>

int proc(SOCKET clsock);
//对套接字做相应操作

bool verif(char* username, char* password);
//验证用户身份

bool exist(char username);
//用于注册时检测是由有重复用户名

bool regi();
//注册，成功返回ture，失败返回false

bool is_online(char id);
//检测用户登录状态,在线返回true

bool exist(char* username, char* password);
//用于搜索注册表中用户，返回匹配用户的数量

int RegistertInBlock(char* username, char* password);
//向注册表添加信息

char* GetUsernameFromPath(char* logpath, char* dirpath);
//返回用户名