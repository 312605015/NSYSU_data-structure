#include <iostream>
#include <Windows.h>
#include "MMSystem.h"
using namespace std;
int main()
{
	PlaySound(TEXT("ai.wav"),NULL,SND_SYNC);
	system("pause");
	return 0;
}
