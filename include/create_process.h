#pragma once

#include <windows.h>

void SpawnChildProcess(LPCWSTR lpAppName, PHANDLE phProcess, PHANDLE phStdinWrite, PHANDLE phStdoutRead, HPCON *phPC);
