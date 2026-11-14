// MinGW shim for ScriptHookV
// ScriptHookV.dll is built with MSVC, so its C++ exports use MSVC name mangling.
// This header replaces the SDK's main.h and resolves them at runtime via
// GetProcAddress instead of linking against ScriptHookV.lib.
// Made by 3rr0r303YT on TikTok - GPL-3.0-or-later
#pragma once
#include <windows.h>

static inline FARPROC shvProc(const char* n)
{
    static HMODULE m = LoadLibraryA("ScriptHookV.dll");
    return m ? GetProcAddress(m, n) : nullptr;
}
static inline void nativeInit(UINT64 h)
{ static auto f = (void(*)(UINT64))shvProc("?nativeInit@@YAX_K@Z"); f(h); }
static inline void nativePush64(UINT64 v)
{ static auto f = (void(*)(UINT64))shvProc("?nativePush64@@YAX_K@Z"); f(v); }
static inline PUINT64 nativeCall()
{ static auto f = (PUINT64(*)())shvProc("?nativeCall@@YAPEA_KXZ"); return f(); }
static inline void scriptWait(DWORD t)
{ static auto f = (void(*)(DWORD))shvProc("?scriptWait@@YAXK@Z"); f(t); }
static inline void scriptRegister(HMODULE m, void(*fn)())
{ auto f = (void(*)(HMODULE, void(*)()))shvProc("?scriptRegister@@YAXPEAUHINSTANCE__@@P6AXXZ@Z"); if (f) f(m, fn); }
static inline void scriptUnregister(HMODULE m)
{ auto f = (void(*)(HMODULE))shvProc("?scriptUnregister@@YAXPEAUHINSTANCE__@@@Z"); if (f) f(m); }
static inline int worldGetAllVehicles(int* arr, int n)
{ static auto f = (int(*)(int*, int))shvProc("?worldGetAllVehicles@@YAHPEAHH@Z"); return f ? f(arr, n) : 0; }
static inline int worldGetAllPeds(int* arr, int n)
{ static auto f = (int(*)(int*, int))shvProc("?worldGetAllPeds@@YAHPEAHH@Z"); return f ? f(arr, n) : 0; }
