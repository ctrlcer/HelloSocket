#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
//避免冲突
#include<Windows.h>
#include<WinSock2.h>
#include<cstdio>
#include<iostream>
#pragma comment(lib,"ws2_32.lib")
//消息头
enum CMD {
	CMD_LOGIN,
	CMD_LOGINOUT,
	CMD_ERROR
};
struct DataHeader
{
	short dataLength;//数据长度
	short cmd;

};
//字节序
//内存对齐
struct Login
{
	char userName[32];
	char PassWord[32];

};
struct LoginResult
{
	int result;
};
struct LoginOut {
	char userName[32];
};
struct LoginoutResult
{
	int result;
};
using namespace std;
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
	cout << _sin.sin_port << endl;
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//0.0.0.0
	//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin))) {
		printf("绑定失败\n");
	}
	else {
		printf("绑定成功\n");
	}
	//3.监听网络端口
	if (SOCKET_ERROR ==listen(_sock, 5)) {
		printf("监听失败\n");
	}
	else {
		printf("监听成功\n");

	}
	//4.accept等待接收客户端连接
	sockaddr_in clientAddr = {};
	int  nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock) {
		printf("接收失败\n");
	}
	else {
		printf("接收成功\n");
	}
	printf("新客户端加入: SOCK = %d,IP = %s \n",_cSock,inet_ntoa(clientAddr.sin_addr));

	while (true) {
		DataHeader  header = {};
		char _recvBuf[128] = {};
		//5.接收客户端数据
		int nrecv = recv(_cSock, (char *)&header, sizeof(header), 0);
		printf("收到命令:%d 数据长度:%d \n", header.cmd,header.dataLength);
		switch (header.cmd)
		{	
			case CMD_LOGIN:
			{
				
				Login login = {};
				recv(_cSock, (char *)&login, sizeof(Login), 0);
				//忽略判断用户名密码是否正确
				LoginResult ret = { 1 };
				send(_cSock, (char *)&header, sizeof(DataHeader), 0);
				send(_cSock, (char *)&ret, sizeof(LoginResult), 0);
			}
			break;
			case CMD_LOGINOUT: 
			{
				LoginOut logout = {};
				recv(_cSock, (char *)&logout, sizeof(Login), 0);
				LoginoutResult ret = { 1 };
				send(_cSock, (char *)&header, sizeof(DataHeader), 0);
				send(_cSock, (char *)&ret, sizeof(LoginoutResult), 0);

			}
			break;
			default:
				header.cmd = CMD_ERROR;
				header.dataLength = 0;
				send(_cSock, (char*)&header, sizeof(header), 0);
				break;
		}
		if (nrecv <= 0) {
			printf("客户端已退出,任务结束\n");
			break;
		}
		//6.处理请求
	}
	//6.关闭套接字
	closesocket(_sock);
	
	//清除Windows sock环境
	WSACleanup();
	printf("已退出，任务结束\n");
	getchar();
	return 0;
}