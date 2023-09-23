// SimpleWinEventHook.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

void CALLBACK OnEvent(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
    LONG idObject, LONG idChild, DWORD idEventThread, DWORD time) {
    CComPtr<IAccessible> spAcc;
    CComVariant child;
    ::AccessibleObjectFromEvent(hwnd, idObject, idChild, &spAcc, &child);
    CComBSTR name;
    if (spAcc)
        spAcc->get_accName(CComVariant(idChild), &name);
    DWORD pid = 0;
    WCHAR exeName[MAX_PATH];
    PCWSTR pExeName = L"";

    if (hwnd && ::GetWindowThreadProcessId(hwnd, &pid)) {
        auto hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProcess) {
            DWORD size = _countof(exeName);
            if (::QueryFullProcessImageName(hProcess, 0, exeName, &size))
                pExeName = wcsrchr(exeName, L'\\') + 1;
            ::CloseHandle(hProcess);
        }
    }
    printf("Event: 0x%X (%s) HWND: 0x%p, ID: 0x%X Child: 0x%X TID: %u PID: %u (%ws) Time: %u Name: %ws\n",
        event, EventNameToString(event),
        hwnd, idObject, idChild, idEventThread, 
        pid, pExeName,
        time, name.m_str);
}

int main() {
	auto hHook = ::SetWinEventHook(EVENT_MIN, EVENT_MAX, nullptr, OnEvent, 0, 0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS | WINEVENT_SKIPOWNTHREAD);

	::GetMessage(nullptr, nullptr, 0, 0);

}
