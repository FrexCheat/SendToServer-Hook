// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include <winternl.h>
#include <TlHelp32.h>
#include <TlHelp32.h>	//CreateToolhelp32Snapshot
#pragma comment(lib,"ntdll.lib" )
#endif //PCH_H
class LTVector
{
public:
	float                                              x;
	float                                              y;
	float                                              z;

	inline LTVector() : x(0), y(0), z(0) {}

	inline LTVector(float x, float y, float z) : x(x), y(y), z(z) {}

	inline LTVector operator + (const LTVector& other) const { return LTVector(x + other.x, y + other.y, z + other.z); }

	inline LTVector operator - (const LTVector& other) const { return LTVector(x - other.x, y - other.y, z - other.z); }

	inline LTVector operator * (float scalar) const { return LTVector(x * scalar, y * scalar, z * scalar); }

	inline LTVector operator * (const LTVector& other) const { return LTVector(x * other.x, y * other.y, z * other.z); }

	inline LTVector operator / (float scalar) const { return LTVector(x / scalar, y / scalar, z / scalar); }

	inline LTVector operator / (const LTVector& other) const { return LTVector(x / other.x, y / other.y, z / other.z); }

	inline LTVector& operator=  (const LTVector& other) { x = other.x; y = other.y; z = other.z; return *this; }

	inline LTVector& operator+= (const LTVector& other) { x += other.x; y += other.y; z += other.z; return *this; }

	inline LTVector& operator-= (const LTVector& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

	inline LTVector& operator*= (const float other) { x *= other; y *= other; z *= other; return *this; }



	friend bool operator==(const LTVector& first, const LTVector& second) { return first.x == second.x && first.y == second.y && first.z == second.z; }

	friend bool operator!=(const LTVector& first, const LTVector& second) { return !(first == second); }
};
inline BOOL InstallSafeThread(
	__in LPVOID function
) {

	HMODULE hModule = GetModuleHandleA(0);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return FALSE;
	THREADENTRY32 te32 = { 0,0,0,0,0,0,0 };
	te32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hSnapshot, &te32))
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}
	do
	{
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, false, te32.th32ThreadID);
		if (hThread)
		{
			DWORD_PTR dwStartAddress = 0;
			if (NT_SUCCESS(NtQueryInformationThread(hThread, static_cast<THREADINFOCLASS>(9), &dwStartAddress, sizeof(DWORD_PTR), NULL)))
			{
				PIMAGE_DOS_HEADER DosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
				PIMAGE_NT_HEADERS NtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>((reinterpret_cast<DWORD_PTR>(hModule) + DosHeader->e_lfanew));
				if (!NtHeader || NtHeader->Signature != IMAGE_NT_SIGNATURE)
				{
					SetLastError(1793);
					return FALSE;
				}
				if (dwStartAddress > reinterpret_cast<DWORD_PTR>(hModule) &&
					dwStartAddress < (reinterpret_cast<DWORD_PTR>(hModule + 0x1000) + NtHeader->OptionalHeader.SizeOfImage))
				{
					//MessageBoxA(0, 0, 0, 0);
					_MEMORY_BASIC_INFORMATION mbi;
					CONTEXT ctx;
					DWORD Old = 0;
					ZeroMemory(&mbi, sizeof(mbi));
					if (!VirtualQuery(reinterpret_cast<LPCVOID>(dwStartAddress), &mbi, sizeof(mbi)))
					{
						CloseHandle(hSnapshot);
						CloseHandle(hThread);
						return FALSE;
					}
					if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &Old))
					{
						CloseHandle(hSnapshot);
						CloseHandle(hThread);
						return FALSE;

					}
					HANDLE Thread = CreateRemoteThread(GetCurrentProcess(), 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(dwStartAddress), 0, CREATE_SUSPENDED, 0);
					if (Thread == 0 || Thread == INVALID_HANDLE_VALUE)
					{
						TerminateThread(Thread, 0);
						CloseHandle(Thread);
						CloseHandle(hSnapshot);
						CloseHandle(hThread);
						return FALSE;
					}
					ctx.ContextFlags = CONTEXT_ALL;
					if (!GetThreadContext(Thread, &ctx))
					{
						TerminateThread(Thread, 0);
						CloseHandle(Thread);
						CloseHandle(hSnapshot);
						CloseHandle(hThread);
						return FALSE;
					}
#ifdef _WIN64
					ctx.Rcx = reinterpret_cast<DWORD64>(function);
#else
					ctx.Eax = reinterpret_cast<DWORD>(function);
#endif

					if (!SetThreadContext(Thread, &ctx))
					{
						TerminateThread(Thread, 0);
						CloseHandle(Thread);
						CloseHandle(hSnapshot);
						CloseHandle(hThread);
						return FALSE;
					}
					ResumeThread(Thread);
					CloseHandle(Thread);
					CloseHandle(hSnapshot);
					CloseHandle(hThread);
					return TRUE;
				}

			}
			CloseHandle(hThread);
		}
	} while (Thread32Next(hSnapshot, &te32));
	CloseHandle(hSnapshot);
	return FALSE;

}
