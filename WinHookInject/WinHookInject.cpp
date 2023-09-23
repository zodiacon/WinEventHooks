// WinHookInject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int main(int argc, const char* argv[]) {
	DWORD pid = argc < 2 ? 0 : atoi(argv[1]);
	if (pid == 0) {
		printf("Warning: injecting to potentially processes with threads connected to the current desktop.\n");
		printf("Continue? (y/n) ");
		char ans[3];
		gets_s(ans);
		if (tolower(ans[0]) != 'y')
			return 0;
	}

	auto hLib = ::LoadLibrary(L"Injected.Dll");
	if (!hLib) {
		printf("DLL not found!\n");
		return 1;
	}
	auto OnEvent = (WINEVENTPROC)::GetProcAddress(hLib, "OnEvent");
	if (!OnEvent) {
		printf("Event handler not found!\n");
		return 1;
	}
	auto hHook = ::SetWinEventHook(EVENT_MIN, EVENT_MAX, 
		hLib, OnEvent, pid, 0, WINEVENT_INCONTEXT);
	if (!hHook) {
		printf("Failed!\n");
		return 1;
	}
	//
	// keep process alive
	//
	::GetMessage(nullptr, nullptr, 0, 0);

	return 0;
}

