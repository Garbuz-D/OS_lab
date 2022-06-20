#include <windows.h>
#include <iostream>
using namespace std;
volatile int n, max_, min_, mid;
volatile int* arr;
DWORD WINAPI min_max()
{
	min_ = 0;
	max_ = 0;
	for (int i = 1; i < n; i++) {
		if (arr[i] > arr[max_]) {
			max_ = i;
		}
		Sleep(7);
		if (arr[i] < arr[min_]) {
			min_ = i;
		}
		Sleep(7);
	}
	cout << "min = " << arr[min_] << "\nmax = " << arr[max_] << "\n";
	return 0;
}
void WINAPI average() {
	int sum = 0;
	for (int i = 0; i < n; i++) {
		sum += arr[i];
		Sleep(12);
	}
	mid = sum / n;
	cout << "average = " << mid << "\n";
}
int main()
{
	HANDLE hmin_max;
	DWORD IDmin_max;
	HANDLE haverage;
	DWORD IDaverage;
	cout << "n = ";
	int N;
	cin >> N;
	n = N;
    arr = new int[n];
	for (int i = 0; i < n; i++) {
		cout << "arr[" << i << "] = ";
		int elem;
		cin >> elem;
		arr[i] = elem;
	}
	hmin_max = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)min_max, NULL, 0, &IDmin_max);
	if (hmin_max == NULL)
		return GetLastError();
	
	haverage = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)average, NULL, 0, &IDaverage);
	if (haverage == NULL)
		return GetLastError();
	WaitForSingleObject(hmin_max, INFINITE);
	WaitForSingleObject(haverage, INFINITE);
	CloseHandle(hmin_max);
	CloseHandle(haverage);
	arr[min_] = arr[max_] = mid;
	for (int i = 0; i < n; i++) {
		cout << arr[i] << " ";
	}
	system("pause");
	return 0;
}