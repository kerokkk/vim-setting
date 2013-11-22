#pragma once

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <malloc.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define new				new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#if !defined(_DEBUG) && defined(USE_NATIVE_API)
extern "C" {
NTSYSAPI PVOID NTAPI RtlAllocateHeap(HANDLE hHeap, ULONG dwFlags, SIZE_T dwBytes);
NTSYSAPI BOOLEAN NTAPI RtlFreeHeap(HANDLE hHeap, ULONG dwFlags, PVOID lpMem);
}
#undef malloc
#undef calloc
#undef free
#define malloc(cb)		RtlAllocateHeap(_GetProcessHeap(), 0, cb)
#define calloc(n, cb)	RtlAllocateHeap(_GetProcessHeap(), HEAP_ZERO_MEMORY, cb)
#define free(pv)		RtlFreeHeap(_GetProcessHeap(), 0, pv)

#pragma warning(push)
#pragma warning(disable: 4035)
FORCEINLINE HANDLE _GetProcessHeap()
{
	__asm {
		mov eax, fs:[18h]
		mov eax, dword ptr [eax+30h]
		mov eax, dword ptr [eax+18h]
	}
}
#pragma warning(pop)

FORCEINLINE void* __cdecl operator new(size_t cb)
{
	return malloc(cb);
}
FORCEINLINE void __cdecl operator delete(void* pv)
{
	free(pv);
}
FORCEINLINE void* __cdecl operator new[](size_t cb)
{
	return malloc(cb);
}
FORCEINLINE void __cdecl operator delete[](void* pv)
{
	free(pv);
}
#endif	//!_DEBUG && USE_NATIVE_API
