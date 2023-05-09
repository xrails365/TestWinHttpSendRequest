#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <psapi.h>
#include <winternl.h>
#include <winhttp.h>
#include <wincrypt.h>
#include <limits>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <codecvt>
#include <fstream>

#pragma comment(lib, "advapi32.lib")
#pragma comment (lib, "crypt32.lib")
#pragma comment(lib, "winhttp")


int WriteLog(std::string message) {
    // Create an output file stream and open the file for writing
    std::string filename = "output.txt";
    std::string text = message;
    ;

    std::ofstream outfile(filename, std::ios::app);
    if (outfile.is_open())
    {
        outfile << text << std::endl;
        outfile.close();
        std::cout << "Text appended to file successfully." << std::endl;
    }
    else
    {
        std::cout << "Unable to open file." << std::endl;
    }

    return 0;
}

std::string WinTest(std::string param) {
    
    HMODULE hModule = GetModuleHandle(NULL);
    std::vector<unsigned char> buffer;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;
    LPSTR pszOutBuffer = NULL;
    BOOL  bResults = FALSE;
    HINTERNET  hSession = NULL,
        hConnect = NULL,
        hRequest = NULL;
    WriteLog("WinHttpOpen");
    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(L"WinHTTP Example/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);


    // Specify an HTTP server.
    if (hSession)
        hConnect = WinHttpConnect(hSession, L"google.com",
            80, 0);
    else
        //printf("Failed in WinHttpConnect (%u)\n", GetLastError());
        WriteLog("Failed in WinHttpConnect");

    DWORD error = GetLastError();
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    std::string message2(messageBuffer, size);
    WriteLog(message2);
    WriteLog(std::to_string(error));

    std::string str = "";
    str.append("?q=Jpar6uG4e1cD8UmPg3oOyspJKjRRZXj0");
    str.append(param);
    LPCWSTR lpwstr = L"";
    int len1 = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0);
    int len2 = lstrlenW(lpwstr);
    std::wstring wstr(len1 + len2, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), &wstr[0], len1);
    std::copy(lpwstr, lpwstr + len2, &wstr[len1]);
    LPCWSTR result = wstr.c_str();
    std::wcout << result << std::endl;

    WriteLog("Before WinHttpConnect result check");
    // Create an HTTP request handle.
    if (hConnect) {
        WriteLog("WinHttpConnect success!");
        hRequest = WinHttpOpenRequest(hConnect, L"GET", result,
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            NULL);
    }
    else {
        WriteLog("Failed in WinHttpConnect: ");
        error = GetLastError();

        size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        std::string message(messageBuffer, size);
        WriteLog(message);
        WriteLog(std::to_string(error));
    }


    WriteLog("WinHttpSendRequest");
    // Send a request.
    if (hRequest)
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0, WINHTTP_NO_REQUEST_DATA, 0,
            0, 0);
    else { 
        WriteLog("Failed in WinHttpOpenRequest: ");
        WriteLog(std::to_string(error));
    }


    // End the request.
    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, NULL);
    else {
        WriteLog("Failed in WinHttpSendRequest: ");
        DWORD error = GetLastError();
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        std::string message(messageBuffer, size);
        WriteLog(messageBuffer);
        WriteLog(std::to_string(error));
    }
   
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);


    return "buffer_str";
}


int Run(bool Test) {
    WinTest("&test");
}

int main(int argc, char* argv[])
{
    Run(FALSE);

    return 0;
}


__declspec(dllexport) int MyFunction(int arg1, int arg2)
{
    // function code goes here
    Run(FALSE);
    return 1;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            Run(FALSE);
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

