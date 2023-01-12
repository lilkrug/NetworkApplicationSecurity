#pragma once
#include <iostream>
#include <string>
#include <list>
#include "Error.h"
#include <tchar.h>
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")
#include <ctime>
#include "WS2tcpip.h"
#pragma warning(disable:4996) 
#define PIPE TEXT("\\\\.\\pipe\\smc")

DWORD WINAPI ConsolePipe(LPVOID pPrm)
{
	cout << "ConsolePipe started;\n" << endl;
	HANDLE hPipe;
	DWORD rc = 0;
	try
	{
		BOOL fSuccess;

		//установка атрибутов безопасности
		SECURITY_ATTRIBUTES SecurityAttributes;
		SECURITY_DESCRIPTOR SecurityDescriptor;

		fSuccess = InitializeSecurityDescriptor(
			&SecurityDescriptor,
			SECURITY_DESCRIPTOR_REVISION);

		if (!fSuccess) {
			throw new string("InitializeSecurityDescriptor():");
		}

		fSuccess = SetSecurityDescriptorDacl(
			&SecurityDescriptor,
			TRUE,
			NULL,
			FALSE);

		if (!fSuccess) {
			throw new string("SetSecurityDescriptorDacl():");
		}

		SecurityAttributes.nLength = sizeof(SecurityAttributes);
		SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
		SecurityAttributes.bInheritHandle = FALSE;

		
		if ((hPipe = CreateNamedPipe(PIPE,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL,INFINITE, &SecurityAttributes)) == INVALID_HANDLE_VALUE)
			throw SetErrorMsgText("Create:", GetLastError());

		while (*((TalkersCmd*)pPrm) != Exit) //цикл работы
		{
			if (!ConnectNamedPipe(hPipe, NULL))
				// ожидать клиента   
				throw SetErrorMsgText("Connect:", GetLastError());
			char ReadBuf[50], WriteBuf[50];
			DWORD nBytesRead = 0, nBytesWrite = 0;
			TalkersCmd SetCommand;
			bool Check;
			while (*((TalkersCmd*)pPrm) != Exit)
			{
				if (*((TalkersCmd*)pPrm) == Getcommand) {

					if (!ReadFile(hPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL))
						break;
					if (nBytesRead > 0)
					{
						Check = true;
						int n = atoi(ReadBuf);
						switch (n)
						{
						case 0:
							sprintf(WriteBuf, "%s", "Start");
							SetCommand = TalkersCmd::Start;
							break;
						case 1:
							sprintf(WriteBuf, "%s", "Stop");
							SetCommand = TalkersCmd::Stop;
							break;
						case 2:
							sprintf(WriteBuf, "%s", "Exit");
							SetCommand = TalkersCmd::Exit;
							break;
						case 3:
							sprintf(WriteBuf, "\nAccept: %i\nFail: %i\nFinished: %i\nWork: %i\n", Accept, Fail, Finished, Work);
							Check = false;
							break;
						case 4:
							sprintf(WriteBuf, "%s", "Wait");
							SetCommand = TalkersCmd::Wait;
							break;
						case 5:
							sprintf(WriteBuf, "%s", "Shutdown");
							SetCommand = TalkersCmd::Shutdown;
							break;
						default:
							sprintf(WriteBuf, "%s", "NoCmd");
							Check = false;
							break;
						}
						if (Check == true)
						{
							*((TalkersCmd*)pPrm) = SetCommand;
							printf("ConsolePipe:%s\n", WriteBuf);
						}

						if (!WriteFile(hPipe, WriteBuf, sizeof(WriteBuf), &nBytesRead, NULL))
							throw new string("CP WRITE ERROR");
					}
				}
				else Sleep(1000);
			}
			if (!DisconnectNamedPipe(hPipe))           // отключить клиента   
				throw SetErrorMsgText("disconnect:", GetLastError());
		}
		DisconnectNamedPipe(hPipe);
		CloseHandle(hPipe);
		cout << "ConsolePipe stoped;\n" << endl;
	}
	catch (string ErrorPipeText)
	{
		cout << ErrorPipeText << endl;
	}
	ExitThread(rc);
}
