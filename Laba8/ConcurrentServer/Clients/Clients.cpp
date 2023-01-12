#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <iostream>
#include <time.h>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")
#include <ctime>
#include "WS2tcpip.h"

using namespace std;


bool SystemMessage(char* ch)
{
	bool result = false;
	char Timeout[50] = "Close: timeout;", Close[50] = "Close: finish;", Abort[50] = "Close: Abort;";
	if (strcmp(ch, Timeout) == NULL) result = true;
	else if (strcmp(ch, Abort) == NULL) result = true;
	else if (strcmp(ch, Close) == NULL) result = true;
	return result;
}

string GetErrorMsgText(int code)
{
	setlocale(LC_ALL, "rus");
	string msgText;
	switch (code)
	{
	case WSAEINTR:msgText = "Работа функции прервана "; break;
	case WSAEACCES:msgText = "Разрешение отвергнуто"; break;
	case WSAEFAULT:msgText = "Ошибочный адрес"; break;
	case WSAEINVAL:msgText = "Ошибка в аргументе "; break;
	case WSAEMFILE:msgText = "Слишком много файлов открыто"; break;
	case WSAEWOULDBLOCK:msgText = "Ресурс временно недоступен"; break;
	case WSAEINPROGRESS:msgText = "Операция в процессе развития"; break;
	case WSAEALREADY:msgText = "Операция уже выполняется "; break;
	case WSAENOTSOCK:msgText = "Сокет задан неправильно   "; break;
	case WSAEDESTADDRREQ:msgText = "Требуется адрес расположения "; break;
	case WSAEMSGSIZE:msgText = "Сообщение слишком длинное "; break;
	case WSAEPROTOTYPE:msgText = "Неправильный тип протокола для сокета "; break;
	case WSAENOPROTOOPT:msgText = "Ошибка в опции протокола"; break;
	case WSAEPROTONOSUPPORT:msgText = "Протокол не поддерживается "; break;
	case WSAESOCKTNOSUPPORT:msgText = "Тип сокета не поддерживается "; break;
	case WSAEOPNOTSUPP:msgText = "Операция не поддерживается "; break;
	case WSAEPFNOSUPPORT:msgText = "Тип протоколов не поддерживается "; break;
	case WSAEAFNOSUPPORT:msgText = "Тип адресов не поддерживается протоколом"; break;
	case WSAEADDRINUSE:msgText = "Адрес уже используется "; break;
	case WSAEADDRNOTAVAIL:msgText = "Запрошенный адрес не может быть использован"; break;
	case WSAENETDOWN:msgText = "Сеть отключена "; break;
	case WSAENETUNREACH:msgText = "Сеть не достижима"; break;
	case WSAENETRESET:msgText = "Сеть разорвала соединение"; break;
	case WSAECONNABORTED:msgText = "Программный отказ связи "; break;
	case WSAECONNRESET:msgText = "Связь восстановлена "; break;
	case WSAENOBUFS:msgText = "Не хватает памяти для буферов"; break;
	case WSAEISCONN:msgText = "Сокет уже подключен"; break;
	case WSAENOTCONN:msgText = "Сокет не подключен"; break;
	case WSAESHUTDOWN:msgText = "Нельзя выполнить send: сокет завершил работу"; break;
	case WSAETIMEDOUT:msgText = "Закончился отведенный интервал  времени"; break;
	case WSAECONNREFUSED:msgText = "Соединение отклонено  "; break;
	case WSAEHOSTDOWN:msgText = "Хост в неработоспособном состоянии"; break;
	case WSAEHOSTUNREACH:msgText = "Нет маршрута для хоста "; break;
	case WSAEPROCLIM:msgText = "Слишком много процессов "; break;
	case WSASYSNOTREADY:msgText = "Сеть не доступна "; break;
	case WSAVERNOTSUPPORTED:msgText = "Данная версия недоступна "; break;
	case WSANOTINITIALISED:msgText = "Не выполнена инициализация WS2_32.DLL "; break;
	case WSAEDISCON:msgText = "Выполняется отключение "; break;
	case WSATYPE_NOT_FOUND:msgText = "Класс не найден "; break;
	case WSAHOST_NOT_FOUND:msgText = "Хост не найден"; break;
	case WSATRY_AGAIN:msgText = "Неавторизированный хост не найден "; break;
	case WSANO_RECOVERY:msgText = "Неопределенная  ошибка "; break;
	case WSANO_DATA:msgText = "Нет записи запрошенного типа "; break;
	case WSA_INVALID_HANDLE:msgText = "Указанный дескриптор события  с ошибкой"; break;
	case WSA_INVALID_PARAMETER:msgText = "Один или более параметров с ошибкой "; break;
	case WSA_IO_INCOMPLETE:msgText = "Объект ввода-вывода не в сигнальном состоянии"; break;
	case WSA_IO_PENDING:msgText = "Операция завершится позже  "; break;
	case WSA_NOT_ENOUGH_MEMORY:msgText = "Не достаточно памяти "; break;
	case WSA_OPERATION_ABORTED:msgText = "Операция отвергнута "; break;
	case WSAEINVALIDPROCTABLE:msgText = "Ошибочный сервис "; break;
	case WSAEINVALIDPROVIDER:msgText = "Ошибка в версии сервиса"; break;
	case WSAEPROVIDERFAILEDINIT:msgText = "Невозможно инициализировать сервис"; break;
	case WSASYSCALLFAILURE:msgText = "Аварийное завершение системного вызова "; break;
	}

	return msgText;
}

string SetErrorMsgText(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
}

bool GetServer(
	char* call, //[in] позывной сервера 
	SOCKADDR_IN* from, //[out] указатель на SOCKADDR_IN
	int* flen, //[out] указатель на размер from 
	SOCKET* cC, //сокет
	SOCKADDR_IN* all
)
{
	char ibuf[50], //буфер ввода 
		obuf[50]; //буфер вывода
	int libuf = 0, //количество принятых байт
		lobuf = 0; //количество отправленных байт

	int optval = 1;
	DWORD val = 2000;

	if (setsockopt(*cC, SOL_SOCKET, SO_BROADCAST, (const char*)&val, sizeof(DWORD) == SOCKET_ERROR))
		throw GetErrorMsgText(WSAGetLastError());

	if ((lobuf = sendto(*cC, call, strlen(call) + 1, NULL,
		(sockaddr*)all, sizeof(*all))) == SOCKET_ERROR)
		throw SetErrorMsgText("Sendto:", WSAGetLastError());


	if ((libuf = recvfrom(*cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)from, flen)) == SOCKET_ERROR)
		if (WSAGetLastError() == WSAETIMEDOUT) return false;
		else throw SetErrorMsgText("Recv:", WSAGetLastError());
	if (strcmp(call, ibuf) == 0) {
		return true;
	}
	else {
		return false;
	}
}

int main()//Программа Client предназначена для пересылки данных серверу и получения ответа от сервера.  
//Программа может работать, как на одном компьютере с сервером 
//(будет использоваться интерфейс внутренней петли), так и  на  другом компьютере, 
//соединенным с компьютером сервера сетью TCP/IP. 
//Для окончания работы с сервером программа формирует и отправляет сегмент данных нулевой длины.
{
	setlocale(LC_ALL, "Russian");
	int port = 0;
	SOCKET ClientSocket;
	WSADATA wsaData;
	try
	{
		// Инициализируем Winsocket
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup:", WSAGetLastError());

		// Создаем сокет
		if ((ClientSocket = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("Socket:", WSAGetLastError());

		int ch = 0;
		bool fin = false;
		int cCall = 0;

		int max = 100, lobuf = 1;
		char obuf[50] = "";
		char ibuf[50] = "";
		int bport = 2000;

		char Call[50]; //запрос клиента
		char Name[50] = "DESKTOP-T60D9R9"; //имя сервера
		char Calls[50] = "KAV"; //позывной

		SOCKADDR_IN Server = { 0 };
		Server.sin_family = AF_INET;
		Server.sin_port = htons(port);

		SOCKADDR_IN Server_IN; 
		int Flen = sizeof(Server);
		int chs = 0;
		cout << "1 - Enter name of server" << endl << "2 - Name" << endl << "> ";
		cin >> chs;
		if (chs == 1)
		{
			cout << "Enter: ";
			cin >> Name;
			hostent* s = gethostbyname(Name);
			if (s == NULL) throw "Server not found;";
			cout << "Enter port of server: ";
			cin >> port;
			Server_IN.sin_addr = *(struct in_addr*)s->h_addr_list[0];
		}
		else if (chs == 2)
		{
			cout << "Enter name(FIO): ";
			cin >> Calls;
			SOCKET cC; //серверный сокет
			if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
				throw SetErrorMsgText("Socket:", WSAGetLastError());

			struct timeval timeout;
			timeout.tv_sec = 3000;
			timeout.tv_usec = 0;
			if (setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR)
				throw SetErrorMsgText("Opt:", WSAGetLastError());

			SOCKADDR_IN all; //параметры сокета sS
			all.sin_family = AF_INET; //используется IP-адресация 
			all.sin_port = htons(bport); //порт для широковещания
			all.sin_addr.s_addr = INADDR_BROADCAST; //всем
			SOCKADDR_IN clnt; //параметры сокета клиента
			memset(&clnt, 0, sizeof(clnt)); //обнулить память
			int lc = sizeof(clnt); //размер SOCKADDR_IN

			bool bsr = GetServer(Calls, &clnt, &lc, &cC, &all);
			if (bsr == false) throw "Server not found;";
			else
			{
				Server_IN.sin_addr.s_addr = clnt.sin_addr.s_addr;
				if (closesocket(cC) == SOCKET_ERROR)
					throw SetErrorMsgText("Closesocket:", WSAGetLastError());
				cout << "Enter port of server: ";
				cin >> port;
			}

		}
		else throw "Wrong code";

		// Параметры сокета сервера
		Server_IN.sin_family = AF_INET;		//используется IP-адресация 
		Server_IN.sin_port = htons(port);	//TCP-порт

		// Установить соединение с сокетом
		if ((connect(ClientSocket, (sockaddr*)&Server_IN, sizeof(Server_IN))) == SOCKET_ERROR)
			/*throw SetErrorMsgText("Connect:",WSAGetLastError());*/
			throw "Fail connection;";

		while (!fin)
		{
			cout << "Service: " << endl << "\t1 - Rand" << endl << "\t2 - Time" << endl << "\t3 - Echo" << endl << "> ";
			cin >> cCall;
			if (cCall == 1 || cCall == 2 || cCall == 3)
			{
				fin = true;
				break;
			}

			else
			{
				if (closesocket(ClientSocket) == SOCKET_ERROR)
					throw SetErrorMsgText("Closesocket:", WSAGetLastError());
				throw "Wrong code;";
			}
		}

		switch (cCall)
		{
		case 1:
			strcpy_s(Call, "Rand");
			cout << "\tEnter Rand command or other:" << endl;
			break;
		case 2:
			strcpy_s(Call, "Time");
			cout << "\tEnter Time command or other:" << endl;
			break;
		case 3:
			strcpy_s(Call, "Echo");
			cout << "\tEnter string or exit :" << endl;
			break;
		default:
			strcpy_s(Call, "Echo");
		}

		// Отправляем запрос серверу
		if ((lobuf = send(ClientSocket, Call, sizeof(Call), NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("Send:", WSAGetLastError());
		char rCall[50];
		if ((lobuf = recv(ClientSocket, rCall, sizeof(rCall), NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("Recv:", WSAGetLastError());

		if (strcmp(Call, rCall) != 0)
			throw "Fail of server";
		else
		{
			bool Check = true;
			fin = false;

			u_long nonblk = 1;
			if (SOCKET_ERROR == ioctlsocket(ClientSocket, FIONBIO, &nonblk))
				throw SetErrorMsgText("Ioctlsocket:", WSAGetLastError());
			clock_t StartTime = clock();
			bool rcv = true;
			char iib[50];
			while (!fin)
			{
				if (rcv)
				{
					std::cout << "->";
					std::cin >> iib;
					// Отправка сообщения
					if ((lobuf = send(ClientSocket, iib, sizeof(iib), NULL)) == SOCKET_ERROR)
						throw SetErrorMsgText("Send:", WSAGetLastError());
					rcv = false;
				}
				// Принимаем сообщение

				if ((recv(ClientSocket, obuf, sizeof(obuf), NULL)) == SOCKET_ERROR)
				{
					switch (WSAGetLastError())
					{
					case WSAEWOULDBLOCK: Sleep(100); break;
					default: throw SetErrorMsgText("Recv:", WSAGetLastError());
					}
				}
				else
				{
					if (SystemMessage(obuf))
					{
						printf("Server stopped connection: %s\n", obuf);
						break;
					}
					else
						// Выводим полученное сообщение
						printf("Received message:[%s]\n", obuf);
					rcv = true;
				}

			}

			clock_t FinishTime = clock();
			printf("Time: %lf sec.\n", (double)(FinishTime - StartTime) / CLOCKS_PER_SEC);

		}

		// Закрываем сокет
		if (closesocket(ClientSocket) == SOCKET_ERROR)
			throw SetErrorMsgText("Closesocket:", WSAGetLastError());

		// Очищаем ресурсы
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (char* errorMsgText)
	{
		cout << errorMsgText << endl;
	}

	system("pause");
	return 0;
}