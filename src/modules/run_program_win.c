#include <windows.h>
#include <stdio.h>
#include "run_program.h"

int run_program(char *program_path, char *args[]) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcess(program_path, args, NULL, NULL, NULL, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        printf("Failed to spawn process\n");
        return 1;
    }
    
    return 0;
}