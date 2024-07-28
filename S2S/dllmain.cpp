// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <iostream>
BYTE buff[0x1000];
ULONG64(__fastcall* oCreateMessage)(ULONG64,ULONG64);
void(__fastcall* oIncRef)(void*);
void(__fastcall* oDecRef)(void*);
ULONG64 (__fastcall* oRead)(void*);
DWORD32(__fastcall* oSize)(void*);
void(__fastcall* oWriteBits)(void*, DWORD, int);
void(__fastcall* oWriteBits64)(void*, ULONG64, int);
void(__fastcall* oWriteData)(void*, const void*, int);
void(__fastcall* oWriteString)(void*, const char*);
void(__fastcall* oWriteCompLTVector)(void*, const LTVector&);
void(__fastcall* oWriteCompPos)(void*, const LTVector&);
void(__fastcall* oWriteObject)(void*, PVOID);



ULONG64 __fastcall HookRead(void* LTMessage_Write) {
    printf("Msg->Read();\n");
    return oRead(LTMessage_Write);
}
DWORD32 __fastcall HookSize(void* LTMessage_Write) {

    printf("Msg->Size();\n");
    return oSize(LTMessage_Write);
}
void __fastcall HookIncRef(void* LTMessage_Write) {
    printf("Msg->IncRef();\n");
    return oIncRef(LTMessage_Write);
}
void __fastcall HookDecRef(void* LTMessage_Write) {
    printf("Msg->DecRef();\n\n");
    return oDecRef(LTMessage_Write);
}
void __fastcall HookWriteBits(void* LTMessage_Write, DWORD nValue, int nSize) {
    if (nValue < 0x1000)
        printf("Msg->WriteBits(0x%lX,%d);\n", nValue, nSize);
    return oWriteBits(LTMessage_Write, nValue, nSize);
}
void __fastcall HookWriteBits64(void* LTMessage_Write, ULONG64 nValue, int nSize) {
    printf("Msg->WriteBits64(0x%llX,%d);\n", nValue, nSize);
    return oWriteBits64(LTMessage_Write, nValue, nSize);
}
void __fastcall HookWriteData(void* LTMessage_Write, const void* pData, int nSize) {
    printf("X->%.1f | Y->%.1f | Z->%.1f\n", *(float*)(ULONG64(pData)), *(float*)(ULONG64(pData) + 4), *(float*)(ULONG64(pData) + 8));
    printf("Msg->WriteData(0x%llX,%d);\n", pData, nSize);
    return oWriteData(LTMessage_Write, pData, nSize);
}
void __fastcall HookWriteString(void* LTMessage_Write, const char* pString) {
    printf("Msg->WriteString(%s);\n", pString);
    return oWriteString(LTMessage_Write, pString);
}
void __fastcall HookWriteCompLTVector(void* LTMessage_Write, const LTVector& vVec) {
    printf("X->%.1f | Y->%.1f | Z->%.1f\n", vVec.x, vVec.y, vVec.z);
    printf("Msg->WriteCompLTVector(xyz);\n");
    return oWriteCompLTVector(LTMessage_Write, vVec);
}
void __fastcall HookWriteCompPos(void* LTMessage_Write, const LTVector& vPos) {
    printf("X->%.1f | Y->%.1f | Z->%.1f\n", vPos.x, vPos.y, vPos.z);
    printf("Msg->WriteCompPos(xyz);\n");
    return oWriteCompPos(LTMessage_Write, vPos);
}
void __fastcall HookWriteObject(void* LTMessage_Write, PVOID hObj) {
    printf("Msg->WriteObject(0x%llX,%d);\n", hObj);
    return oWriteObject(LTMessage_Write, hObj);
}





ULONG64 CSHELL;

ULONG64 __fastcall HookCreateMessage(ULONG64 RCX, ULONG64 Message) {
    auto result = oCreateMessage(RCX, Message);
    auto v1 = *(ULONG64*)Message;
    auto v2 = *(ULONG64*)v1;
    if (v1 != (ULONG64)buff) {
        memcpy(buff, (PVOID)v2, 0x1000);
        oIncRef = reinterpret_cast<decltype(oIncRef)>(*(ULONG64*)(v2 + 0x08));
        oDecRef = reinterpret_cast<decltype(oIncRef)>(*(ULONG64*)(v2 + 0x10));
        oRead = reinterpret_cast<decltype(oRead)>(*(ULONG64*)(v2 + 0x38));
        oSize = reinterpret_cast<decltype(oSize)>(*(ULONG64*)(v2 + 0x40));
        oWriteBits = reinterpret_cast<decltype(oWriteBits)>(*(ULONG64*)(v2 + 0x48));
        oWriteBits64 = reinterpret_cast<decltype(oWriteBits64)>(*(ULONG64*)(v2 + 0x50));
        oWriteData = reinterpret_cast<decltype(oWriteData)>(*(ULONG64*)(v2 + 0x58));
        oWriteString = reinterpret_cast<decltype(oWriteString)>(*(ULONG64*)(v2 + 0x70));
        oWriteCompLTVector = reinterpret_cast<decltype(oWriteCompLTVector)>(*(ULONG64*)(v2 + 0x80));
        oWriteCompPos = reinterpret_cast<decltype(oWriteCompPos)>(*(ULONG64*)(v2 + 0x88));
        oWriteObject = reinterpret_cast<decltype(oWriteObject)>(*(ULONG64*)(v2 + 0xB8));
        *(PVOID*)(buff + 0x08) = HookIncRef;
        *(PVOID*)(buff + 0x10) = HookDecRef;
        *(PVOID*)(buff + 0x38) = HookRead;
        *(PVOID*)(buff + 0x40) = HookSize;
        *(PVOID*)(buff + 0x48) = HookWriteBits;
        *(PVOID*)(buff + 0x50) = HookWriteBits64;
        *(PVOID*)(buff + 0x58) = HookWriteData;
        *(PVOID*)(buff + 0x70) = HookWriteString;
        *(PVOID*)(buff + 0x80) = HookWriteCompLTVector;
        *(PVOID*)(buff + 0x88) = HookWriteCompPos;
        *(PVOID*)(buff + 0xB8) = HookWriteObject;
        *(ULONG64*)v1 = (ULONG64)buff;
    }
    return result;
}
typedef bool(__fastcall* WeaponFunc)(ULONG64* a1, BYTE* a2, int a3);
WeaponFunc Func = nullptr;
ULONG64 PlayCInt, WeaponMgr;
void FuncStart() {
    auto addr = *(ULONG64**)PlayCInt;
    if (!addr)return;
    //printf("PlayeCInt:%p\n", addr);
    auto weaponaddr = *(ULONG64*)WeaponMgr;
    if (!weaponaddr) return;
    //printf("weaponaddr:%p\n", weaponaddr);
    auto weapon = *(ULONG64**)(weaponaddr + 0x6D20);
    //printf("weapon:%p\n", weapon);
    Func(addr, (BYTE*)weapon, 1);
}


BYTE buff1[0x1000];
void MainThrad() {
    while (true) {
        if (GetAsyncKeyState(VK_HOME) & 1)
            break;
    }
    CSHELL = ULONG64(GetModuleHandleA("cshell_x64.dll"));
    auto CLTCommonClient = *(ULONG64*)(0x14042C778);
    auto v2 = *(ULONG64*)(CLTCommonClient);
    oCreateMessage = reinterpret_cast<decltype(oCreateMessage)>(*(ULONG64*)(v2 + 0x48));

    memcpy(buff1, PVOID(v2), 0x1000);

    *(PVOID*)(buff1 + 0x48) = HookCreateMessage;
    *(ULONG64*)CLTCommonClient = (ULONG64)buff1;

    //PlayCInt = CSHELL + 0x2733B28;
    //WeaponMgr = CSHELL + 0x26DD790;
    //Func = (WeaponFunc)(CSHELL + 0x7898E0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        SetConsoleTitleA("Frex S2S Debug");
        InstallSafeThread(MainThrad);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

