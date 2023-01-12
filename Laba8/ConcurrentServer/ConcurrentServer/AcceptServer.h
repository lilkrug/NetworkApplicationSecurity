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


static void WaitClients()//ожидание клиента
{
	bool ListEmpty = false;
	while (!ListEmpty)
	{

		EnterCriticalSection(&scListContact);
		ListEmpty = Contacts.empty();
		LeaveCriticalSection(&scListContact);
		SleepEx(0, TRUE);
	}
}

bool AcceptCycle(int squirt, SOCKET* s)
{
	bool rc = false;

	Contact c(Contact::ACCEPT, "AcceptServer");

	c.hAcceptServer = hAcceptServer;


	if ((c.s = accept(*s, (sockaddr*)&c.prms, &c.lprms)) == INVALID_SOCKET)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			throw  SetErrorMsgText("Accept:", WSAGetLastError());
	}
	else
	{
		//Критические секции, является одним из самых простых механизмов синхронизации и в  нашей модели могут быть использованы для  исключения совместного  использования списка ListContact  потоками  AcceptServer и GarbageCleaner. 	
		//Критическая секция является объектом операционной системы    типа CRITICAL_SECTION. 
		rc = true;
		InterlockedIncrement(&Accept);
		InterlockedIncrement(&Work);
		EnterCriticalSection(&scListContact);//Войти в критическую секцию
		Contacts.push_front(c);//добавть новый элемент в список подключений
		LeaveCriticalSection(&scListContact);//Покинуть критическую секцию
		SetEvent(Event);

	}
	return rc;
};

void CommandsCycle(TalkersCmd& cmd, SOCKET* s)      // цикл обработки команд
{
	int counter = 0;
	while (cmd != Exit)     // цикл обработки команд консоли и подключений
	{
		switch (cmd)
		{
		case Start:  //возобновить подключение клиентов
			cmd = Getcommand;
			counter = 10;
			break;
		case Stop:  //остановить подключение клиентов 
			cmd = Getcommand;
			counter = 0;
			break;
		case Wait:  //обработать текущих, а после разрешить подключение новых клиентов
			WaitClients();
			cmd = Getcommand;
			counter = MAXCLIENTS;
			break;
		case Shutdown:  //обработать текущих, а после завершить сервер
			WaitClients();
			cmd = Exit;
			break;
		};

		if (cmd != Exit && counter > Work)
		{
			if (AcceptCycle(counter, s))   //цикл  запрос/подключение (accept) 
			{
				cmd = Getcommand;
			}
			SleepEx(0, TRUE);
		}

	}
};



DWORD WINAPI AcceptServer(LPVOID pPrm)//Подключение клиентов к серверу
{// для выполнения процедуры подключения клиентов к серверу, 
//для исполнения команд консоли управления,
//для запуска процессов EchoServer, обслуживающих запросы клиентских программ по  созданным соединениям. 
//Создает список подключений,  который далее будем называть ListContanct.  
//При подключении очередного клиента, процесс AcceptServer добавляет  в ListContact  элемент, 
//предназначенный для хранения информации о состоянии  данного подключения
	cout << "AcceptServer started\n" << endl;
	DWORD rc = 0; //код возврата  
	SOCKET  ServerSocket;
	WSADATA wsaData;
	
	try
	{
		// Инициализируем Winsocket 
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());

		// Создаем сокет сервера для приема подключений клиентов
		if ((ServerSocket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("Socket:", WSAGetLastError());

		// Параметры сокета сервера
		SOCKADDR_IN Server_IN;
		Server_IN.sin_family = AF_INET;
		Server_IN.sin_port = htons(port);
		Server_IN.sin_addr.s_addr = ADDR_ANY;
		// Применить параметры к сокету
		if (bind(ServerSocket, (LPSOCKADDR)&Server_IN, sizeof(Server_IN)) == SOCKET_ERROR)
			throw  SetErrorMsgText("Bind:", WSAGetLastError());

		// Переводим сокет в режим прослушивания
		if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
			throw  SetErrorMsgText("Listen:", WSAGetLastError());

		// Переводим сокет в неблокирующий режим
		u_long nonblk;
		if (ioctlsocket(ServerSocket, FIONBIO, &(nonblk = 1)) == SOCKET_ERROR)
			throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());

		TalkersCmd* command = (TalkersCmd*)pPrm;
		// Функция обрабатывает сообщения от консоли
		CommandsCycle(*((TalkersCmd*)command), &ServerSocket);

		// Закрываем сокет
		if (closesocket(ServerSocket) == SOCKET_ERROR)
			throw  SetErrorMsgText("Сlosesocket:", WSAGetLastError());
		// Очищаем ресурсы
		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		std::cout << errorMsgText << endl;
	}
	cout << "AcceptServer stoped;\n" << endl;
	// Выходим из потока
	ExitThread(rc);
}





