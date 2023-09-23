#include "pch.h"

#pragma comment(lib, "oleacc")

const char* EventNameToString(DWORD event) {
    switch (event) {
        case EVENT_SYSTEM_SOUND: return "Sound";
        case EVENT_SYSTEM_ALERT: return "Alert";
        case EVENT_SYSTEM_FOREGROUND: return "Foreground";
        case EVENT_SYSTEM_MENUSTART: return "Menu Start";
        case EVENT_SYSTEM_MENUEND: return "Menu End";
        case EVENT_SYSTEM_CAPTURESTART: return "Capture Start";
        case EVENT_SYSTEM_CAPTUREEND: return "Capture End";
        case EVENT_SYSTEM_MOVESIZESTART: return "Move/Size Start";
        case EVENT_SYSTEM_MOVESIZEEND: return "Move/Size End";
        case EVENT_SYSTEM_DIALOGSTART: return "Dialog Start";
        case EVENT_SYSTEM_DIALOGEND: return "Dialog End";
        case EVENT_SYSTEM_SWITCHSTART: return "Switch Start";
        case EVENT_SYSTEM_SWITCHEND: return "Switch End";
        case EVENT_SYSTEM_MINIMIZESTART: return "Minimize Start";
        case EVENT_SYSTEM_MINIMIZEEND: return "Minimize End";

        case EVENT_OBJECT_CREATE: return "Object Create";
        case EVENT_OBJECT_DESTROY: return "Object Destroy";
        case EVENT_OBJECT_SHOW: return "Object Show";
        case EVENT_OBJECT_HIDE: return "Object Hide";
        case EVENT_OBJECT_STATECHANGE: return "State Changed";
        case EVENT_OBJECT_LOCATIONCHANGE: return "Location Changed";
        case EVENT_OBJECT_NAMECHANGE: return "Name Change";
        case EVENT_OBJECT_DESCRIPTIONCHANGE: return "Desc Changed";
        case EVENT_OBJECT_VALUECHANGE: return "Value Changed";
        case EVENT_OBJECT_PARENTCHANGE: return "Parent Changed";
    }
    return "";
}

HANDLE hConsole;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, PVOID lpReserved) {
	switch (reason) {
		case DLL_PROCESS_DETACH:
			if (hConsole)   // be nice
				::CloseHandle(hConsole);
			break;

		case DLL_PROCESS_ATTACH:
			if (::AllocConsole()) {
				auto hConsole = ::CreateFile(L"CONOUT$", GENERIC_WRITE, 
                    0, nullptr, OPEN_EXISTING, 0, nullptr);
				if (hConsole == INVALID_HANDLE_VALUE)
					return FALSE;
				::SetStdHandle(STD_OUTPUT_HANDLE, hConsole);
			}
			break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport)
void CALLBACK OnEvent(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild, DWORD idEventThread, DWORD time) {
	CComPtr<IAccessible> spAcc;
	CComVariant child;
	::AccessibleObjectFromEvent(hwnd, idObject, idChild, &spAcc, &child);
	CComBSTR name;
	if (spAcc)
		spAcc->get_accName(CComVariant(idChild), &name);

	printf("Event: 0x%X (%s) HWND: 0x%p, ID: 0x%X Child: 0x%X TID: %u Time: %u Name: %ws\n",
		event, EventNameToString(event),
        hwnd, idObject, idChild, idEventThread,
		time, name.m_str);
}
