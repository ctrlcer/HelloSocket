#define WIN32_LEAN_AND_MEAN
//±‹√‚≥ÂÕª
#include<Windows.h>
#include<WinSock2.h>

//#pragma comment(lib,"ws2_32.lib")

int main(){
	//∆Ù∂ØWindows socket2.xª∑æ≥
	WORD ver = MAKEWORD(2,2);
	WSADATA dat;
	WSAStartup(ver,&dat);
	///
	
	///
	WSACleanup();
	return 0;
}
