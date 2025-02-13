#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

void SpawnChildProcess(LPCWSTR lpAppName, PHANDLE phProcess, PHANDLE phStdinWrite, PHANDLE phStdoutRead, HPCON *phPC);

#ifdef __cplusplus
}
#endif
