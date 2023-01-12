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

using std::iterator;

DWORD WINAPI GarbageCleaner(LPVOID pPrm)
{
	cout << "GarbageCleaner started;\n" << endl;
	DWORD rc = 0;
	try
	{
		while (*((TalkersCmd*)pPrm) != Exit && &Work > 0)
		{
			EnterCriticalSection(&scListContact);//Войти в критическую секцию
			//удаление элемента из списка подклюений
			//просматривает  последовательно элементы  списка подключений и удаляет неиспользуемые элементы
			//Для того, чтобы   добавление и удаление элементов списка не осуществлялось одновременно,
			//эти   операции помещают внутри  критической   секции  соответствующего потока
			for (std::list<Contact>::iterator first = Contacts.begin(); first != Contacts.end();)
			{
				if (first->sthread == Contact::FINISH || first->sthread == Contact::TIMEOUT || first->sthread == Contact::ABORT || first->type == Contact::EMPTY)
				{
					printf("IP of deleted client:-%s", inet_ntoa(first->prms.sin_addr));
					cout << " with code " << first->sthread << ";" << endl;
					if (first->type == Contact::EMPTY) InterlockedIncrement(&Fail);
					else
					{
						if (first->sthread == Contact::FINISH) InterlockedIncrement(&Finished);
						if (first->sthread == Contact::TIMEOUT) InterlockedIncrement(&Fail);
						if (first->sthread == Contact::ABORT) InterlockedIncrement(&Fail);
						CloseHandle(first->hthread);
						CloseHandle(first->htimer);
					}
					closesocket(first->s);
					first = Contacts.erase(first);
					InterlockedDecrement(&Work);
				}
				else first++;
			}
			LeaveCriticalSection(&scListContact); //Покинуть критическую секцию
			Sleep(1000);
		}
	}
	catch (string errorMsgText)
	{
		std::cout << errorMsgText << endl;
	}
	cout << "GarbageCleaner stoped;\n" << endl;
	ExitThread(rc);
}
