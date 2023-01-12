#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")

#define SERVER_CALLSIGN "Hello\0"
#define SERVER_PORT 2000 

using namespace std;

string GetErrorMsgText(int code)
{
    string msgText;
    switch (code)
    {
        case WSAEINTR: msgText = "WSAEINTR"; break;
        case WSAEACCES: msgText = "WSAEACCES"; break;
        case WSAEFAULT: msgText = "WSAEFAULT"; break;
        case WSAEINVAL: msgText = "WSAEINVAL"; break;
        case WSAEMFILE: msgText = "WSAEMFILE"; break;
        case WSAEWOULDBLOCK:msgText = "WSAEWOULDBLOCK"; break;
        case WSAEINPROGRESS:msgText = "WSAEINPROGRESS"; break;
        case WSAENOTSOCK:msgText = "WSAENOCTSOCK"; break;
        case WSAEDESTADDRREQ:msgText = "WSAEDESTADDRREQ"; break;
        case WSAEMSGSIZE:msgText = "WSAEMSGSIZE"; break;
        case WSAEPROTOTYPE:msgText = "WSAEPROTOTYPE"; break;
        case WSAENOPROTOOPT:msgText = "WSAENOPROTOOPT"; break;
        case WSAEPROTONOSUPPORT:msgText = "WSAEPROTONOSUPPORT"; break;
        case WSAESOCKTNOSUPPORT:msgText = "WSAESOCKTNOSUPPORT"; break;
        case WSAEOPNOTSUPP:msgText = "WSAEOPNOTSUPP"; break;
        case WSAEPFNOSUPPORT:msgText = "WSAEPFNOSUPPORT"; break;
        case WSAEAFNOSUPPORT:msgText = "WSAEAFNOSUPPORT"; break;
        case WSAEADDRINUSE:msgText = "WSAEADDRINUSE"; break;
        case WSAEADDRNOTAVAIL:msgText = "WSAEADDRNOTAVAIL"; break;
        case WSAENETDOWN:msgText = "WSAENETDOWN"; break;
        case WSAENETUNREACH:msgText = "WSAENETUNREACH"; break;
        case WSAENETRESET:msgText = "WSAENETRESET"; break;
        case WSAECONNABORTED:msgText = "WSAECONNABORTED"; break;
        case WSAECONNRESET:msgText = "WSAECONNRESET"; break;
        case WSAENOBUFS:msgText = "WSAENOBUFS"; break;
        case WSAEISCONN:msgText = "WSAEISCONN"; break;
        case WSAENOTCONN:msgText = "WSAENOTCONN"; break;
        case WSAESHUTDOWN:msgText = "WSAESHUTDOWN"; break;
        case WSAETIMEDOUT:msgText = "WSAETIMEDOUT"; break;
        case WSAECONNREFUSED:msgText = "WSAECONNREFUSED"; break;
        case WSAEHOSTDOWN:msgText = "WSAEHOSTDOWN"; break;
        case WSAEHOSTUNREACH:msgText = "WSAEHOSTUNREACH"; break;
        case WSAEPROCLIM:msgText = "WSAEPROCLIM"; break;
        case WSASYSNOTREADY:msgText = "WSASYSNOTREADY"; break;
        case WSAVERNOTSUPPORTED:msgText = "WSAVERNOTSUPPORTED"; break;
        case WSANOTINITIALISED:msgText = "WSANOTINITIALISED"; break;
        case WSAEDISCON:msgText = "WSAEDISCON"; break;
        case WSATYPE_NOT_FOUND:msgText = "WSATYPE_NOT_FOUND"; break;
        case WSAHOST_NOT_FOUND:msgText = "WSAHOST_NOT_FOUND"; break;
        case WSATRY_AGAIN:msgText = "WSATRY_AGAIN"; break;
        case WSANO_RECOVERY:msgText = "WSANO_RECOVERY"; break;
        case WSANO_DATA:msgText = "WSANO_DATA"; break;
        case WSA_INVALID_HANDLE:msgText = "WSA_INVALID_HANDLE"; break;
        case WSA_INVALID_PARAMETER:msgText = "WSA_INVALID_PARAMETER"; break;
        case WSA_IO_INCOMPLETE:msgText = "WSA_IO_INCOMPLETE"; break;
        case WSA_IO_PENDING:msgText = "WSA_IO_PENDING"; break;
        case WSA_NOT_ENOUGH_MEMORY:msgText = "WSA_NOT_ENOUGH_MEMORY"; break;
        case WSA_OPERATION_ABORTED:msgText = "WSA_OPERATION_ABORTED"; break;
        case WSAEINVALIDPROCTABLE:msgText = "WSAEINVALIDPROCTABLE"; break;
        case WSAEINVALIDPROVIDER:msgText = "WSAEINVALIDPROVIDER"; break;
        case WSAEPROVIDERFAILEDINIT:msgText = "WSAEPROVIDERFAILEDINIT"; break;
        case WSASYSCALLFAILURE:msgText = "WSASYSCALLFAILURE"; break;
        default: msgText = "***ERROR***"; break;
    }
    return msgText;
}

string SetErrorMsgText(string msgText, int code)
{
    return msgText + GetErrorMsgText(code);
}

bool GetServer(char* call, short port, struct sockaddr* from, int* flen)//Функция предназначена для отправки широковещательного запроса в локальную сеть 
{
    SOCKET cC;
    if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
        throw SetErrorMsgText("Socket", WSAGetLastError());

    int optval = 1;
    int timeout = 2000;

    SOCKADDR_IN all;
    all.sin_family = AF_INET;
    all.sin_port = htons(port);
    all.sin_addr.s_addr = INADDR_BROADCAST;

    char ibuf[50];
    int  lobuf;
    int lc = sizeof(all);

    if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)//Использование широковещательного IP-адреса требует специального режима работы сокета, 
        throw SetErrorMsgText("opt: ", WSAGetLastError());

    if ((lobuf = sendto(cC, call, strlen(call) + 1, NULL, (sockaddr*)&all, lc)) == SOCKET_ERROR)//отправка  широковещательного запроса 
        throw  SetErrorMsgText("send:", WSAGetLastError());

    if (setsockopt(cC, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(int)) == SOCKET_ERROR)
        throw SetErrorMsgText("opt: ", WSAGetLastError());

    bool result = true;

    if ((lobuf = recvfrom(cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)&from, flen)) == INVALID_SOCKET)//recvfrom   для ожидания отклика сервера
    {
        result = false;
    }

    return result;
}

bool PutAnswerToClient(SOCKET* sS, char* name, struct sockaddr* to, int* lto)//Функция предназначена для подтверждения сервером  запроса  клиента на установку соединения
{
    int  lobuf = 0;
    if ((lobuf = sendto(*sS, name, strlen(name) + 1, NULL, to, *lto)) == SOCKET_ERROR)
    {
        throw  SetErrorMsgText("Sendto:", WSAGetLastError());
        return false;
    }
    if (closesocket(*sS) == SOCKET_ERROR)
        throw SetErrorMsgText("CloseSocket: ", WSAGetLastError());
    return true;
};

bool GetRequestFromClient(SOCKET* sS, char* name, short port, struct sockaddr* from, int* flen)//предназначена для ожидания запроса клиентской программы
{
    if ((*sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
        throw SetErrorMsgText("socket:", WSAGetLastError());

    SOCKADDR_IN serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(*sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
        throw SetErrorMsgText("bind:", WSAGetLastError());

    char ibuf[50];
    int  lobuf = 0;
    while (true)
    {
        if ((lobuf = recvfrom(*sS, ibuf, sizeof(ibuf), NULL, from, flen)) == SOCKET_ERROR)//функция ожидания
        {//входит если сообщения является позывным
            if (WSAGetLastError() == WSAETIMEDOUT)
            {
                return false;
            }
            throw  SetErrorMsgText("recv:", WSAGetLastError());
        }

        if (strcmp(ibuf, name) == 0) 
        {
            return true;
        }
    }
};



int main()
{
    WSADATA wsadata;
    try
    {
        while (true)
        {
            if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
                throw SetErrorMsgText("Startup: ", WSAGetLastError());

            SOCKET sS;

            SOCKADDR_IN clnt1;
            memset(&clnt1, 0, sizeof(clnt1));
            int lc = sizeof(clnt1);

            char name[32];
            name[0] = '\0';
            strcpy(name, SERVER_CALLSIGN);

            SOCKADDR_IN serv;
            int tmplc = sizeof(serv);

            if (GetServer(name, 2000, (sockaddr*)&serv, &tmplc))
                cout << "===== WARNING: Another same server detected =====\n";

            int  i = 0;
            while (true)
            {
                cout << "GetRequestFromClient: ";
                bool result = GetRequestFromClient(&sS, name, SERVER_PORT, (sockaddr*)&clnt1, &lc);
                if (result)
                {
                    cout << "OK!\n";

                    cout << "PutAnswerToClient: ";

                    PutAnswerToClient(&sS, name, (sockaddr*)&clnt1, &lc);

                    cout << "OK!\n\n";
                }

                cout << inet_ntoa(clnt1.sin_addr) << endl;
                cout << clnt1.sin_port << endl << endl;
            }

            if (WSACleanup() == SOCKET_ERROR)
                throw SetErrorMsgText("Cleanup: ", WSAGetLastError());

            system("pause");

        }
    }
    catch (string errorMsgText)
    {
        cout << endl << errorMsgText;
    }
}
