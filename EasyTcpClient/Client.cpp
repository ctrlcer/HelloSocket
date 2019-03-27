#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
//避免冲突
#include<Windows.h>
#include<WinSock2.h>
#include<stdio.h>
#pragma comment(lib,"ws2_32.lib")
enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGINOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
struct DataHeader
{
	short dataLength;//数据长度
	short cmd;

};
//字节序
//内存对齐
struct Login :public DataHeader
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];

};
struct LoginResult :public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
struct LoginOut :public DataHeader
{
	LoginOut() {
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginoutResult :public DataHeader
{
	LoginoutResult() {
		dataLength = sizeof(LoginoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 1;
	}
	int result;
};
struct NewUserJoin :public DataHeader
{
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};
int processor(SOCKET _cSock) {
	//缓冲区
	char szRecv[1024] = {};
	//5.接收客户端数据
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		printf("与服务器断开连接,任务结束\n");
		return -1;
	}
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//偏移
			LoginResult* login = (LoginResult*)szRecv;
			printf("收到服务端<socket=%d>消息: CMD_LOGIN_RESULT  数据长度: %d\n", _cSock, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//偏移
			LoginoutResult* logout = (LoginoutResult*)szRecv;
			printf("收到服务端<socket=%d>消息: CMD_LOGOUT_RESULT  数据长度: %d\n", _cSock, logout->dataLength);

		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//偏移
			NewUserJoin* newjoin = (NewUserJoin*)szRecv;
			printf("收到服务端<socket=%d>消息: CMD_LOGOUT_RESULT  数据长度: %d\n", _cSock, newjoin->dataLength);
		}
		break;
		}
}
int main() {
	//启动Windows socket2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//1.建立一个socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		printf("建立socket错误\n");
	}
	else {
		printf("建立socket成功\n");
	}
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//2.连接服务器connect
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if(SOCKET_ERROR==ret){
		printf("连接错误\n");
	}
	else {
		printf("连接成功\n");
	}
	while (true) {
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads,NULL);
		timeval t = { 0,0 };
		int ret = select(_sock, &fdReads,0,0,&t);
		if (ret < 0) {
			printf("select任务结束1\n");
			break;
		}
		if (FD_ISSET(_sock, &fdReads)) {
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock)) {
				printf("select任务结束2\n");
				break;
			}
		}
		Login login;
		strcpy(login.userName, "zgx");
		strcpy(login.PassWord, "zgxpw");
		send(_sock, (const char*)&login, sizeof(Login), 0);
		//Sleep(1000);
		printf("空闲时间处理其他业务\n");
		
	}
	closesocket(_sock);
	WSACleanup();
	printf("已退出，任务结束\n");
	getchar();
	return 0;
}