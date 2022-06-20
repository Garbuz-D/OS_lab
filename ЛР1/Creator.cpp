#include <windows.h>
#include <fstream>
#include <iostream>
using namespace std;

struct employee
{
    int num; // идентификационный номер сотрудника
    char name[10]; // имя сотрудника
    double hours; // количество отработанных часов
};

int main(int argc, char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    ofstream out(argv[1], ios::binary | ios::out);
    employee emp;
    for (int i = 0; i < atoi(argv[2]); i++) {
        cout << "Введите номер сотрудника\n";
        cin >> emp.num;
        cout << "Введите имя сотрудника\n";
        cin >> emp.name;
        cout << "Введите количество отработанных часов\n";
        cin >> emp.hours;
        out.write((char*)&emp, sizeof(emp));
    }
    out.close();
    return 0;
}

