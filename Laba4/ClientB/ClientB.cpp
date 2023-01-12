#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "Winsock2.h"
#include <chrono>
#pragma comment(lib, "WS2_32.lib")

#define SERVER_CALLSIGN "Hello"


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


bool GetServer(char* call, short port, struct sockaddr* from, int* flen)
{
    SOCKET cC;
    if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
        throw SetErrorMsgText("Socket", WSAGetLastError());

    int optval = 1;
    if (setsockopt(cC, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR)
        throw SetErrorMsgText("opt: ", WSAGetLastError());

    SOCKADDR_IN all;
    all.sin_family = AF_INET;
    all.sin_port = htons(port);
    all.sin_addr.s_addr = INADDR_BROADCAST;

    char ibuf[50];
    int  lobuf;
    int lc = sizeof(all);

    if ((lobuf = sendto(cC, call, strlen(call) + 1, NULL, (sockaddr*)&all, lc)) == SOCKET_ERROR)
        throw  SetErrorMsgText("send:", WSAGetLastError());

    if ((lobuf = recvfrom(cC, ibuf, sizeof(ibuf), NULL, (sockaddr*)&from, flen)) == SOCKET_ERROR)
    {
        throw  SetErrorMsgText("recv:", WSAGetLastError());
    }

    cout << "Received answer from server: " << ibuf << endl;

    if (closesocket(cC) == SOCKET_ERROR)
        throw SetErrorMsgText("CloseSocket: ", WSAGetLastError());

    return true;
}

int main(int argc, char** argv)
{
    SOCKET cS;
    WSADATA wsadata;
    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
            throw SetErrorMsgText("Startup: ", WSAGetLastError());

        SOCKADDR_IN serv;
        int lc = sizeof(serv);
        char call[50];
        call[0] = '\0';
        strcpy(call, SERVER_CALLSIGN);

        if (GetServer(call, 2000, (sockaddr*)&serv, &lc))
            cout << "Server is found, OK";

        cout << endl << inet_ntoa(serv.sin_addr) << endl;

        if (WSACleanup() == SOCKET_ERROR)
            throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
        system("pause");
    }
    catch (string errorMsgText)
    {
        cout << endl << errorMsgText;
    }
}