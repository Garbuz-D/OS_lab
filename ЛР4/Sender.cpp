#include <windows.h>
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    

    HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"ready");
    HANDLE hSemaphore1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"isFilled");
    HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"message");
    ReleaseSemaphore(hSemaphore, 1, NULL);

    while (true) {
        cout << "1 - send a message\n2 - exit\n";
        int answer;
        cin >> answer;
        if (answer == 1) {
            if (WaitForSingleObject(hSemaphore1, 0) != WAIT_OBJECT_0) {
                cout << "there is no space for a message. Wait...\n";
                WaitForSingleObject(hSemaphore1, INFINITE);
            }

            cout << "message:\n";
            string message;
            string str;
            do {
                cin >> str;
                if (str.size() > 19) {
                    cout << "string length must be less than 20\n";
                }
            } while (str.size() > 19);

            message = str;
            unsigned strLen = message.size();
            ofstream fout(argv[1], ios::binary | ios::out | ios::app);
            fout.write((char*)&strLen, sizeof(unsigned));
            fout.write((char*)message.c_str(), message.size());
            fout.close();
            PulseEvent(hEvent);
        }
        else {
            return 0;
        }
    }
}

