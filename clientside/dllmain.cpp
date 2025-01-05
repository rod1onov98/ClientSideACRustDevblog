#include "pch.h"
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <tchar.h>
#include <wininet.h>
#include "httplib.h"

#pragma comment(lib, "wininet.lib")

std::vector<std::wstring> triggers = { L"exloader", L"injector", L"loader", L"cheat", L"extreme", L"superiority", L"asta.cc", L"hydrogenesis", L"destroyer", L"clarity", L"Tox1dProject", L"Skyline", L"BestHack", L"plusminus", L"nonbasy", L"MyCheats" };

std::wstring getname(HANDLE process) {
    wchar_t processName[MAX_PATH] = L"<RustClient.exe>";
    HMODULE hMod;
    DWORD cbNeeded;

    if (EnumProcessModules(process, &hMod, sizeof(hMod), &cbNeeded)) {
        GetModuleBaseName(process, hMod, processName, sizeof(processName) / sizeof(wchar_t));
    }

    return std::wstring(processName);
}

bool istrigger(const std::wstring& processName) {
    for (const auto& word : triggers) {
        if (processName.find(word) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

std::wstring getdevice() {
    wchar_t deviceName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(deviceName) / sizeof(deviceName[0]);
    GetComputerName(deviceName, &size);
    return std::wstring(deviceName);
}

std::wstring getsteampath() {
    HKEY hKey;
    wchar_t steamPath[MAX_PATH];
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Valve\\Steam", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD pathLength = sizeof(steamPath) / sizeof(steamPath[0]);
        if (RegQueryValueEx(hKey, L"InstallPath", NULL, NULL, (LPBYTE)steamPath, &pathLength) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return steamPath;
        }
        RegCloseKey(hKey);
    }
    wchar_t defaultSteamPath[] = L"C:\\Program Files (x86)\\Steam";
    return defaultSteamPath;
}

std::wstring getsteamid(const std::wstring& steamPath) {
    std::wstring configFilePath = steamPath + L"\\config\\config.vdf";
    std::wifstream file(configFilePath);
    if (!file.is_open()) {
        return L"not found cfg";
    }

    std::wstring line;
    while (std::getline(file, line)) {
        if (line.find(L"SteamID") != std::wstring::npos) {
            std::wstringstream ss(line);
            std::wstring key, value;
            ss >> key >> value;
            return value;
        }
    }

    return L"not found steam id";
}

void loadtoftp(const std::wstring& localFilePath, const std::wstring& remoteFileName, const std::wstring& remoteDir) {
    HINTERNET hInternet = InternetOpen(L"ftp addr", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return;

    HINTERNET hFtpSession = InternetConnect(hInternet, L"ftp address", INTERNET_DEFAULT_FTP_PORT, L"username", L"password", INTERNET_SERVICE_FTP, 0, 0);
    if (!hFtpSession) {
        InternetCloseHandle(hInternet);
        return;
    }

    if (!FtpSetCurrentDirectory(hFtpSession, remoteDir.c_str())) {
        InternetCloseHandle(hFtpSession);
        InternetCloseHandle(hInternet);
        return;
    }

    FtpPutFile(hFtpSession, localFilePath.c_str(), remoteFileName.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);

    InternetCloseHandle(hFtpSession);
    InternetCloseHandle(hInternet);
}

void logdetect(const std::wstring& triggerProcessName) {
    std::wstring steamPath = getsteampath();
    std::wstring steamID = getsteamid(steamPath);

    std::wstring logFileName = steamID + L".txt";
    std::wofstream logFile(logFileName, std::ios_base::app);
    logFile << L"detected: " << triggerProcessName << std::endl;
    logFile.close();

    loadtoftp(logFileName, logFileName, L"detectedusers");
}

void makescreen() {
    int screenX = GetSystemMetrics(SM_CXSCREEN);
    int screenY = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenX, screenY);

    SelectObject(hMemoryDC, hBitmap);
    BitBlt(hMemoryDC, 0, 0, screenX, screenY, hScreenDC, 0, 0, SRCCOPY);

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fileHeader.bfType = 0x4D42;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = screenX;
    infoHeader.biHeight = screenY;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 32;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    DWORD dwBmpSize = ((screenX * infoHeader.biBitCount + 31) / 32) * 4 * screenY;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char* lpbitmap = (char*)GlobalLock(hDIB);

    GetDIBits(hMemoryDC, hBitmap, 0, (UINT)screenY, lpbitmap, (BITMAPINFO*)&infoHeader, DIB_RGB_COLORS);

    std::wstring steamPath = getsteampath();
    std::wstring steamID = getsteamid(steamPath);
    std::wstring localFilePath = steamID + L".bmp";
    std::ofstream ofs(std::string(localFilePath.begin(), localFilePath.end()), std::ios::binary);
    ofs.write((char*)&fileHeader, sizeof(BITMAPFILEHEADER));
    ofs.write((char*)&infoHeader, sizeof(BITMAPINFOHEADER));
    ofs.write(lpbitmap, dwBmpSize);
    ofs.close();

    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    loadtoftp(localFilePath, localFilePath, L"screens");
}

void ftpside(const std::wstring& deviceName) {
    HINTERNET hInternet = InternetOpen(L"ftp addr", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return;

    HINTERNET hFtpSession = InternetConnect(hInternet, L"ftp address", INTERNET_DEFAULT_FTP_PORT, L"username", L"password", INTERNET_SERVICE_FTP, 0, 0);
    if (!hFtpSession) {
        InternetCloseHandle(hInternet);
        return;
    }

    const int bufferSize = 512;
    char buffer[bufferSize];
    DWORD bytesRead;
    HINTERNET hFile = FtpOpenFile(hFtpSession, L"banned/usersbanned.txt", GENERIC_READ, FTP_TRANSFER_TYPE_ASCII, 0);
    if (hFile) {
        std::string content;
        while (InternetReadFile(hFile, buffer, bufferSize, &bytesRead) && bytesRead > 0) {
            content.append(buffer, bytesRead);
        }
        InternetCloseHandle(hFile);

        std::wstring contentW(content.begin(), content.end());
        if (contentW.find(deviceName) != std::wstring::npos) {
            MessageBox(NULL, L"YOUR BANNED!", L"USER DEVICE IS BANNED. ", MB_OK | MB_ICONERROR);
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, GetCurrentProcessId());
            if (hProcess) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }
    }

    InternetCloseHandle(hFtpSession);
    InternetCloseHandle(hInternet);
}

void scanproc() {
    DWORD processes[1024], cbNeeded, cProcesses;
    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        return;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for (unsigned int i = 0; i < cProcesses; i++) {
        if (processes[i] != 0) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
            if (hProcess) {
                std::wstring processName = getname(hProcess);
                if (istrigger(processName)) {
                    logdetect(processName);
                    HANDLE hAtomicProcess = OpenProcess(PROCESS_TERMINATE, FALSE, GetCurrentProcessId());
                    if (hAtomicProcess) {
                        TerminateProcess(hAtomicProcess, 0);
                        CloseHandle(hAtomicProcess);
                    }
                }
                CloseHandle(hProcess);
            }
        }
    }
}

void saveinfo() {
    std::wstring steamPath = getsteampath();
    std::wstring steamID = getsteamid(steamPath);
    std::wstring deviceName = getdevice();
    std::wstring playerName = L"Player" + std::to_wstring(rand() % 1000000);

    std::wstring playerInfo = playerName + L"-" + steamID + L"-" + deviceName + L"\n";

    std::ofstream playerFile("players.txt", std::ios_base::app);
    playerFile << std::string(playerInfo.begin(), playerInfo.end());
    playerFile.close();

    loadtoftp(L"players.txt", L"players.txt", L"players");
}

void startlocal() {
    httplib::Server svr;
    svr.Get("/check-anticheat", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 200;
        res.set_content("ac is running", "text/plain");
        });
    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 200;
        res.set_content("serv is up and running", "text/plain");
        });
    svr.listen("127.0.0.1", 28015);
}

DWORD WINAPI monitorproc(LPVOID lpParam) {
    while (true) {
        scanproc();
        Sleep(1000);
    }
    return 0;
}

DWORD WINAPI monitorscreen(LPVOID lpParam) {
    while (true) {
        makescreen();
        Sleep(60000);
    }
    return 0;
}

DWORD WINAPI showwindow(LPVOID lpParam) {
    Sleep(5000);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        CreateThread(NULL, 0, [](LPVOID lpParam) -> DWORD {
            startlocal();
            return 0;
            }, NULL, 0, NULL);

        CreateThread(NULL, 0, showwindow, NULL, 0, NULL);
        HANDLE hProcess = GetCurrentProcess();
        logdetect(L"<Process Name>");
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}