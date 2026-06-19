#include <windows.h>
#include <stdio.h>
#include "run_program.h"

int run_program(char *program_path, char *args[], char *output_file, int fd, int append) {
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

void run_builtin(Command cmd, char **cwd, char *output_file, int fd, int append) {
    
}