#include <windows.h>
#include <iostream>
#include <vector>
using namespace std;
volatile int n;
 HANDLE* cantContinue;
 HANDLE* stopThread;
 HANDLE* continueThread;
 HANDLE beginAll;
 CRITICAL_SECTION cs;
volatile int* arr;

void WINAPI marker(int num) {
	WaitForSingleObject(beginAll, INFINITY);
	srand(num);
	vector<int> list;
	for (int i = 0; ; i++) {
		int k = rand() % n;
		if (arr[k] == 0) {
			list.push_back(k);
			Sleep(5);
			arr[k] = num;
			Sleep(5);
		}
		else {
			EnterCriticalSection(&cs);
			cout << "thread number: " << num << "\nelements marked: " << i << "\nunmarked element index: " << k << "\n";
			LeaveCriticalSection(&cs);
			SetEvent(cantContinue[num]);
			
			
			HANDLE eventArr[2];
			
			eventArr[0] = stopThread[num];
			eventArr[1] = continueThread[num];
			WaitForMultipleObjects(2, eventArr, false, INFINITE);
			if (WaitForSingleObject(stopThread[num], 0) == WAIT_OBJECT_0) {
				for (int j = 0; j < list.size(); j++) {
					arr[list[j]] = 0;
				}
				return;
			}
			ResetEvent(continueThread[num]);
			ResetEvent(cantContinue[num]);
		}
	}
}
int main()
{
	InitializeCriticalSection(&cs);
	cout << "n = ";
	int N;
	cin >> N;
	n = N;
	arr = new int[n];
	for (int i = 0; i < n; i++) {
		arr[i] = 0;
	}
	cout << "number of threads: ";
	int num;
	cin >> num;
	HANDLE* threads = new HANDLE[num];
	DWORD* IDs = new DWORD[num];
	cantContinue = new HANDLE[num];
	stopThread = new HANDLE[num];
	continueThread = new HANDLE[num];
	beginAll = CreateEvent(NULL, true, false, NULL);
	for (int i = 0; i < num; i++) {
		cantContinue[i] = CreateEvent(NULL, true, false, NULL);
		stopThread[i] = CreateEvent(NULL, true, false, NULL);
		continueThread[i] = CreateEvent(NULL, true, false, NULL);
		threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)marker, (LPVOID)i, 0, &IDs[i]);
	}

	SetEvent(beginAll);

	while (WaitForMultipleObjects(num, threads, TRUE, 0) != WAIT_OBJECT_0) {
		WaitForMultipleObjects(num, cantContinue, TRUE, INFINITE);
		EnterCriticalSection(&cs);
		for (int i = 0; i < n; i++) {
			cout << arr[i] << " ";
		}
		cout << "\nEnter thread number: ";
		int threadNum;
		cin >> threadNum;
		LeaveCriticalSection(&cs);
		SetEvent(stopThread[threadNum]);
		WaitForSingleObject(threads[threadNum], INFINITE);
		for (int i = 0; i < n; i++) {
			if (i != threadNum) {
				SetEvent(continueThread[i]);
			}
		}
		EnterCriticalSection(&cs);
		for (int i = 0; i < n; i++) {
			cout << arr[i] << " ";
		}
		cout << '\n';
		LeaveCriticalSection(&cs);
	}
	system("pause");
	return 0;
}