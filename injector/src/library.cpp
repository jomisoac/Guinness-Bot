#include <iostream>

#include <HadesMemory/Injector.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <AsmJit/Assembler.h>
#include <jni.h>
#include "com_guiness_bot_external_NativeAPI.h"
#include <tlhelp32.h>
#include <cstdio>
#include <string>
#include <thread>
#include <list>
#include "keysend.h"

using HadesMem::MemoryMgr;
using HadesMem::Injector;

int main(int ac, char **args) {
    auto current = findWindowHandle(GetCurrentProcessId());
    auto win = findWindowHandle(L"Dofus.exe");
    DWORD myThread  = GetCurrentThreadId();

    DWORD curThread = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
    AttachThreadInput(myThread, curThread, TRUE);

    DWORD newThread = GetWindowThreadProcessId(win, NULL);
    AttachThreadInput(curThread, newThread, TRUE);
    AttachThreadInput(myThread, newThread, TRUE);
    //SetWindowTextA(window, "Guinness");
    PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    SetFocus(current);
    SetFocus(win);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sendKeyStroke(win,0x09);
    PostMessage(win, WM_CHAR, 'R', 0);
    PostMessage(win, WM_CHAR, 'o', 0);
    PostMessage(win, WM_CHAR, 'm', 0);
    PostMessage(win, WM_CHAR, 'a', 0);
    PostMessage(win, WM_CHAR, 'i', 0);
    PostMessage(win, WM_CHAR, 'N', 0);
    PostMessage(win, WM_CHAR, 200, 0);
}

std::wstring toWstring(JNIEnv *env, jstring str) {
    unsigned char isCopy = 1;
    std::string a((*env).GetStringUTFChars(str, &isCopy));
    return std::wstring(a.begin(), a.end());
}

JNIEXPORT jintArray JNICALL
Java_com_guiness_bot_external_NativeAPI_availableProcesses
(JNIEnv *env, jobject, jstring processName)
{
    std::vector<long> processIdList;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE){
        while (Process32Next(snapshot, &entry) == TRUE) {
            if ((wcscmp((wchar_t *) entry.szExeFile, toWstring(env, processName).c_str()) == 0)) {
                processIdList.push_back(entry.th32ProcessID);
            }
        }
    }
    CloseHandle(snapshot);
    jintArray jniArray  = env->NewIntArray(processIdList.size());
    env->SetIntArrayRegion(jniArray, 0, processIdList.size(), &processIdList[0]);
    return jniArray;
}

JNIEXPORT void JNICALL
Java_com_guiness_bot_external_NativeAPI_inject
(JNIEnv *env, jobject, jint processId, jstring dllPath)
{
    auto id = (DWORD) processId;
    MemoryMgr const memory(id);
    Injector const injector(memory);

    injector.InjectDll(toWstring(env, dllPath), HadesMem::Injector::InjectFlag_PathResolution);
}

JNIEXPORT void JNICALL
Java_com_guiness_bot_external_NativeAPI_login
(JNIEnv *env, jobject, jint processId, jstring username, jstring password)
{
    auto current = findWindowHandle(GetCurrentProcessId());
    auto win = findWindowHandle(processId);
    auto wUsername = toWstring(env, username);
    auto wPassword = toWstring(env, password);

    /** attach thread to window **/
    DWORD myThread  = GetCurrentThreadId();
    DWORD curThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    AttachThreadInput(myThread, curThread, TRUE);

    DWORD newThread = GetWindowThreadProcessId(win, nullptr);
    AttachThreadInput(curThread, newThread, TRUE);
    AttachThreadInput(myThread, newThread, TRUE);

    /** reset tab index by focusing window **/
    PostMessage(win, WM_SYSCOMMAND, SC_RESTORE, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    SetFocus(current);
    SetFocus(win);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    /** fill username, password and press enter **/
    WPARAM KEY_TAB = 0x09;
    WPARAM KEY_ENTER = 0x0D;

    sendKeyStroke(win, KEY_TAB);
    sendUtf16String(win, wUsername);
    sendKeyStroke(win, KEY_TAB);
    sendUtf16String(win, wPassword);
    sendKeyStroke(win, KEY_TAB);
    sendKeyStroke(win, KEY_ENTER);
}

JNIEXPORT void JNICALL
Java_com_guiness_bot_external_NativeAPI_reLogin
(JNIEnv *, jobject, jint, jstring username, jstring password)
{

}