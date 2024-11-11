#include <stdio.h>
#include <windows.h>
#include <Psapi.h>
#include <stdbool.h>

DWORD IdProcesow[1024];
DWORD PoprzedniaLiczbaProcesow = 0;
DWORD LiczbaProcesow = 0;

HMODULE Moduly[1024];
DWORD PoprzedniaLiczbaModulow = 0;
DWORD LiczbaModulow = 0;

HANDLE UchwytProcesu = NULL;
WCHAR SciezkaProcesu [MAX_PATH];
WCHAR SciezkaModulu [MAX_PATH];

DWORD Rozmiar1;
DWORD Rozmiar2;
DWORD Rozmiar3;

bool Pobrano = false;


int main() 
{
    EnumProcesses(IdProcesow, sizeof(IdProcesow), &Rozmiar1);
    LiczbaProcesow = Rozmiar1 / sizeof(DWORD);

    for (DWORD i = 0; i < LiczbaProcesow; i++) {
        HANDLE Proces = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, IdProcesow[i]);

        if (Proces == NULL) {
            continue;
        }


        if (GetModuleFileNameEx(Proces, 0, SciezkaProcesu, &Rozmiar2)) {
            WCHAR* NazwaProcesu = wcsrchr(SciezkaProcesu, L'\\') + 1;
            if (wcscmp(L"Example.exe", NazwaProcesu)) {
                CloseHandle(Proces);
                continue;
            }
            wprintf(L"Znaleziono proces: %ls PID: %d\n", NazwaProcesu, IdProcesow[i]);
            UchwytProcesu = Proces;
        }
        else {
            CloseHandle(Proces);
        }
    }
    

    while (UchwytProcesu != NULL) {
        EnumProcessModulesEx(UchwytProcesu, Moduly, sizeof(Moduly), &Rozmiar3, LIST_MODULES_ALL);
        LiczbaModulow = Rozmiar3 / sizeof(HMODULE);
        if (Pobrano == false) {
            for (DWORD i = 0; i < LiczbaModulow; i++) {
                if (GetModuleFileNameEx(UchwytProcesu, Moduly[i], SciezkaModulu, sizeof(SciezkaModulu) / sizeof(WCHAR))) {
                    WCHAR* NazwaModulu = wcsrchr(SciezkaModulu, L'\\') + 1;
                    wprintf(L"Wykryto modul: %ls\n", NazwaModulu);
                }
            }
            Pobrano = true;
            PoprzedniaLiczbaModulow = LiczbaModulow;
        }
        if (Pobrano == true && LiczbaModulow != PoprzedniaLiczbaModulow) {
            wprintf(L"Wykryto inject\n");
        }
        Sleep(1000);
    }

    return 0;
}
