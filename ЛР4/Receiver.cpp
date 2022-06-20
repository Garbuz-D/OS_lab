#include<windows.h>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <string>
#include <locale>
using namespace std;
int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    cout << "File name:\n";
    string fileName;
    cin >> fileName;
    ofstream fout(fileName, ios::binary | ios::out);
    fout.close();
    ifstream fin(fileName, ios::binary | ios::in);
    cout << "Records maximum number:\n";
    int recNum;
    cin >> recNum;
    cout << "Processes number:\n";
    int procNum;
    cin >> procNum;

    HANDLE semaphore = CreateSemaphore(NULL, -procNum, 0, L"ready");
    HANDLE semaphore1 = CreateSemaphore(NULL, recNum, recNum, L"isFilled");

    STARTUPINFO* si = new STARTUPINFO[procNum];
    PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[procNum];
    ZeroMemory(si, procNum*sizeof(si[0]));
    for (int i = 0; i < procNum; i++) {
        si[i].cb = sizeof(STARTUPINFO);
        CreateProcess(NULL, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("Sender.exe " + fileName + " " + to_string(recNum)).c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &(si[i]), &(pi[i]));
    }

    WaitForSingleObject(semaphore, INFINITE);

    HANDLE hEvent = CreateEvent(NULL, true, false, L"message");

    while (true) {
        cout << "1 - read a message\n2 - exit\n";
        int answer;
        cin >> answer;
        if (answer == 1) {
                unsigned strLen;
                fin.read((char*)&strLen, sizeof(unsigned));
                if (fin.eof()) {
                    cout << "There are no messages. Wait...\n";
                    fin.clear();
                    WaitForSingleObject(hEvent, INFINITE);
                }
                else {
                string message;
                message.resize(strLen);
                fin.read((char*)message.data(), strLen);
                ReleaseSemaphore(semaphore1, 1, NULL);
                cout << message << '\n';
            }
        }
        else {
            fin.close();
            return 0;
        }
    }
}

