#ifndef headerfile
#define headerfile
#include<iostream>
#include<WinSock2.h>
#include<socketapi.h>
#include<queue>
#include<vector>
#include<pthread.h>
#define BUFFER_MAX_SIZE 255
using namespace std;
#pragma comment(lib,"ws2_32.lib")

void initWinsock() {
    //��ʼ��WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;//WSADATA�ṹ������ĵ�ֵַ
    setlocale(LC_ALL, "chs");
    //int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
    //�ɹ�ʱ�᷵��0��ʧ��ʱ���ط���Ĵ������ֵ
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        cout << "WSAStartup() error!" << endl;
    }
}
//UTF-8תUnicode 
std::wstring Utf82Unicode(const std::string& utf8string) {
    int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
    if (widesize == ERROR_NO_UNICODE_TRANSLATION) {
        throw std::exception("Invalid UTF-8 sequence.");
    }
    if (widesize == 0) {
        throw std::exception("Error in conversion.");
    }
    std::vector<wchar_t> resultstring(widesize);
    int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
    if (convresult != widesize) {
        throw std::exception("La falla!");
    }
    return std::wstring(&resultstring[0]);
}
//unicode תΪ ascii 
string WideByte2Acsi(wstring& wstrcode) {
    int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
    if (asciisize == ERROR_NO_UNICODE_TRANSLATION) {
        throw std::exception("Invalid UTF-8 sequence.");
    }
    if (asciisize == 0) {
        throw std::exception("Error in conversion.");
    }
    std::vector<char> resultstring(asciisize);
    int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);
    if (convresult != asciisize) {
        throw std::exception("La falla!");
    }
    return std::string(&resultstring[0]);
}
//utf-8 ת ascii 
string UTF_82ASCII(string& strUtf8Code) {
    string strRet("");
    //�Ȱ� utf8 תΪ unicode 
    wstring wstr = Utf82Unicode(strUtf8Code);
    //���� unicode תΪ ascii 
    strRet = WideByte2Acsi(wstr);
    return strRet;
}
//ascii ת Unicode 
wstring Acsi2WideByte(string& strascii) {
    int widesize = MultiByteToWideChar(CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);
    if (widesize == ERROR_NO_UNICODE_TRANSLATION) {
        throw std::exception("Invalid UTF-8 sequence.");
    }
    if (widesize == 0) {
        throw std::exception("Error in conversion.");
    }
    std::vector<wchar_t> resultstring(widesize);
    int convresult = MultiByteToWideChar(CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);
    if (convresult != widesize) {
        throw std::exception("La falla!");
    }
    return std::wstring(&resultstring[0]);
}
//Unicode ת Utf8 
std::string Unicode2Utf8(const std::wstring& widestring) {
    int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
    if (utf8size == 0) {
        throw std::exception("Error in conversion.");
    }
    std::vector<char> resultstring(utf8size);
    int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);
    if (convresult != utf8size) {
        throw std::exception("La falla!");
    }
    return std::string(&resultstring[0]);
}
//ascii ת Utf8 
string ASCII2UTF8(string& strAsciiCode) {
    string strRet("");
    //�Ȱ� ascii תΪ unicode 
    wstring wstr = Acsi2WideByte(strAsciiCode);
    //���� unicode תΪ utf8 
    strRet = Unicode2Utf8(wstr);
    return strRet;
}

#endif // DEBUG