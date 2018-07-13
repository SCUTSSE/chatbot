#ifndef CHATBOT_H
#define CHATBOT_H


class Tuling
{
public:
	
	//function prototype
	char *UnicodeToANSI(const wchar_t*);
	wchar_t *UTF8ToUnicode(const char*);
	wchar_t *ANSIToUnicode(const char*);
	
	bool GetHttpPage(char*);//the initial version of the following functions
	
	//performance function
	void get_weather();
	void get_horoscope();
	void get_date();
	void chatting();//the core function
	void Interface();
};





#endif