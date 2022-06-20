
#include <iostream>
#include <windows.h>
#include <fstream>
#include <codecvt>
#include <string>

using namespace std;

struct employee
{
    int num; // идентификационный номер сотрудника
    char name[10]; // имя сотрудника
    double hours; // количество отработанных часов
};

struct message {
    char type;
    int id;
};

int procNum, recordsNum;
fstream file;
employee emp;
HANDLE* processes;

void thread(HANDLE hNamedPipe);

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    string fileName;
    cout << "Введите имя файла\n";
    cin >> fileName;
    cout << "Введите число сотрудников\n";
    cin >> recordsNum;
     file =  fstream(fileName, ios::trunc | ios::binary | ios::in | ios::out);
    

    HANDLE* readAccess = new HANDLE[recordsNum];
    HANDLE* writeAccess = new HANDLE[recordsNum];

    for (int i = 0; i < recordsNum; i++) {
        cout << "Введите номер сотрудника\n";
        cin >> emp.num;
        cout << "Введите имя сотрудника\n";
        cin >> emp.name;
        cout << "Введите количество отработанных часов\n";
        cin >> emp.hours;
        file.write((char*)&emp, sizeof(emp));
        readAccess[i] = CreateEvent(NULL, 1, 1, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("r" + to_string(emp.num)).c_str());
       writeAccess[i] = CreateEvent(NULL, 1, 1, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("w" + to_string(emp.num)).c_str());
    }
    cout << "\n" << fileName << "\n";
    file.clear();
    file.seekg(0);
    for (int i = 0; i < recordsNum; i++) {
        file.read((char*)&emp, sizeof(emp));
        cout << emp.num << "\n" << emp.name << '\n' << emp.hours << "\n\n";
    }

    
    cout << "Введите колилчество процессов\n";
    cin >> procNum;




    STARTUPINFO* si = new STARTUPINFO[procNum];
    PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[procNum];
    processes = new HANDLE[procNum];
    ZeroMemory(si, procNum * sizeof(si[0]));
    for (int i = 0; i < procNum; i++) {
        si[i].cb = sizeof(STARTUPINFO);
        CreateProcess(NULL, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("Client.exe").c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &(si[i]), &(pi[i]));
        processes[i] = pi[i].hProcess;
    }

    DWORD* idthread = new DWORD[procNum];
    HANDLE* threads = new HANDLE[procNum];
    for (int i = 0; i < procNum; i++) {
        HANDLE hNamedPipe = CreateNamedPipe(
            L"\\\\.\\pipe\\named_pipe",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_WAIT, // синхронная передача сообщений
            procNum, // максимальное количество экземпляров канала
            0, // размер выходного буфера по умолчанию
            0, // размер входного буфера по умолчанию
            INFINITE, // клиент ждет связь бесконечно долго
            (LPSECURITY_ATTRIBUTES)NULL // защита по умолчанию
        );
        threads[i] = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(thread), (void*)hNamedPipe, 0, &idthread[i]);
    }

    WaitForMultipleObjects(procNum, threads, true, INFINITE);

    system("pause");
}

void thread(HANDLE hNamedPipe) {
    ConnectNamedPipe(hNamedPipe, NULL);

    while (WaitForMultipleObjects(procNum, processes, true, 0)) {
        message msg;
        DWORD dwRead;
        ReadFile(hNamedPipe, (char*)&msg, sizeof(msg), &dwRead, NULL);
        file.clear();
        file.seekg(0);
        int pos;
        for (int i = 0; i < recordsNum; i++) {
            pos = file.tellg();
            file.read((char*)&emp, sizeof(emp));
            if (emp.num == msg.id) {
                break;
            }
        }
        WriteFile(hNamedPipe, &emp, sizeof(emp), &dwRead, NULL);
        if (msg.type == 'w') {
            ReadFile(hNamedPipe, &emp, sizeof(emp), &dwRead, NULL);
            for (int i = 0; i < recordsNum; i++) {
                file.clear();
                file.seekg(pos);
                file.write((char*)&emp, sizeof(emp));
            }
        }
    }
}
