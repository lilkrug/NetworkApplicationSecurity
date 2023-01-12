#pragma once
#include <iostream>
#include <string>
#include <list>
#include <time.h>
#include "locale.h"
#include "Winsock2.h"              
#pragma comment(lib,"WS2_32.lib")
#include "Windows.h"
#include "Error.h"

using std::string;
using std::list;
using namespace std;

int port = 2000;					//������������ ���� (�������� �� ��������� 2000)
int uport = 2000;					//udp-����, ������������ ResponseServer
const char* dllname = "ServiceLibrary";	//�������� ����������� ���������� 
const char* npname = "smc";				//��� ������������ ������, ������������ ConsolePipe
const char* ucall = "ZNR";				//�������� �������, ������������ ResponseServer
int MAXCLIENTS = 10;				//������������ ���������� �������� ������������
//int squirt = AS_SQUIRT;			//������� ������������ ���������� �������� ������������
HANDLE(*ts1)(char*, LPVOID);		//����������, ������������ ��� ������� ������� �� dll
HANDLE hAcceptServer;				//���������� AcceptServer
HMODULE st1;						//���������� dll
HANDLE Event = CreateEvent(NULL, FALSE, FALSE, NULL);				//���������� �������

enum TalkersCmd { Start, Stop, Exit, Statistics, Wait, Shutdown, Getcommand }; //���������� ������� RConsole

volatile LONG Accept = 0;				//���������� �����������
volatile LONG Fail = 0;				//��������� �����������
volatile LONG Finished = 0;			//����������� ������
volatile LONG Work = 0;				//���������� � ������� ������


CRITICAL_SECTION scListContact;		//����������� ������

struct Contact				// ������� ������ �����������       
{
	// ��������� ������� �����������
	enum TE {
		EMPTY,              // ������ ������� ������ ����������� 
		ACCEPT,             // ��������� (ACCEPT), �� �� �������������
		CONTACT             // ������� �������������� �������  
	}    type;              // ��� �������� ������ ����������

							// ��������� �������������� �������
	enum ST {
		WORK,               // ���� ����� ������� � ��������
		ABORT,              // ������������� ������ ���������� ����������� 
		TIMEOUT,            // ������������� ������ ���������� �� ������� (�������)
		FINISH              // ������������� ������ ����������  ��������� 
	}      sthread;         // ��������� �������������� ������� (������)

	SOCKET      s;         // ����� ��� ������ ������� � ��������
	SOCKADDR_IN prms;      // ���������  ������ 
	int         lprms;     // ����� prms 
	HANDLE      hthread;   // ���������� ������ ��� (��������)
	HANDLE      htimer;    // ���������� �������

	bool        TimerOff;   // ����� ������������ �������
	bool        CloseConn;  // ����� ���������� ������������

	char msg[50];           // ��������� 
	char srvname[15];       // ��� �������������� ������ (�������)

	HANDLE hAcceptServer;// Handle �������������� ������

	Contact(TE t = EMPTY, const char* namesrv = "") // ����������� 
	{
		memset(&prms, 0, sizeof(SOCKADDR_IN));
		lprms = sizeof(SOCKADDR_IN); // ������ ������
		type = t;
		strcpy(srvname, namesrv);
		msg[0] = 0;
		CloseConn = false;
		TimerOff = false;
	};

	void SetST(ST sth, const char* m = "")
	{
		sthread = sth;
		strcpy(msg, m);
	}
};

typedef list<Contact> ListContact;
ListContact Contacts;

void CALLBACK ASStartMessage(DWORD Lprm)
{
	Contact* client = (Contact*)Lprm;
	/*EnterCriticalSection(&scListContact);*/
	// ������ ����� ������������ �������
	char* sn = client->srvname;
	// �������� ����������� ������
	//LeaveCriticalSection(&scListContact);
	// ��������� �������
	SYSTEMTIME stt;
	// �������� ������� ����� � ����
	GetLocalTime(&stt);
	// ������� ���������
	printf("%d.%d.%d %d:%02d ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute);
	std::cout << sn << " started;" << std::endl;
}
// ����������� ������� ���������� �������������� ������
void CALLBACK ASFinishMessage(DWORD Lprm)
{
	Contact* client = (Contact*)Lprm;
	/*EnterCriticalSection(&scListContact);*/
	// ������ ����� ������������ �������
	char* sn = client->srvname;
	// �������� ����������� ������
	/*LeaveCriticalSection(&scListContact);*/
	// ��������� �������
	SYSTEMTIME stt;
	// �������� ������� ����� � ����
	GetLocalTime(&stt);
	// ������� ���������
	printf("%d.%d.%d %d:%02d ", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute);
	std::cout << sn << " stoped;" << std::endl;
}