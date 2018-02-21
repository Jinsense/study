#pragma once

#define __PROFILING__
#ifdef __PROFILING__

extern "C" __declspec(dllimport) void Begin(char* Name);
extern "C" __declspec(dllimport) void End(char* Name);
extern "C" __declspec(dllimport) void ProfileSave(void);

#else
#define Begin(X)
#define End(X)
#define ProfileSave()
#endif