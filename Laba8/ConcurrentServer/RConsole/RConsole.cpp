
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <consoleapi2.h>
#define PIPE TEXT("\\\\.\\pipe\\smc")

using namespace std;

string GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	default: msgText = "***ERROR***";
		break;
	};
	return msgText;
}
string SetPipeError(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
};

int main()
{//Программа RConsole предназначена для ввода команд управления сервером и  
	//для вывода диагностических сообщений полученных  от сервера .  
	//RConsole  является клиентом именованного канала
	setlocale(LC_ALL, "Rus");
	//SetConsoleTitle("Remote Console"); // заголовок консольного окна

	try
	{
		printf("\nCommands:\n 1 - Start  \t \n 2 - Stop  \t \n 3 - Exit  \t \n 4 - Statistics  \n 5 - Wait  \t\n 6 - Shutdown  \t \n 7 - Finish RConsole\n\n");
		char ReadBuf[50] = "";// Буффер для примема сообшения от сервера
		char WriteBuf[2] = "";// Буффер для отправки сообщения серверу
		DWORD nBytesRead;
		DWORD nBytesWrite;

		int Code = 0;// Код команды

		char serverName[256];
		char PipeName[512]="\\\\.\\pipe\\smc";
		bool result;


		cout << "Enter server name: ";
		cin >> serverName;
		result = sprintf_s(PipeName, "\\\\%s\\pipe\\smc", serverName);

		BOOL fSuccess;

		SECURITY_ATTRIBUTES SecurityAttributes;
		SECURITY_DESCRIPTOR SecurityDescriptor;

		fSuccess = InitializeSecurityDescriptor(
			&SecurityDescriptor,
			SECURITY_DESCRIPTOR_REVISION);

		if (!fSuccess) {
			throw new string("InitializeSecurityDescriptor(): Ошибка");
		}

		fSuccess = SetSecurityDescriptorDacl(
			&SecurityDescriptor,
			TRUE,
			NULL,
			FALSE);

		if (!fSuccess) {
			throw new string("SetSecurityDescriptorDacl(): Ошибка");
		}

		SecurityAttributes.nLength = sizeof(SecurityAttributes);
		SecurityAttributes.lpSecurityDescriptor = &SecurityDescriptor;
		SecurityAttributes.bInheritHandle = FALSE;

		HANDLE hNamedPipe = CreateFile(PIPE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, &SecurityAttributes);

		do
		{
			printf("Command: ");
			scanf_s("%d", &Code);
			if (Code > 0 && Code < 7)
			{
				sprintf_s(WriteBuf, "%d", Code - 1);
				if (!WriteFile(hNamedPipe, WriteBuf, strlen(WriteBuf) + 1, &nBytesWrite, NULL))
					throw "WriteFile: Ошибка ";
				if (ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL))
					cout << ReadBuf << endl;
				else
					throw "ReadFile: Ошибка ";

			}
			else if (Code > 7) printf("Неверная команда.\n\n");
		} while (Code != 7 && Code != 3 && Code != 6);
	}
	catch (char* ErrorPipeText)
	{
		printf("%s", ErrorPipeText);
		cout << GetLastError() << endl;
	}
	system("pause");
	return 0;
}
