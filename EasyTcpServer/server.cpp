#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
//�����ͻ
#include<Windows.h>
#include<WinSock2.h>
#include<stdio.h>
#include<iostream>
#pragma comment(lib,"ws2_32.lib")
//��Ϣͷ
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
int main() {
	//����Windows socket2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	///
	//1.����һ��socket�׽���
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.bind�����ڽ��տͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);//host to net unsigned
	cout << _sin.sin_port << endl;
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//0.0.0.0
	//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin))) {
		printf("��ʧ��\n");
	}
	else {
		printf("�󶨳ɹ�\n");
	}
	//3.��������˿�
	if (SOCKET_ERROR ==listen(_sock, 5)) {
		printf("����ʧ��\n");
	}
	else {
		printf("�����ɹ�\n");

	}
	//4.accept�ȴ����տͻ�������
	sockaddr_in clientAddr = {};
	int  nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock) {
		printf("����ʧ��\n");
	}
	else {
		printf("���ճɹ�\n");
	}
	printf("�¿ͻ��˼���: SOCK = %d,IP = %s \n",_cSock,inet_ntoa(clientAddr.sin_addr));

	while (true) {
		DataHeader  header = {};
		char _recvBuf[128] = {};
		//5.���տͻ�������
		int nrecv = recv(_cSock, (char *)&header, sizeof(header), 0);
		if (nrecv <= 0) {
			printf("�ͻ������˳�,�������\n");
			break;
		}
		switch (header.cmd)
		{	
			case CMD_LOGIN:
			{
				
				Login login = {};
				recv(_cSock, (char *)&login+sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);//ƫ��
				printf("�յ�����: CMD_LOGIN %d ���ݳ���: %d,userName=%s PassWord=%s\n",login.cmd,login.dataLength,login.userName,login.PassWord);
				//�����ж��û��������Ƿ���ȷ
				LoginResult ret;
				send(_cSock, (char *)&ret, sizeof(LoginResult), 0);
			}
			break;
			case CMD_LOGINOUT: 
			{
				LoginOut logout = {};
				recv(_cSock, (char *)&logout + sizeof(DataHeader), sizeof(logout) - sizeof(DataHeader), 0);//ƫ��
				printf("�յ�����: CMD_LOGOUT %d ���ݳ���: %d,userName=%s", logout.cmd, logout.dataLength, logout.userName);
				LoginoutResult ret;
				send(_cSock, (char *)&ret, sizeof(ret), 0);
			}
			break;
			default:
				header.cmd = CMD_ERROR;
				header.dataLength = 0;
				send(_cSock, (char*)&header, sizeof(header), 0);
				break;
		}
		//6.��������
	}
	//6.�ر��׽���
	closesocket(_sock);
	
	//���Windows sock����
	WSACleanup();
	printf("���˳����������\n");
	getchar();
	return 0;
}