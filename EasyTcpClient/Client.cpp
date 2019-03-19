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
		//3.������������
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		//4.������������
		if (0 == strcmp(cmdBuf, "exit")) {
			printf("�յ�exit���������\n");
			break;
		}
		else if(0 == strcmp(cmdBuf,"login")){
			Login login;
			strcpy(login.userName, "zgx");
			strcpy(login.PassWord, "zgxtest");
			//5.�������������������
			send(_sock, (const char *)&login, sizeof(login), 0);
			//���շ��������ص�����
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet, sizeof(loginRet), 0);
			printf("LoginResult: %d \n", loginRet.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			LoginOut logout ;
			strcpy(logout.userName, "zgx");
			
			//5.�������������������
			send(_sock, (const char *)&logout, sizeof(logout), 0);
			//���շ��������ص�����
			LoginoutResult logoutRet = {};
			recv(_sock, (char*)&logoutRet, sizeof(logoutRet), 0);
			printf("LoginoutResult: %d \n", logoutRet.result);
		}
		else {
			printf("��֧�ֵ��������������\n");
		}
	}
	closesocket(_sock);
	WSACleanup();
	printf("���˳����������\n");
	getchar();
	return 0;
}