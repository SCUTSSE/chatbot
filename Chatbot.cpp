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
			printf("\n聊天结束");
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
	printf("请输入你想要查询的星座序号：\n");
	printf("1--水瓶座\t7--狮子座\n");
	printf("2--双鱼座\t8--处女座\n");
	printf("3--白羊座\t9--天秤座\n");
	printf("4--金牛座\t10--天蝎座\n");
	printf("5--双子座\t11--射手座\n");
	printf("6--巨蟹座\t12--摩羯座\n");

	//Get users's input
	char temp[128];
	std::cin >> temp;
	int len = strlen(temp);

	//Determine if it's a number, if not, return to initial interface.
	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("输入错误，默认为选项一");
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
		strcpy_s(horoscope, "水瓶座");
		break;
	case 2:
		strcpy_s(horoscope, "双鱼座"); 
		break;
	case 3:
		strcpy_s(horoscope, "白羊座"); 
		break;
	case 4:
		strcpy_s(horoscope, "金牛座"); 
		break;
	case 5:
		strcpy_s(horoscope, "双子座"); 
		break;
	case 6:
		strcpy_s(horoscope, "巨蟹座"); 
		break;
	case 7:
		strcpy_s(horoscope, "狮子座"); 
		break;
	case 8:
		strcpy_s(horoscope, "处女座");
		break;
	case 9:
		strcpy_s(horoscope, "天秤座"); 
		break;
	case 10:
		strcpy_s(horoscope, "天蝎座"); 
		break;
	case 11:
		strcpy_s(horoscope, "射手座"); 
		break;
	case 12:
		strcpy_s(horoscope, "摩羯座"); 
		break;
	default:
		printf("输入错误，默认为选项一\n"); 
		strcpy_s(horoscope, "水瓶座"); 
		Sleep(1000);

	}

	system("cls");

	printf("请输入你想要查询的序号：\n");

	printf("1--整体运势\n");
	printf("2--爱情运势\n");
	printf("3--财富运势\n");
	printf("4--资料\n");
	printf("5--性格\n");

	//The same as above, ensuring correct input.
	std::cin >> temp;
	len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("输入错误，默认为选项一");
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
		case 1:strcat_s(horoscope, "整体运势");
			break;

		case 2:strcat_s(horoscope, "爱情运势");
			break;
		case 3:strcat_s(horoscope, "财富运势");
			break;
		case 4:strcat_s(horoscope, "资料");
			break;
		case 5:strcat_s(horoscope, "性格");
			break;
		default: 
			printf("输入错误，系统默认为选项一\n"); 
			strcat_s(horoscope, "整体运势");
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
	std::cout << "是否继续查询？\n"
		<< "1-- Yes\n2-- No\t（其他输入默认为No）\n"
		<< "User: ";
	
	//Ensure the input is correct.
	std::cin >> temp;
	len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("\n输入错误，返回初始界面");
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
	printf("请输入你想要查询天气的城市：\n");
	std::cin >> city;
	const char weather[] = "天气";
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
	std::cout << "是否继续查询？\n"
		<< "1-- Yes\n2-- No\t（其他输入默认为No）\n"
		<< "User: ";

	char temp[128];
	std::cin >> temp;
	int len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("\n输入错误，返回初始界面");
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
	printf("请输入你想要查询的日期类型：\n");

	printf("1--今天日期\n");
	printf("2--今天农历日期\n");
	
	int num;
	char temp[128];
	std::cin >> temp;
	int len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("输入错误，默认为选项一");
			num = 1;
			Sleep(1000);
			break;
		}
		sscanf_s(temp, "%d", &num);
	}



	switch (num)
	{
	case 1:
		strcpy_s(date, "今天日期");
		break;
	case 2:
		strcpy_s(date, "今天农历日期");
		break;
	default: 
		printf("\n输入错误，系统默认为当天日期\n"); 
		strcpy_s(date, "今天日期");
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
	std::cout << "是否继续查询？\n"
		      <<"1-- Yes\n2-- No\t（其他输入默认为No）\n"
		      << "User: " ;
	

	std::cin >> temp;
	len = strlen(temp);

	for (int i = 0; i < len; i++)
	{
		if (temp[i]<'0' || temp[i]>'9')
		{
			printf("输入错误，返回初始界面");
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
	
	std::cout<<"我是图灵机器人,请问有什么能帮到您的嘛?\n\n";

	//Introduce the function of Tuling robot.
	std::cout << "请输入所需服务类型的序号:\n"
		      << "1-- 跟小灵畅聊\n"
			  << "2-- 国内城市的天气\n"
			  << "3-- 十二星座\n"
			  << "4-- 查询日期\n"
			  << "5-- 退出程序" << std::endl;

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
				printf("\n输入错误，退出本程序");
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
			std::cout << "温馨提示：若想退出聊天功能，输入“exit”即可返回初始界面。\n\n" ;
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
			std::cout << "输入的序号不正确。请输入一个新的序号。\n\n";
			break;
		}

		if (type != 5)
		{
			std::cout << "请输入所需服务类型的序号:\n"
				<< "1-- 跟小灵畅聊\n"
				<< "2-- 国内城市的天气\n"
				<< "3-- 十二星座\n"
				<< "4-- 查询日期\n"
				<< "5-- 退出程序" << std::endl;
		}
	}
}