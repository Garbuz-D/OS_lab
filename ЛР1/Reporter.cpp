#include <windows.h>
#include <fstream>
#include <iostream>
#include <iomanip>
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

    ifstream in(argv[1], ios::binary | ios::in);
    ofstream out(argv[2]);
    out << "Отчёт по файлу «" << argv[1] << "»\n";
    out << "Номер сотрудника, имя сотрудника, часы, зарплата\n";
    employee emp;
    while (!in.eof()) {
        in.read((char*)&emp, sizeof(emp));
        if(!in.eof())
            out << emp.num << ", " << emp.name << ", " << emp.hours << ", " << emp.hours * atoi(argv[3]) << '\n';
    }
    out.close();
    in.close();
    return 0;
}