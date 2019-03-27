#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
//�����ͻ
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
	short dataLength;//���ݳ���
	short cmd;

};
//�ֽ���
//�ڴ����
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
	//������
	char szRecv[1024] = {};
	//5.���տͻ�������
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		printf("��������Ͽ�����,�������\n");
		return -1;
	}
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//ƫ��
			LoginResult* login = (LoginResult*)szRecv;
			printf("�յ������<socket=%d>��Ϣ: CMD_LOGIN_RESULT  ���ݳ���: %d\n", _cSock, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//ƫ��
			LoginoutResult* logout = (LoginoutResult*)szRecv;
			printf("�յ������<socket=%d>��Ϣ: CMD_LOGOUT_RESULT  ���ݳ���: %d\n", _cSock, logout->dataLength);

		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//ƫ��
			NewUserJoin* newjoin = (NewUserJoin*)szRecv;
			printf("�յ������<socket=%d>��Ϣ: CMD_LOGOUT_RESULT  ���ݳ���: %d\n", _cSock, newjoin->dataLength);
		}
		break;
		}
}
int main() {
	//����Windows socket2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//1.����һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		printf("����socket����\n");
	}
	else {
		printf("����socket�ɹ�\n");
	}
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//2.���ӷ�����connect
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if(SOCKET_ERROR==ret){
		printf("���Ӵ���\n");
	}
	else {
		printf("���ӳɹ�\n");
	}
	while (true) {
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads,NULL);
		timeval t = { 0,0 };
		int ret = select(_sock, &fdReads,0,0,&t);
		if (ret < 0) {
			printf("select�������1\n");
			break;
		}
		if (FD_ISSET(_sock, &fdReads)) {
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock)) {
				printf("select�������2\n");
				break;
			}
		}
		Login login;
		strcpy(login.userName, "zgx");
		strcpy(login.PassWord, "zgxpw");
		send(_sock, (const char*)&login, sizeof(Login), 0);
		//Sleep(1000);
		printf("����ʱ�䴦������ҵ��\n");
		
	}
	closesocket(_sock);
	WSACleanup();
	printf("���˳����������\n");
	getchar();
	return 0;
}