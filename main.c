#include<stdio.h>
#include<windows.h>

int main(int argc, char* argv[]){

    if(argc < 2){
        printf("You should pass shellcode file as an argument");
        return -1;
    }

    // shellcode file
    char *filename = argv[1];
    
    //obtaining a handle to the file
    HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open file. Error: %lu\n", GetLastError());
        return -1;
    }

    //Obtaining the file size
    LARGE_INTEGER FileSize;
    if (!GetFileSizeEx(hFile, &FileSize)) {
        printf("Failed to get file size. Error: %lu\n", GetLastError());
        CloseHandle(hFile);
        return -1;
    }

    // Allocating a memory buffer for the shellcode
    LPVOID ShellcodeBuffer = VirtualAlloc(NULL, FileSize.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!ShellcodeBuffer) {
        printf("Memory allocation failed. Error: %lu\n", GetLastError());
        CloseHandle(hFile);
        return -1;
    }

    
    // Reading the shellcode into the buffer
    DWORD BytesRead;
    if (!ReadFile(hFile, ShellcodeBuffer, (DWORD)FileSize.QuadPart, &BytesRead, NULL)) {
        printf("Failed to read file. Error: %lu\n", GetLastError());
        VirtualFree(ShellcodeBuffer, 0, MEM_RELEASE);
        CloseHandle(hFile);
        return -1;
    }

    
    // Create a thread that executes the shellcode
    DWORD ThreadID;
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShellcodeBuffer, NULL, CREATE_SUSPENDED, &ThreadID);
    if (hThread == NULL) {
        printf("Thread creation failed. Error: %lu\n", GetLastError());
        VirtualFree(ShellcodeBuffer, 0, MEM_RELEASE);
        return -1;
    }
    printf("Created Thread : %d\n", ThreadID);
    printf("Entrypoint : %p\n", ShellcodeBuffer);
    printf("Attach a Debugger, set a breakpoint and press Enter...");
    getchar();
    ResumeThread(hThread);
    WaitForSingleObject(hThread, INFINITE);

    VirtualFree(ShellcodeBuffer, 0, MEM_RELEASE);
    CloseHandle(hFile);
    return 0;
}