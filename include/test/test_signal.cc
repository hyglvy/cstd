/*ע���źŴ����������������*/
#include <signal.h>
#include <Windows.h>
#include <iostream>
using namespace std;
void cs(int n)
{
	if (n == SIGINT)
	{
		cout << "\n�㰴����ctrl+c" << endl;
		signal(SIGINT, cs);
		return;
	}
	if (n == SIGBREAK)
	{
		cout << "\n�㰴����ctrl+break,�����˳�����\n";
		exit(0);
	}
}
int test_signal()
{
	char ch;
	signal(SIGINT, cs); /*ע��ctrl+c�źŲ�����*/
	signal(SIGBREAK, cs); /*ע��ctrl+break�źŲ�����*/
	while (1)
	{
		cin >> ch;
		Sleep(10);
		if (cin.fail())
		{
			cin.clear();
			cin.sync();
		}
	}
	return 0;
}
