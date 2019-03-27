#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
//避免冲突
#include<Windows.h>
#include<WinSock2.h>
#include<cstdio>
#include<stdlib.h>
#include<vector>
#pragma comment(lib,"ws2_32.lib")
//消息头
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
//DataPackage
struct Login:public DataHeader
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];

};
struct LoginResult:public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
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

struct LoginOut:public DataHeader
{
	LoginOut() {
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginoutResult:public DataHeader
{
	LoginoutResult() {
		dataLength = sizeof(LoginoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 1;
	}
	int result;
};
using namespace std;
vector<SOCKET> g_Client;

int processor(SOCKET _cSock) {
	//缓冲区
	char szRecv[1024] = {}; 
	//5.接收客户端数据
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		printf("客户端<Socket=%d>已退出,任务结束\n",_cSock);
		return -1;
	}
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//偏移
		Login* login = (Login*)szRecv;
		printf("收到客户端<Socket=%d>请求: CMD_LOGIN %d 数据长度: %d,userName=%s PassWord=%s\n",_cSock,login->cmd, login->dataLength, login->userName, login->PassWord);
		//忽略判断用户名密码是否正确
		LoginResult ret;
		send(_cSock, (char *)&ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGINOUT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//偏移
		LoginOut* logout = (LoginOut*)szRecv;
		printf("收到客户端<Socket=%d>请求: CMD_LOGOUT %d 数据长度: %d,userName=%s\n",_cSock, logout->cmd, logout->dataLength, logout->userName);
		LoginoutResult ret;
		send(_cSock, (char *)&ret, sizeof(ret), 0);
	}
	break;
	default:
		DataHeader header = { 0,CMD_ERROR };
		send(_cSock, (char*)&header, sizeof(header), 0);
	}
	return 0;
}
int main() {
	//启动Windows socket2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	///
	//1.建立一个socket套接字
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.bind绑定用于接收客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);//host to net unsigned
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//0.0.0.0
	//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin))) {
		printf("绑定失败\n");
	}
	else {
		printf("绑定成功\n");
	}
	//3.监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5)) {
		printf("监听失败\n");
	}
	else {
		printf("监听成功\n");
	}
	while (true) {
		//伯克利socket描述符
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);
		for (int n = (int)g_Client.size() - 1; n >= 0; n--) {
			FD_SET(g_Client[n], &fdRead);
		}
		timeval t = { 0, 0 };
		//是一个整数值  是指fd_set集合中所有描述符(socket)的范围 而不是数量
		//这是所有文件描述符最大值-1,在Windows中这个参数写0
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp,&t);
		if (ret < 0) {
			printf("select,任务结束\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead)) {
			FD_CLR(_sock, &fdRead);
			//4.accept等待接收客户端连接
			sockaddr_in clientAddr = {};
			int  nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock) {
				printf("错误，接受到无效客户端socket...\n");
			}
			for (int n = (int)g_Client.size() - 1; n >= 0; n--) {
				NewUserJoin userJoin;
				send(g_Client[n],(const char*)&userJoin, sizeof(NewUserJoin),0);
			}
			g_Client.push_back(_cSock);
			printf("新客户端加入: SOCK = %d,IP = %s \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
		}
		for (size_t n = 0; n < fdRead.fd_count; n++) {
			if (-1 == processor(fdRead.fd_array[n])) {
				auto iter = find(g_Client.begin(), g_Client.end(), fdRead.fd_array[n]);
				if (iter != g_Client.end()) {
					g_Client.erase(iter);
				}
			}
		}

		printf("空闲时间处理其他业务\n");
		//6.处理请求
	}
	for (size_t n = g_Client.size() - 1; n >= 0; n--) {
		closesocket(g_Client[n]);
	}
	//6.关闭套接字
	closesocket(_sock);
	//清除Windows sock环境
	WSACleanup();
	printf("已退出，任务结束\n");
	getchar();
	return 0;
}