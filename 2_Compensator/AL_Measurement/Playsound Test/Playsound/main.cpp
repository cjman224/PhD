#include <stdio.h>
#include <Windows.h>

#pragma comment(lib,"winmm.lib")


void main()
{
	getchar();

	PlaySound(L"test.wav", NULL, SND_FILENAME);


	getchar();

	PlaySound(L"test.wav", NULL, SND_FILENAME | SND_LOOP);

	getchar();

	PlaySound(NULL, NULL, NULL);
}