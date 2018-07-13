#include"Chatbot.h"
#include <iostream>
#include <Windows.h> 
#include <winhttp.h> 
#pragma comment(lib,"winhttp.lib")

//The API port is dynamic, where is Tuling APIs convenience provides for programmers.
#define TULING_URL  L"www.tuling123.com/openapi/api?key=7546f791d5c14a2db298f38379534293&info=%s"
static wchar_t String[1024];

//Three code conversions, using in the GetHttpPage function.
char* Tuling::UnicodeToANSI(const wchar_t *str)
{
	static char result[1024];
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL); //function detail https://www.cnblogs.com/MrYuan/p/5254040.html
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, len, NULL, NULL);
	result[len] = '\0';
	return result;
}

wchar_t* Tuling::UTF8ToUnicode(const char *str)
{
	static wchar_t result[1024];
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, result, len);
	result[len] = L'\0';
	return result;
}

wchar_t* Tuling::ANSIToUnicode(const char* str)
{
	int textlen;
	static wchar_t result[1024];
	textlen = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	memset(result, 0, sizeof(char) * (textlen + 1));
	MultiByteToWideChar(CP_ACP, 0, str, -1, (LPWSTR)result, textlen);
	return result;
}

//A main function for chatting
bool Tuling::GetHttpPage(char*uin)
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer = NULL;
	
	static HINTERNET hSession = WinHttpOpen(L"A Tuling API Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	static HINTERNET hConnect = NULL, hRequest = NULL;
	BOOL  bResults = FALSE;

	//Read data from parameter. ANSI encoding converts to Unicode. 
	wsprintf(String, TULING_URL, ANSIToUnicode(uin));

	//Create an HTTP connection session and give the host name, either a domain name or an IP address without prefix.
	if (hSession)
	{
		if (!hConnect)
			hConnect = WinHttpConnect(hSession, L"www.tuling123.com", INTERNET_DEFAULT_HTTP_PORT, 0);
	}

	//Create an HTTP request handle
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", String, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_ESCAPE_PERCENT | WINHTTP_FLAG_REFRESH);

	//Send request data
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

	// Request ends and data received
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	else
		printf("Error %d has occurred.\n", GetLastError());
	//If return FALSE, use getlasterror function to get the error message, the same as follows
	//The detail can be checked in the below website
	//http://blog.csdn.net/fengsh998/article/details/8201591

	// A loop is used internally to ensure all datas are received
	if (bResults)
	{
		do
		{
			//Check if there is any left data
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
				break;
			}

			if (!dwSize)
				break;

			//Allocate memory for the buffer and read it
			pszOutBuffer = new char[dwSize + 1];

			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				break;
			}

			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
			{
				printf("Error %u in WinHttpReadData.\n", GetLastError());
			}
			else
			{
				//The content returned by Tuling APIs is in UTF-8 encoding, which we need convert back to ANSI to display in the console;
				//printf("return:%s\n", UnicodeToANSI(UTF8ToUnicode(pszOutBuffer)) );

				//Because we didn't use the JSON library, we broke the string into pieces;
				pszOutBuffer[strlen(pszOutBuffer) - 2] = '\0';
				printf("Tuling:%s\n\n", UnicodeToANSI(UTF8ToUnicode(pszOutBuffer)) + 23);

				return true;
			}

			delete[] pszOutBuffer;
			if (!dwDownloaded)
				break;

		} while (dwSize > 0);
	}


	//Closing the opened handle
	if (hRequest) 
		WinHttpCloseHandle(hRequest);
	if (hConnect) 
		WinHttpCloseHandle(hConnect);
	if (hSession) 
		WinHttpCloseHandle(hSession);

	return false;
}

//A function to chat with Tuling AI robot (core function)
void Tuling::chatting()
{
	//Tuling chatting;
	char words[1024];
	
	printf("User:");
	while (gets_s(words))
	{
		//If users enter "exit", it returns to initial Interface.
		if (strstr(words, "exit") != NULL)
		{
			printf("\n�������");
			Sleep(1000);
			break;
		}
		else
		{
			Sleep(1000);
			//this pointer or obj is OK
			this->GetHttpPage(words);
			//chatting.GetHttpPage(words);

			printf("User:");
		}
	}

}

//A function to get horoscope fortune from APIs
void Tuling::get_horoscope()
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer = NULL;
	static HINTERNET hSession = WinHttpOpen(L"A Tuling API Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	static HINTERNET hConnect = NULL, hRequest = NULL;
	BOOL  bResults = FALSE;
	
	int num;
	char horoscope[20];

	//Print the choices for users.
	printf("����������Ҫ��ѯ��������ţ�\n");
	printf("1--ˮƿ��\t7--ʨ����\n");
	printf("2--˫����\t8--��Ů��\n");
	printf("3--������\t9--�����\n");
	printf("4--��ţ��\t10--��Ы��\n");
	printf("5--˫����\t11--������\n");
	printf("6--��з��\t12--Ħ����\n");

	//Get users's input
	char temp[128];
	std::cin >> temp;
	int len = strlen(temp);

	//Determine if it's a number, if not, return to initial interface.
	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("�������Ĭ��Ϊѡ��һ");
			num = 1;
			Sleep(1000);
			break;
		}
		sscanf_s(temp, "%d", &num);//Change the string into an integer.
	}

	
	//For users to choose
	switch (num)
	{
	case 1:
		strcpy_s(horoscope, "ˮƿ��");
		break;
	case 2:
		strcpy_s(horoscope, "˫����"); 
		break;
	case 3:
		strcpy_s(horoscope, "������"); 
		break;
	case 4:
		strcpy_s(horoscope, "��ţ��"); 
		break;
	case 5:
		strcpy_s(horoscope, "˫����"); 
		break;
	case 6:
		strcpy_s(horoscope, "��з��"); 
		break;
	case 7:
		strcpy_s(horoscope, "ʨ����"); 
		break;
	case 8:
		strcpy_s(horoscope, "��Ů��");
		break;
	case 9:
		strcpy_s(horoscope, "�����"); 
		break;
	case 10:
		strcpy_s(horoscope, "��Ы��"); 
		break;
	case 11:
		strcpy_s(horoscope, "������"); 
		break;
	case 12:
		strcpy_s(horoscope, "Ħ����"); 
		break;
	default:
		printf("�������Ĭ��Ϊѡ��һ\n"); 
		strcpy_s(horoscope, "ˮƿ��"); 
		Sleep(1000);

	}

	system("cls");

	printf("����������Ҫ��ѯ����ţ�\n");

	printf("1--��������\n");
	printf("2--��������\n");
	printf("3--�Ƹ�����\n");
	printf("4--����\n");
	printf("5--�Ը�\n");

	//The same as above, ensuring correct input.
	std::cin >> temp;
	len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("�������Ĭ��Ϊѡ��һ");
			num = 1;
			Sleep(1000);
			break;
		}
		sscanf_s(temp, "%d", &num);
	}

	system("cls");
	Sleep(300);
	
	switch (num)
	{
		case 1:strcat_s(horoscope, "��������");
			break;

		case 2:strcat_s(horoscope, "��������");
			break;
		case 3:strcat_s(horoscope, "�Ƹ�����");
			break;
		case 4:strcat_s(horoscope, "����");
			break;
		case 5:strcat_s(horoscope, "�Ը�");
			break;
		default: 
			printf("�������ϵͳĬ��Ϊѡ��һ\n"); 
			strcat_s(horoscope, "��������");
			Sleep(1000);
	}

	//The next step is similar to the chatting function, but it doesn't include the part of closing handle.
	char uin[1024];
	strcpy_s(uin, horoscope);

	wsprintf(String, TULING_URL, ANSIToUnicode(uin));
	system("cls");

	if (hSession)
	{
		if (!hConnect)
			hConnect = WinHttpConnect(hSession, L"www.tuling123.com", INTERNET_DEFAULT_HTTP_PORT, 0);
	}


	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", String, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_ESCAPE_PERCENT | WINHTTP_FLAG_REFRESH);

	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	else
		printf("Error %d has occurred.\n", GetLastError());


	if (bResults)
	{
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
				break;
			}

			if (!dwSize)
				break;


			pszOutBuffer = new char[dwSize + 1];

			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				break;
			}

			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
			{
				printf("Error %u in WinHttpReadData.\n", GetLastError());
			}
			else
			{
				pszOutBuffer[strlen(pszOutBuffer) - 2] = '\0';
				printf("%s\n\n", UnicodeToANSI(UTF8ToUnicode(pszOutBuffer)) + 23);
			}

			delete[] pszOutBuffer;
			if (!dwDownloaded)
				break;

		} while (dwSize > 0);
	}


	int choice;
	std::cout << "�Ƿ������ѯ��\n"
		<< "1-- Yes\n2-- No\t����������Ĭ��ΪNo��\n"
		<< "User: ";
	
	//Ensure the input is correct.
	std::cin >> temp;
	len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("\n������󣬷��س�ʼ����");
			choice = 2;
			Sleep(1000);
			break;
		}
		sscanf_s(temp, "%d", &choice);
	}

	if (choice == 1)
	{
		system("cls");
		get_horoscope();
	}

}

//A function to get weather from APIs
void Tuling::get_weather()
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer = NULL;
	static HINTERNET hSession = WinHttpOpen(L"A Tuling API Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	static HINTERNET hConnect = NULL, hRequest = NULL;
	BOOL  bResults = FALSE;

	char city[20];
	printf("����������Ҫ��ѯ�����ĳ��У�\n");
	std::cin >> city;
	const char weather[] = "����";
	strcat_s(city, weather);
	
	
	char uin[1024];
	strcpy_s(uin, city);
	
	wsprintf(String, TULING_URL, ANSIToUnicode(uin));
	system("cls");
	
	if (hSession)
	{
		if (!hConnect)
			hConnect = WinHttpConnect(hSession, L"www.tuling123.com", INTERNET_DEFAULT_HTTP_PORT, 0);
	}


	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", String, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_ESCAPE_PERCENT | WINHTTP_FLAG_REFRESH);

	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	else
		printf("Error %d has occurred.\n", GetLastError());


	if (bResults)
	{
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
				break;
			}

			if (!dwSize)
				break;


			pszOutBuffer = new char[dwSize + 1];

			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				break;
			}

			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
			{
				printf("Error %u in WinHttpReadData.\n", GetLastError());
			}
			else
			{
				pszOutBuffer[strlen(pszOutBuffer) - 2] = '\0';
				printf("%s\n\n", UnicodeToANSI(UTF8ToUnicode(pszOutBuffer)) + 23);
			}

			delete[] pszOutBuffer;
			if (!dwDownloaded)
				break;

		} while (dwSize > 0);
	}


	int choice;
	std::cout << "�Ƿ������ѯ��\n"
		<< "1-- Yes\n2-- No\t����������Ĭ��ΪNo��\n"
		<< "User: ";

	char temp[128];
	std::cin >> temp;
	int len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("\n������󣬷��س�ʼ����");
			choice = 2;
			Sleep(1000);
			break;
		}
		sscanf_s(temp, "%d", &choice);
	}
	if (choice == 1)
	{
		system("cls");
		get_weather();
	}


}

//A function to get date from APIs
void Tuling::get_date()
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer = NULL;
	static HINTERNET hSession = WinHttpOpen(L"A Tuling API Example Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	static HINTERNET hConnect = NULL, hRequest = NULL;
	BOOL  bResults = FALSE;

	char date[128];
	printf("����������Ҫ��ѯ���������ͣ�\n");

	printf("1--��������\n");
	printf("2--����ũ������\n");
	
	int num;
	char temp[128];
	std::cin >> temp;
	int len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("�������Ĭ��Ϊѡ��һ");
			num = 1;
			Sleep(1000);
			break;
		}
		sscanf_s(temp, "%d", &num);
	}



	switch (num)
	{
	case 1:
		strcpy_s(date, "��������");
		break;
	case 2:
		strcpy_s(date, "����ũ������");
		break;
	default: 
		printf("\n�������ϵͳĬ��Ϊ��������\n"); 
		strcpy_s(date, "��������");
		Sleep(1000);
	}


	char uin[1024];
	strcpy_s(uin, date);

	wsprintf(String, TULING_URL, ANSIToUnicode(uin));
	system("cls");

	if (hSession)
	{
		if (!hConnect)
			hConnect = WinHttpConnect(hSession, L"www.tuling123.com", INTERNET_DEFAULT_HTTP_PORT, 0);
	}


	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", String, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_ESCAPE_PERCENT | WINHTTP_FLAG_REFRESH);

	if (hRequest)
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	else
		printf("Error %d has occurred.\n", GetLastError());


	if (bResults)
	{
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
				break;
			}

			if (!dwSize)
				break;


			pszOutBuffer = new char[dwSize + 1];

			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				break;
			}

			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
			{
				printf("Error %u in WinHttpReadData.\n", GetLastError());
			}
			else
			{
				pszOutBuffer[strlen(pszOutBuffer) - 2] = '\0';
				printf("%s\n\n", UnicodeToANSI(UTF8ToUnicode(pszOutBuffer)) + 23);
			}

			delete[] pszOutBuffer;
			if (!dwDownloaded)
				break;

		} while (dwSize > 0);
	}


	int choice;
	std::cout << "�Ƿ������ѯ��\n"
		      <<"1-- Yes\n2-- No\t����������Ĭ��ΪNo��\n"
		      << "User: " ;
	

	std::cin >> temp;
	len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("������󣬷��س�ʼ����");
			choice = 2;
			Sleep(1000);
			break;
		}
		sscanf_s(temp, "%d", &choice);
	}

	if (choice == 1)
	{
		system("cls");
		get_date();
	}



}

//This is the initial interface for Tuling robot.
void Tuling::Interface()
{
	system("color F0");
	system("title Chatbot");
	
	std::cout<<"����ͼ�������,������ʲô�ܰﵽ������?\n\n";

	//Introduce the function of Tuling robot.
	std::cout << "����������������͵����:\n"
		      << "1-- ��С�鳩��\n"
			  << "2-- ���ڳ��е�����\n"
			  << "3-- ʮ������\n"
			  << "4-- ��ѯ����\n"
			  << "5-- �˳�����" << std::endl;

	int type = 0;
	char temp[128];
	while (type!=5)
	{
		std::cin >> temp;
		int len = strlen(temp);
	
		//Ensure the input is right.
		for (int i = 0; i < len; i++)
		{
			if (temp[i]<'0'||temp[i]>'9')
			{
				printf("\n��������˳�������");
				type = 5;
				Sleep(800);
				break;
			}
		}

		sscanf_s(temp, "%d", &type); // Converts a string to an integer
		system("cls");
		
		switch (type)
		{
		case 1:
			std::cout << "��ܰ��ʾ�������˳����칦�ܣ����롰exit�����ɷ��س�ʼ���档\n\n" ;
			char temp_s[128];
			gets_s(temp_s);
			chatting();
			system("cls");
			break;

		case 2:
			get_weather();
			system("cls");
			break;

		case 3:
			get_horoscope();
			system("cls");
			break;

		case 4:
			get_date();
			system("cls");
			break;

		case 5:
			system("cls");
			break;

		default:
			system("cls");
			std::cout << "�������Ų���ȷ��������һ���µ���š�\n\n";
			break;
		}

		if (type != 5)
		{
			std::cout << "����������������͵����:\n"
				<< "1-- ��С�鳩��\n"
				<< "2-- ���ڳ��е�����\n"
				<< "3-- ʮ������\n"
				<< "4-- ��ѯ����\n"
				<< "5-- �˳�����" << std::endl;
		}
	}
}