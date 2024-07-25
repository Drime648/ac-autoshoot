// dllmain.cpp : Defines the entry point for the DLL application
#include "Windows.h"
#include "string"
#include "iostream"
#include <TlHelp32.h>
#include <cstdio>




uintptr_t jmpAddress;

INPUT input { 0 };
int x{ 0 };
int prevX{ 0 };
__declspec(naked) void codecave() {
    __asm {
        pushad
    }
    
    x++;


    __asm {
        popad
        mov eax, ebx
        mov dl, [eax]
        cmp dl, [ecx]
        jmp jmpAddress
    }
}

void hack() {
    AllocConsole();						//attaches console	

    FILE* f;


    freopen_s(&f, "CONOUT$", "w", stdout);	//sets cout to be used with our newly created console
    std::cout << "Finished version!\n" << std::hex;
    //std::cout << &codecave;

    DWORD oldProtect = 0;
    unsigned char* moduleBase = (unsigned char*)GetModuleHandle(L"ac_client.exe");

    unsigned char* targetCode = moduleBase + 0x81A65;
    jmpAddress = (uintptr_t)moduleBase + 0x81A6B;




    VirtualProtect((void*)targetCode, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset((void*)targetCode, 0x90, 6);
    (*targetCode) = 0xE9;

    //DWORD 
    *(DWORD*)(targetCode + 1) = (DWORD) &codecave - (DWORD)(targetCode) - 5;

    VirtualProtect((void*)targetCode, 6, oldProtect, &oldProtect);

    
    
    while (true) {
        
        //std::cout << x << '\n';
        //x = false;
        if (x != prevX) {
            //press down
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(1, &input, sizeof(input));
            prevX = x;
        }
        else {
            //press up
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &input, sizeof(input));
        }
        if (x >= 100) {
            //reset both ints
            x = 0;
            prevX = 0;
        }
        Sleep(10);
    }
    //cleanup
    fclose(f);
    FreeConsole();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hack, NULL, 0, NULL);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

