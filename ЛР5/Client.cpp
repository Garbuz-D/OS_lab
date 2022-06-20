#include <windows.h>
#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <conio.h>

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

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    HANDLE hPipe;
    DWORD cbRead, dwMode;
    while (1)
    {
        WaitNamedPipe(L"\\\\.\\pipe\\named_pipe", INFINITE);
        hPipe = CreateFile(
            L"\\\\.\\pipe\\named_pipe",   // pipe name 
            GENERIC_READ |  // read and write access 
            GENERIC_WRITE,
            0,              // no sharing 
            NULL,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe 
            0,              // default attributes 
            NULL);          // no template file 

         // Break if the pipe handle is valid. 
        if (hPipe != INVALID_HANDLE_VALUE)
            break;

        // Exit if an error other than ERROR_PIPE_BUSY occurs. 
        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            printf("Could not open pipe\n");
            return 0;
        }

        // All pipe instances are busy, so wait for 20 seconds. 
        if (!WaitNamedPipe(L"\\\\.\\pipe\\named_pipe", 20000))
        {
            printf("Could not open pipe\n");
            return 0;
        }
    }

    // The pipe connected; change to message-read mode. 
    dwMode = PIPE_READMODE_MESSAGE;
    boolean fSuccess = SetNamedPipeHandleState(
        hPipe,    // pipe handle 
        &dwMode,  // new pipe mode 
        NULL,     // don't set maximum bytes 
        NULL);    // don't set maximum time 
    if (!fSuccess)
    {
        printf("SetNamedPipeHandleState failed.\n");
        return 0;
    }

    while (true) {
        cout << "1 - модифицировать запись\n2 - прочитать запись\n3 - выйти\n";
        int ans;
        cin >> ans;
        if (ans == 1) {
            boolean ok = false;
            while (!ok) {
                cout << "Введите ID\n";
                int id;
                cin >> id;
                HANDLE* event = new HANDLE[2];
                event[0] = OpenEvent(EVENT_ALL_ACCESS, true, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("r" + to_string(id)).c_str());
                if (event[0] != NULL) {
                    ok = true;
                    event[1] = OpenEvent(EVENT_ALL_ACCESS, false, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("w" + to_string(id)).c_str());
                    cout << "Подождите, доступ к записи может быть заблокирован...\n";
                    WaitForMultipleObjects(2, event, true, INFINITE);
                    ResetEvent(event[1]);
                    message msg;
                    msg.id = id;
                    msg.type = 'w';
                    employee emp;
                        TransactNamedPipe(
                            hPipe,                  // pipe handle 
                            (char*)&msg,              // message to server
                            sizeof(msg), // message length 
                            (char*)&emp,              // buffer to receive reply
                            sizeof(emp),  // size of read buffer
                            &cbRead,                // bytes read
                            NULL);                  // not overlapped 
                        cout << "Имя сотрудника: " << emp.name << "\nЧисло отработанных часов: " << emp.hours << "\nВведите изменённое имя\n";
                        cin >> emp.name;
                        cout << "\nВведите изменённое число часов\n";
                        cin >> emp.hours;
                        DWORD dwRead;
                        WriteFile(hPipe, (char*)&emp, sizeof(emp), &dwRead, NULL);
                        cout << "\nНажмите любую клавишу, чтобы завершить доступ\n";
                        _getch();
                        SetEvent(event[1]);
                }
                else {
                    cout << "\nСотрудника с таким номером нет\n";
                }
            }
        }
        else if (ans == 2) {
            boolean ok = false;
            while (!ok) {
                cout << "\nВведите ID\n";
                int id;
                cin >> id;
                HANDLE* event = new HANDLE[2];
                event[0] = OpenEvent(EVENT_ALL_ACCESS, true, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("r" + to_string(id)).c_str());
                if (event[0] != NULL) {
                    ok = true;
                    event[1] = OpenEvent(EVENT_ALL_ACCESS, false, (LPWSTR)wstring_convert<codecvt_utf8<wchar_t>>().from_bytes("w" + to_string(id)).c_str());
                    cout << "\nПодождите, доступ к записи может быть заблокирован...\n";
                    WaitForSingleObject(event[1], INFINITE);
                    ResetEvent(event[0]);
                    message msg;
                    msg.id = id;
                    msg.type = 'r';
                    employee emp;
                    TransactNamedPipe(
                        hPipe,                  // pipe handle 
                        (char*)&msg,              // message to server
                        sizeof(msg), // message length 
                        (char*)&emp,              // buffer to receive reply
                        sizeof(emp),  // size of read buffer
                        &cbRead,                // bytes read
                        NULL);                  // not overlapped 
                    cout << "Имя сотрудника: " << emp.name << "\nЧисло отработанных часов: " << emp.hours << "\nНажмите любую клавишу, чтобы завершить доступ\n";
                    _getch();
                    SetEvent(event[0]);
                }
                else {
                    cout << "\nСотрудника с таким номером нет\n";
                }
            }
        }
        else {
            return 0;
        }
    }
}
