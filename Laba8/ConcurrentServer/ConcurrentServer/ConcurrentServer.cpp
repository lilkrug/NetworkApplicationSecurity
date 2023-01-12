#include "Files.h"
#include "AcceptServer.h"
#include "GarbageCleaner.h"
#include "Error.h"
#include "ConsolePipe.h"
#include "DispathServer.h"
#include "ResponseServer.h"

int main(int argc, _TCHAR* argv[]) //Главный поток сервера
{//запуск, инициализация  и завершение работы  сервера
   InitializeCriticalSection(&scListContact);//Инициализировать критическую секцию 
   volatile TalkersCmd cmd = Start;

   st1 = LoadLibrary(TEXT("Servicelibrary.dll"));
   ts1 = (HANDLE(*)(char*, LPVOID))GetProcAddress(st1, "SSS"); //импортируем функцию
   if (st1 == NULL) cout << "Fail DLL;" << endl;
   else cout << "Loaded DLL:      " << dllname << ";" << endl << endl;

   hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
  //DispathServer((LPVOID)&cmd);
   HANDLE hDispathServer = CreateThread(NULL, NULL, DispathServer, (LPVOID)&cmd, NULL, NULL);
   //GarbageCleaner((LPVOID)&cmd);
   HANDLE hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);
   HANDLE hConsolePipe = CreateThread(NULL, NULL, ConsolePipe, (LPVOID)&cmd, NULL, NULL);
   HANDLE hResponseServer = CreateThread(NULL, NULL, ResponseServer, (LPVOID)&cmd, NULL, NULL);

   //выставление приоритетов потокам
   SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_BELOW_NORMAL);
   SetThreadPriority(hDispathServer, THREAD_PRIORITY_NORMAL);
   SetThreadPriority(hConsolePipe, THREAD_PRIORITY_NORMAL);
   SetThreadPriority(hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL);
   SetThreadPriority(hAcceptServer, THREAD_PRIORITY_HIGHEST);

   WaitForSingleObject(hAcceptServer, INFINITE);
   CloseHandle(hAcceptServer);

   WaitForSingleObject(hDispathServer, INFINITE);
   CloseHandle(hDispathServer);

   WaitForSingleObject(hGarbageCleaner, INFINITE);
   CloseHandle(hGarbageCleaner);

   WaitForSingleObject(hConsolePipe, INFINITE);
   CloseHandle(hConsolePipe);

   WaitForSingleObject(hResponseServer, INFINITE);
   CloseHandle(hResponseServer);

   DeleteCriticalSection(&scListContact);//Разрушить критическую секцию

   FreeLibrary(st1);

   system("pause");
   return 0;
//    AcceptServer((LPVOID)&cmd);
   // WSAGetLastError();

}
